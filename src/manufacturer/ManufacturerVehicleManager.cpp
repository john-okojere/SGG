#include "ManufacturerVehicleManager.h"

#include "../auth/SessionManager.h"
#include "../network/ApiClient.h"
#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"

#include <QJsonArray>
#include <QJsonObject>

ManufacturerVehicleManager::ManufacturerVehicleManager(ApiClient *api,
                                                       SessionManager *session,
                                                       AccessManager *access,
                                                       GcsEventSyncManager *events,
                                                       QObject *parent)
    : QObject(parent), m_api(api), m_session(session), m_access(access), m_events(events)
{
}

bool ManufacturerVehicleManager::loading() const { return m_loading; }
QString ManufacturerVehicleManager::status() const { return m_status; }
QString ManufacturerVehicleManager::error() const { return m_error; }
QVariantList ManufacturerVehicleManager::profiles() const { return m_profiles; }
QVariantMap ManufacturerVehicleManager::selectedProfile() const { return m_selectedProfile; }
QVariantList ManufacturerVehicleManager::auditRecords() const { return m_auditRecords; }

void ManufacturerVehicleManager::fetchVehicleProfiles()
{
    if (!ensureOnlineAction(QStringLiteral("manufacturer_tools"),
                            {QStringLiteral("can_configure_vehicle"),
                             QStringLiteral("can_register_vehicle"),
                             QStringLiteral("can_edit_vehicle_profile"),
                             QStringLiteral("can_bind_flight_controller"),
                             QStringLiteral("can_configure_rc"),
                             QStringLiteral("can_read_vehicle_parameters"),
                             QStringLiteral("can_release_vehicle_to_organization")},
                            QStringLiteral("Vehicle profile list blocked by local permissions."))) {
        return;
    }
    setLoading(true);
    setStatus(QStringLiteral("Fetching vehicle profiles from Control Center..."));
    m_api->get(QStringLiteral("/api/vehicle/profiles/"), true, true,
               [this](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("Vehicle profile fetch failed: %1").arg(statusCode) : error);
            return;
        }
        m_profiles = listFromBody(body);
        if (m_selectedProfile.isEmpty() && !m_profiles.isEmpty()) {
            m_selectedProfile = m_profiles.first().toMap();
        } else if (!m_selectedProfile.isEmpty()) {
            const QString selectedId = idString(m_selectedProfile.value(QStringLiteral("id")));
            for (const QVariant &entry : std::as_const(m_profiles)) {
                const QVariantMap profile = entry.toMap();
                if (idString(profile.value(QStringLiteral("id"))) == selectedId) {
                    m_selectedProfile = profile;
                    break;
                }
            }
        }
        setError(QString());
        setStatus(QStringLiteral("Vehicle profiles synchronized."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_vehicle_profiles_fetched"), QStringLiteral("info"), QStringLiteral("Manufacturer vehicle profiles fetched"), {{QStringLiteral("count"), m_profiles.size()}});
    });
}

void ManufacturerVehicleManager::fetchProfileDetail(const QVariant &profileId)
{
    const QString id = idString(profileId);
    if (id.isEmpty()) {
        setError(QStringLiteral("Select a vehicle profile first."));
        return;
    }
    if (!profileInScope(id)) {
        setError(QStringLiteral("Vehicle profile detail blocked: profile is outside scope."));
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("manufacturer_tools"),
                                    m_error,
                                    {{QStringLiteral("profile_id"), id}});
        }
        return;
    }
    if (!ensureOnlineAction(QStringLiteral("manufacturer_tools"),
                            {QStringLiteral("can_configure_vehicle"),
                             QStringLiteral("can_edit_vehicle_profile"),
                             QStringLiteral("can_read_vehicle_parameters")},
                            QStringLiteral("Vehicle profile detail blocked by local permissions."),
                            {{QStringLiteral("profile_id"), id}})) {
        return;
    }
    setLoading(true);
    setStatus(QStringLiteral("Fetching vehicle profile detail..."));
    m_api->get(QStringLiteral("/api/vehicle/profiles/%1/").arg(id), true, true,
               [this, id](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("Vehicle profile detail failed: %1").arg(statusCode) : error);
            return;
        }
        const QVariantMap profile = body.toVariantMap();
        m_selectedProfile = profile;
        upsertProfile(profile);
        setError(QString());
        setStatus(QStringLiteral("Vehicle profile detail synchronized."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_vehicle_profile_detail_fetched"), QStringLiteral("info"), QStringLiteral("Manufacturer vehicle profile detail fetched"), {{QStringLiteral("profile_id"), id}});
    });
}

