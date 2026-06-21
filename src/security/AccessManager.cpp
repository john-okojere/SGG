#include "AccessManager.h"

#include "../access/RbacBootstrapNormalizer.h"
#include "../cache/LocalSyncCache.h"
#include "../sync/GcsEventSyncManager.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaType>
#include <QProcessEnvironment>

#include <optional>

namespace {
constexpr qint64 OfflineAuthorizationHours = 8;

bool rbacDebugEnabled()
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    return env.value(QStringLiteral("SKYGRID_RBAC_DEBUG")) == QStringLiteral("true")
        || env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true");
}

QJsonArray sortedArray(const QStringList &values)
{
    QStringList sorted = values;
    sorted.sort();
    QJsonArray array;
    for (const QString &value : sorted) {
        array.append(value);
    }
    return array;
}

QString firstStringValue(const QVariantMap &map, const QStringList &keys)
{
    for (const QString &key : keys) {
        const QString value = map.value(key).toString().trimmed();
        if (!value.isEmpty()) {
            return value;
        }
    }
    return {};
}

QVariantMap firstMapValue(const QVariantMap &map, const QStringList &keys)
{
    for (const QString &key : keys) {
        const QVariantMap value = map.value(key).toMap();
        if (!value.isEmpty()) {
            return value;
        }
    }
    return {};
}

std::optional<bool> boolValue(const QVariantMap &map, const QStringList &keys)
{
    for (const QString &key : keys) {
        const QVariant value = map.value(key);
        if (value.isValid() && !value.isNull()) {
            return value.toBool();
        }
    }
    return std::nullopt;
}

QStringList fallbackPermissionsForAction(const QString &action)
{
    static const QHash<QString, QStringList> map{
        {QStringLiteral("gcs_tools"), {QStringLiteral("can_plan_mission"),
                                       QStringLiteral("can_upload_mission"),
                                       QStringLiteral("can_start_mission"),
                                       QStringLiteral("can_stream_telemetry"),
                                       QStringLiteral("can_view_telemetry"),
                                       QStringLiteral("can_configure_vehicle"),
                                       QStringLiteral("can_view_reports"),
                                       QStringLiteral("can_run_initial_setup"),
                                       QStringLiteral("can_tune_vehicle"),
                                       QStringLiteral("can_flash_firmware"),
                                       QStringLiteral("can_view_logs"),
                                       QStringLiteral("can_download_logs"),
                                       QStringLiteral("can_use_simulation"),
                                       QStringLiteral("can_use_advanced_mavlink"),
                                       QStringLiteral("can_configure_payload"),
                                       QStringLiteral("can_configure_optional_hardware"),
                                       QStringLiteral("can_view_video_stream"),
                                       QStringLiteral("can_configure_video_payload"),
                                       QStringLiteral("can_use_terminal"),
                                       QStringLiteral("can_manage_multi_vehicle"),
                                       QStringLiteral("can_view_mission_logs"),
                                       QStringLiteral("can_view_vehicle_audit"),
                                       QStringLiteral("can_view_fleet")}},
        {QStringLiteral("connect"), {QStringLiteral("can_stream_telemetry")}},
        {QStringLiteral("flight_data"), {QStringLiteral("can_stream_telemetry"),
                                         QStringLiteral("can_view_telemetry")}},
        {QStringLiteral("initial_setup"), {QStringLiteral("can_configure_vehicle"),
                                           QStringLiteral("can_configure_rc")}},
        {QStringLiteral("vehicle_tuning"), {QStringLiteral("can_read_vehicle_parameters"),
                                            QStringLiteral("can_write_vehicle_parameters")}},
        {QStringLiteral("firmware_manager"), {QStringLiteral("can_configure_vehicle")}},
        {QStringLiteral("firmware_flash"), {QStringLiteral("can_configure_vehicle")}},
        {QStringLiteral("optional_hardware"), {QStringLiteral("can_configure_vehicle"),
                                               QStringLiteral("can_configure_optional_hardware")}},
        {QStringLiteral("video_stream"), {QStringLiteral("can_view_video_stream"),
                                          QStringLiteral("can_configure_payload"),
                                          QStringLiteral("can_configure_vehicle")}},
        {QStringLiteral("video_payload_configuration"), {QStringLiteral("can_configure_video_payload"),
                                                        QStringLiteral("can_configure_payload"),
                                                        QStringLiteral("can_configure_vehicle")}},
        {QStringLiteral("terminal"), {QStringLiteral("can_use_terminal"),
                                      QStringLiteral("can_use_advanced_mavlink")}},
        {QStringLiteral("advanced_mavlink"), {QStringLiteral("can_use_advanced_mavlink"),
                                              QStringLiteral("can_use_terminal")}},
        {QStringLiteral("parameter_safety_override"), {QStringLiteral("can_override_parameter_safety"),
                                                       QStringLiteral("can_use_advanced_mavlink")}},
        {QStringLiteral("logs_analysis"), {QStringLiteral("can_view_mission_logs"),
                                           QStringLiteral("can_view_vehicle_audit")}},
        {QStringLiteral("log_download"), {QStringLiteral("can_view_mission_logs")}},
        {QStringLiteral("command_center_sync"), {QStringLiteral("can_view_reports"),
                                                 QStringLiteral("can_view_vehicle_audit")}},
        {QStringLiteral("advanced_mission_editor"), {QStringLiteral("can_plan_mission")}},
        {QStringLiteral("payload_configuration"), {QStringLiteral("can_configure_vehicle"),
                                                   QStringLiteral("can_configure_payload"),
                                                   QStringLiteral("can_view_video_stream"),
                                                   QStringLiteral("can_configure_video_payload")}},
        {QStringLiteral("multi_vehicle"), {QStringLiteral("can_view_fleet"),
                                           QStringLiteral("can_assign_aircraft")}}
    };
    return map.value(action);
}

}

