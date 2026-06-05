#include "AccessManager.h"

#include "../cache/LocalSyncCache.h"
#include "../sync/GcsEventSyncManager.h"

#include <QCryptographicHash>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaType>

#include <optional>

namespace {
constexpr qint64 OfflineAuthorizationHours = 8;

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
QStringList AccessManager::permissions() const { return m_permissions; }
QStringList AccessManager::allowedModules() const { return m_allowedModules; }
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
    const QVariantMap user = bootstrap.value(QStringLiteral("user")).toMap();
    const QVariantMap pilot = firstMapValue(bootstrap, {QStringLiteral("pilot"), QStringLiteral("pilot_profile")});
    const QVariantMap profile = firstMapValue(bootstrap, {QStringLiteral("profile"), QStringLiteral("operator_profile")});
    m_sessionStatus = bootstrap.value(QStringLiteral("session_status")).toMap();
    m_deviceSummary = firstMapValue(bootstrap, {QStringLiteral("device_summary"), QStringLiteral("device")});

    m_roles = stringListFromVariant(bootstrap.value(QStringLiteral("roles")));
    const QStringList roleCandidates{
        firstStringValue(user, {QStringLiteral("role"), QStringLiteral("role_name"), QStringLiteral("primary_role")}),
        firstStringValue(pilot, {QStringLiteral("role"), QStringLiteral("role_name"), QStringLiteral("primary_role")}),
        firstStringValue(profile, {QStringLiteral("role"), QStringLiteral("role_name"), QStringLiteral("primary_role")})
    };
    for (const QString &candidate : roleCandidates) {
        if (!candidate.isEmpty()) {
            m_roles << candidate;
        }
    }
    m_roles.removeDuplicates();
    m_role = m_roles.isEmpty() ? QString() : m_roles.first();

    QVariantMap permissionMap = bootstrap.value(QStringLiteral("permissions")).toMap();
    if (permissionMap.isEmpty()) {
        permissionMap = user.value(QStringLiteral("permissions")).toMap();
    }
    if (permissionMap.isEmpty()) {
        permissionMap = pilot.value(QStringLiteral("permissions")).toMap();
    }
    if (permissionMap.isEmpty()) {
        permissionMap = profile.value(QStringLiteral("permissions")).toMap();
    }
    m_permissions.clear();
    for (auto it = permissionMap.constBegin(); it != permissionMap.constEnd(); ++it) {
        if (it.value().toBool()) {
            m_permissions << it.key();
        }
    }
    if (m_permissions.isEmpty()) {
        m_permissions = stringListFromVariant(bootstrap.value(QStringLiteral("permissions")));
    }
    if (m_permissions.isEmpty()) {
        m_permissions = stringListFromVariant(user.value(QStringLiteral("permissions")));
    }
    if (m_permissions.isEmpty()) {
        m_permissions = stringListFromVariant(pilot.value(QStringLiteral("permissions")));
    }
    if (m_permissions.isEmpty()) {
        m_permissions = stringListFromVariant(profile.value(QStringLiteral("permissions")));
    }
    m_permissions.removeDuplicates();
    m_permissions.sort();

    m_allowedModules = stringListFromVariant(bootstrap.value(QStringLiteral("allowed_modules")));
    if (m_allowedModules.isEmpty()) {
        m_allowedModules = stringListFromVariant(bootstrap.value(QStringLiteral("allowed_gcs_modules")));
    }
    if (m_allowedModules.isEmpty()) {
        m_allowedModules = stringListFromVariant(user.value(QStringLiteral("allowed_modules")));
    }
    if (m_allowedModules.isEmpty()) {
        m_allowedModules = stringListFromVariant(pilot.value(QStringLiteral("allowed_modules")));
    }
    if (m_allowedModules.isEmpty()) {
        m_allowedModules = stringListFromVariant(profile.value(QStringLiteral("allowed_modules")));
    }
    m_allowedModules.removeDuplicates();
    m_allowedModules.sort();

    QVariantMap organization = bootstrap.value(QStringLiteral("organization")).toMap();
    if (organization.isEmpty()) {
        organization = firstMapValue(user, {QStringLiteral("organization"), QStringLiteral("organization_profile")});
    }
    if (organization.isEmpty()) {
        organization = firstMapValue(pilot, {QStringLiteral("organization"), QStringLiteral("organization_profile")});
    }
    if (organization.isEmpty()) {
        organization = firstMapValue(profile, {QStringLiteral("organization"), QStringLiteral("organization_profile")});
    }
    m_organizationId = organization.value(QStringLiteral("id")).toInt();
    m_aircraftIds = idsFromList(bootstrap.value(QStringLiteral("assigned_aircraft")).toList(),
                                {QStringLiteral("id"), QStringLiteral("aircraft_id")});
    QVariantList missions = bootstrap.value(QStringLiteral("missions")).toList();
    missions += bootstrap.value(QStringLiteral("active_missions")).toList();
    missions += bootstrap.value(QStringLiteral("mission_history")).toList();
    missions += bootstrap.value(QStringLiteral("approved_missions")).toList();
    m_missionIds = idsFromList(missions, {QStringLiteral("id"), QStringLiteral("mission_id")});
    const auto sessionTrusted = boolValue(m_sessionStatus,
                                          {QStringLiteral("operations_allowed"),
                                           QStringLiteral("trusted"),
                                           QStringLiteral("device_trusted"),
                                           QStringLiteral("session_trusted")});
    const auto deviceTrusted = boolValue(m_deviceSummary,
                                         {QStringLiteral("trusted"),
                                          QStringLiteral("is_trusted"),
                                          QStringLiteral("approved"),
                                          QStringLiteral("device_trusted")});
    const auto reachable = boolValue(m_sessionStatus,
                                     {QStringLiteral("control_center_reachable"),
                                      QStringLiteral("backend_reachable"),
                                      QStringLiteral("reachable"),
                                      QStringLiteral("online")});
    if (sessionTrusted.has_value()) {
        m_sessionTrusted = sessionTrusted.value();
    }
    if (deviceTrusted.has_value()) {
        m_sessionTrusted = m_sessionTrusted && deviceTrusted.value();
    }
    if (reachable.has_value()) {
        m_backendReachable = reachable.value();
    }
    m_lastVerifiedAt = QDateTime::currentDateTimeUtc();
    m_accessLoaded = !m_permissions.isEmpty() || !m_allowedModules.isEmpty();
    m_fingerprint = accessFingerprint(bootstrap);
    persistSnapshot();

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
    return m_accessLoaded && m_permissions.contains(permission);
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
    return m_accessLoaded && m_allowedModules.contains(module);
}

