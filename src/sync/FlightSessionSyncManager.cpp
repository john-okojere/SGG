#include "FlightSessionSyncManager.h"

#include "../auth/SessionManager.h"
#include "../cache/LocalSyncCache.h"
#include "../controllers/AppState.h"
#include "../models/MissionPlanModel.h"
#include "../network/ApiClient.h"
#include "../vehicle/VehicleTelemetryModel.h"
#include "GcsEventSyncManager.h"
#include "MissionSyncManager.h"

#include <QDateTime>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QUuid>

namespace {

QString backendIdString(const QString &value)
{
    const QString trimmed = value.trimmed();
    if (trimmed.isEmpty()) {
        return QString();
    }
    bool ok = false;
    const int numericId = trimmed.toInt(&ok);
    return ok && numericId > 0 ? QString::number(numericId) : QString();
}

} // namespace

FlightSessionSyncManager::FlightSessionSyncManager(ApiClient *api,
                                                   SessionManager *session,
                                                   MissionSyncManager *missionSync,
                                                   MissionPlanModel *plan,
                                                   AppState *appState,
                                                   VehicleTelemetryModel *telemetry,
                                                   GcsEventSyncManager *events,
                                                   LocalSyncCache *cache,
                                                   QObject *parent)
    : QObject(parent),
      m_api(api),
      m_session(session),
      m_missionSync(missionSync),
      m_plan(plan),
      m_appState(appState),
      m_telemetry(telemetry),
      m_events(events),
      m_cache(cache)
{
}

bool FlightSessionSyncManager::active() const { return m_active; }
QString FlightSessionSyncManager::serverSessionId() const { return m_serverSessionId; }
QString FlightSessionSyncManager::clientSessionId() const { return m_clientSessionId; }
QString FlightSessionSyncManager::mode() const { return m_mode; }
QString FlightSessionSyncManager::status() const { return m_status; }

void FlightSessionSyncManager::applyBootstrap(const QVariantMap &bootstrap)
{
    const QVariantMap session = bootstrap.value(QStringLiteral("active_flight_session")).toMap();
    if (session.isEmpty()) {
        return;
    }
    adoptServerSession(session);
    setStatus(QStringLiteral("Restored active flight session from Control Center."));
}

