#include "ModuleAccessManager.h"

#include "RbacBootstrapNormalizer.h"

#include <QDebug>
#include <QMetaType>
#include <QProcessEnvironment>
#include <QVariant>

namespace {

QString normalize(QString value)
{
    value = value.trimmed();
    value.replace(QLatin1Char('-'), QLatin1Char('_'));
    value.replace(QLatin1Char(' '), QLatin1Char('_'));
    return value.toLower();
}

bool rbacDebugEnabled()
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    return env.value(QStringLiteral("SKYGRID_RBAC_DEBUG")) == QStringLiteral("true")
        || env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true");
}

} // namespace

ModuleAccessManager::ModuleAccessManager(QObject *parent)
    : QObject(parent)
{
}

QStringList ModuleAccessManager::allowedModules() const
{
    return m_allowedModules;
}

QStringList ModuleAccessManager::visibleModules() const
{
    return m_allowedModules;
}

QString ModuleAccessManager::defaultWorkspace() const
{
    if (moduleAllowed(QStringLiteral("manufacturer_dashboard"))) {
        return QStringLiteral("home");
    }
    const bool hasMissionOrPilot = missionWorkspaceAllowed() || pilotWorkspaceAllowed();
    if (hasMissionOrPilot) {
        return QStringLiteral("home");
    }
    if (vehicleConfigurationWorkspaceAllowed()) {
        return QStringLiteral("vehicleConfiguration");
    }
    if (manufacturerTestFlightWorkspaceAllowed()) {
        return QStringLiteral("manufacturerTestFlight");
    }
    if (fleetWorkspaceAllowed() || administrationWorkspaceAllowed()) {
        return QStringLiteral("home");
    }
    return QStringLiteral("home");
}

bool ModuleAccessManager::missionWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("mission_planning"));
}

bool ModuleAccessManager::pilotWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("pilot_operations"));
}

bool ModuleAccessManager::manufacturerTestFlightWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("manufacturer_test_flight"));
}

bool ModuleAccessManager::vehicleConfigurationWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("vehicle_configuration"));
}

bool ModuleAccessManager::fleetWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("fleet"));
}

bool ModuleAccessManager::administrationWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("administration"));
}

bool ModuleAccessManager::gcsToolsWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("gcs_tools"));
}

bool ModuleAccessManager::flightDataWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("flight_data"));
}

bool ModuleAccessManager::initialSetupWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("initial_setup"));
}

bool ModuleAccessManager::configurationTuningWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("configuration_tuning"));
}

bool ModuleAccessManager::logsAnalysisWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("logs_analysis"));
}

bool ModuleAccessManager::simulationWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("simulation"));
}

bool ModuleAccessManager::advancedToolsWorkspaceAllowed() const
{
    return moduleAllowed(QStringLiteral("advanced_tools"));
}

void ModuleAccessManager::applyBootstrap(const QVariantMap &bootstrap)
{
    const RbacBootstrapProfile profile = RbacBootstrapNormalizer::fromBootstrap(bootstrap);
    setAllowedModules(profile.allowedModules);
    if (rbacDebugEnabled()) {
        qInfo().noquote()
            << "[RBAC] modules"
            << "visible=" << m_allowedModules.join(QStringLiteral(","));
    }
}

void ModuleAccessManager::reset()
{
    setAllowedModules({});
}

bool ModuleAccessManager::moduleAllowed(const QString &module) const
{
    return m_allowedModules.contains(normalizeModule(module));
}

QString ModuleAccessManager::normalizeModule(const QString &module)
{
    return RbacBootstrapNormalizer::normalizeModule(module);
}

QStringList ModuleAccessManager::listFromVariant(const QVariant &value)
{
    return RbacBootstrapNormalizer::listFromVariant(value);
}

bool ModuleAccessManager::permissionEnabled(const QVariant &permissions, const QString &scope)
{
    return RbacBootstrapNormalizer::permissionMapFromVariant(permissions)
        .value(RbacBootstrapNormalizer::normalizePermission(scope)).toBool();
}

void ModuleAccessManager::setAllowedModules(const QStringList &modules)
{
    QStringList normalized;
    for (const QString &module : modules) {
        const QString text = normalizeModule(module);
        if (!text.isEmpty() && !normalized.contains(text)) {
            normalized << text;
        }
    }
    if (m_allowedModules == normalized) {
        return;
    }
    m_allowedModules = normalized;
    emit modulesChanged();
}
