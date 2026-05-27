#include "FlightStatsManager.h"

#include "../cache/LocalSyncCache.h"
#include "../controllers/AppState.h"
#include "../models/MissionPlanModel.h"
#include "../sync/FlightSessionSyncManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/VehicleTelemetryModel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantMap>
#include <QUuid>
#include <QtMath>

FlightStatsManager::FlightStatsManager(VehicleTelemetryModel *telemetry,
                                       FlightSessionSyncManager *flightSessions,
                                       AppState *appState,
                                       MissionPlanModel *plan,
                                       GcsEventSyncManager *events,
                                       LocalSyncCache *cache,
                                       QObject *parent)
    : QObject(parent),
      m_telemetry(telemetry),
      m_flightSessions(flightSessions),
      m_appState(appState),
      m_plan(plan),
      m_events(events),
      m_cache(cache)
{
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &FlightStatsManager::tick);
    if (m_telemetry) {
        connect(m_telemetry, &VehicleTelemetryModel::telemetryChanged, this, &FlightStatsManager::ingestTelemetry);
    }
}

bool FlightStatsManager::active() const { return m_active; }
QString FlightStatsManager::flightSessionId() const { return m_flightSessionId; }
QString FlightStatsManager::startedAt() const { return m_startedAt; }
int FlightStatsManager::flightDurationSeconds() const { return m_flightDurationSeconds; }
QString FlightStatsManager::flightTimeText() const
{
    const int hours = m_flightDurationSeconds / 3600;
    const int minutes = (m_flightDurationSeconds % 3600) / 60;
    const int seconds = m_flightDurationSeconds % 60;
    return QStringLiteral("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}
double FlightStatsManager::distanceMeters() const { return m_distanceMeters; }
double FlightStatsManager::distanceKm() const { return m_distanceMeters / 1000.0; }
double FlightStatsManager::maxAltitude() const { return m_maxAltitude; }
double FlightStatsManager::batteryStartPercent() const { return m_batteryStartPercent; }
double FlightStatsManager::batteryEndPercent() const { return m_batteryEndPercent; }
QVariantList FlightStatsManager::flightPath() const { return m_flightPath; }
QString FlightStatsManager::status() const { return m_status; }

void FlightStatsManager::startSession()
{
    if (m_active) {
        return;
    }
    m_active = true;
    if (m_flightSessions && !m_flightSessions->clientSessionId().isEmpty()) {
        m_flightSessionId = m_flightSessions->clientSessionId();
    } else {
        m_flightSessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    m_startedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
    m_startedMsec = QDateTime::currentMSecsSinceEpoch();
    m_flightDurationSeconds = 0;
    m_distanceMeters = 0.0;
    m_maxAltitude = 0.0;
    m_batteryStartPercent = m_telemetry ? m_telemetry->battery() : 0.0;
    m_batteryEndPercent = m_batteryStartPercent;
    m_lastLatitude = 0.0;
    m_lastLongitude = 0.0;
    m_flightPath.clear();
    m_timer.start();
    setStatus(QStringLiteral("Pilot flight session active"));
    if (m_flightSessions && !m_flightSessions->active()) {
        const bool pilotMode = !m_appState || m_appState->operationalMode() == QStringLiteral("pilot");
        if (pilotMode) {
            m_flightSessions->beginPilotSession(m_flightSessionId);
        } else {
            QString missionId;
            if (m_appState && !m_appState->selectedMissionId().isEmpty()) {
                missionId = m_appState->selectedMissionId();
            } else if (m_plan) {
                missionId = m_plan->missionId();
            }
            m_flightSessions->beginMissionSession(m_flightSessionId, missionId);
        }
    } else if (m_events) {
        m_events->recordEvent(QStringLiteral("flight_session_started"),
                              QStringLiteral("info"),
                              QStringLiteral("Flight session started"),
                              QJsonObject{
                                  {QStringLiteral("flight_session_id"), m_flightSessionId},
                                  {QStringLiteral("started_at"), m_startedAt}
                              });
    }
    ingestTelemetry();
    emit statsChanged();
}

void FlightStatsManager::endSession()
{
    if (!m_active) {
        return;
    }
    tick();
    m_active = false;
    m_timer.stop();
    m_batteryEndPercent = m_telemetry ? m_telemetry->battery() : m_batteryEndPercent;

    QVariantMap record{
        {QStringLiteral("operation_mode"), QStringLiteral("PILOT")},
        {QStringLiteral("flight_session_id"), m_flightSessionId},
        {QStringLiteral("started_at"), m_startedAt},
        {QStringLiteral("ended_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)},
        {QStringLiteral("duration_seconds"), m_flightDurationSeconds},
        {QStringLiteral("distance_m"), m_distanceMeters},
        {QStringLiteral("max_altitude_m"), m_maxAltitude},
        {QStringLiteral("battery_start_percent"), m_batteryStartPercent},
        {QStringLiteral("battery_end_percent"), m_batteryEndPercent},
        {QStringLiteral("flight_path"), m_flightPath}
    };
    if (m_cache) {
        const QString id = QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyyMMddhhmmsszzz"));
        m_cache->saveObject(QStringLiteral("flight_records"), id, record);
        m_cache->enqueueSync(QStringLiteral("flight_record"), record);
    }
    if (m_flightSessions) {
        m_flightSessions->endActiveSession(QStringLiteral("completed"), QStringLiteral("Flight timer ended"));
    } else if (m_events) {
        m_events->recordEvent(QStringLiteral("flight_session_ended"),
                              QStringLiteral("info"),
                              QStringLiteral("Flight session ended"),
                              QJsonObject::fromVariantMap(record));
    }
    setStatus(QStringLiteral("Flight saved: %1, %2 m").arg(flightTimeText()).arg(qRound(m_distanceMeters)));
    emit statsChanged();
}

void FlightStatsManager::reset()
{
    m_active = false;
    m_flightSessionId.clear();
    m_startedAt.clear();
    m_timer.stop();
    m_startedMsec = 0;
    m_flightDurationSeconds = 0;
    m_distanceMeters = 0.0;
    m_maxAltitude = 0.0;
    m_batteryStartPercent = 0.0;
    m_batteryEndPercent = 0.0;
    m_lastLatitude = 0.0;
    m_lastLongitude = 0.0;
    m_flightPath.clear();
    setStatus(QStringLiteral("No active flight session"));
    emit statsChanged();
}

double FlightStatsManager::distanceBetween(double latA, double lonA, double latB, double lonB)
{
    const double earth = 6371000.0;
    const double phiA = qDegreesToRadians(latA);
    const double phiB = qDegreesToRadians(latB);
    const double dPhi = qDegreesToRadians(latB - latA);
    const double dLambda = qDegreesToRadians(lonB - lonA);
    const double a = qSin(dPhi / 2.0) * qSin(dPhi / 2.0)
        + qCos(phiA) * qCos(phiB) * qSin(dLambda / 2.0) * qSin(dLambda / 2.0);
    return earth * 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));
}

void FlightStatsManager::ingestTelemetry()
{
    if (!m_telemetry || !m_telemetry->connected()) {
        if (!m_active) {
            setStatus(QStringLiteral("Waiting for aircraft connection"));
        }
        return;
    }
    if (!m_active && m_telemetry->inAir()) {
        startSession();
    }
    if (m_active && !m_telemetry->inAir()) {
        endSession();
        return;
    }
    if (!m_active) {
        setStatus(m_telemetry->armed()
                      ? QStringLiteral("Armed: waiting for takeoff")
                      : QStringLiteral("Connected: flight timer standby"));
        return;
    }
    const double lat = m_telemetry->latitude();
    const double lon = m_telemetry->longitude();
    if (qAbs(lat) < 0.000001 || qAbs(lon) < 0.000001) {
        return;
    }
    if (qAbs(m_lastLatitude) > 0.000001 && qAbs(m_lastLongitude) > 0.000001) {
        const double step = distanceBetween(m_lastLatitude, m_lastLongitude, lat, lon);
        if (step < 200.0) {
            m_distanceMeters += step;
        }
    }
    m_lastLatitude = lat;
    m_lastLongitude = lon;
    m_maxAltitude = qMax(m_maxAltitude, m_telemetry->altitude());
    m_batteryEndPercent = m_telemetry->battery();
    if (m_flightPath.isEmpty() || m_flightPath.size() % 2 == 0) {
        m_flightPath << QVariantMap{
            {QStringLiteral("latitude"), lat},
            {QStringLiteral("longitude"), lon},
            {QStringLiteral("altitude_m"), m_telemetry->altitude()},
            {QStringLiteral("recorded_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
        };
        while (m_flightPath.size() > 900) {
            m_flightPath.removeFirst();
        }
    }
    emit statsChanged();
}

void FlightStatsManager::tick()
{
    if (!m_active || m_startedMsec <= 0) {
        return;
    }
    m_flightDurationSeconds = static_cast<int>((QDateTime::currentMSecsSinceEpoch() - m_startedMsec) / 1000);
    emit statsChanged();
}

void FlightStatsManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit statsChanged();
}