AccessManager::AccessManager(LocalSyncCache *cache, QObject *parent)
    : QObject(parent), m_cache(cache)
{
    loadSnapshot();
}

bool AccessManager::accessLoaded() const { return m_accessLoaded; }
bool AccessManager::sessionTrusted() const { return m_sessionTrusted; }
QString AccessManager::role() const { return m_role; }
QStringList AccessManager::roles() const { return m_roles; }
QStringList AccessManager::rawRoles() const { return m_rawRoles; }
QStringList AccessManager::permissions() const { return m_permissions; }
QStringList AccessManager::allowedModules() const { return m_allowedModules; }
QStringList AccessManager::assignedAircraftIds() const
{
    QStringList ids(m_aircraftIds.cbegin(), m_aircraftIds.cend());
    ids.sort();
    return ids;
}
QStringList AccessManager::assignedMissionIds() const
{
    QStringList ids(m_missionIds.cbegin(), m_missionIds.cend());
    ids.sort();
    return ids;
}
int AccessManager::organizationId() const { return m_organizationId; }
QVariantMap AccessManager::sessionStatus() const { return m_sessionStatus; }
QVariantMap AccessManager::deviceSummary() const { return m_deviceSummary; }
QString AccessManager::status() const { return m_status; }

bool AccessManager::offlineAuthorizationValid() const
{
    return m_accessLoaded
        && m_sessionTrusted
        && m_lastVerifiedAt.isValid()
        && m_lastVerifiedAt.secsTo(QDateTime::currentDateTimeUtc()) <= OfflineAuthorizationHours * 3600;
}

void AccessManager::setEventSyncManager(GcsEventSyncManager *events)
{
    m_events = events;
}

void AccessManager::applyBootstrap(const QVariantMap &bootstrap)
{
    const QString previous = m_fingerprint;
    const RbacBootstrapProfile profile = RbacBootstrapNormalizer::fromBootstrap(bootstrap);

    m_rawRoles = profile.rawRoles;
    m_roles = profile.normalizedRoles;
    m_role = m_roles.isEmpty() ? QString() : m_roles.first();
    m_permissions = profile.permissionKeys;
    m_allowedModules = profile.allowedModules;
    m_sessionStatus = profile.sessionStatus;
    m_deviceSummary = profile.deviceSummary;
    m_organizationId = profile.organizationId;
    m_aircraftIds = profile.aircraftIds;
    m_missionIds = profile.missionIds;
    if (profile.hasSessionTrusted) {
        m_sessionTrusted = profile.sessionTrusted;
    }
    if (profile.hasBackendReachable) {
        m_backendReachable = profile.backendReachable;
    }
    m_lastVerifiedAt = QDateTime::currentDateTimeUtc();
    m_accessLoaded = !m_permissions.isEmpty() || !m_allowedModules.isEmpty();
    m_fingerprint = accessFingerprint(bootstrap);
    persistSnapshot();

    if (rbacDebugEnabled()) {
        qInfo().noquote()
            << "[RBAC] bootstrap applied"
            << "rawRoles=" << m_rawRoles.join(QStringLiteral(","))
            << "roles=" << m_roles.join(QStringLiteral(","))
            << "permissions=" << m_permissions.join(QStringLiteral(","))
            << "modules=" << m_allowedModules.join(QStringLiteral(","))
            << "trusted=" << m_sessionTrusted
            << "reachable=" << m_backendReachable
            << "org=" << m_organizationId
            << "aircraft=" << assignedAircraftIds().join(QStringLiteral(","))
            << "missions=" << assignedMissionIds().join(QStringLiteral(","));
    }

    if (!previous.isEmpty() && previous != m_fingerprint && m_cache) {
        m_cache->purgeAccessControlledData(QStringLiteral("access profile changed"));
        emitAudit(QStringLiteral("cache_purged_access_changed"),
                  QStringLiteral("warning"),
                  QStringLiteral("Local cache purged because access scope changed."));
    }
    setStatus(m_accessLoaded ? QStringLiteral("Access profile verified.") : QStringLiteral("Access profile has no permissions."));
    emit accessChanged();
}