void ManufacturerVehicleManager::selectProfile(const QVariant &profileId)
{
    const QString id = idString(profileId);
    for (const QVariant &entry : std::as_const(m_profiles)) {
        const QVariantMap profile = entry.toMap();
        if (idString(profile.value(QStringLiteral("id"))) == id) {
            m_selectedProfile = profile;
            emit dataChanged();
            fetchProfileDetail(id);
            return;
        }
    }
    setError(QStringLiteral("Selected vehicle profile is outside the local manufacturer scope."));
    if (m_access) {
        m_access->recordBlocked(QStringLiteral("manufacturer_tools"),
                                QStringLiteral("Selected vehicle profile is outside the local manufacturer scope."),
                                {{QStringLiteral("profile_id"), id}});
    }
}

void ManufacturerVehicleManager::createVehicleProfile(const QVariantMap &profile)
{
    if (!ensureOnlineAction(QStringLiteral("vehicle_profile_setup"),
                            {QStringLiteral("can_register_vehicle")},
                            QStringLiteral("Vehicle profile creation blocked by local permissions."))) {
        return;
    }
    setLoading(true);
    setStatus(QStringLiteral("Creating vehicle profile..."));
    m_api->post(QStringLiteral("/api/vehicle/profiles/"), QJsonObject::fromVariantMap(profile), true, true,
                [this](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("Vehicle profile creation failed: %1").arg(statusCode) : error);
            return;
        }
        const QVariantMap profile = body.toVariantMap();
        m_selectedProfile = profile;
        upsertProfile(profile);
        setError(QString());
        setStatus(QStringLiteral("Vehicle profile created."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_vehicle_profile_created"), QStringLiteral("info"), QStringLiteral("Manufacturer vehicle profile created"), {{QStringLiteral("profile_id"), idString(profile.value(QStringLiteral("id")))}});
    });
}

void ManufacturerVehicleManager::updateVehicleProfile(const QVariant &profileId, const QVariantMap &profile)
{
    const QString id = idString(profileId);
    if (!profileInScope(id)) {
        setError(QStringLiteral("Vehicle profile update blocked: profile is outside scope."));
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("vehicle_profile_setup"), m_error, {{QStringLiteral("profile_id"), id}});
        }
        return;
    }
    if (!ensureOnlineAction(QStringLiteral("vehicle_profile_setup"),
                            {QStringLiteral("can_edit_vehicle_profile")},
                            QStringLiteral("Vehicle profile update blocked by local permissions."),
                            {{QStringLiteral("profile_id"), id}})) {
        return;
    }
    setLoading(true);
    setStatus(QStringLiteral("Updating vehicle profile..."));
    m_api->patch(QStringLiteral("/api/vehicle/profiles/%1/").arg(id), QJsonObject::fromVariantMap(profile), true, true,
                 [this, id](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("Vehicle profile update failed: %1").arg(statusCode) : error);
            return;
        }
        const QVariantMap profile = body.toVariantMap();
        m_selectedProfile = profile;
        upsertProfile(profile);
        setError(QString());
        setStatus(QStringLiteral("Vehicle profile updated."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_vehicle_profile_updated"), QStringLiteral("info"), QStringLiteral("Manufacturer vehicle profile updated"), {{QStringLiteral("profile_id"), id}});
    });
}

