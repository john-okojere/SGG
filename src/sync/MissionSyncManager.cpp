#include "MissionSyncManager.h"

#include "../auth/SessionManager.h"
#include "../cache/LocalSyncCache.h"
#include "../models/MissionPlanModel.h"
#include "../network/ApiClient.h"
#include "GcsEventSyncManager.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QMetaType>

namespace {

QString missionIdentifier(const QVariantMap &mission)
{
    const QVariant id = mission.value(QStringLiteral("id"));
    if (id.isValid() && !id.isNull()) {
        return id.toString();
    }
    return mission.value(QStringLiteral("local_id")).toString();
}

bool missionHasGeometry(const QVariantMap &mission)
{
    const QVariantMap geometry = mission.value(QStringLiteral("geometry")).toMap();
    if (geometry.isEmpty()) {
        return false;
    }
    if (!geometry.value(QStringLiteral("waypoints")).toList().isEmpty()) {
        return true;
    }
    if (!geometry.value(QStringLiteral("polygon")).toList().isEmpty()) {
        return true;
    }
    const QVariantMap poi = geometry.value(QStringLiteral("poi")).toMap();
    return poi.contains(QStringLiteral("latitude")) && poi.contains(QStringLiteral("longitude"));
}

QVariantMap normalizeMission(const QVariantMap &mission)
{
    QVariantMap normalized = mission;
    const QVariant aircraft = mission.value(QStringLiteral("assigned_aircraft"));
    if (aircraft.metaType().id() == QMetaType::Int || aircraft.metaType().id() == QMetaType::UInt
        || aircraft.metaType().id() == QMetaType::LongLong || aircraft.metaType().id() == QMetaType::Double) {
        normalized[QStringLiteral("assigned_aircraft")] = QString::number(aircraft.toInt());
    } else if (aircraft.metaType().id() == QMetaType::QVariantMap) {
        normalized[QStringLiteral("assigned_aircraft")] = aircraft.toMap().value(QStringLiteral("id")).toString();
    }
    return normalized;
}

} // namespace

MissionSyncManager::MissionSyncManager(ApiClient *api,
                                       SessionManager *session,
                                       LocalSyncCache *cache,
                                       MissionPlanModel *plan,
                                       GcsEventSyncManager *events,
                                       QObject *parent)
    : QObject(parent), m_api(api), m_session(session), m_cache(cache), m_plan(plan), m_events(events)
{
    loadCached();
}

void MissionSyncManager::saveActiveMission()
{
    if (!m_plan) {
        setStatus(QStringLiteral("No active mission to save."));
        emit activeMissionSaved(false, QStringLiteral("No active mission to save."));
        return;
    }
    QVariantMap mission = m_plan->serializeForBackend();
    mission.remove(QStringLiteral("local_id"));
    if (mission.value(QStringLiteral("id")).toString().isEmpty()) {
        mission.remove(QStringLiteral("id"));
    }
    if (!m_organization.isEmpty()) {
        mission[QStringLiteral("organization")] = m_organization.value(QStringLiteral("id"));
    }
    if (mission.value(QStringLiteral("assigned_aircraft")).toString().isEmpty() && !m_assignedAircraft.isEmpty()) {
        mission[QStringLiteral("assigned_aircraft")] = m_assignedAircraft.first().toMap().value(QStringLiteral("id"));
    }
    if (!m_pilotProfile.isEmpty()) {
        mission[QStringLiteral("assigned_pilot")] = m_pilotProfile.value(QStringLiteral("id"));
    }
    if (m_cache) {
        m_cache->saveObject(QStringLiteral("mission_drafts"), m_plan->missionId(), mission);
        m_cache->enqueueSync(QStringLiteral("mission"), mission);
    }
    m_plan->saveLocal();
    if (m_events) {
        m_events->recordEvent(QStringLiteral("mission_saved"),
                              QStringLiteral("info"),
                              QStringLiteral("Mission draft saved"),
                              QJsonObject::fromVariantMap(mission));
    }
    if (!m_session || !m_session->operationsAllowed() || !m_api) {
        setStatus(QStringLiteral("Device approval required before aircraft upload."));
        emit activeMissionSaved(false, QStringLiteral("Device approval required before aircraft upload."));
        return;
    }

    setSyncing(true);
    setStatus(QStringLiteral("Saving mission to Control Center."));
    const QJsonObject payload = QJsonObject::fromVariantMap(mission);
    if (m_plan->createdLocally()) {
        m_api->post(QStringLiteral("/api/missions/"), payload, true, true,
                    [this](int statusCode, const QJsonObject &body, const QString &error) {
            setSyncing(false);
            if (statusCode < 200 || statusCode >= 300) {
                const QString message = error.isEmpty() ? QStringLiteral("Mission could not sync with Control Center.") : error;
                setStatus(message);
                emit activeMissionSaved(false, message);
                return;
            }
            if (m_plan) {
                m_plan->loadMission(body.toVariantMap());
            }
            syncMissions();
            setStatus(QStringLiteral("Mission synchronized."));
            emit activeMissionSaved(true, QStringLiteral("Mission synchronized."));
        });
    } else {
        m_api->patch(QStringLiteral("/api/missions/%1/").arg(m_plan->missionId()), payload, true, true,
                     [this](int statusCode, const QJsonObject &body, const QString &error) {
            setSyncing(false);
            if (statusCode < 200 || statusCode >= 300) {
                const QString message = error.isEmpty() ? QStringLiteral("Mission update could not sync with Control Center.") : error;
                setStatus(message);
                emit activeMissionSaved(false, message);
                return;
            }
            if (m_plan) {
                m_plan->loadMission(body.toVariantMap());
            }
            syncMissions();
            setStatus(QStringLiteral("Mission synchronized."));
            emit activeMissionSaved(true, QStringLiteral("Mission synchronized."));
        });
    }
}