void AccessManager::clearAccess(const QString &reason)
{
    m_accessLoaded = false;
    m_sessionTrusted = false;
    m_backendReachable = false;
    m_role.clear();
    m_rawRoles.clear();
    m_roles.clear();
    m_permissions.clear();
    m_allowedModules.clear();
    m_organizationId = 0;
    m_sessionStatus.clear();
    m_deviceSummary.clear();
    m_aircraftIds.clear();
    m_missionIds.clear();
    m_lastVerifiedAt = QDateTime();
    m_fingerprint.clear();
    if (m_cache) {
        m_cache->purgeAccessControlledData(reason.isEmpty() ? QStringLiteral("access cleared") : reason);
    }
    setStatus(reason.isEmpty() ? QStringLiteral("Access cleared.") : reason);
    emit accessRevoked(m_status);
    emit accessChanged();
}

void AccessManager::setSessionState(bool trusted, bool reachable, const QString &reason)
{
    const bool changed = m_sessionTrusted != trusted || m_backendReachable != reachable;
    m_sessionTrusted = trusted;
    m_backendReachable = reachable;
    if (rbacDebugEnabled()) {
        qInfo().noquote()
            << "[RBAC] session state"
            << "sessionManager.operationsAllowed=" << trusted
            << "backendReachable=" << reachable
            << "reason=" << reason;
    }
    if (!trusted && m_cache) {
        m_cache->lockAccessControlledData(reason.isEmpty() ? QStringLiteral("session not trusted") : reason);
    }
    if (!trusted) {
        setStatus(reason.isEmpty() ? QStringLiteral("Device approval required.") : reason);
    } else if (!reachable) {
        setStatus(offlineAuthorizationValid()
                      ? QStringLiteral("Offline mode: last verified permissions active.")
                      : QStringLiteral("Offline mode blocked: permissions are not verified."));
    } else if (m_accessLoaded) {
        setStatus(QStringLiteral("Access profile verified."));
    }
    if (changed) {
        emit accessChanged();
    }
}

bool AccessManager::can(const QString &permission) const
{
    return m_accessLoaded && m_permissions.contains(RbacBootstrapNormalizer::normalizePermission(permission));
}

bool AccessManager::canAny(const QStringList &permissions) const
{
    for (const QString &permission : permissions) {
        if (can(permission)) {
            return true;
        }
    }
    return false;
}

bool AccessManager::canModule(const QString &module) const
{
    return m_accessLoaded && m_allowedModules.contains(RbacBootstrapNormalizer::normalizeModule(module));
}

bool AccessManager::canPerform(const QString &action) const
{
    return actionDenialReason(action).isEmpty();
}

QString AccessManager::denialReasonForAction(const QString &action) const
{
    return actionDenialReason(action);
}

QString AccessManager::moduleDenialReason(const QString &module) const
{
    if (!m_accessLoaded) {
        return QStringLiteral("Access profile is not loaded.");
    }
    const QString normalized = RbacBootstrapNormalizer::normalizeModule(module);
    if (m_allowedModules.contains(normalized)) {
        return {};
    }
    return QStringLiteral("Module not allowed: %1").arg(normalized);
}