void FlightSessionSyncManager::beginPilotSession(const QString &clientSessionId)
{
    QString clientId = clientSessionId.trimmed();
    if (clientId.isEmpty()) {
        clientId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    if (m_active && m_mode == QStringLiteral("PILOT") && m_clientSessionId == clientId) {
        return;
    }
    if (m_active) {
        endActiveSession(QStringLiteral("completed"), QStringLiteral("Switching to pilot flight session"));
    }
    m_clientSessionId = clientId;
    startSessionOnServer(QStringLiteral("PILOT"), clientId, QString());
}

void FlightSessionSyncManager::beginMissionSession(const QString &clientSessionId, const QString &missionId)
{
    if (clientSessionId.isEmpty()) {
        return;
    }
    const QString backendMissionId = backendIdString(missionId);
    if (backendMissionId.isEmpty()) {
        setStatus(QStringLiteral("Mission flight session waiting for synced mission id."));
        return;
    }
    if (m_active && m_mode == QStringLiteral("MISSION") && m_clientSessionId == clientSessionId) {
        return;
    }
    if (m_active) {
        endActiveSession(QStringLiteral("completed"), QStringLiteral("Switching to mission flight session"));
    }
    m_clientSessionId = clientSessionId;
    startSessionOnServer(QStringLiteral("MISSION"), clientSessionId, backendMissionId);
}

void FlightSessionSyncManager::endActiveSession(const QString &endStatus, const QString &reason, const QVariantMap &summary)
{
    if (!m_active) {
        return;
    }

    const QString serverId = m_serverSessionId;
    const QVariantMap snapshot{
        {QStringLiteral("client_session_id"), m_clientSessionId},
        {QStringLiteral("server_session_id"), m_serverSessionId},
        {QStringLiteral("mode"), m_mode},
        {QStringLiteral("end_status"), endStatus},
        {QStringLiteral("reason"), reason},
        {QStringLiteral("summary"), summary},
        {QStringLiteral("ended_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
    };

    clearSession();

    if (m_events) {
        m_events->recordEvent(QStringLiteral("flight_session_ended"),
                              QStringLiteral("info"),
                              QStringLiteral("Flight session ended"),
                              QJsonObject::fromVariantMap(snapshot));
    }

    if (!m_api || !m_session || !m_session->operationsAllowed() || serverId.isEmpty()) {
        if (m_cache) {
            m_cache->enqueueSync(QStringLiteral("flight_session_end"), snapshot);
        }
        setStatus(QStringLiteral("Flight session ended locally; sync queued."));
        return;
    }

    QJsonObject payload{
        {QStringLiteral("status"), endStatus},
        {QStringLiteral("reason"), reason}
    };
    if (!summary.isEmpty()) {
        payload.insert(QStringLiteral("summary"), QJsonObject::fromVariantMap(summary));
    }
    m_api->post(QStringLiteral("/api/flight-sessions/%1/end/").arg(serverId), payload, true, true,
                [this, endStatus](int statusCode, const QJsonObject &, const QString &error) {
        if (statusCode < 200 || statusCode >= 300) {
            setStatus(error.isEmpty()
                          ? QStringLiteral("Flight session end sync failed.")
                          : error);
            return;
        }
        setStatus(QStringLiteral("Flight session closed on Control Center (%1).").arg(endStatus));
    });
}

void FlightSessionSyncManager::recordPilotAction(const QString &actionType,
                                                 const QJsonObject &actionData,
                                                 const QString &message)
{
    const QString normalizedType = normalizedActionType(actionType);
    QJsonObject payload = actionData;
    payload[QStringLiteral("message")] = message;
    payload[QStringLiteral("operation_mode")] = m_mode.isEmpty() ? QStringLiteral("PILOT") : m_mode;
    payload[QStringLiteral("client_session_id")] = m_clientSessionId;

    const QJsonObject position = positionPayload();
    for (auto it = position.begin(); it != position.end(); ++it) {
        if (!payload.contains(it.key())) {
            payload.insert(it.key(), it.value());
        }
    }

    if (m_events) {
        m_events->recordEvent(actionType,
                              QStringLiteral("info"),
                              message.isEmpty() ? QStringLiteral("Pilot action: %1").arg(normalizedType) : message,
                              payload);
    }

    if (!m_api || !m_session || !m_session->operationsAllowed() || m_serverSessionId.isEmpty()) {
        if (m_cache) {
            QVariantMap queued = payload.toVariantMap();
            queued[QStringLiteral("action_type")] = normalizedType;
            queued[QStringLiteral("server_session_id")] = m_serverSessionId;
            m_cache->enqueueSync(QStringLiteral("pilot_action"), queued);
        }
        setStatus(QStringLiteral("Pilot action queued: %1").arg(normalizedType));
        return;
    }

    QJsonObject body{
        {QStringLiteral("action_type"), normalizedType},
        {QStringLiteral("action_data"), payload}
    };
    const QJsonObject positionFields = positionPayload();
    for (auto it = positionFields.begin(); it != positionFields.end(); ++it) {
        body.insert(it.key(), it.value());
    }

    m_api->post(QStringLiteral("/api/flight-sessions/%1/actions/").arg(m_serverSessionId), body, true, true,
                [this, normalizedType](int statusCode, const QJsonObject &, const QString &error) {
        if (statusCode < 200 || statusCode >= 300) {
            setStatus(error.isEmpty()
                          ? QStringLiteral("Pilot action sync failed: %1").arg(normalizedType)
                          : error);
            return;
        }
        setStatus(QStringLiteral("Pilot action logged: %1").arg(normalizedType));
    });
}

void FlightSessionSyncManager::startSessionOnServer(const QString &mode,
                                                    const QString &clientSessionId,
                                                    const QString &missionId)
{
    m_mode = mode;
    m_clientSessionId = clientSessionId;

    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        m_active = true;
        setStatus(QStringLiteral("Flight session active locally; Control Center sync pending."));
        emit sessionChanged();
        return;
    }

    if (m_starting) {
        return;
    }
    m_starting = true;

    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString source = env.value(QStringLiteral("SKYGRID_FLIGHT_SOURCE"),
                                     env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true")
                                         ? QStringLiteral("gazebo")
                                         : QStringLiteral("mavsdk"));

    QJsonObject payload{
        {QStringLiteral("mode"), mode},
        {QStringLiteral("aircraft"), selectedAircraftId()},
        {QStringLiteral("source"), source}
    };
    if (!missionId.isEmpty()) {
        payload[QStringLiteral("mission")] = missionId.toInt();
    }

    setStatus(QStringLiteral("Starting %1 flight session on Control Center...").arg(mode));
    m_api->post(QStringLiteral("/api/flight-sessions/start/"), payload, true, true,
                [this, clientSessionId, mode](int statusCode, const QJsonObject &body, const QString &error) {
        m_starting = false;
        if (statusCode < 200 || statusCode >= 300) {
            m_active = true;
            setStatus(error.isEmpty()
                          ? QStringLiteral("Flight session active locally; server start failed.")
                          : error);
            emit sessionChanged();
            return;
        }

        QVariantMap session = body.toVariantMap();
        session[QStringLiteral("client_session_id")] = clientSessionId;
        adoptServerSession(session);
        if (m_events) {
            m_events->recordEvent(QStringLiteral("flight_session_started"),
                                  QStringLiteral("info"),
                                  QStringLiteral("Flight session started on Control Center"),
                                  QJsonObject{
                                      {QStringLiteral("server_session_id"), m_serverSessionId},
                                      {QStringLiteral("client_session_id"), m_clientSessionId},
                                      {QStringLiteral("mode"), mode}
                                  });
        }
        setStatus(QStringLiteral("Flight session active on Control Center."));
    });
}

void FlightSessionSyncManager::adoptServerSession(const QVariantMap &session)
{
    m_serverSessionId = session.value(QStringLiteral("id")).toString();
    if (m_serverSessionId.isEmpty()) {
        m_serverSessionId = QString::number(session.value(QStringLiteral("pk")).toInt());
    }
    const QString clientId = session.value(QStringLiteral("client_session_id")).toString();
    if (!clientId.isEmpty()) {
        m_clientSessionId = clientId;
    }
    m_mode = session.value(QStringLiteral("mode"), m_mode).toString();
    m_active = !m_serverSessionId.isEmpty();
    emit sessionChanged();
}

void FlightSessionSyncManager::clearSession()
{
    m_active = false;
    m_starting = false;
    m_serverSessionId.clear();
    m_clientSessionId.clear();
    m_mode.clear();
    emit sessionChanged();
}

int FlightSessionSyncManager::selectedAircraftId() const
{
    const QVariantList aircraft = m_missionSync ? m_missionSync->assignedAircraft() : QVariantList{};
    if (aircraft.isEmpty()) {
        return 0;
    }
    QVariantMap aircraftMap = aircraft.first().toMap();
    for (const QVariant &entry : aircraft) {
        const QVariantMap candidate = entry.toMap();
        if (candidate.value(QStringLiteral("selected")).toBool()
            || candidate.value(QStringLiteral("is_default")).toBool()
            || candidate.value(QStringLiteral("default")).toBool()
            || candidate.value(QStringLiteral("active")).toBool()) {
            aircraftMap = candidate;
            break;
        }
    }
    QVariant id = aircraftMap.value(QStringLiteral("id"), aircraftMap.value(QStringLiteral("aircraft_id")));
    return id.toInt();
}

QJsonObject FlightSessionSyncManager::positionPayload() const
{
    if (!m_telemetry || !m_telemetry->connected()) {
        return {};
    }
    return {
        {QStringLiteral("latitude"), QString::number(m_telemetry->latitude(), 'f', 7)},
        {QStringLiteral("longitude"), QString::number(m_telemetry->longitude(), 'f', 7)},
        {QStringLiteral("altitude_m"), QString::number(m_telemetry->altitude(), 'f', 2)}
    };
}

void FlightSessionSyncManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit sessionChanged();
}

QString FlightSessionSyncManager::normalizedActionType(const QString &actionType) const
{
    const QString key = actionType.trimmed().toLower();
    if (key == QStringLiteral("vehicle_arm") || key == QStringLiteral("arm")) {
        return QStringLiteral("ARM");
    }
    if (key == QStringLiteral("takeoff_started") || key == QStringLiteral("takeoff")) {
        return QStringLiteral("TAKEOFF");
    }
    if (key == QStringLiteral("vehicle_disarm") || key == QStringLiteral("disarm")) {
        return QStringLiteral("DISARM");
    }
    if (key == QStringLiteral("vehicle_land") || key == QStringLiteral("land_requested") || key == QStringLiteral("land")) {
        return QStringLiteral("LAND");
    }
    if (key == QStringLiteral("vehicle_return_to_home") || key == QStringLiteral("return_to_home_requested") || key == QStringLiteral("return_to_home") || key == QStringLiteral("rtl")) {
        return QStringLiteral("RETURN_HOME");
    }
    if (key == QStringLiteral("vehicle_hold") || key == QStringLiteral("hold")) {
        return QStringLiteral("HOLD");
    }
    if (key == QStringLiteral("vehicle_emergency_stop") || key == QStringLiteral("emergency_stop_requested") || key == QStringLiteral("emergency_stop")) {
        return QStringLiteral("EMERGENCY_STOP");
    }
    if (key == QStringLiteral("manual_input_active")) {
        return QStringLiteral("MANUAL_INPUT_ACTIVE");
    }
    if (key == QStringLiteral("camera_action")) {
        return QStringLiteral("CAMERA_ACTION");
    }
    if (key == QStringLiteral("joystick_visibility_changed")) {
        return QStringLiteral("JOYSTICK_VISIBILITY");
    }
    if (key == QStringLiteral("pilot_view_mode_changed")) {
        return QStringLiteral("VIEW_MODE_CHANGED");
    }
    if (key == QStringLiteral("pilot_mode_started")) {
        return QStringLiteral("PILOT_MODE_STARTED");
    }
    return actionType.trimmed().toUpper();
}