void MissionSyncManager::validateActiveMission()
{
    if (!m_plan) {
        setStatus(QStringLiteral("No active mission to validate."));
        emit activeMissionValidated(false, QStringLiteral("No active mission to validate."));
        return;
    }
    m_plan->validateMission();
    if (!m_session || !m_session->operationsAllowed() || !m_api) {
        setStatus(QStringLiteral("Device approval required before aircraft upload."));
        emit activeMissionValidated(false, QStringLiteral("Device approval required before aircraft upload."));
        return;
    }

    QVariantMap mission = m_plan->serializeForBackend();
    if (!m_organization.isEmpty()) {
        mission[QStringLiteral("organization")] = m_organization.value(QStringLiteral("id"));
    }
    if (mission.value(QStringLiteral("assigned_aircraft")).toString().isEmpty() && !m_assignedAircraft.isEmpty()) {
        mission[QStringLiteral("assigned_aircraft")] = m_assignedAircraft.first().toMap().value(QStringLiteral("id"));
    }
    if (!m_pilotProfile.isEmpty()) {
        mission[QStringLiteral("assigned_pilot")] = m_pilotProfile.value(QStringLiteral("id"));
    }

    setSyncing(true);
    setStatus(QStringLiteral("Validating mission with Control Center."));
    m_api->post(QStringLiteral("/api/missions/validate/"),
                QJsonObject::fromVariantMap(mission),
                true,
                true,
                [this](int statusCode, const QJsonObject &body, const QString &error) {
        setSyncing(false);
        if (statusCode < 200 || statusCode >= 300) {
            const QString message = error.isEmpty() ? QStringLiteral("Backend validation required.") : error;
            setStatus(message);
            emit activeMissionValidated(false, message);
            return;
        }
        if (m_plan) {
            m_plan->applyBackendValidation(body.toVariantMap());
        }
        const QString validationState = body.value(QStringLiteral("validation_state")).toString();
        const bool blocked = validationState == QStringLiteral("blocked")
            || !body.value(QStringLiteral("sync_ready")).toBool()
            || !body.value(QStringLiteral("upload_eligible")).toBool();
        QString message = body.value(QStringLiteral("message")).toString();
        if (message.isEmpty()) {
            const QJsonArray checks = body.value(QStringLiteral("checks")).toArray();
            for (const QJsonValue &entry : checks) {
                const QJsonObject check = entry.toObject();
                if (!check.value(QStringLiteral("passed")).toBool(true)) {
                    message = check.value(QStringLiteral("message")).toString(check.value(QStringLiteral("label")).toString());
                    if (!message.isEmpty()) {
                        break;
                    }
                }
            }
        }
        if (message.isEmpty()) {
            message = blocked ? QStringLiteral("Backend validation required.") : QStringLiteral("Mission validated by Control Center.");
        }
        if (m_events) {
            m_events->recordEvent(QStringLiteral("mission_validated"),
                                  body.value(QStringLiteral("validation_state")).toString() == QStringLiteral("blocked")
                                      ? QStringLiteral("warning")
                                      : QStringLiteral("info"),
                                  QStringLiteral("Mission validated by Control Center"),
                                  body);
        }
        setStatus(blocked ? message : QStringLiteral("Control Center validation synchronized."));
        emit activeMissionValidated(!blocked, message);
    });
}

