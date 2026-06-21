#include "RoleAccessManager.h"

#include "RbacBootstrapNormalizer.h"

#include <QDebug>
#include <QProcessEnvironment>
#include <QVariant>

namespace {

QString normalizeToken(QString value)
{
    value = value.trimmed();
    value.replace(QLatin1Char('-'), QLatin1Char('_'));
    value.replace(QLatin1Char(' '), QLatin1Char('_'));
    return value.toUpper();
}

QString normalizeAccessKey(QString value)
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

QStringList stringListFromVariant(const QVariant &value)
{
    QStringList out;
    if (value.metaType().id() == QMetaType::QStringList) {
        out = value.toStringList();
    } else if (value.metaType().id() == QMetaType::QVariantList) {
        const QVariantList list = value.toList();
        for (const QVariant &entry : list) {
            const QString text = entry.toString().trimmed();
            if (!text.isEmpty()) {
                out << text;
            }
        }
    } else {
        const QString text = value.toString().trimmed();
        if (!text.isEmpty()) {
            out = text.split(QLatin1Char(','), Qt::SkipEmptyParts);
        }
    }
    return out;
}

} // namespace

RoleAccessManager::RoleAccessManager(QObject *parent)
    : QObject(parent)
{
}

QString RoleAccessManager::currentRole() const
{
    return m_roles.isEmpty() ? QString() : m_roles.first();
}

QString RoleAccessManager::currentUserRole() const
{
    return currentRole();
}

QStringList RoleAccessManager::roles() const
{
    return m_roles;
}

QVariantMap RoleAccessManager::user() const
{
    return m_user;
}

QVariantMap RoleAccessManager::permissions() const
{
    return m_permissions;
}

QStringList RoleAccessManager::allowedModules() const
{
    return m_allowedModules;
}

bool RoleAccessManager::isSkyGridAdmin() const
{
    return hasRole(QStringLiteral("SKYGRID_ADMIN")) || hasRole(QStringLiteral("SUPER_ADMIN"));
}

bool RoleAccessManager::isManufacturer() const
{
    return hasAnyRole({QStringLiteral("MANUFACTURER_ADMIN"), QStringLiteral("MANUFACTURER_ENGINEER")});
}

bool RoleAccessManager::isOrganizationAdmin() const
{
    return hasAnyRole({QStringLiteral("ORGANIZATION_ADMIN"), QStringLiteral("OPERATIONS_ADMIN")});
}

bool RoleAccessManager::isFleetManager() const
{
    return hasRole(QStringLiteral("FLEET_MANAGER"));
}

bool RoleAccessManager::isPilot() const
{
    return hasRole(QStringLiteral("PILOT"));
}

void RoleAccessManager::applyBootstrap(const QVariantMap &bootstrap)
{
    const RbacBootstrapProfile profile = RbacBootstrapNormalizer::fromBootstrap(bootstrap);
    setAccess(profile.rawRoles, profile.user, profile.permissions, profile.allowedModules);
    if (rbacDebugEnabled()) {
        qInfo().noquote()
            << "[RBAC] roles"
            << "raw=" << profile.rawRoles.join(QStringLiteral(","))
            << "normalized=" << profile.normalizedRoles.join(QStringLiteral(","));
    }
}

void RoleAccessManager::reset()
{
    setAccess({}, {}, {}, {});
}

bool RoleAccessManager::hasRole(const QString &role) const
{
    return m_roles.contains(normalizeRole(role));
}

bool RoleAccessManager::hasAnyRole(const QStringList &roles) const
{
    for (const QString &role : roles) {
        if (hasRole(role)) {
            return true;
        }
    }
    return false;
}

bool RoleAccessManager::hasPermission(const QString &permission) const
{
    QString key = normalizeKey(permission);
    if (!key.startsWith(QStringLiteral("can_"))) {
        key.prepend(QStringLiteral("can_"));
    }
    return m_permissions.value(key).toBool();
}

bool RoleAccessManager::moduleAllowed(const QString &module) const
{
    return m_allowedModules.contains(normalizeKey(module));
}

QString RoleAccessManager::normalizeRole(const QString &role)
{
    return RbacBootstrapNormalizer::normalizeRole(role);
}

QString RoleAccessManager::normalizeKey(const QString &value)
{
    return RbacBootstrapNormalizer::normalizeKey(value);
}

QStringList RoleAccessManager::listFromVariant(const QVariant &value)
{
    QStringList out = RbacBootstrapNormalizer::listFromVariant(value);
    for (QString &entry : out) {
        entry = normalizeKey(entry);
    }
    out.removeDuplicates();
    return out;
}

QVariantMap RoleAccessManager::permissionMapFromVariant(const QVariant &value)
{
    return RbacBootstrapNormalizer::permissionMapFromVariant(value);
}

void RoleAccessManager::setAccess(const QStringList &roles,
                                  const QVariantMap &user,
                                  const QVariantMap &permissions,
                                  const QStringList &modules)
{
    QStringList normalized;
    for (const QString &role : roles) {
        const QString text = normalizeRole(role);
        if (!text.isEmpty() && !normalized.contains(text)) {
            normalized << text;
        }
    }

    QStringList normalizedModules;
    for (const QString &module : modules) {
        const QString text = RbacBootstrapNormalizer::normalizeModule(module);
        if (!text.isEmpty() && !normalizedModules.contains(text)) {
            normalizedModules << text;
        }
    }

    if (m_roles == normalized
        && m_user == user
        && m_permissions == permissions
        && m_allowedModules == normalizedModules) {
        return;
    }
    m_roles = normalized;
    m_user = user;
    m_permissions = permissions;
    m_allowedModules = normalizedModules;
    emit rolesChanged();
}