void ManufacturerVehicleManager::bindFlightController(const QVariant &profileId, const QString &uid, const QString &autopilot)
{
    const QString id = idString(profileId);
    if (!profileInScope(id)) {
        setError(QStringLiteral("Flight controller binding blocked: profile is outside scope."));
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("flight_controller_binding"), m_error, {{QStringLiteral("profile_id"), id}});
        }
        return;
    }
    if (uid.trimmed().isEmpty()) {
        setError(QStringLiteral("Flight controller UID is required."));
        return;
    }
    if (!ensureOnlineAction(QStringLiteral("flight_controller_binding"),
                            {QStringLiteral("can_bind_flight_controller")},
                            QStringLiteral("Flight controller binding blocked by local permissions."),
                            {{QStringLiteral("profile_id"), id}})) {
        return;
    }
    QJsonObject payload{{QStringLiteral("flight_controller_uid"), uid.trimmed()}};
    if (!autopilot.trimmed().isEmpty()) {
        payload.insert(QStringLiteral("autopilot"), autopilot.trimmed());
    }
    setLoading(true);
    setStatus(QStringLiteral("Binding flight controller..."));
    m_api->post(QStringLiteral("/api/vehicle/profiles/%1/bind-flight-controller/").arg(id), payload, true, true,
                [this, id](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("Flight controller binding failed: %1").arg(statusCode) : error);
            return;
        }
        const QVariantMap profile = body.toVariantMap();
        m_selectedProfile = profile;
        upsertProfile(profile);
        setError(QString());
        setStatus(QStringLiteral("Flight controller bound."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_flight_controller_bound"), QStringLiteral("info"), QStringLiteral("Flight controller bound from GCS"), {{QStringLiteral("profile_id"), id}});
    });
}

void ManufacturerVehicleManager::saveRcMapping(const QVariant &profileId, const QVariantMap &mapping)
{
    const QString id = idString(profileId);
    if (!profileInScope(id)) {
        setError(QStringLiteral("RC mapping blocked: profile is outside scope."));
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("rc_mapping"), m_error, {{QStringLiteral("profile_id"), id}});
        }
        return;
    }
    if (!ensureOnlineAction(QStringLiteral("rc_mapping"),
                            {QStringLiteral("can_configure_rc")},
                            QStringLiteral("RC mapping save blocked by local permissions."),
                            {{QStringLiteral("profile_id"), id}})) {
        return;
    }
    setLoading(true);
    setStatus(QStringLiteral("Saving RC mapping..."));
    m_api->post(QStringLiteral("/api/vehicle/profiles/%1/save-rc-mapping/").arg(id),
                QJsonObject{{QStringLiteral("rc_mapping"), QJsonObject::fromVariantMap(mapping)}},
                true,
                true,
                [this, id](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("RC mapping save failed: %1").arg(statusCode) : error);
            return;
        }
        const QVariantMap profile = body.toVariantMap();
        m_selectedProfile = profile;
        upsertProfile(profile);
        setError(QString());
        setStatus(QStringLiteral("RC mapping saved."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_rc_mapping_saved"), QStringLiteral("info"), QStringLiteral("RC mapping saved from GCS"), {{QStringLiteral("profile_id"), id}});
    });
}

void ManufacturerVehicleManager::saveParameterSnapshot(const QVariant &profileId, const QVariantMap &snapshot)
{
    const QString id = idString(profileId);
    if (!profileInScope(id)) {
        setError(QStringLiteral("Parameter snapshot blocked: profile is outside scope."));
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("vehicle_parameter_write"), m_error, {{QStringLiteral("profile_id"), id}});
        }
        return;
    }
    if (!ensureOnlineAction(QStringLiteral("vehicle_parameter_write"),
                            {QStringLiteral("can_write_vehicle_parameters")},
                            QStringLiteral("Parameter snapshot save blocked by local permissions."),
                            {{QStringLiteral("profile_id"), id}})) {
        return;
    }
    setLoading(true);
    setStatus(QStringLiteral("Saving parameter snapshot..."));
    m_api->post(QStringLiteral("/api/vehicle/profiles/%1/save-parameter-snapshot/").arg(id),
                QJsonObject{{QStringLiteral("parameter_snapshot"), QJsonObject::fromVariantMap(snapshot)}},
                true,
                true,
                [this, id](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("Parameter snapshot save failed: %1").arg(statusCode) : error);
            return;
        }
        const QVariantMap profile = body.toVariantMap();
        m_selectedProfile = profile;
        upsertProfile(profile);
        setError(QString());
        setStatus(QStringLiteral("Parameter snapshot saved."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_parameter_snapshot_saved"), QStringLiteral("info"), QStringLiteral("Parameter snapshot saved from GCS"), {{QStringLiteral("profile_id"), id}});
    });
}