QVariantMap AccessManager::diagnosticSnapshot() const
{
    auto workspaceRow = [this](const QString &name, const QString &module, const QString &action) {
        const QString moduleReason = moduleDenialReason(module);
        const QString actionReason = actionDenialReason(action);
        const bool visible = moduleReason.isEmpty() && actionReason.isEmpty();
        return QVariantMap{
            {QStringLiteral("name"), name},
            {QStringLiteral("module"), RbacBootstrapNormalizer::normalizeModule(module)},
            {QStringLiteral("action"), normalizedAction(action)},
            {QStringLiteral("visible"), visible},
            {QStringLiteral("reason"), visible ? QStringLiteral("visible")
                                                : (!moduleReason.isEmpty() ? moduleReason : actionReason)}
        };
    };

    QVariantList workspaces{
        workspaceRow(QStringLiteral("Pilot Mode"), QStringLiteral("pilot_operations"), QStringLiteral("manual_flight")),
        workspaceRow(QStringLiteral("Mission Planner"), QStringLiteral("mission_planning"), QStringLiteral("mission_planning")),
        workspaceRow(QStringLiteral("Flight Data"), QStringLiteral("flight_data"), QStringLiteral("flight_data")),
        workspaceRow(QStringLiteral("GCS Tools"), QStringLiteral("gcs_tools"), QStringLiteral("gcs_tools")),
        workspaceRow(QStringLiteral("Fleet"), QStringLiteral("fleet"), QStringLiteral("aircraft_profile_access")),
        workspaceRow(QStringLiteral("Manufacturer"), QStringLiteral("manufacturer_dashboard"), QStringLiteral("manufacturer_tools"))
    };

    return QVariantMap{
        {QStringLiteral("role"), m_role},
        {QStringLiteral("roles"), m_roles},
        {QStringLiteral("raw_roles"), m_rawRoles},
        {QStringLiteral("permissions"), m_permissions},
        {QStringLiteral("allowed_modules"), m_allowedModules},
        {QStringLiteral("trusted_device"), m_sessionTrusted},
        {QStringLiteral("backend_reachable"), m_backendReachable},
        {QStringLiteral("offline_authorization_valid"), offlineAuthorizationValid()},
        {QStringLiteral("organization_id"), m_organizationId},
        {QStringLiteral("assigned_aircraft_ids"), assignedAircraftIds()},
        {QStringLiteral("assigned_mission_ids"), assignedMissionIds()},
        {QStringLiteral("session_status"), m_sessionStatus},
        {QStringLiteral("device_summary"), m_deviceSummary},
        {QStringLiteral("status"), m_status},
        {QStringLiteral("workspaces"), workspaces}
    };
}

QString AccessManager::actionDenialReason(const QString &action) const
{
    const QString normalized = normalizedAction(action);
    const QString permission = permissionForAction(normalized);
    const QStringList fallbacks = fallbackPermissionsForAction(normalized);
    if (normalized == QStringLiteral("vehicle_profile_setup")) {
        if (!m_accessLoaded) {
            return QStringLiteral("Access profile is not loaded.");
        }
        if (!canAny({QStringLiteral("can_register_vehicle"), QStringLiteral("can_edit_vehicle_profile")})) {
            return QStringLiteral("Missing permission: can_register_vehicle or can_edit_vehicle_profile");
        }
        if (actionRequiresTrustedSession(normalized) && !m_sessionTrusted) {
            return QStringLiteral("Trusted device session is required.");
        }
        if (actionRequiresTrustedSession(normalized) && !m_backendReachable && !actionAllowsOfflineAuthorization(normalized)) {
            return QStringLiteral("Online authorization is required.");
        }
        return {};
    }
    if (permission.isEmpty() && fallbacks.isEmpty()) {
        return QStringLiteral("No permission mapping for action.");
    }
    if (!m_accessLoaded) {
        return QStringLiteral("Access profile is not loaded.");
    }
    if ((permission.isEmpty() || !can(permission)) && !canAny(fallbacks)) {
        QStringList accepted;
        if (!permission.isEmpty()) {
            accepted << permission;
        }
        accepted << fallbacks;
        accepted.removeDuplicates();
        return QStringLiteral("Missing permission: %1").arg(accepted.join(QStringLiteral(" or ")));
    }
    if (actionRequiresTrustedSession(normalized) && !m_sessionTrusted) {
        return QStringLiteral("Trusted device session is required.");
    }
    if (actionRequiresTrustedSession(normalized) && !m_backendReachable && !actionAllowsOfflineAuthorization(normalized)) {
        return QStringLiteral("Online authorization is required.");
    }
    return {};
}

bool AccessManager::canAccessAircraft(const QVariant &aircraftId) const
{
    const QString id = idString(aircraftId);
    return m_accessLoaded && !id.isEmpty() && m_aircraftIds.contains(id);
}

bool AccessManager::canAccessMission(const QVariant &missionId) const
{
    const QString id = idString(missionId);
    return m_accessLoaded && !id.isEmpty() && m_missionIds.contains(id);
}

QVariantList AccessManager::filterAircraft(const QVariantList &aircraft) const
{
    QVariantList filtered;
    for (const QVariant &entry : aircraft) {
        const QVariantMap map = entry.toMap();
        if (canAccessAircraft(map.value(QStringLiteral("id"), map.value(QStringLiteral("aircraft_id"))))) {
            filtered << entry;
        }
    }
    return filtered;
}

