#include "PermissionManager.h"

#include "RbacBootstrapNormalizer.h"

#include <QDebug>
#include <QMetaType>
#include <QProcessEnvironment>
#include <QVariant>

namespace {

QString normalizedKey(QString value)
{
    value = value.trimmed();
    value.replace(QLatin1Char('-'), QLatin1Char('_'));
    value.replace(QLatin1Char(' '), QLatin1Char('_'));
    return value.toLower();
}

bool truthy(const QVariant &value)
{
    if (value.metaType().id() == QMetaType::Bool) {
        return value.toBool();
    }
    const QString text = value.toString().trimmed().toLower();
    return text == QStringLiteral("1")
        || text == QStringLiteral("true")
        || text == QStringLiteral("yes")
        || text == QStringLiteral("allowed");
}

bool rbacDebugEnabled()
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    return env.value(QStringLiteral("SKYGRID_RBAC_DEBUG")) == QStringLiteral("true")
        || env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true");
}

} // namespace

PermissionManager::PermissionManager(QObject *parent)
    : QObject(parent)
{
}

QVariantMap PermissionManager::permissions() const
{
    return m_permissions;
}

QStringList PermissionManager::permissionKeys() const
{
    return m_permissions.keys();
}

bool PermissionManager::canConfigureVehicle() const { return hasPermission(QStringLiteral("can_configure_vehicle")); }
bool PermissionManager::canRegisterVehicle() const { return hasPermission(QStringLiteral("can_register_vehicle")); }
bool PermissionManager::canBindFlightController() const { return hasPermission(QStringLiteral("can_bind_flight_controller")); }
bool PermissionManager::canConfigureRc() const { return hasPermission(QStringLiteral("can_configure_rc")); }
bool PermissionManager::canEditVehicleProfile() const { return hasPermission(QStringLiteral("can_edit_vehicle_profile")); }
bool PermissionManager::canReadVehicleParameters() const { return hasPermission(QStringLiteral("can_read_vehicle_parameters")); }
bool PermissionManager::canWriteVehicleParameters() const { return hasPermission(QStringLiteral("can_write_vehicle_parameters")); }
bool PermissionManager::canReleaseVehicleToOrganization() const { return hasPermission(QStringLiteral("can_release_vehicle_to_organization")); }
bool PermissionManager::canPlanMission() const { return hasPermission(QStringLiteral("can_plan_mission")); }
bool PermissionManager::canUploadMission() const { return hasPermission(QStringLiteral("can_upload_mission")); }
bool PermissionManager::canStartMission() const { return hasPermission(QStringLiteral("can_start_mission")); }
bool PermissionManager::canFlyManual() const { return hasPermission(QStringLiteral("can_fly_manual")); }
bool PermissionManager::canFlyManualTest() const { return hasPermission(QStringLiteral("can_fly_manual_test")); }
bool PermissionManager::canRunManufacturerTestFlight() const { return hasPermission(QStringLiteral("can_run_manufacturer_test_flight")); }
bool PermissionManager::canStreamTelemetry() const { return hasPermission(QStringLiteral("can_stream_telemetry")); }
bool PermissionManager::canAssignAircraft() const { return hasPermission(QStringLiteral("can_assign_aircraft")); }
bool PermissionManager::canAssignPilots() const { return hasPermission(QStringLiteral("can_assign_pilots")); }
bool PermissionManager::canViewFleet() const { return hasPermission(QStringLiteral("can_view_fleet")); }
bool PermissionManager::canViewMissionLogs() const { return hasPermission(QStringLiteral("can_view_mission_logs")); }
bool PermissionManager::canViewTelemetry() const { return hasPermission(QStringLiteral("can_view_telemetry")); }
bool PermissionManager::canViewReports() const { return hasPermission(QStringLiteral("can_view_reports")); }
bool PermissionManager::canApproveDevices() const { return hasPermission(QStringLiteral("can_approve_devices")); }
bool PermissionManager::canRevokeDevices() const { return hasPermission(QStringLiteral("can_revoke_devices")); }
bool PermissionManager::canManageUsers() const { return hasPermission(QStringLiteral("can_manage_users")); }
bool PermissionManager::canManageRoles() const { return hasPermission(QStringLiteral("can_manage_roles")); }
bool PermissionManager::canManageManufacturers() const { return hasPermission(QStringLiteral("can_manage_manufacturers")); }
bool PermissionManager::canManageOrganizations() const { return hasPermission(QStringLiteral("can_manage_organizations")); }
bool PermissionManager::canUseFlightData() const { return hasPermission(QStringLiteral("can_use_flight_data")); }
bool PermissionManager::canRunInitialSetup() const { return hasPermission(QStringLiteral("can_run_initial_setup")); }
bool PermissionManager::canTuneVehicle() const { return hasPermission(QStringLiteral("can_tune_vehicle")); }
bool PermissionManager::canFlashFirmware() const { return hasPermission(QStringLiteral("can_flash_firmware")); }
bool PermissionManager::canViewLogs() const { return hasPermission(QStringLiteral("can_view_logs")); }
bool PermissionManager::canDownloadLogs() const { return hasPermission(QStringLiteral("can_download_logs")); }
bool PermissionManager::canUseSimulation() const { return hasPermission(QStringLiteral("can_use_simulation")); }
bool PermissionManager::canUseAdvancedMavlink() const { return hasPermission(QStringLiteral("can_use_advanced_mavlink")); }
bool PermissionManager::canConfigurePayload() const { return hasPermission(QStringLiteral("can_configure_payload")); }
bool PermissionManager::canManageMultiVehicle() const { return hasPermission(QStringLiteral("can_manage_multi_vehicle")); }
bool PermissionManager::canConfigureOptionalHardware() const { return hasPermission(QStringLiteral("can_configure_optional_hardware")); }
bool PermissionManager::canViewVideoStream() const { return hasPermission(QStringLiteral("can_view_video_stream")); }
bool PermissionManager::canConfigureVideoPayload() const { return hasPermission(QStringLiteral("can_configure_video_payload")); }
bool PermissionManager::canUseTerminal() const { return hasPermission(QStringLiteral("can_use_terminal")); }
bool PermissionManager::canOverrideParameterSafety() const { return hasPermission(QStringLiteral("can_override_parameter_safety")); }

