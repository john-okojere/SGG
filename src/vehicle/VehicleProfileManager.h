#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class ApiClient;
class PermissionManager;
class SessionManager;

class VehicleProfileManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList profiles READ profiles NOTIFY profilesChanged)
    Q_PROPERTY(QVariantMap activeProfile READ activeProfile NOTIFY profilesChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY profilesChanged)
    Q_PROPERTY(QString status READ status NOTIFY profilesChanged)

public:
    explicit VehicleProfileManager(ApiClient *api,
                                   SessionManager *session,
                                   PermissionManager *permissions,
                                   QObject *parent = nullptr);

    QVariantList profiles() const;
    QVariantMap activeProfile() const;
    bool busy() const;
    QString status() const;

    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);
    Q_INVOKABLE void refreshProfiles();
    Q_INVOKABLE void selectProfile(const QString &profileId);
    Q_INVOKABLE void saveProfile(const QVariantMap &profile);
    Q_INVOKABLE QString activeProfileId() const;

signals:
    void profilesChanged();

private:
    bool requirePermission(const QStringList &permissions, const QString &message);
    void setBusy(bool busy);
    void setStatus(const QString &status);
    void setProfiles(const QVariantList &profiles);
    void setActiveProfile(const QVariantMap &profile);

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    PermissionManager *m_permissions = nullptr;
    QVariantList m_profiles;
    QVariantMap m_activeProfile;
    bool m_busy = false;
    QString m_status = "Vehicle profiles not synchronized";
};