QVariantList AccessManager::filterMissions(const QVariantList &missions) const
{
    QVariantList filtered;
    for (const QVariant &entry : missions) {
        const QVariantMap map = entry.toMap();
        if (canAccessMission(map.value(QStringLiteral("id"), map.value(QStringLiteral("mission_id"))))) {
            filtered << entry;
        }
    }
    return filtered;
}

bool AccessManager::authorizeAction(const QString &action, const QVariantMap &context, const QString &message)
{
    const QString normalized = normalizedAction(action);
    const QString reason = actionDenialReason(normalized);
    if (!reason.isEmpty()) {
        recordBlocked(normalized, reason, context);
        setStatus(message.isEmpty() ? reason : message);
        return false;
    }
    recordAllowed(normalized, context);
    return true;
}

void AccessManager::recordAllowed(const QString &action, const QVariantMap &context)
{
    QVariantMap payload = context;
    payload[QStringLiteral("action")] = normalizedAction(action);
    payload[QStringLiteral("role")] = m_role;
    payload[QStringLiteral("permission")] = permissionForAction(action);
    payload[QStringLiteral("offline")] = !m_backendReachable;
    emitAudit(QStringLiteral("authorization_allowed"),
              QStringLiteral("info"),
              QStringLiteral("Sensitive action authorized."),
              payload);
}

void AccessManager::recordBlocked(const QString &action, const QString &reason, const QVariantMap &context)
{
    QVariantMap payload = context;
    payload[QStringLiteral("action")] = normalizedAction(action);
    payload[QStringLiteral("role")] = m_role;
    payload[QStringLiteral("permission")] = permissionForAction(action);
    payload[QStringLiteral("reason")] = reason;
    payload[QStringLiteral("offline")] = !m_backendReachable;
    emitAudit(QStringLiteral("authorization_blocked"),
              QStringLiteral("warning"),
              QStringLiteral("Sensitive action blocked by local RBAC."),
              payload);
}

QString AccessManager::permissionForAction(const QString &action) const
{
    const QString key = normalizedAction(action);
    static const QHash<QString, QString> map{
        {QStringLiteral("mission_planning"), QStringLiteral("can_plan_mission")},
        {QStringLiteral("mission_open"), QStringLiteral("can_plan_mission")},
        {QStringLiteral("mission_save"), QStringLiteral("can_plan_mission")},
        {QStringLiteral("mission_sync"), QStringLiteral("can_plan_mission")},
        {QStringLiteral("mission_validation"), QStringLiteral("can_plan_mission")},
        {QStringLiteral("mission_upload"), QStringLiteral("can_upload_mission")},
        {QStringLiteral("mission_start"), QStringLiteral("can_start_mission")},
        {QStringLiteral("mission_pause"), QStringLiteral("can_start_mission")},
        {QStringLiteral("mission_resume"), QStringLiteral("can_start_mission")},
        {QStringLiteral("mission_finish"), QStringLiteral("can_start_mission")},
        {QStringLiteral("mission_abort"), QStringLiteral("can_start_mission")},
        {QStringLiteral("advanced_mission_editor"), QStringLiteral("can_plan_mission")},
        {QStringLiteral("manual_flight"), QStringLiteral("can_fly_manual")},
        {QStringLiteral("gcs_tools"), QStringLiteral("can_use_flight_data")},
        {QStringLiteral("connect"), QStringLiteral("can_use_flight_data")},
        {QStringLiteral("flight_data"), QStringLiteral("can_use_flight_data")},
        {QStringLiteral("aircraft_connection"), QStringLiteral("can_stream_telemetry")},
        {QStringLiteral("telemetry_stream"), QStringLiteral("can_stream_telemetry")},
        {QStringLiteral("telemetry_export"), QStringLiteral("can_view_reports")},
        {QStringLiteral("aircraft_profile_access"), QStringLiteral("can_view_fleet")},
        {QStringLiteral("initial_setup"), QStringLiteral("can_run_initial_setup")},
        {QStringLiteral("vehicle_tuning"), QStringLiteral("can_tune_vehicle")},
        {QStringLiteral("vehicle_parameter_read"), QStringLiteral("can_read_vehicle_parameters")},
        {QStringLiteral("vehicle_parameter_write"), QStringLiteral("can_write_vehicle_parameters")},
        {QStringLiteral("emergency_stop"), QStringLiteral("can_fly_manual")},
        {QStringLiteral("firmware_manager"), QStringLiteral("can_flash_firmware")},
        {QStringLiteral("firmware_flash"), QStringLiteral("can_flash_firmware")},
        {QStringLiteral("optional_hardware"), QStringLiteral("can_configure_optional_hardware")},
        {QStringLiteral("video_stream"), QStringLiteral("can_view_video_stream")},
        {QStringLiteral("video_payload_configuration"), QStringLiteral("can_configure_video_payload")},
        {QStringLiteral("terminal"), QStringLiteral("can_use_terminal")},
        {QStringLiteral("parameter_safety_override"), QStringLiteral("can_override_parameter_safety")},
        {QStringLiteral("logs_analysis"), QStringLiteral("can_view_logs")},
        {QStringLiteral("log_download"), QStringLiteral("can_download_logs")},
        {QStringLiteral("simulation"), QStringLiteral("can_use_simulation")},
        {QStringLiteral("advanced_mavlink"), QStringLiteral("can_use_advanced_mavlink")},
        {QStringLiteral("payload_configuration"), QStringLiteral("can_configure_payload")},
        {QStringLiteral("multi_vehicle"), QStringLiteral("can_manage_multi_vehicle")},
        {QStringLiteral("command_center_sync"), QStringLiteral("can_view_reports")},
        {QStringLiteral("manufacturer_tools"), QStringLiteral("can_configure_vehicle")},
        {QStringLiteral("manufacturer_dashboard"), QStringLiteral("can_configure_vehicle")},
        {QStringLiteral("manufacturer_test_flight"), QStringLiteral("can_run_manufacturer_test_flight")},
        {QStringLiteral("manual_test_mode"), QStringLiteral("can_fly_manual_test")},
        {QStringLiteral("vehicle_configuration"), QStringLiteral("can_configure_vehicle")},
        {QStringLiteral("vehicle_profile_setup"), QStringLiteral("can_edit_vehicle_profile")},
        {QStringLiteral("flight_controller_binding"), QStringLiteral("can_bind_flight_controller")},
        {QStringLiteral("rc_mapping"), QStringLiteral("can_configure_rc")},
        {QStringLiteral("vehicle_release_lock"), QStringLiteral("can_release_vehicle_to_organization")},
        {QStringLiteral("settings"), QStringLiteral("can_view_reports")},
        {QStringLiteral("security_audit"), QStringLiteral("can_view_vehicle_audit")}
    };
    return map.value(key);
}