bool AccessManager::canPerform(const QString &action) const
{
    const QString normalized = normalizedAction(action);
    if (normalized == QStringLiteral("vehicle_profile_setup")) {
        if (!canAny({QStringLiteral("can_register_vehicle"), QStringLiteral("can_edit_vehicle_profile")})) {
            return false;
        }
        return !actionRequiresTrustedSession(normalized)
            || (m_sessionTrusted && (m_backendReachable || actionAllowsOfflineAuthorization(normalized)));
    }
    const QString permission = permissionForAction(action);
    if (permission.isEmpty() || !can(permission)) {
        return false;
    }
    if (actionRequiresTrustedSession(action)) {
        return m_sessionTrusted && (m_backendReachable || actionAllowsOfflineAuthorization(action));
    }
    return true;
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
    const QString permission = permissionForAction(normalized);
    QString reason;
    if (normalized == QStringLiteral("vehicle_profile_setup")) {
        if (!m_accessLoaded) {
            reason = QStringLiteral("Access profile is not loaded.");
        } else if (!canAny({QStringLiteral("can_register_vehicle"), QStringLiteral("can_edit_vehicle_profile")})) {
            reason = QStringLiteral("Missing permission: can_register_vehicle or can_edit_vehicle_profile");
        } else if (actionRequiresTrustedSession(normalized) && !m_sessionTrusted) {
            reason = QStringLiteral("Trusted device session is required.");
        } else if (actionRequiresTrustedSession(normalized) && !m_backendReachable && !actionAllowsOfflineAuthorization(normalized)) {
            reason = QStringLiteral("Online authorization is required.");
        }
    } else if (permission.isEmpty()) {
        reason = QStringLiteral("No permission mapping for action.");
    } else if (!m_accessLoaded) {
        reason = QStringLiteral("Access profile is not loaded.");
    } else if (!can(permission)) {
        reason = QStringLiteral("Missing permission: %1").arg(permission);
    } else if (actionRequiresTrustedSession(normalized) && !m_sessionTrusted) {
        reason = QStringLiteral("Trusted device session is required.");
    } else if (actionRequiresTrustedSession(normalized) && !m_backendReachable && !actionAllowsOfflineAuthorization(normalized)) {
        reason = QStringLiteral("Online authorization is required.");
    }
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
        {QStringLiteral("manual_flight"), QStringLiteral("can_fly_manual")},
        {QStringLiteral("aircraft_connection"), QStringLiteral("can_stream_telemetry")},
        {QStringLiteral("telemetry_stream"), QStringLiteral("can_stream_telemetry")},
        {QStringLiteral("telemetry_export"), QStringLiteral("can_view_reports")},
        {QStringLiteral("aircraft_profile_access"), QStringLiteral("can_view_fleet")},
        {QStringLiteral("vehicle_parameter_read"), QStringLiteral("can_read_vehicle_parameters")},
        {QStringLiteral("vehicle_parameter_write"), QStringLiteral("can_write_vehicle_parameters")},
        {QStringLiteral("emergency_stop"), QStringLiteral("can_fly_manual")},
        {QStringLiteral("firmware_manager"), QStringLiteral("can_configure_vehicle")},
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
        QStringLiteral("manual_flight"),
        QStringLiteral("aircraft_connection"),
        QStringLiteral("telemetry_stream"),
        QStringLiteral("telemetry_export"),
        QStringLiteral("vehicle_parameter_read"),
        QStringLiteral("vehicle_parameter_write"),
        QStringLiteral("emergency_stop"),
        QStringLiteral("firmware_manager"),
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
        || key == QStringLiteral("settings");
}

void AccessManager::persistSnapshot()
{
    if (!m_cache) {
        return;
    }
    QVariantMap snapshot{
        {QStringLiteral("access_loaded"), m_accessLoaded},
        {QStringLiteral("role"), m_role},
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
    m_role = snapshot.value(QStringLiteral("role")).toString();
    m_roles = stringListFromVariant(snapshot.value(QStringLiteral("roles")));
    m_permissions = stringListFromVariant(snapshot.value(QStringLiteral("permissions")));
    m_allowedModules = stringListFromVariant(snapshot.value(QStringLiteral("allowed_modules")));
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
