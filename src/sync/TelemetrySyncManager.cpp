#include "TelemetrySyncManager.h"

#include "../auth/SessionManager.h"
#include "../cache/LocalSyncCache.h"
#include "../controllers/AppState.h"
#include "../flight/FlightStatsManager.h"
#include "FlightSessionSyncManager.h"
#include "../flight/ManualControlManager.h"
#include "../models/MissionPlanModel.h"
#include "../network/ApiClient.h"
#include "../vehicle/HomePositionManager.h"
#include "../vehicle/VehicleTelemetryModel.h"
#include "../network/WebSocketClient.h"
#include "../telemetry/WindTelemetryManager.h"
#include "MissionSyncManager.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QtMath>

#include <cmath>

TelemetrySyncManager::TelemetrySyncManager(ApiClient *api,
                                           SessionManager *session,
                                           MissionSyncManager *missionSync,
                                           VehicleTelemetryModel *telemetry,
                                           MissionPlanModel *plan,
                                           AppState *appState,
                                           ManualControlManager *manualControl,
                                           WindTelemetryManager *wind,
                                           FlightStatsManager *flightStats,
                                           HomePositionManager *homePosition,
                                           FlightSessionSyncManager *flightSessions,
                                           WebSocketClient *websocket,
                                           LocalSyncCache *cache,
                                           QObject *parent)
    : QObject(parent),
      m_api(api),
      m_session(session),
      m_missionSync(missionSync),
      m_telemetry(telemetry),
      m_plan(plan),
      m_appState(appState),
      m_manualControl(manualControl),
      m_wind(wind),
      m_flightStats(flightStats),
      m_homePosition(homePosition),
      m_flightSessions(flightSessions),
      m_websocket(websocket),
      m_cache(cache)
{
    connect(&m_timer, &QTimer::timeout, this, &TelemetrySyncManager::uploadNow);
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    m_uploadIntervalMs = env.value(QStringLiteral("SKYGRID_TELEMETRY_UPLOAD_INTERVAL_MS"),
                                   env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true")
                                       ? QStringLiteral("1500")
                                       : QStringLiteral("1500")).toInt();
    m_uploadIntervalMs = qBound(500, m_uploadIntervalMs, 10000);
    m_queueDrainIntervalMs = env.value(QStringLiteral("SKYGRID_QUEUE_DRAIN_INTERVAL_MS"),
                                       env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true")
                                           ? QStringLiteral("5000")
                                           : QStringLiteral("3000")).toInt();
    m_queueDrainIntervalMs = qBound(2000, m_queueDrainIntervalMs, 30000);
    m_timer.setInterval(m_uploadIntervalMs);
}

bool TelemetrySyncManager::uploading() const { return m_uploading; }
bool TelemetrySyncManager::gazeboMode() const { return m_gazeboMode; }
QString TelemetrySyncManager::status() const { return m_status; }
QString TelemetrySyncManager::syncState() const { return m_syncState; }
QString TelemetrySyncManager::lastAckAt() const { return m_lastAckAt; }
int TelemetrySyncManager::pendingQueueCount() const { return m_pendingQueueCount; }

void TelemetrySyncManager::setGazeboMode(bool enabled)
{
    if (m_gazeboMode == enabled) {
        return;
    }
    m_gazeboMode = enabled;
    emit telemetrySyncChanged();
}

void TelemetrySyncManager::start()
{
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Telemetry blocked: %1").arg(m_session ? m_session->blockReason() : QStringLiteral("session unavailable")));
        return;
    }
    if (!m_timer.isActive()) {
        m_timer.start();
    }
    if (m_websocket) {
        m_websocket->connectLiveStreams();
    }
    setSyncState(QStringLiteral("online"));
    drainQueuedSync();
    setStatus(QStringLiteral("Operational telemetry stream active."));
}

void TelemetrySyncManager::stop()
{
    m_timer.stop();
    if (m_websocket) {
        m_websocket->disconnectLiveStreams();
    }
    setSyncState(QStringLiteral("offline"));
    setStatus(QStringLiteral("Telemetry upload stopped."));
}