bool MissionSyncManager::syncing() const { return m_syncing; }
QString MissionSyncManager::status() const { return m_status; }
QVariantMap MissionSyncManager::organization() const { return m_organization; }
QVariantMap MissionSyncManager::pilotProfile() const { return m_pilotProfile; }
QVariantMap MissionSyncManager::deviceSummary() const { return m_deviceSummary; }
QVariantMap MissionSyncManager::sessionStatus() const { return m_sessionStatus; }
QVariantList MissionSyncManager::assignedAircraft() const { return m_assignedAircraft; }
QVariantList MissionSyncManager::approvedMissions() const { return m_approvedMissions; }
QVariantList MissionSyncManager::activeMissions() const { return m_activeMissions; }
QVariantList MissionSyncManager::missionHistory() const { return m_missionHistory; }

void MissionSyncManager::bootstrap()
{
    const QDateTime now = QDateTime::currentDateTimeUtc();
    if (m_syncing) {
        setStatus(QStringLiteral("Control Center sync already running."));
        return;
    }
    if (m_lastBootstrapAt.isValid() && m_lastBootstrapAt.msecsTo(now) < 15000) {
        setStatus(QStringLiteral("Control Center dashboard data recently refreshed."));
        return;
    }
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Bootstrap blocked: %1").arg(m_session ? m_session->blockReason() : QStringLiteral("session unavailable")));
        return;
    }
    setSyncing(true);
    m_lastBootstrapAt = now;
    setStatus(QStringLiteral("Bootstrapping Control Center data..."));
    m_api->get(QStringLiteral("/api/missions/sync/bootstrap/"), true, true,
               [this](int statusCode, const QJsonObject &body, const QString &error) {
        setSyncing(false);
        if (statusCode < 200 || statusCode >= 300) {
            loadCached();
            setStatus(error.isEmpty() ? QStringLiteral("Bootstrap offline; using local cache.") : QStringLiteral("%1; using local cache.").arg(error));
            if (m_events) {
                QJsonObject payload{
                    {QStringLiteral("source"), QStringLiteral("SGG_CC")},
                    {QStringLiteral("cached"), true},
                    {QStringLiteral("status_code"), statusCode},
                    {QStringLiteral("error"), error}
                };
                m_events->recordEvent(QStringLiteral("control_center_unreachable"),
                                      QStringLiteral("warning"),
                                      QStringLiteral("Control Center unreachable; dashboard is using local cache"),
                                      payload);
            }
            return;
        }
        const QVariantMap bootstrap = body.toVariantMap();
        m_organization = bootstrap.value(QStringLiteral("organization")).toMap();
        m_pilotProfile = bootstrap.value(QStringLiteral("pilot"), bootstrap.value(QStringLiteral("pilot_profile"), bootstrap.value(QStringLiteral("profile")))).toMap();
        m_deviceSummary = bootstrap.value(QStringLiteral("device_summary")).toMap();
        m_sessionStatus = bootstrap.value(QStringLiteral("session_status")).toMap();
        m_assignedAircraft = bootstrap.value(QStringLiteral("assigned_aircraft")).toList();
        m_activeMissions = bootstrap.value(QStringLiteral("active_missions"),
                                           bootstrap.value(QStringLiteral("approved_missions"),
                                                           bootstrap.value(QStringLiteral("missions")))).toList();
        m_missionHistory = bootstrap.value(QStringLiteral("mission_history"),
                                           bootstrap.value(QStringLiteral("completed_missions"))).toList();
        m_approvedMissions = m_activeMissions;
        if (m_approvedMissions.isEmpty()) {
            m_approvedMissions = bootstrap.value(QStringLiteral("approved_missions"),
                                                 bootstrap.value(QStringLiteral("missions"))).toList();
        }
        if (m_cache) {
            m_cache->saveObject(QStringLiteral("mission_sync"), QStringLiteral("organization"), m_organization);
            m_cache->saveObject(QStringLiteral("mission_sync"), QStringLiteral("pilot_profile"), m_pilotProfile);
            m_cache->saveObject(QStringLiteral("mission_sync"), QStringLiteral("device_summary"), m_deviceSummary);
            m_cache->saveObject(QStringLiteral("mission_sync"), QStringLiteral("session_status"), m_sessionStatus);
            m_cache->saveList(QStringLiteral("mission_sync"), QStringLiteral("assigned_aircraft"), m_assignedAircraft);
            m_cache->saveList(QStringLiteral("mission_sync"), QStringLiteral("approved_missions"), m_approvedMissions);
            m_cache->saveList(QStringLiteral("mission_sync"), QStringLiteral("active_missions"), m_activeMissions);
            m_cache->saveList(QStringLiteral("mission_sync"), QStringLiteral("mission_history"), m_missionHistory);
            m_cache->saveObject(QStringLiteral("mission_sync"), QStringLiteral("bootstrap"), bootstrap);
        }
        setStatus(QStringLiteral("Bootstrap synchronized."));
        if (m_events) {
            QJsonObject payload{
                {QStringLiteral("source"), QStringLiteral("SGG_CC")},
                {QStringLiteral("cached"), false},
                {QStringLiteral("active_missions"), m_activeMissions.size()},
                {QStringLiteral("mission_history"), m_missionHistory.size()},
                {QStringLiteral("assigned_aircraft"), m_assignedAircraft.size()},
                {QStringLiteral("synced_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
            };
            m_events->recordEvent(QStringLiteral("control_center_reachable"),
                                  QStringLiteral("info"),
                                  QStringLiteral("Control Center dashboard data synchronized"),
                                  payload);
        }
        emit bootstrapReceived(bootstrap);
        emit syncChanged();
    });
}

void MissionSyncManager::openMission(const QVariantMap &mission)
{
    if (!m_plan) {
        setStatus(QStringLiteral("No mission planner available."));
        return;
    }

    const QString missionId = missionIdentifier(mission);
    if (missionId.isEmpty()) {
        setStatus(QStringLiteral("Cannot open mission without a Control Center id."));
        return;
    }

    const QString missionType = mission.value(QStringLiteral("mission_type"), mission.value(QStringLiteral("type"))).toString();
    if (missionType.isEmpty()) {
        setStatus(QStringLiteral("Cannot open mission: mission type is missing."));
        return;
    }

    const bool needsFetch = !missionHasGeometry(mission);
    if (!needsFetch) {
        applyOpenMission(mission);
        return;
    }

    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        if (m_cache) {
            const QVariantMap cached = m_cache->loadObject(QStringLiteral("mission_drafts"), missionId);
            if (!cached.isEmpty()) {
                applyOpenMission(cached);
                setStatus(QStringLiteral("Mission opened from local cache."));
                return;
            }
        }
        setStatus(QStringLiteral("Mission geometry unavailable offline."));
        return;
    }

    setSyncing(true);
    setStatus(QStringLiteral("Loading mission from Control Center..."));
    m_api->get(QStringLiteral("/api/missions/%1/").arg(missionId), true, true,
               [this, missionType, missionId](int statusCode, const QJsonObject &body, const QString &error) {
        setSyncing(false);
        if (statusCode < 200 || statusCode >= 300) {
            if (m_cache) {
                const QVariantMap cached = m_cache->loadObject(QStringLiteral("mission_drafts"), missionId);
                if (!cached.isEmpty()) {
                    applyOpenMission(cached);
                    setStatus(QStringLiteral("Mission opened from local cache."));
                    return;
                }
            }
            setStatus(error.isEmpty() ? QStringLiteral("Failed to load mission details.") : error);
            return;
        }
        applyOpenMission(body.toVariantMap());
    });
}