void PermissionManager::applyBootstrap(const QVariantMap &bootstrap)
{
    const RbacBootstrapProfile profile = RbacBootstrapNormalizer::fromBootstrap(bootstrap);
    setPermissionMap(profile.permissions);
    if (rbacDebugEnabled()) {
        qInfo().noquote()
            << "[RBAC] permissions"
            << "keys=" << profile.permissionKeys.join(QStringLiteral(","))
            << "canFlyManual=" << canFlyManual()
            << "canPlanMission=" << canPlanMission()
            << "canUploadMission=" << canUploadMission()
            << "canStartMission=" << canStartMission()
            << "canViewTelemetry=" << canViewTelemetry()
            << "canStreamTelemetry=" << canStreamTelemetry();
    }
}

void PermissionManager::reset()
{
    setPermissionMap({});
}

bool PermissionManager::hasPermission(const QString &permission) const
{
    const QString key = normalizePermission(permission);
    return m_permissions.value(key).toBool();
}

QString PermissionManager::denialMessage(const QString &permission) const
{
    return hasPermission(permission)
        ? QString()
        : QStringLiteral("Permission denied: %1").arg(normalizePermission(permission));
}

QString PermissionManager::normalizePermission(const QString &permission)
{
    return RbacBootstrapNormalizer::normalizePermission(permission);
}

QVariantMap PermissionManager::mapFromVariant(const QVariant &value)
{
    return RbacBootstrapNormalizer::permissionMapFromVariant(value);
}

QStringList PermissionManager::listFromVariant(const QVariant &value)
{
    return RbacBootstrapNormalizer::listFromVariant(value);
}

void PermissionManager::setPermissionMap(const QVariantMap &permissions)
{
    if (m_permissions == permissions) {
        return;
    }
    m_permissions = permissions;
    emit permissionsChanged();
}