void TelemetrySyncManager::uploadNow()
{
    if (!m_telemetry || !m_telemetry->connected()) {
        setStatus(QStringLiteral("Telemetry idle: no connected aircraft."));
        return;
    }
    if (m_uploading) {
        setStatus(QStringLiteral("Telemetry upload already in flight."));
        return;
    }
    const QDateTime now = QDateTime::currentDateTimeUtc();
    if (m_lastUploadAt.isValid() && m_lastUploadAt.msecsTo(now) < m_uploadIntervalMs) {
        return;
    }
    if (m_nextRetryAt.isValid() && now < m_nextRetryAt) {
        setSyncState(QStringLiteral("pending"));
        setStatus(QStringLiteral("Telemetry retry backoff active."));
        return;
    }
    const QJsonObject payload = telemetryPayload();
    if (!meaningfulTelemetryChanged(payload)
        && m_lastUploadAt.isValid()
        && m_lastUploadAt.msecsTo(now) < 5000) {
        setStatus(QStringLiteral("Telemetry unchanged; upload skipped."));
        return;
    }
    if (m_cache) {
        m_cache->cacheTelemetry(payload.toVariantMap());
    }
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        if (m_cache) {
            m_cache->enqueueSync(QStringLiteral("telemetry"), payload.toVariantMap());
        }
        setSyncState(QStringLiteral("pending"));
        setStatus(QStringLiteral("Telemetry blocked: %1").arg(m_session ? m_session->blockReason() : QStringLiteral("session unavailable")));
        return;
    }
    setUploading(true);
    setSyncState(QStringLiteral("syncing"));
    m_api->post(QStringLiteral("/api/telemetry/"), payload, true, true,
                [this, payload](int statusCode, const QJsonObject &, const QString &error) {
        setUploading(false);
        if (statusCode < 200 || statusCode >= 300) {
            if (m_cache) {
                m_cache->enqueueSync(QStringLiteral("telemetry"), payload.toVariantMap());
            }
            m_retryBackoffMs = qMin(m_retryBackoffMs * 2, 30000);
            m_nextRetryAt = QDateTime::currentDateTimeUtc().addMSecs(m_retryBackoffMs);
            setSyncState(QStringLiteral("pending"));
            setStatus(error.isEmpty() ? QStringLiteral("Telemetry upload failed.") : error);
            return;
        }
        m_lastAckAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
        m_lastUploadAt = QDateTime::currentDateTimeUtc();
        m_lastSentState = stateForComparison(payload);
        m_retryBackoffMs = 1000;
        m_nextRetryAt = QDateTime();
        setSyncState(QStringLiteral("online"));
        setStatus(QStringLiteral("Telemetry acknowledged by Control Center."));
        drainQueuedSync();
        emit telemetrySyncChanged();
    });
}

