#include "PermissionGuard.h"

#include "ModuleAccessManager.h"
#include "PermissionManager.h"
#include "../auth/SessionManager.h"

PermissionGuard::PermissionGuard(PermissionManager *permissions,
                                 ModuleAccessManager *modules,
                                 SessionManager *session,
                                 QObject *parent)
    : QObject(parent),
      m_permissions(permissions),
      m_modules(modules),
      m_session(session)
{
}

QString PermissionGuard::lastDenial() const
{
    return m_lastDenial;
}

bool PermissionGuard::requirePermission(const QString &permission, const QString &message)
{
    if (m_permissions && m_permissions->hasPermission(permission)) {
        return true;
    }
    setLastDenial(message.isEmpty() ? denialForPermission(permission) : message);
    return false;
}

bool PermissionGuard::requireModule(const QString &module, const QString &message)
{
    if (m_modules && m_modules->moduleAllowed(module)) {
        return true;
    }
    setLastDenial(message.isEmpty() ? denialForModule(module) : message);
    return false;
}

bool PermissionGuard::requireTrustedSession(const QString &message)
{
    if (m_session && m_session->operationsAllowed()) {
        return true;
    }
    setLastDenial(message.isEmpty() ? QStringLiteral("Device approval required.") : message);
    return false;
}

bool PermissionGuard::allowRoute(const QString &route)
{
    if (route == QStringLiteral("vehicleConfiguration")) {
        return requireModule(QStringLiteral("vehicle_configuration"),
                             QStringLiteral("You do not have permission to access vehicle configuration."));
    }
    if (route == QStringLiteral("manufacturerTestFlight")) {
        return requireModule(QStringLiteral("manufacturer_test_flight"),
                             QStringLiteral("You do not have permission to access manufacturer test flight."));
    }
    if (route == QStringLiteral("planner")) {
        return requireModule(QStringLiteral("mission_planning"),
                             QStringLiteral("You do not have permission to access mission planning."));
    }
    if (route == QStringLiteral("pilot")) {
        return requireModule(QStringLiteral("pilot_operations"),
                             QStringLiteral("You do not have permission to access Pilot Mode."));
    }
    return true;
}

QString PermissionGuard::denialForPermission(const QString &permission) const
{
    const QString normalized = normalizePermission(permission);
    if (normalized == QStringLiteral("can_upload_mission")) {
        return QStringLiteral("You do not have permission to upload missions.");
    }
    if (normalized == QStringLiteral("can_start_mission")) {
        return QStringLiteral("You do not have permission to start missions.");
    }
    if (normalized == QStringLiteral("can_fly_manual")) {
        return QStringLiteral("You do not have permission to use Pilot Mode.");
    }
    if (normalized == QStringLiteral("can_configure_vehicle")) {
        return QStringLiteral("You do not have permission to access vehicle configuration.");
    }
    if (normalized == QStringLiteral("can_write_vehicle_parameters")) {
        return QStringLiteral("You do not have permission to write vehicle parameters.");
    }
    if (normalized == QStringLiteral("can_run_manufacturer_test_flight")) {
        return QStringLiteral("You do not have permission to run manufacturer test flights.");
    }
    return QStringLiteral("This action is not available for your role.");
}

QString PermissionGuard::denialForModule(const QString &module) const
{
    if (module == QStringLiteral("mission_planning")) {
        return QStringLiteral("Mission Planning is not available for this role.");
    }
    if (module == QStringLiteral("pilot_operations")) {
        return QStringLiteral("Pilot Mode is not available for this role.");
    }
    if (module == QStringLiteral("vehicle_configuration")) {
        return QStringLiteral("Vehicle configuration is not available for this role.");
    }
    if (module == QStringLiteral("manufacturer_test_flight")) {
        return QStringLiteral("Manufacturer test flight is not available for this role.");
    }
    return QStringLiteral("This module is not available for your role.");
}

QString PermissionGuard::normalizePermission(QString permission)
{
    permission = permission.trimmed();
    permission.replace(QLatin1Char('-'), QLatin1Char('_'));
    permission.replace(QLatin1Char(' '), QLatin1Char('_'));
    permission = permission.toLower();
    if (!permission.startsWith(QStringLiteral("can_"))) {
        permission.prepend(QStringLiteral("can_"));
    }
    return permission;
}

void PermissionGuard::setLastDenial(const QString &message)
{
    if (m_lastDenial == message) {
        return;
    }
    m_lastDenial = message;
    emit denialChanged();
}
