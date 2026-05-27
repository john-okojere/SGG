#pragma once

#include <QObject>
#include <QVariantMap>

class ApiClient;
class ProfileManager;
class SessionManager;

class ProfileSyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY profileSyncChanged)
    Q_PROPERTY(bool syncing READ syncing NOTIFY profileSyncChanged)

public:
    explicit ProfileSyncManager(ApiClient *api, SessionManager *session, ProfileManager *profile, QObject *parent = nullptr);

    QString status() const;
    bool syncing() const;

    Q_INVOKABLE void syncProfile();
    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);

signals:
    void profileSyncChanged();

private:
    void setStatus(const QString &status);
    void setSyncing(bool syncing);

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    ProfileManager *m_profile = nullptr;
    QString m_status = "Profile cached";
    bool m_syncing = false;
};
