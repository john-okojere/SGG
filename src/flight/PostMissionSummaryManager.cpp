#include "PostMissionSummaryManager.h"

#include "../auth/SessionManager.h"
#include "../cache/LocalSyncCache.h"
#include "../models/MissionPlanModel.h"
#include "../network/ApiClient.h"
#include "../profile/ProfileManager.h"
#include "../sync/FlightSessionSyncManager.h"
#include "../sync/MissionSyncManager.h"
#include "../sync/TelemetrySyncManager.h"
#include "../vehicle/VehicleTelemetryModel.h"
#include "FlightStatsManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QStandardPaths>
#include <QUuid>
#include <QtGlobal>

namespace {

QString stringValue(const QVariantMap &map, const QString &key, const QString &fallback = QString())
{
    const QString value = map.value(key).toString().trimmed();
    return value.isEmpty() ? fallback : value;
}

double doubleValue(const QVariantMap &map, const QString &key, double fallback = 0.0)
{
    bool ok = false;
    const double value = map.value(key).toDouble(&ok);
    return ok ? value : fallback;
}

QString formatSeconds(int seconds)
{
    const int hours = seconds / 3600;
    const int minutes = (seconds % 3600) / 60;
    const int secs = seconds % 60;
    return QStringLiteral("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(secs, 2, 10, QLatin1Char('0'));
}

} // namespace

PostMissionSummaryManager::PostMissionSummaryManager(ApiClient *api,
                                                     SessionManager *session,
                                                     LocalSyncCache *cache,
                                                     MissionPlanModel *plan,
                                                     FlightStatsManager *flightStats,
                                                     FlightSessionSyncManager *flightSessions,
                                                     TelemetrySyncManager *telemetrySync,
                                                     MissionSyncManager *missionSync,
                                                     ProfileManager *profile,
                                                     VehicleTelemetryModel *telemetry,
                                                     QObject *parent)
    : QObject(parent),
      m_api(api),
      m_session(session),
      m_cache(cache),
      m_plan(plan),
      m_flightStats(flightStats),
      m_flightSessions(flightSessions),
      m_telemetrySync(telemetrySync),
      m_missionSync(missionSync),
      m_profile(profile),
      m_telemetry(telemetry)
{
    if (m_flightStats) {
        connect(m_flightStats, &FlightStatsManager::flightEnded,
                this, &PostMissionSummaryManager::handleFlightEnded);
    }
}

bool PostMissionSummaryManager::visible() const { return m_visible; }
QString PostMissionSummaryManager::status() const { return m_status; }
QString PostMissionSummaryManager::syncStatus() const { return m_syncStatus; }
QVariantMap PostMissionSummaryManager::lastSummary() const { return m_lastSummary; }
bool PostMissionSummaryManager::fullLogVisible() const { return m_fullLogVisible; }

void PostMissionSummaryManager::setFullLogVisible(bool visible)
{
    if (m_fullLogVisible == visible) {
        return;
    }
    m_fullLogVisible = visible;
    emit summaryChanged();
}

void PostMissionSummaryManager::dismiss()
{
    m_visible = false;
    m_fullLogVisible = false;
    emit summaryChanged();
}

void PostMissionSummaryManager::syncNow()
{
    if (m_lastSummary.isEmpty()) {
        return;
    }
    if (m_telemetrySync) {
        m_telemetrySync->uploadNow();
    }
    syncSummary(m_lastSummary);
}

void PostMissionSummaryManager::exportReport()
{
    if (m_lastSummary.isEmpty()) {
        return;
    }
    QVariantMap copy = m_lastSummary;
    writeReportFiles(&copy);
    m_lastSummary = copy;
    saveLocalSummary(m_lastSummary);
    setStatus(QStringLiteral("Report exported locally."));
    emit summaryChanged();
}

void PostMissionSummaryManager::viewFullLog()
{
    setFullLogVisible(true);
}

void PostMissionSummaryManager::returnToDashboard()
{
    dismiss();
    emit returnToDashboardRequested();
}

void PostMissionSummaryManager::startNewMission()
{
    dismiss();
    emit startNewMissionRequested();
}

void PostMissionSummaryManager::handleMissionFinished(const QVariantMap &result)
{
    const QString resultStatus = stringValue(result, QStringLiteral("result_status"), QStringLiteral("failed"));
    const QString reason = stringValue(result, QStringLiteral("reason"));
    publishSummary(buildSummary(QStringLiteral("mission"), resultStatus, reason, result));
}

void PostMissionSummaryManager::handleFlightEnded(const QVariantMap &record)
{
    const QString sessionId = stringValue(record, QStringLiteral("server_session_id"),
                                          stringValue(record, QStringLiteral("flight_session_id")));
    const QString currentSession = stringValue(m_lastSummary, QStringLiteral("server_session_id"),
                                               stringValue(m_lastSummary, QStringLiteral("flight_session_id")));
    if (m_visible && !sessionId.isEmpty() && sessionId == currentSession) {
        return;
    }
    const QString mode = stringValue(record, QStringLiteral("operation_mode"), QStringLiteral("PILOT"));
    if (mode == QStringLiteral("MISSION") && m_plan && m_plan->executionState() == QStringLiteral("completed")) {
        return;
    }
    publishSummary(buildSummary(mode == QStringLiteral("MISSION") ? QStringLiteral("mission") : QStringLiteral("manual"),
                                QStringLiteral("completed"),
                                QStringLiteral("Manual flight ended"),
                                record));
}

QVariantMap PostMissionSummaryManager::buildSummary(const QString &kind,
                                                    const QString &resultStatus,
                                                    const QString &reason,
                                                    const QVariantMap &source) const
{
    const QString summaryId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QString endedAt = stringValue(source, QStringLiteral("ended_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    const int duration = source.contains(QStringLiteral("duration_seconds"))
        ? source.value(QStringLiteral("duration_seconds")).toInt()
        : (m_flightStats ? m_flightStats->flightDurationSeconds() : 0);
    const double distance = source.contains(QStringLiteral("distance_m"))
        ? doubleValue(source, QStringLiteral("distance_m"))
        : (m_flightStats ? m_flightStats->distanceMeters() : 0.0);
    const double batteryStart = source.contains(QStringLiteral("battery_start_percent"))
        ? doubleValue(source, QStringLiteral("battery_start_percent"))
        : (m_flightStats ? m_flightStats->batteryStartPercent() : 0.0);
    const double batteryEnd = source.contains(QStringLiteral("battery_end_percent"))
        ? doubleValue(source, QStringLiteral("battery_end_percent"))
        : (m_flightStats ? m_flightStats->batteryEndPercent() : (m_telemetry ? m_telemetry->battery() : 0.0));
    const QVariantList route = m_plan ? m_plan->generatedRoute() : QVariantList{};
    const QVariantList path = source.value(QStringLiteral("flight_path")).toList().isEmpty() && m_flightStats
        ? m_flightStats->flightPath()
        : source.value(QStringLiteral("flight_path")).toList();
    QVariantMap aircraft;
    if (m_missionSync && !m_missionSync->assignedAircraft().isEmpty()) {
        aircraft = m_missionSync->assignedAircraft().first().toMap();
    }

    QVariantMap sync{
        {QStringLiteral("control_center"), m_session && m_session->operationsAllowed() ? QStringLiteral("Ready") : QStringLiteral("Pending Sync")},
        {QStringLiteral("telemetry"), m_telemetrySync ? m_telemetrySync->status() : QStringLiteral("Telemetry sync unavailable")},
        {QStringLiteral("telemetry_queue"), m_telemetrySync ? m_telemetrySync->pendingQueueCount() : 0},
        {QStringLiteral("flight_log"), QStringLiteral("Saved locally")},
        {QStringLiteral("preflight"), kind == QStringLiteral("mission") ? QStringLiteral("Recorded before start") : QStringLiteral("Manual flight")},
        {QStringLiteral("weather"), QStringLiteral("Latest mission weather state retained")}
    };

    return QVariantMap{
        {QStringLiteral("summary_id"), summaryId},
        {QStringLiteral("kind"), kind},
        {QStringLiteral("result_status"), resultStatus},
        {QStringLiteral("title"), kind == QStringLiteral("manual")
             ? QStringLiteral("Flight Summary")
             : (resultStatus == QStringLiteral("completed") ? QStringLiteral("Mission Completed") : QStringLiteral("Mission Failed"))},
        {QStringLiteral("reason"), reason},
        {QStringLiteral("mission_id"), stringValue(source, QStringLiteral("mission_id"), m_plan ? m_plan->missionId() : QString())},
        {QStringLiteral("mission_name"), stringValue(source, QStringLiteral("mission_name"), m_plan ? m_plan->name() : QStringLiteral("Manual Flight"))},
        {QStringLiteral("mission_type"), kind == QStringLiteral("manual") ? QStringLiteral("Manual Flight") : stringValue(source, QStringLiteral("mission_type"), m_plan ? m_plan->missionType() : QString())},
        {QStringLiteral("pilot_name"), m_profile ? m_profile->displayName() : QStringLiteral("Operator")},
        {QStringLiteral("aircraft_name"), aircraft.value(QStringLiteral("serial"), m_telemetry ? m_telemetry->aircraftId() : QStringLiteral("Aircraft")).toString()},
        {QStringLiteral("start_time"), stringValue(source, QStringLiteral("started_at"), m_flightStats ? m_flightStats->startedAt() : QString())},
        {QStringLiteral("end_time"), endedAt},
        {QStringLiteral("duration_seconds"), duration},
        {QStringLiteral("duration_text"), formatSeconds(duration)},
        {QStringLiteral("distance_m"), distance},
        {QStringLiteral("distance_km"), distance / 1000.0},
        {QStringLiteral("max_altitude_m"), source.contains(QStringLiteral("max_altitude_m")) ? doubleValue(source, QStringLiteral("max_altitude_m")) : (m_flightStats ? m_flightStats->maxAltitude() : 0.0)},
        {QStringLiteral("average_speed_mps"), source.contains(QStringLiteral("average_speed_mps")) ? doubleValue(source, QStringLiteral("average_speed_mps")) : (duration > 0 ? distance / duration : 0.0)},
        {QStringLiteral("battery_start_percent"), batteryStart},
        {QStringLiteral("battery_end_percent"), batteryEnd},
        {QStringLiteral("battery_used_percent"), qMax(0.0, batteryStart - batteryEnd)},
        {QStringLiteral("waypoints_completed"), source.value(QStringLiteral("active_waypoint"), m_plan ? m_plan->activeWaypointIndex() : -1).toInt()},
        {QStringLiteral("waypoint_count"), source.value(QStringLiteral("waypoint_count"), route.size()).toInt()},
        {QStringLiteral("photos_captured"), source.value(QStringLiteral("photos_captured"), m_plan ? m_plan->routeEstimates().value(QStringLiteral("estimated_photos")).toInt() : 0).toInt()},
        {QStringLiteral("warnings_count"), source.value(QStringLiteral("warnings_count"), 0).toInt()},
        {QStringLiteral("events_count"), source.value(QStringLiteral("events_count"), 0).toInt()},
        {QStringLiteral("route_preview"), route},
        {QStringLiteral("flight_path"), path},
        {QStringLiteral("sync"), sync},
        {QStringLiteral("sync_status"), sync.value(QStringLiteral("control_center"))},
        {QStringLiteral("flight_session_id"), stringValue(source, QStringLiteral("flight_session_id"), m_flightSessions ? m_flightSessions->clientSessionId() : QString())},
        {QStringLiteral("server_session_id"), stringValue(source, QStringLiteral("server_session_id"), m_flightSessions ? m_flightSessions->serverSessionId() : QString())},
        {QStringLiteral("created_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
    };
}

void PostMissionSummaryManager::publishSummary(QVariantMap summary)
{
    writeReportFiles(&summary);
    m_lastSummary = summary;
    m_visible = true;
    m_fullLogVisible = false;
    saveLocalSummary(m_lastSummary);
    setStatus(QStringLiteral("Post-flight summary saved locally."));
    syncSummary(m_lastSummary);
    emit summaryChanged();
}

void PostMissionSummaryManager::saveLocalSummary(const QVariantMap &summary)
{
    if (!m_cache) {
        return;
    }
    const QString id = stringValue(summary, QStringLiteral("summary_id"), QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyyMMddhhmmsszzz")));
    m_cache->saveObject(QStringLiteral("post_mission_summaries"), id, summary);
}

void PostMissionSummaryManager::syncSummary(const QVariantMap &summary)
{
    const QString serverSessionId = stringValue(summary, QStringLiteral("server_session_id"));
    if (!m_api || !m_session || !m_session->operationsAllowed() || serverSessionId.isEmpty()) {
        if (m_cache) {
            m_cache->enqueueSync(QStringLiteral("post_mission_summary"), summary);
        }
        setSyncStatus(QStringLiteral("Pending Sync"));
        return;
    }
    setSyncStatus(QStringLiteral("Syncing"));
    m_api->post(QStringLiteral("/api/flight-sessions/%1/summary/").arg(serverSessionId),
                QJsonObject::fromVariantMap(summary),
                true,
                true,
                [this](int statusCode, const QJsonObject &, const QString &error) {
        if (statusCode >= 200 && statusCode < 300) {
            setSyncStatus(QStringLiteral("Synced"));
            setStatus(QStringLiteral("Post-flight summary synced to Control Center."));
            return;
        }
        if (m_cache && !m_lastSummary.isEmpty()) {
            m_cache->enqueueSync(QStringLiteral("post_mission_summary"), m_lastSummary);
        }
        setSyncStatus(QStringLiteral("Pending Sync"));
        setStatus(error.isEmpty()
                      ? QStringLiteral("Summary sync queued.")
                      : QStringLiteral("Summary sync queued: %1").arg(error));
    });
}

QVariantMap PostMissionSummaryManager::reportFilePaths(const QString &summaryId) const
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (base.isEmpty()) {
        base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    if (base.isEmpty()) {
        base = QDir::tempPath();
    }
    QDir dir(base + QStringLiteral("/SkyGrid/reports"));
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }
    const QString id = summaryId.isEmpty() ? QStringLiteral("post-flight") : summaryId;
    return {
        {QStringLiteral("json"), dir.filePath(QStringLiteral("%1.json").arg(id))},
        {QStringLiteral("pdf"), dir.filePath(QStringLiteral("%1.pdf").arg(id))}
    };
}

void PostMissionSummaryManager::writeReportFiles(QVariantMap *summary)
{
    if (!summary || summary->isEmpty()) {
        return;
    }
    const QVariantMap paths = reportFilePaths(stringValue(*summary, QStringLiteral("summary_id")));
    QFile json(paths.value(QStringLiteral("json")).toString());
    if (json.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        json.write(QJsonDocument::fromVariant(*summary).toJson(QJsonDocument::Indented));
        json.close();
        summary->insert(QStringLiteral("json_report_path"), paths.value(QStringLiteral("json")));
    }

    QPdfWriter pdf(paths.value(QStringLiteral("pdf")).toString());
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setResolution(96);
    QPainter painter(&pdf);
    if (painter.isActive()) {
        painter.setPen(QColor(QStringLiteral("#2f006f")));
        QFont title = painter.font();
        title.setPointSize(22);
        title.setBold(true);
        painter.setFont(title);
        painter.drawText(QRect(48, 40, 700, 48), summary->value(QStringLiteral("title")).toString());

        QFont body = painter.font();
        body.setPointSize(11);
        body.setBold(false);
        painter.setFont(body);
        painter.setPen(QColor(QStringLiteral("#1f1f27")));
        const QStringList lines{
            QStringLiteral("Mission: %1").arg(summary->value(QStringLiteral("mission_name")).toString()),
            QStringLiteral("Type: %1").arg(summary->value(QStringLiteral("mission_type")).toString()),
            QStringLiteral("Pilot: %1").arg(summary->value(QStringLiteral("pilot_name")).toString()),
            QStringLiteral("Aircraft: %1").arg(summary->value(QStringLiteral("aircraft_name")).toString()),
            QStringLiteral("Start: %1").arg(summary->value(QStringLiteral("start_time")).toString()),
            QStringLiteral("End: %1").arg(summary->value(QStringLiteral("end_time")).toString()),
            QStringLiteral("Flight time: %1").arg(summary->value(QStringLiteral("duration_text")).toString()),
            QStringLiteral("Distance: %1 km").arg(summary->value(QStringLiteral("distance_km")).toDouble(), 0, 'f', 2),
            QStringLiteral("Max altitude: %1 m").arg(summary->value(QStringLiteral("max_altitude_m")).toDouble(), 0, 'f', 1),
            QStringLiteral("Average speed: %1 m/s").arg(summary->value(QStringLiteral("average_speed_mps")).toDouble(), 0, 'f', 1),
            QStringLiteral("Battery used: %1%").arg(summary->value(QStringLiteral("battery_used_percent")).toDouble(), 0, 'f', 1),
            QStringLiteral("Waypoints: %1 / %2").arg(summary->value(QStringLiteral("waypoints_completed")).toInt()).arg(summary->value(QStringLiteral("waypoint_count")).toInt()),
            QStringLiteral("Photos: %1").arg(summary->value(QStringLiteral("photos_captured")).toInt()),
            QStringLiteral("Sync: %1").arg(summary->value(QStringLiteral("sync_status")).toString())
        };
        int y = 110;
        for (const QString &line : lines) {
            painter.drawText(QRect(56, y, 680, 26), line);
            y += 30;
        }
        painter.end();
        summary->insert(QStringLiteral("pdf_report_path"), paths.value(QStringLiteral("pdf")));
    }
}

void PostMissionSummaryManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit summaryChanged();
}

void PostMissionSummaryManager::setSyncStatus(const QString &status)
{
    if (m_syncStatus == status) {
        return;
    }
    m_syncStatus = status;
    if (!m_lastSummary.isEmpty()) {
        m_lastSummary.insert(QStringLiteral("sync_status"), status);
        saveLocalSummary(m_lastSummary);
    }
    emit summaryChanged();
}