void MissionSyncManager::applyOpenMission(const QVariantMap &mission)
{
    const QVariantMap normalized = normalizeMission(mission);
    const QString missionType = normalized.value(QStringLiteral("mission_type"), normalized.value(QStringLiteral("type"))).toString();
    const QString missionId = missionIdentifier(normalized);
    if (missionType.isEmpty() || missionId.isEmpty()) {
        setStatus(QStringLiteral("Mission data is incomplete."));
        return;
    }

    m_plan->loadMission(normalized);
    if (m_cache) {
        m_cache->saveObject(QStringLiteral("mission_drafts"), missionId, normalized);
    }
    if (m_events) {
        m_events->recordEvent(QStringLiteral("mission_opened"),
                              QStringLiteral("info"),
                              QStringLiteral("Mission opened in planner"),
                              QJsonObject{
                                  {QStringLiteral("mission_id"), missionId},
                                  {QStringLiteral("mission_type"), missionType},
                                  {QStringLiteral("name"), normalized.value(QStringLiteral("name")).toString()}
                              });
    }
    setStatus(QStringLiteral("Mission loaded: %1").arg(normalized.value(QStringLiteral("name"), QStringLiteral("Untitled Mission")).toString()));
    emit missionOpened(missionType, missionId);
    emit syncChanged();
}