void ManufacturerVehicleManager::releaseVehicleToOrganization(const QVariant &profileId, const QVariant &organizationId, const QString &notes)
{
    const QString id = idString(profileId);
    const QString orgId = idString(organizationId);
    if (!profileInScope(id)) {
        setError(QStringLiteral("Vehicle release blocked: profile is outside scope."));
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("vehicle_release_lock"), m_error, {{QStringLiteral("profile_id"), id}});
        }
        return;
    }
    if (orgId.isEmpty()) {
        setError(QStringLiteral("Organization id is required to release a vehicle."));
        return;
    }
    if (!ensureOnlineAction(QStringLiteral("vehicle_release_lock"),
                            {QStringLiteral("can_release_vehicle_to_organization")},
                            QStringLiteral("Vehicle release blocked by local permissions."),
                            {{QStringLiteral("profile_id"), id}, {QStringLiteral("organization_id"), orgId}})) {
        return;
    }
    QJsonObject payload{{QStringLiteral("organization_id"), orgId}, {QStringLiteral("notes"), notes}};
    setLoading(true);
    setStatus(QStringLiteral("Releasing vehicle to organization..."));
    m_api->post(QStringLiteral("/api/vehicle/profiles/%1/release-to-organization/").arg(id), payload, true, true,
                [this, id](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("Vehicle release failed: %1").arg(statusCode) : error);
            return;
        }
        const QVariantMap profile = body.toVariantMap();
        m_selectedProfile = profile;
        upsertProfile(profile);
        setError(QString());
        setStatus(QStringLiteral("Vehicle released to organization."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_vehicle_released"), QStringLiteral("info"), QStringLiteral("Vehicle released from GCS"), {{QStringLiteral("profile_id"), id}});
    });
}

void ManufacturerVehicleManager::lockVehicle(const QVariant &profileId)
{
    const QString id = idString(profileId);
    if (!profileInScope(id)) {
        setError(QStringLiteral("Vehicle lock blocked: profile is outside scope."));
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("vehicle_release_lock"), m_error, {{QStringLiteral("profile_id"), id}});
        }
        return;
    }
    if (!ensureOnlineAction(QStringLiteral("vehicle_release_lock"),
                            {QStringLiteral("can_edit_vehicle_profile")},
                            QStringLiteral("Vehicle lock blocked by local permissions."),
                            {{QStringLiteral("profile_id"), id}})) {
        return;
    }
    setLoading(true);
    setStatus(QStringLiteral("Locking vehicle profile..."));
    m_api->post(QStringLiteral("/api/vehicle/profiles/%1/lock/").arg(id), {}, true, true,
                [this, id](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("Vehicle lock failed: %1").arg(statusCode) : error);
            return;
        }
        const QVariantMap profile = body.toVariantMap();
        m_selectedProfile = profile;
        upsertProfile(profile);
        setError(QString());
        setStatus(QStringLiteral("Vehicle profile locked."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_vehicle_locked"), QStringLiteral("info"), QStringLiteral("Vehicle locked from GCS"), {{QStringLiteral("profile_id"), id}});
    });
}