QString AccessManager::normalizedAction(const QString &action) const
{
    QString key = action.trimmed().toLower();
    key.replace(QLatin1Char('-'), QLatin1Char('_'));
    key.replace(QLatin1Char(' '), QLatin1Char('_'));
    return key;
}

QString AccessManager::idString(const QVariant &value) const
{
    if (!value.isValid() || value.isNull()) {
        return {};
    }
    return value.toString().trimmed();
}

QStringList AccessManager::stringListFromVariant(const QVariant &value) const
{
    QStringList output;
    if (value.metaType().id() == QMetaType::QStringList) {
        output = value.toStringList();
        output.removeAll(QString());
        output.removeDuplicates();
        return output;
    }
    for (const QVariant &entry : value.toList()) {
        const QString text = entry.toString().trimmed();
        if (!text.isEmpty()) {
            output << text;
        }
    }
    output.removeDuplicates();
    return output;
}

QSet<QString> AccessManager::idsFromList(const QVariantList &items, const QStringList &keys) const
{
    QSet<QString> ids;
    for (const QVariant &entry : items) {
        const QVariantMap map = entry.toMap();
        for (const QString &key : keys) {
            const QString id = idString(map.value(key));
            if (!id.isEmpty()) {
                ids.insert(id);
                break;
            }
        }
    }
    return ids;
}

QString AccessManager::accessFingerprint(const QVariantMap &bootstrap) const
{
    QJsonObject object{
        {QStringLiteral("roles"), sortedArray(m_roles)},
        {QStringLiteral("permissions"), sortedArray(m_permissions)},
        {QStringLiteral("allowed_modules"), sortedArray(m_allowedModules)},
        {QStringLiteral("organization_id"), m_organizationId},
        {QStringLiteral("session_status"), QJsonObject::fromVariantMap(m_sessionStatus)},
        {QStringLiteral("device_summary"), QJsonObject::fromVariantMap(m_deviceSummary)}
    };
    QJsonArray aircraft;
    QStringList aircraftIds(m_aircraftIds.cbegin(), m_aircraftIds.cend());
    aircraftIds.sort();
    for (const QString &id : aircraftIds) {
        aircraft.append(id);
    }
    object.insert(QStringLiteral("aircraft_ids"), aircraft);
    QJsonArray missions;
    QStringList missionIds(m_missionIds.cbegin(), m_missionIds.cend());
    missionIds.sort();
    for (const QString &id : missionIds) {
        missions.append(id);
    }
    object.insert(QStringLiteral("mission_ids"), missions);
    const QVariantMap user = bootstrap.value(QStringLiteral("user")).toMap();
    object.insert(QStringLiteral("user_id"), user.value(QStringLiteral("id")).toString());
    return QString::fromLatin1(QCryptographicHash::hash(QJsonDocument(object).toJson(QJsonDocument::Compact),
                                                        QCryptographicHash::Sha256).toHex());
}

