#pragma once

#include <QObject>
#include <QVariantMap>

class ApiClient;
class PreferencesManager;
class SessionManager;

class PreferencesSyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY preferencesSyncChanged)

public:
    explicit PreferencesSyncManager(ApiClient *api, SessionManager *session, PreferencesManager *preferences, QObject *parent = nullptr);

    QString status() const;

    Q_INVOKABLE void syncPreferences();
    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);

signals:
    void preferencesSyncChanged();

private:
    void setStatus(const QString &status);

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    PreferencesManager *m_preferences = nullptr;
    QString m_status = "Preferences cached";
};