void ManufacturerVehicleManager::fetchVehicleAudit(const QVariant &profileId)
{
    const QString id = idString(profileId);
    if (!profileInScope(id)) {
        setError(QStringLiteral("Vehicle audit blocked: profile is outside scope."));
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("security_audit"), m_error, {{QStringLiteral("profile_id"), id}});
        }
        return;
    }
    if (!ensureOnlineAction(QStringLiteral("security_audit"),
                            {QStringLiteral("can_view_vehicle_audit")},
                            QStringLiteral("Vehicle audit blocked by local permissions."),
                            {{QStringLiteral("profile_id"), id}})) {
        return;
    }
    setLoading(true);
    setStatus(QStringLiteral("Fetching vehicle audit..."));
    m_api->get(QStringLiteral("/api/vehicle/profiles/%1/audit/").arg(id), true, true,
               [this, id](int statusCode, const QJsonObject &body, const QString &error) {
        setLoading(false);
        if (statusCode < 200 || statusCode >= 300) {
            setError(error.isEmpty() ? QStringLiteral("Vehicle audit fetch failed: %1").arg(statusCode) : error);
            return;
        }
        m_auditRecords = listFromBody(body);
        setError(QString());
        setStatus(QStringLiteral("Vehicle audit synchronized."));
        emit dataChanged();
        recordEvent(QStringLiteral("manufacturer_vehicle_audit_fetched"), QStringLiteral("info"), QStringLiteral("Vehicle audit fetched from GCS"), {{QStringLiteral("profile_id"), id}, {QStringLiteral("count"), m_auditRecords.size()}});
    });
}

bool ManufacturerVehicleManager::ensureOnlineAction(const QString &action,
                                                    const QStringList &permissions,
                                                    const QString &message,
                                                    const QVariantMap &context)
{
    if (!m_api || !m_session || !m_access) {
        setError(QStringLiteral("Manufacturer vehicle services are not initialized."));
        return false;
    }
    if (!m_access->canAny(permissions)) {
        m_access->recordBlocked(action, QStringLiteral("Missing manufacturer vehicle permission."), context);
        setError(message);
        return false;
    }
    if (!m_session->operationsAllowed()) {
        setError(m_session->blockReason().isEmpty() ? QStringLiteral("Trusted Control Center session is required.") : m_session->blockReason());
        m_access->recordBlocked(action, m_error, context);
        return false;
    }
    m_access->recordAllowed(action, context);
    return true;
}

bool ManufacturerVehicleManager::profileInScope(const QString &profileId) const
{
    if (profileId.isEmpty()) {
        return false;
    }
    if (idString(m_selectedProfile.value(QStringLiteral("id"))) == profileId) {
        return true;
    }
    for (const QVariant &entry : m_profiles) {
        if (idString(entry.toMap().value(QStringLiteral("id"))) == profileId) {
            return true;
        }
    }
    return false;
}

QString ManufacturerVehicleManager::idString(const QVariant &value) const
{
    if (!value.isValid() || value.isNull()) {
        return {};
    }
    return value.toString().trimmed();
}

QVariantList ManufacturerVehicleManager::listFromBody(const QJsonObject &body) const
{
    const QJsonValue results = body.value(QStringLiteral("results"));
    if (results.isArray()) {
        return results.toArray().toVariantList();
    }
    const QJsonValue data = body.value(QStringLiteral("data"));
    if (data.isArray()) {
        return data.toArray().toVariantList();
    }
    return {};
}

void ManufacturerVehicleManager::upsertProfile(const QVariantMap &profile)
{
    const QString id = idString(profile.value(QStringLiteral("id")));
    if (id.isEmpty()) {
        return;
    }
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (idString(m_profiles.at(i).toMap().value(QStringLiteral("id"))) == id) {
            m_profiles[i] = profile;
            return;
        }
    }
    m_profiles.prepend(profile);
}

void ManufacturerVehicleManager::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    emit stateChanged();
}

void ManufacturerVehicleManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit stateChanged();
}

void ManufacturerVehicleManager::setError(const QString &error)
{
    if (m_error == error) {
        return;
    }
    m_error = error;
    emit stateChanged();
}

void ManufacturerVehicleManager::recordEvent(const QString &eventType, const QString &severity, const QString &message, const QVariantMap &payload)
{
    if (m_events) {
        m_events->recordEvent(eventType, severity, message, QJsonObject::fromVariantMap(payload));
    }
}
