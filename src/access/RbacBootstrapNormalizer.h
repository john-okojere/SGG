#pragma once

#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariantMap>

struct RbacBootstrapProfile
{
    QVariantMap user;
    QStringList rawRoles;
    QStringList normalizedRoles;
    QVariantMap permissions;
    QStringList permissionKeys;
    QStringList allowedModules;
    QVariantMap sessionStatus;
    QVariantMap deviceSummary;
    int organizationId = 0;
    QSet<QString> aircraftIds;
    QSet<QString> missionIds;
    bool hasSessionTrusted = false;
    bool sessionTrusted = false;
    bool hasBackendReachable = false;
    bool backendReachable = false;
};

class RbacBootstrapNormalizer
{
public:
    static RbacBootstrapProfile fromBootstrap(const QVariantMap &bootstrap);
    static QString normalizePermission(const QString &permission);
    static QString normalizeModule(const QString &module);
    static QString normalizeRole(const QString &role);
    static QString normalizeKey(const QString &value);
    static QVariantMap permissionMapFromVariant(const QVariant &value);
    static QStringList listFromVariant(const QVariant &value);
    static QStringList enabledPermissionKeys(const QVariantMap &permissions);
    static QStringList derivedModulesForPermissions(const QVariantMap &permissions);
    static bool truthy(const QVariant &value);
};
