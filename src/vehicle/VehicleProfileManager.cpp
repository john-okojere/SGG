#include "VehicleProfileManager.h"

#include "../access/PermissionManager.h"
#include "../auth/SessionManager.h"
#include "../network/ApiClient.h"

#include <QJsonArray>
#include <QJsonObject>

namespace {

QString profileIdFromMap(const QVariantMap &profile)
{
    const QVariant id = profile.value(QStringLiteral("id"));
    if (id.isValid() && !id.isNull()) {
        return id.toString();
    }
    return profile.value(QStringLiteral("local_id")).toString();
}

QVariantList resultList(const QJsonObject &body)
{
    QVariantList results = body.value(QStringLiteral("results")).toArray().toVariantList();
    if (results.isEmpty()) {
        results = body.value(QStringLiteral("profiles")).toArray().toVariantList();
    }
    return results;
}

} // namespace

VehicleProfileManager::VehicleProfileManager(ApiClient *api,
                                             SessionManager *session,
                                             PermissionManager *permissions,
                                             QObject *parent)
    : QObject(parent),
      m_api(api),
      m_session(session),
      m_permissions(permissions)
{
}

QVariantList VehicleProfileManager::profiles() const { return m_profiles; }
QVariantMap VehicleProfileManager::activeProfile() const { return m_activeProfile; }
bool VehicleProfileManager::busy() const { return m_busy; }
QString VehicleProfileManager::status() const { return m_status; }

void VehicleProfileManager::applyBootstrap(const QVariantMap &bootstrap)
{
    QVariantList profiles = bootstrap.value(QStringLiteral("vehicle_profiles")).toList();
    if (profiles.isEmpty()) {
        profiles = bootstrap.value(QStringLiteral("vehicleProfiles")).toList();
    }
    setProfiles(profiles);
    if (!profiles.isEmpty() && m_activeProfile.isEmpty()) {
        setActiveProfile(profiles.first().toMap());
    }
    setStatus(profiles.isEmpty()
                  ? QStringLiteral("No vehicle profiles assigned to this workspace.")
                  : QStringLiteral("Vehicle profiles synchronized."));
}

void VehicleProfileManager::refreshProfiles()
{
    if (!requirePermission({QStringLiteral("can_configure_vehicle"), QStringLiteral("can_view_fleet")},
                           QStringLiteral("Vehicle profile refresh is not permitted."))) {
        return;
    }
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Vehicle profile refresh blocked: Control Center session unavailable."));
        return;
    }
    setBusy(true);
    setStatus(QStringLiteral("Refreshing vehicle profiles..."));
    m_api->get(QStringLiteral("/api/vehicle/profiles/"), true, true,
               [this](int statusCode, const QJsonObject &body, const QString &error) {
        setBusy(false);
        if (statusCode < 200 || statusCode >= 300) {
            setStatus(error.isEmpty() ? QStringLiteral("Vehicle profile refresh failed.") : error);
            return;
        }
        const QVariantList profiles = resultList(body);
        setProfiles(profiles);
        if (!profiles.isEmpty()) {
            const QString activeId = activeProfileId();
            QVariantMap selected;
            for (const QVariant &entry : profiles) {
                const QVariantMap profile = entry.toMap();
                if (profileIdFromMap(profile) == activeId) {
                    selected = profile;
                    break;
                }
            }
            setActiveProfile(selected.isEmpty() ? profiles.first().toMap() : selected);
        } else {
            setActiveProfile({});
        }
        setStatus(QStringLiteral("Vehicle profiles refreshed."));
    });
}

void VehicleProfileManager::selectProfile(const QString &profileId)
{
    const QString target = profileId.trimmed();
    if (target.isEmpty()) {
        setActiveProfile({});
        setStatus(QStringLiteral("New vehicle profile draft."));
        return;
    }
    for (const QVariant &entry : m_profiles) {
        const QVariantMap profile = entry.toMap();
        if (profileIdFromMap(profile) == target) {
            setActiveProfile(profile);
            setStatus(QStringLiteral("Vehicle profile selected."));
            return;
        }
    }
    setStatus(QStringLiteral("Vehicle profile not found."));
}

void VehicleProfileManager::saveProfile(const QVariantMap &profile)
{
    if (!requirePermission({QStringLiteral("can_edit_vehicle_profile"), QStringLiteral("can_register_vehicle")},
                           QStringLiteral("Vehicle profile save is not permitted."))) {
        return;
    }
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Vehicle profile save blocked: Control Center session unavailable."));
        return;
    }

    const QString profileId = profileIdFromMap(profile);
    const bool updating = !profileId.isEmpty();
    setBusy(true);
    setStatus(updating ? QStringLiteral("Saving vehicle profile...") : QStringLiteral("Registering vehicle profile..."));
    const QJsonObject payload = QJsonObject::fromVariantMap(profile);
    const QString path = updating
        ? QStringLiteral("/api/vehicle/profiles/%1/").arg(profileId)
        : QStringLiteral("/api/vehicle/profiles/");

    const auto callback = [this](int statusCode, const QJsonObject &body, const QString &error) {
        setBusy(false);
        if (statusCode < 200 || statusCode >= 300) {
            setStatus(error.isEmpty() ? QStringLiteral("Vehicle profile save failed.") : error);
            return;
        }
        setActiveProfile(body.toVariantMap());
        refreshProfiles();
        setStatus(QStringLiteral("Vehicle profile saved."));
    };

    if (updating) {
        m_api->patch(path, payload, true, true, callback);
    } else {
        m_api->post(path, payload, true, true, callback);
    }
}

QString VehicleProfileManager::activeProfileId() const
{
    return profileIdFromMap(m_activeProfile);
}

bool VehicleProfileManager::requirePermission(const QStringList &permissions, const QString &message)
{
    if (!m_permissions) {
        setStatus(message);
        return false;
    }
    for (const QString &permission : permissions) {
        if (m_permissions->hasPermission(permission)) {
            return true;
        }
    }
    setStatus(message);
    return false;
}

void VehicleProfileManager::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit profilesChanged();
}

void VehicleProfileManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit profilesChanged();
}

void VehicleProfileManager::setProfiles(const QVariantList &profiles)
{
    if (m_profiles == profiles) {
        return;
    }
    m_profiles = profiles;
    emit profilesChanged();
}

void VehicleProfileManager::setActiveProfile(const QVariantMap &profile)
{
    if (m_activeProfile == profile) {
        return;
    }
    m_activeProfile = profile;
    emit profilesChanged();
}
