#include "ProfileSyncManager.h"

#include "../auth/SessionManager.h"
#include "../network/ApiClient.h"
#include "../profile/ProfileManager.h"

#include <QJsonObject>

ProfileSyncManager::ProfileSyncManager(ApiClient *api, SessionManager *session, ProfileManager *profile, QObject *parent)
    : QObject(parent), m_api(api), m_session(session), m_profile(profile)
{
}

QString ProfileSyncManager::status() const { return m_status; }
bool ProfileSyncManager::syncing() const { return m_syncing; }

void ProfileSyncManager::syncProfile()
{
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Profile sync blocked."));
        return;
    }
    setSyncing(true);
    m_api->get(QStringLiteral("/api/missions/sync/bootstrap/"), true, true,
               [this](int statusCode, const QJsonObject &body, const QString &error) {
        setSyncing(false);
        if (statusCode < 200 || statusCode >= 300) {
            setStatus(error.isEmpty() ? QStringLiteral("Profile sync failed.") : error);
            return;
        }
        applyBootstrap(body.toVariantMap());
    });
}

void ProfileSyncManager::applyBootstrap(const QVariantMap &bootstrap)
{
    if (m_profile) {
        m_profile->applyBootstrap(bootstrap);
    }
    setStatus(QStringLiteral("Profile synchronized."));
}

void ProfileSyncManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit profileSyncChanged();
}

void ProfileSyncManager::setSyncing(bool syncing)
{
    if (m_syncing == syncing) {
        return;
    }
    m_syncing = syncing;
    emit profileSyncChanged();
}
