#include "PreferencesSyncManager.h"

#include "../auth/SessionManager.h"
#include "../network/ApiClient.h"
#include "../preferences/PreferencesManager.h"

#include <QJsonObject>

PreferencesSyncManager::PreferencesSyncManager(ApiClient *api, SessionManager *session, PreferencesManager *preferences, QObject *parent)
    : QObject(parent), m_api(api), m_session(session), m_preferences(preferences)
{
}

QString PreferencesSyncManager::status() const { return m_status; }

void PreferencesSyncManager::syncPreferences()
{
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Preferences sync blocked."));
        return;
    }
    m_api->get(QStringLiteral("/api/missions/sync/bootstrap/"), true, true,
               [this](int statusCode, const QJsonObject &body, const QString &error) {
        if (statusCode < 200 || statusCode >= 300) {
            setStatus(error.isEmpty() ? QStringLiteral("Preferences sync failed.") : error);
            return;
        }
        applyBootstrap(body.toVariantMap());
    });
}

void PreferencesSyncManager::applyBootstrap(const QVariantMap &bootstrap)
{
    if (m_preferences) {
        m_preferences->applyBootstrap(bootstrap);
    }
    setStatus(QStringLiteral("Preferences synchronized."));
}

void PreferencesSyncManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit preferencesSyncChanged();
}
