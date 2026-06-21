#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class RoleAccessManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentRole READ currentRole NOTIFY rolesChanged)
    Q_PROPERTY(QString currentUserRole READ currentUserRole NOTIFY rolesChanged)
    Q_PROPERTY(QStringList roles READ roles NOTIFY rolesChanged)
    Q_PROPERTY(QVariantMap user READ user NOTIFY rolesChanged)
    Q_PROPERTY(QVariantMap permissions READ permissions NOTIFY rolesChanged)
    Q_PROPERTY(QStringList allowedModules READ allowedModules NOTIFY rolesChanged)
    Q_PROPERTY(bool isSkyGridAdmin READ isSkyGridAdmin NOTIFY rolesChanged)
    Q_PROPERTY(bool isManufacturer READ isManufacturer NOTIFY rolesChanged)
    Q_PROPERTY(bool isOrganizationAdmin READ isOrganizationAdmin NOTIFY rolesChanged)
    Q_PROPERTY(bool isFleetManager READ isFleetManager NOTIFY rolesChanged)
    Q_PROPERTY(bool isPilot READ isPilot NOTIFY rolesChanged)

public:
    explicit RoleAccessManager(QObject *parent = nullptr);

    QString currentRole() const;
    QString currentUserRole() const;
    QStringList roles() const;
    QVariantMap user() const;
    QVariantMap permissions() const;
    QStringList allowedModules() const;
    bool isSkyGridAdmin() const;
    bool isManufacturer() const;
    bool isOrganizationAdmin() const;
    bool isFleetManager() const;
    bool isPilot() const;

    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);
    Q_INVOKABLE void reset();
    Q_INVOKABLE bool hasRole(const QString &role) const;
    Q_INVOKABLE bool hasAnyRole(const QStringList &roles) const;
    Q_INVOKABLE bool hasPermission(const QString &permission) const;
    Q_INVOKABLE bool moduleAllowed(const QString &module) const;

signals:
    void rolesChanged();

private:
    static QString normalizeRole(const QString &role);
    static QString normalizeKey(const QString &value);
    static QStringList listFromVariant(const QVariant &value);
    static QVariantMap permissionMapFromVariant(const QVariant &value);
    void setAccess(const QStringList &roles,
                   const QVariantMap &user,
                   const QVariantMap &permissions,
                   const QStringList &modules);

    QStringList m_roles;
    QVariantMap m_user;
    QVariantMap m_permissions;
    QStringList m_allowedModules;
};