bool AccessManager::actionRequiresTrustedSession(const QString &action) const
{
    const QString key = normalizedAction(action);
    static const QSet<QString> actions{
        QStringLiteral("mission_validation"),
        QStringLiteral("mission_save"),
        QStringLiteral("mission_sync"),
        QStringLiteral("mission_upload"),
        QStringLiteral("mission_start"),
        QStringLiteral("mission_pause"),
        QStringLiteral("mission_resume"),
        QStringLiteral("mission_finish"),
        QStringLiteral("mission_abort"),
        QStringLiteral("advanced_mission_editor"),
        QStringLiteral("manual_flight"),
        QStringLiteral("connect"),
        QStringLiteral("flight_data"),
        QStringLiteral("aircraft_connection"),
        QStringLiteral("telemetry_stream"),
        QStringLiteral("telemetry_export"),
        QStringLiteral("initial_setup"),
        QStringLiteral("vehicle_tuning"),
        QStringLiteral("vehicle_parameter_read"),
        QStringLiteral("vehicle_parameter_write"),
        QStringLiteral("emergency_stop"),
        QStringLiteral("firmware_manager"),
        QStringLiteral("firmware_flash"),
        QStringLiteral("optional_hardware"),
        QStringLiteral("video_stream"),
        QStringLiteral("video_payload_configuration"),
        QStringLiteral("terminal"),
        QStringLiteral("parameter_safety_override"),
        QStringLiteral("logs_analysis"),
        QStringLiteral("log_download"),
        QStringLiteral("simulation"),
        QStringLiteral("advanced_mavlink"),
        QStringLiteral("payload_configuration"),
        QStringLiteral("multi_vehicle"),
        QStringLiteral("command_center_sync"),
        QStringLiteral("manufacturer_tools"),
        QStringLiteral("manufacturer_dashboard"),
        QStringLiteral("manufacturer_test_flight"),
        QStringLiteral("manual_test_mode"),
        QStringLiteral("vehicle_configuration"),
        QStringLiteral("vehicle_profile_setup"),
        QStringLiteral("flight_controller_binding"),
        QStringLiteral("rc_mapping"),
        QStringLiteral("vehicle_release_lock"),
        QStringLiteral("security_audit")
    };
    return actions.contains(key);
}

bool AccessManager::actionAllowsOfflineAuthorization(const QString &action) const
{
    const QString key = normalizedAction(action);
    return key == QStringLiteral("mission_planning")
        || key == QStringLiteral("mission_open")
        || key == QStringLiteral("settings")
        || key == QStringLiteral("connect")
        || key == QStringLiteral("flight_data")
        || key == QStringLiteral("aircraft_connection")
        || key == QStringLiteral("telemetry_stream")
        || key == QStringLiteral("initial_setup")
        || key == QStringLiteral("vehicle_tuning")
        || key == QStringLiteral("vehicle_parameter_read")
        || key == QStringLiteral("vehicle_parameter_write")
        || key == QStringLiteral("firmware_manager")
        || key == QStringLiteral("firmware_flash")
        || key == QStringLiteral("optional_hardware")
        || key == QStringLiteral("video_stream")
        || key == QStringLiteral("video_payload_configuration")
        || key == QStringLiteral("terminal")
        || key == QStringLiteral("parameter_safety_override")
        || key == QStringLiteral("logs_analysis")
        || key == QStringLiteral("log_download")
        || key == QStringLiteral("simulation")
        || key == QStringLiteral("advanced_mavlink")
        || key == QStringLiteral("payload_configuration")
        || key == QStringLiteral("multi_vehicle")
        || key == QStringLiteral("manufacturer_tools")
        || key == QStringLiteral("manufacturer_dashboard")
        || key == QStringLiteral("manufacturer_test_flight")
        || key == QStringLiteral("manual_test_mode")
        || key == QStringLiteral("vehicle_configuration")
        || key == QStringLiteral("vehicle_profile_setup")
        || key == QStringLiteral("flight_controller_binding")
        || key == QStringLiteral("rc_mapping");
}