void MissionSyncManager::syncMissions()
{
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Mission sync blocked: %1").arg(m_session ? m_session->blockReason() : QStringLiteral("session unavailable")));
        return;
    }
    setSyncing(true);
    setStatus(QStringLiteral("Synchronizing missions..."));
    m_api->get(QStringLiteral("/api/missions/sync/"), true, true,
               [this](int statusCode, const QJsonObject &body, const QString &error) {
        setSyncing(false);
        if (statusCode < 200 || statusCode >= 300) {
            setStatus(error.isEmpty() ? QStringLiteral("Mission sync failed.") : error);
            return;
        }
        QJsonArray missions = body.value(QStringLiteral("missions")).toArray();
        if (missions.isEmpty()) {
            missions = body.value(QStringLiteral("active_missions")).toArray();
        }
        m_approvedMissions = missions.toVariantList();
        if (m_cache) {
            m_cache->saveList(QStringLiteral("mission_sync"), QStringLiteral("approved_missions"), m_approvedMissions);
        }
        setStatus(QStringLiteral("Missions synchronized."));
        emit syncChanged();
    });
}

void MissionSyncManager::loadCached()
{
    if (!m_cache) {
        return;
    }
    m_organization = m_cache->loadObject(QStringLiteral("mission_sync"), QStringLiteral("organization"));
    m_pilotProfile = m_cache->loadObject(QStringLiteral("mission_sync"), QStringLiteral("pilot_profile"));
    m_deviceSummary = m_cache->loadObject(QStringLiteral("mission_sync"), QStringLiteral("device_summary"));
    m_sessionStatus = m_cache->loadObject(QStringLiteral("mission_sync"), QStringLiteral("session_status"));
    m_assignedAircraft = m_cache->loadList(QStringLiteral("mission_sync"), QStringLiteral("assigned_aircraft"));
    m_approvedMissions = m_cache->loadList(QStringLiteral("mission_sync"), QStringLiteral("approved_missions"));
    m_activeMissions = m_cache->loadList(QStringLiteral("mission_sync"), QStringLiteral("active_missions"));
    m_missionHistory = m_cache->loadList(QStringLiteral("mission_sync"), QStringLiteral("mission_history"));
    if (m_activeMissions.isEmpty()) {
        m_activeMissions = m_approvedMissions;
    }
    if (!m_organization.isEmpty() || !m_assignedAircraft.isEmpty() || !m_approvedMissions.isEmpty() || !m_missionHistory.isEmpty()) {
        m_status = QStringLiteral("Loaded local workspace cache.");
    }
    emit syncChanged();
}

void MissionSyncManager::setSyncing(bool syncing)
{
    if (m_syncing == syncing) {
        return;
    }
    m_syncing = syncing;
    emit syncChanged();
}

void MissionSyncManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit syncChanged();
}