QJsonObject TelemetrySyncManager::telemetryPayload() const
{
    const QVariantMap organization = m_missionSync ? m_missionSync->organization() : QVariantMap{};
    const QVariantMap poi = m_plan ? m_plan->poi() : QVariantMap{};
    const bool pilotMode = m_appState && m_appState->operationalMode() == QStringLiteral("pilot");
    const QString operationMode = pilotMode ? QStringLiteral("PILOT") : QStringLiteral("MISSION");

    const QString clientSessionId = m_flightSessions && !m_flightSessions->clientSessionId().isEmpty()
        ? m_flightSessions->clientSessionId()
        : (m_flightStats ? m_flightStats->flightSessionId() : QString());
    const QJsonValue serverSessionId = m_flightSessions && !m_flightSessions->serverSessionId().isEmpty()
        ? backendIdValue(m_flightSessions->serverSessionId())
        : QJsonValue(QJsonValue::Null);

    return {
        {QStringLiteral("flight_session_id"), nullableString(clientSessionId)},
        {QStringLiteral("flight_session"), serverSessionId},
        {QStringLiteral("organization"), organization.value(QStringLiteral("id")).toInt()},
        {QStringLiteral("aircraft"), selectedAircraftId()},
        {QStringLiteral("mission"), pilotMode ? QJsonValue(QJsonValue::Null) : backendIdValue(m_plan ? m_plan->missionId() : QString())},
        {QStringLiteral("operation_mode"), operationMode},
        {QStringLiteral("latitude"), roundNumber(m_telemetry ? m_telemetry->latitude() : poi.value(QStringLiteral("latitude")).toDouble(), 7)},
        {QStringLiteral("longitude"), roundNumber(m_telemetry ? m_telemetry->longitude() : poi.value(QStringLiteral("longitude")).toDouble(), 7)},
        {QStringLiteral("altitude_m"), roundNumber(m_telemetry ? m_telemetry->altitude() : (m_plan ? m_plan->altitude() : 0.0), 2)},
        {QStringLiteral("relative_altitude_m"), roundNumber(m_telemetry ? m_telemetry->altitude() : (m_plan ? m_plan->altitude() : 0.0), 2)},
        {QStringLiteral("ground_speed_mps"), roundNumber(m_telemetry ? m_telemetry->speed() : 0.0, 2)},
        {QStringLiteral("vertical_speed_mps"), roundNumber(m_telemetry ? m_telemetry->verticalSpeed() : 0.0, 2)},
        {QStringLiteral("heading"), roundNumber(m_telemetry ? m_telemetry->heading() : 0.0, 2)},
        {QStringLiteral("battery_percent"), roundNumber(m_telemetry ? m_telemetry->battery() : 0.0, 2)},
        {QStringLiteral("battery_voltage"), m_telemetry && m_telemetry->batteryVoltage() > 0.0
             ? QJsonValue(roundNumber(m_telemetry->batteryVoltage(), 2))
             : QJsonValue(QJsonValue::Null)},
        {QStringLiteral("rc_signal"), roundNumber(m_telemetry ? m_telemetry->rcSignal() : 0.0, 2)},
        {QStringLiteral("telemetry_signal"), roundNumber(m_telemetry ? m_telemetry->transmission() : 0.0, 2)},
        {QStringLiteral("gps_fix"), m_telemetry ? m_telemetry->gpsMode() : QString()},
        {QStringLiteral("satellites"), m_telemetry ? m_telemetry->satellites() : 0},
        {QStringLiteral("hdop"), roundNumber(m_telemetry ? m_telemetry->hdop() : 0.0, 2)},
        {QStringLiteral("armed"), m_telemetry ? m_telemetry->armed() : false},
        {QStringLiteral("flight_mode"), m_telemetry ? m_telemetry->flightMode() : QString()},
        {QStringLiteral("mode"), operationMode},
        {QStringLiteral("distance_from_home_m"), roundNumber(distanceFromHomeMeters(), 2)},
        {QStringLiteral("distance_travelled_m"), roundNumber(m_flightStats ? m_flightStats->distanceMeters() : 0.0, 2)},
        {QStringLiteral("flight_duration_seconds"), m_flightStats ? m_flightStats->flightDurationSeconds() : 0},
        {QStringLiteral("wind"), windPayload()},
        {QStringLiteral("camera"), QJsonObject{
            {QStringLiteral("live"), m_telemetry ? m_telemetry->connected() : false},
            {QStringLiteral("resolution"), QStringLiteral("1080p")},
            {QStringLiteral("framerate"), 30},
            {QStringLiteral("recording_state"), m_telemetry ? m_telemetry->recordingState() : QStringLiteral("NO CAM")},
            {QStringLiteral("recording"), m_telemetry ? m_telemetry->recording() : false},
            {QStringLiteral("fpv_url"), QString()},
            {QStringLiteral("camera_health"), m_telemetry && m_telemetry->connected() ? QStringLiteral("online") : QStringLiteral("offline")},
            {QStringLiteral("latency_ms"), m_telemetry ? m_telemetry->latency() : 0},
            {QStringLiteral("dropped_frames"), 0}
        }},
        {QStringLiteral("mission_metrics"), missionMetricsPayload()},
        {QStringLiteral("manual_control"), manualControlPayload()},
        {QStringLiteral("alert_code"), QString()},
        {QStringLiteral("payload"), QJsonObject{
            {QStringLiteral("source"), m_gazeboMode ? QStringLiteral("gazebo") : QStringLiteral("mavsdk")},
            {QStringLiteral("armed"), m_telemetry ? m_telemetry->armed() : false},
            {QStringLiteral("heading"), roundNumber(m_telemetry ? m_telemetry->heading() : 0.0, 2)},
            {QStringLiteral("satellites"), m_telemetry ? m_telemetry->satellites() : 0},
            {QStringLiteral("hdop"), roundNumber(m_telemetry ? m_telemetry->hdop() : 0.0, 2)},
            {QStringLiteral("in_air"), m_telemetry ? m_telemetry->inAir() : false},
            {QStringLiteral("roll_deg"), roundNumber(m_telemetry ? m_telemetry->roll() : 0.0, 2)},
            {QStringLiteral("pitch_deg"), roundNumber(m_telemetry ? m_telemetry->pitch() : 0.0, 2)},
            {QStringLiteral("yaw_deg"), roundNumber(m_telemetry ? m_telemetry->yaw() : 0.0, 2)},
            {QStringLiteral("ui_view_mode"), m_appState ? m_appState->operationalMode() : QString()},
            {QStringLiteral("sync_state"), m_syncState}
        }},
        {QStringLiteral("recorded_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
    };
}

QJsonObject TelemetrySyncManager::missionMetricsPayload() const
{
    if (!m_plan) {
        return {};
    }
    return {
        {QStringLiteral("mission_id"), backendIdValue(m_plan->missionId())},
        {QStringLiteral("mission_type"), m_plan->missionType()},
        {QStringLiteral("waypoint_count"), m_plan->waypoints().size()},
        {QStringLiteral("polygon_vertex_count"), m_plan->polygon().size()},
        {QStringLiteral("has_poi"), m_plan->hasPoi()},
        {QStringLiteral("route_item_count"), m_plan->generatedRoute().size()},
        {QStringLiteral("route_distance_km"), roundNumber(m_plan->routeDistanceKm(), 2)},
        {QStringLiteral("estimated_time"), m_plan->estimatedTime()},
        {QStringLiteral("estimated_battery"), roundNumber(m_plan->estimatedBattery(), 2)},
        {QStringLiteral("area_ha"), roundNumber(m_plan->missionAreaHa(), 2)},
        {QStringLiteral("front_overlap"), roundNumber(m_plan->frontOverlap(), 2)},
        {QStringLiteral("side_overlap"), roundNumber(m_plan->sideOverlap(), 2)},
        {QStringLiteral("altitude_m"), roundNumber(m_plan->altitude(), 2)},
        {QStringLiteral("speed_mps"), roundNumber(m_plan->speed(), 2)},
        {QStringLiteral("readiness_percent"), m_plan->missionReadiness()},
        {QStringLiteral("upload_state"), m_plan->uploadState()},
        {QStringLiteral("execution_state"), m_plan->executionState()},
        {QStringLiteral("active_waypoint"), m_plan->activeWaypointIndex()},
        {QStringLiteral("progress_percent"), m_plan->progressPercent()}
    };
}

bool TelemetrySyncManager::meaningfulTelemetryChanged(const QJsonObject &payload) const
{
    if (m_lastSentState.isEmpty()) {
        return true;
    }
    return QJsonDocument(stateForComparison(payload)).toJson(QJsonDocument::Compact)
        != QJsonDocument(m_lastSentState).toJson(QJsonDocument::Compact);
}

QJsonObject TelemetrySyncManager::stateForComparison(const QJsonObject &payload) const
{
    const QJsonObject metrics = payload.value(QStringLiteral("mission_metrics")).toObject();
    const QJsonObject manual = payload.value(QStringLiteral("manual_control")).toObject();
    return {
        {QStringLiteral("latitude"), payload.value(QStringLiteral("latitude"))},
        {QStringLiteral("longitude"), payload.value(QStringLiteral("longitude"))},
        {QStringLiteral("altitude_m"), payload.value(QStringLiteral("altitude_m"))},
        {QStringLiteral("ground_speed_mps"), payload.value(QStringLiteral("ground_speed_mps"))},
        {QStringLiteral("vertical_speed_mps"), payload.value(QStringLiteral("vertical_speed_mps"))},
        {QStringLiteral("heading"), payload.value(QStringLiteral("heading"))},
        {QStringLiteral("battery_percent"), payload.value(QStringLiteral("battery_percent"))},
        {QStringLiteral("battery_voltage"), payload.value(QStringLiteral("battery_voltage"))},
        {QStringLiteral("gps_fix"), payload.value(QStringLiteral("gps_fix"))},
        {QStringLiteral("satellites"), payload.value(QStringLiteral("satellites"))},
        {QStringLiteral("armed"), payload.value(QStringLiteral("armed"))},
        {QStringLiteral("flight_mode"), payload.value(QStringLiteral("flight_mode"))},
        {QStringLiteral("operation_mode"), payload.value(QStringLiteral("operation_mode"))},
        {QStringLiteral("active_waypoint"), metrics.value(QStringLiteral("active_waypoint"))},
        {QStringLiteral("progress_percent"), metrics.value(QStringLiteral("progress_percent"))},
        {QStringLiteral("upload_state"), metrics.value(QStringLiteral("upload_state"))},
        {QStringLiteral("execution_state"), metrics.value(QStringLiteral("execution_state"))},
        {QStringLiteral("manual_enabled"), manual.value(QStringLiteral("enabled"))},
        {QStringLiteral("manual_forward"), manual.value(QStringLiteral("forward"))},
        {QStringLiteral("manual_lateral"), manual.value(QStringLiteral("lateral"))},
        {QStringLiteral("manual_vertical"), manual.value(QStringLiteral("vertical"))},
        {QStringLiteral("manual_yaw"), manual.value(QStringLiteral("yaw"))}
    };
}

QJsonObject TelemetrySyncManager::manualControlPayload() const
{
    if (!m_manualControl) {
        return {};
    }
    return {
        {QStringLiteral("enabled"), m_manualControl->active()},
        {QStringLiteral("forward"), roundNumber(m_manualControl->forward(), 2)},
        {QStringLiteral("lateral"), roundNumber(m_manualControl->lateral(), 2)},
        {QStringLiteral("vertical"), roundNumber(m_manualControl->vertical(), 2)},
        {QStringLiteral("yaw"), roundNumber(m_manualControl->yaw(), 2)},
        {QStringLiteral("status"), m_manualControl->status()}
    };
}

QJsonObject TelemetrySyncManager::windPayload() const
{
    return {
        {QStringLiteral("available"), m_wind ? m_wind->available() : false},
        {QStringLiteral("speed_mps"), roundNumber(m_wind ? m_wind->windSpeed() : 0.0, 2)},
        {QStringLiteral("direction_deg"), roundNumber(m_wind ? m_wind->windDirection() : 0.0, 2)},
        {QStringLiteral("direction_label"), m_wind ? m_wind->directionLabel() : QStringLiteral("--")},
        {QStringLiteral("gust_mps"), roundNumber(m_wind ? m_wind->gust() : 0.0, 2)},
        {QStringLiteral("status"), m_wind ? m_wind->status() : QStringLiteral("Wind unavailable")}
    };
}

QJsonValue TelemetrySyncManager::nullableString(const QString &value) const
{
    const QString trimmed = value.trimmed();
    if (trimmed.isEmpty()) {
        return QJsonValue(QJsonValue::Null);
    }
    return trimmed;
}

QJsonValue TelemetrySyncManager::backendIdValue(const QString &value) const
{
    const QString trimmed = value.trimmed();
    if (trimmed.isEmpty()) {
        return QJsonValue(QJsonValue::Null);
    }
    bool ok = false;
    const int numericId = trimmed.toInt(&ok);
    return ok && numericId > 0 ? QJsonValue(numericId) : QJsonValue(QJsonValue::Null);
}

int TelemetrySyncManager::selectedAircraftId() const
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
    QVariant id = aircraftMap.contains(QStringLiteral("id"))
        ? aircraftMap.value(QStringLiteral("id"))
        : aircraftMap.value(QStringLiteral("aircraft_id"));
    if (id.toInt() == 0 && aircraftMap.value(QStringLiteral("aircraft")).canConvert<QVariantMap>()) {
        const QVariantMap nestedAircraft = aircraftMap.value(QStringLiteral("aircraft")).toMap();
        id = nestedAircraft.contains(QStringLiteral("id"))
            ? nestedAircraft.value(QStringLiteral("id"))
            : nestedAircraft.value(QStringLiteral("aircraft_id"));
    }
    return id.toInt();
}

double TelemetrySyncManager::roundNumber(double value, int decimals) const
{
    const double factor = std::pow(10.0, decimals);
    return std::round(value * factor) / factor;
}

double TelemetrySyncManager::distanceFromHomeMeters() const
{
    if (!m_telemetry || !m_telemetry->connected()) {
        return 0.0;
    }
    if (!m_homePosition || !m_homePosition->hasHome()) {
        return 0.0;
    }
    return m_homePosition->distanceFromHomeMeters(m_telemetry->latitude(), m_telemetry->longitude());
}

void TelemetrySyncManager::drainQueuedSync()
{
    if (!m_cache || !m_api || !m_session || !m_session->operationsAllowed()) {
        return;
    }
    const QDateTime now = QDateTime::currentDateTimeUtc();
    if (m_lastQueueDrainAt.isValid() && m_lastQueueDrainAt.msecsTo(now) < m_queueDrainIntervalMs) {
        return;
    }
    m_lastQueueDrainAt = now;

    const QVariantList items = m_cache->queuedSyncItems(20);
    setPendingQueueCount(items.size());
    int posted = 0;
    bool telemetryPosted = false;
    for (const QVariant &entry : items) {
        const QVariantMap item = entry.toMap();
        const QString kind = item.value(QStringLiteral("kind")).toString();
        if (kind == QStringLiteral("telemetry")) {
            if (telemetryPosted) {
                continue;
            }
            telemetryPosted = true;
        }
        postQueuedItem(item);
        ++posted;
        if (posted >= 5) {
            break;
        }
    }
    if (!m_websocket || !m_websocket->connected()) {
        pollOutboundMessages();
    }
}

void TelemetrySyncManager::postQueuedItem(const QVariantMap &item)
{
    const int id = item.value(QStringLiteral("id")).toInt();
    const QString kind = item.value(QStringLiteral("kind")).toString();
    QJsonObject payload = QJsonObject::fromVariantMap(item.value(QStringLiteral("payload")).toMap());
    QString endpoint;
    if (kind == QStringLiteral("telemetry")) {
        const QJsonValue missionValue = payload.value(QStringLiteral("mission"));
        if (missionValue.isString()) {
            payload.insert(QStringLiteral("mission"), backendIdValue(missionValue.toString()));
        }
        QJsonObject metrics = payload.value(QStringLiteral("mission_metrics")).toObject();
        if (metrics.value(QStringLiteral("mission_id")).isString()) {
            metrics.insert(QStringLiteral("mission_id"), backendIdValue(metrics.value(QStringLiteral("mission_id")).toString()));
            payload.insert(QStringLiteral("mission_metrics"), metrics);
        }
        endpoint = QStringLiteral("/api/telemetry/");
    } else if (kind == QStringLiteral("gcs_event")) {
        endpoint = QStringLiteral("/api/gcs-events/");
    } else if (kind == QStringLiteral("pilot_action") || kind == QStringLiteral("flight_record")) {
        payload = QJsonObject{
            {QStringLiteral("event_type"), kind},
            {QStringLiteral("severity"), QStringLiteral("info")},
            {QStringLiteral("message"), kind == QStringLiteral("flight_record")
                 ? QStringLiteral("Flight record summary queued from GCS")
                 : QStringLiteral("Pilot action queued from GCS")},
            {QStringLiteral("payload"), payload},
            {QStringLiteral("recorded_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
        };
        endpoint = QStringLiteral("/api/gcs-events/");
    } else {
        return;
    }
    m_api->post(endpoint, payload, true, true, [this, id](int statusCode, const QJsonObject &, const QString &error) {
        if (!m_cache) {
            return;
        }
        if (statusCode >= 200 && statusCode < 300) {
            m_cache->markSyncDone(id);
            setPendingQueueCount(qMax(0, m_pendingQueueCount - 1));
        } else {
            m_cache->markSyncFailed(id, error);
            setSyncState(QStringLiteral("pending"));
        }
    });
}

void TelemetrySyncManager::pollOutboundMessages()
{
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        return;
    }
    m_api->get(QStringLiteral("/api/operations/pending-sync/"), true, true,
               [this](int statusCode, const QJsonObject &body, const QString &) {
        if (statusCode < 200 || statusCode >= 300) {
            return;
        }
        QJsonArray messages;
        if (body.contains(QStringLiteral("results")) && body.value(QStringLiteral("results")).isArray()) {
            messages = body.value(QStringLiteral("results")).toArray();
        } else if (body.value(QStringLiteral("messages")).isArray()) {
            messages = body.value(QStringLiteral("messages")).toArray();
        }
        if (messages.isEmpty() && body.isEmpty()) {
            return;
        }
        if (messages.isEmpty()) {
            return;
        }
        QJsonArray ids;
        for (const QJsonValue &value : messages) {
            const QJsonObject message = value.toObject();
            const int id = message.value(QStringLiteral("id")).toInt();
            if (id > 0) {
                ids.append(id);
            }
        }
        if (ids.isEmpty()) {
            return;
        }
        m_api->post(QStringLiteral("/api/operations/ack-sync/"),
                    QJsonObject{{QStringLiteral("ids"), ids}},
                    true,
                    true,
                    [](int, const QJsonObject &, const QString &) {});
    });
}

void TelemetrySyncManager::setUploading(bool uploading)
{
    if (m_uploading == uploading) {
        return;
    }
    m_uploading = uploading;
    emit telemetrySyncChanged();
}

void TelemetrySyncManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit telemetrySyncChanged();
}

void TelemetrySyncManager::setSyncState(const QString &state)
{
    if (m_syncState == state) {
        return;
    }
    m_syncState = state;
    emit telemetrySyncChanged();
}

void TelemetrySyncManager::setPendingQueueCount(int count)
{
    if (m_pendingQueueCount == count) {
        return;
    }
    m_pendingQueueCount = count;
    emit telemetrySyncChanged();
}