void AccessManager::persistSnapshot()
{
    if (!m_cache) {
        return;
    }
    QVariantMap snapshot{
        {QStringLiteral("access_loaded"), m_accessLoaded},
        {QStringLiteral("role"), m_role},
        {QStringLiteral("raw_roles"), m_rawRoles},
        {QStringLiteral("roles"), m_roles},
        {QStringLiteral("permissions"), m_permissions},
        {QStringLiteral("allowed_modules"), m_allowedModules},
        {QStringLiteral("organization_id"), m_organizationId},
        {QStringLiteral("session_status"), m_sessionStatus},
        {QStringLiteral("device_summary"), m_deviceSummary},
        {QStringLiteral("last_verified_at"), m_lastVerifiedAt.toString(Qt::ISODateWithMs)},
        {QStringLiteral("fingerprint"), m_fingerprint}
    };
    QVariantList aircraft;
    for (const QString &id : std::as_const(m_aircraftIds)) {
        aircraft << id;
    }
    QVariantList missions;
    for (const QString &id : std::as_const(m_missionIds)) {
        missions << id;
    }
    snapshot[QStringLiteral("aircraft_ids")] = aircraft;
    snapshot[QStringLiteral("mission_ids")] = missions;
    m_cache->saveObject(QStringLiteral("access"), QStringLiteral("snapshot"), snapshot);
    m_cache->setAccessFingerprint(m_fingerprint);
}

void AccessManager::loadSnapshot()
{
    if (!m_cache) {
        return;
    }
    const QVariantMap snapshot = m_cache->loadObject(QStringLiteral("access"), QStringLiteral("snapshot"));
    if (snapshot.isEmpty()) {
        return;
    }
    m_accessLoaded = snapshot.value(QStringLiteral("access_loaded")).toBool();
    m_rawRoles = stringListFromVariant(snapshot.value(QStringLiteral("raw_roles")));
    m_roles.clear();
    for (const QString &role : stringListFromVariant(snapshot.value(QStringLiteral("roles")))) {
        const QString normalized = RbacBootstrapNormalizer::normalizeRole(role);
        if (!normalized.isEmpty() && !m_roles.contains(normalized)) {
            m_roles << normalized;
        }
    }
    m_role = m_roles.isEmpty()
        ? RbacBootstrapNormalizer::normalizeRole(snapshot.value(QStringLiteral("role")).toString())
        : m_roles.first();
    m_permissions.clear();
    for (const QString &permission : stringListFromVariant(snapshot.value(QStringLiteral("permissions")))) {
        const QString normalized = RbacBootstrapNormalizer::normalizePermission(permission);
        if (!normalized.isEmpty() && !m_permissions.contains(normalized)) {
            m_permissions << normalized;
        }
    }
    m_permissions.sort();
    m_allowedModules.clear();
    for (const QString &module : stringListFromVariant(snapshot.value(QStringLiteral("allowed_modules")))) {
        const QString normalized = RbacBootstrapNormalizer::normalizeModule(module);
        if (!normalized.isEmpty() && !m_allowedModules.contains(normalized)) {
            m_allowedModules << normalized;
        }
    }
    m_allowedModules.sort();
    m_organizationId = snapshot.value(QStringLiteral("organization_id")).toInt();
    m_sessionStatus = snapshot.value(QStringLiteral("session_status")).toMap();
    m_deviceSummary = snapshot.value(QStringLiteral("device_summary")).toMap();
    for (const QVariant &id : snapshot.value(QStringLiteral("aircraft_ids")).toList()) {
        const QString text = idString(id);
        if (!text.isEmpty()) {
            m_aircraftIds.insert(text);
        }
    }
    for (const QVariant &id : snapshot.value(QStringLiteral("mission_ids")).toList()) {
        const QString text = idString(id);
        if (!text.isEmpty()) {
            m_missionIds.insert(text);
        }
    }
    m_lastVerifiedAt = QDateTime::fromString(snapshot.value(QStringLiteral("last_verified_at")).toString(), Qt::ISODateWithMs);
    m_fingerprint = snapshot.value(QStringLiteral("fingerprint")).toString();
    m_status = m_accessLoaded ? QStringLiteral("Loaded last verified access profile.") : m_status;
}

void AccessManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit accessChanged();
}

void AccessManager::emitAudit(const QString &eventType, const QString &severity, const QString &message, const QVariantMap &context)
{
    if (!m_events) {
        return;
    }
    QJsonObject payload = QJsonObject::fromVariantMap(context);
    payload.insert(QStringLiteral("organization_id"), m_organizationId);
    payload.insert(QStringLiteral("access_loaded"), m_accessLoaded);
    payload.insert(QStringLiteral("session_trusted"), m_sessionTrusted);
    payload.insert(QStringLiteral("backend_reachable"), m_backendReachable);
    payload.insert(QStringLiteral("last_verified_at"), m_lastVerifiedAt.toString(Qt::ISODateWithMs));
    m_events->recordEvent(eventType, severity, message, payload);
}
