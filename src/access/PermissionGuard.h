#pragma once

#include <QObject>
#include <QString>

class ModuleAccessManager;
class PermissionManager;
class SessionManager;

class PermissionGuard : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastDenial READ lastDenial NOTIFY denialChanged)

public:
    explicit PermissionGuard(PermissionManager *permissions,
                             ModuleAccessManager *modules,
                             SessionManager *session,
                             QObject *parent = nullptr);

    QString lastDenial() const;

    Q_INVOKABLE bool requirePermission(const QString &permission, const QString &message = QString());
    Q_INVOKABLE bool requireModule(const QString &module, const QString &message = QString());
    Q_INVOKABLE bool requireTrustedSession(const QString &message = QString());
    Q_INVOKABLE bool allowRoute(const QString &route);
    Q_INVOKABLE QString denialForPermission(const QString &permission) const;
    Q_INVOKABLE QString denialForModule(const QString &module) const;

signals:
    void denialChanged();

private:
    static QString normalizePermission(QString permission);
    void setLastDenial(const QString &message);

    PermissionManager *m_permissions = nullptr;
    ModuleAccessManager *m_modules = nullptr;
    SessionManager *m_session = nullptr;
    QString m_lastDenial;
};
