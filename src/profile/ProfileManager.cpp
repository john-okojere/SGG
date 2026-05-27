#include "ProfileManager.h"

#include "../cache/LocalSyncCache.h"

ProfileManager::ProfileManager(LocalSyncCache *cache, QObject *parent)
    : QObject(parent), m_cache(cache)
{
    loadCachedProfile();
}

QString ProfileManager::displayName() const
{
    return m_profile.value(QStringLiteral("display_name"),
           m_profile.value(QStringLiteral("name"),
           m_profile.value(QStringLiteral("full_name"),
           m_profile.value(QStringLiteral("email"), QStringLiteral("Operator"))))).toString();
}

QString ProfileManager::username() const
{
    return m_profile.value(QStringLiteral("username"), displayName()).toString();
}

QString ProfileManager::avatarUrl() const
{
    return m_profile.value(QStringLiteral("avatar_url"), m_profile.value(QStringLiteral("avatar")).toString()).toString();
}

QString ProfileManager::organizationName() const
{
    return m_organization.value(QStringLiteral("name"), QStringLiteral("SkyGrid Operations")).toString();
}

int ProfileManager::organizationId() const
{
    return m_organization.value(QStringLiteral("id")).toInt();
}

QVariantMap ProfileManager::analyticsSummary() const { return m_analytics; }
QVariantList ProfileManager::notifications() const { return m_notifications; }
QVariantList ProfileManager::recentActivity() const { return m_recentActivity; }

void ProfileManager::loadCachedProfile()
{
    if (!m_cache) {
        return;
    }
    m_profile = m_cache->loadObject(QStringLiteral("profile"), QStringLiteral("user"));
    m_organization = m_cache->loadObject(QStringLiteral("profile"), QStringLiteral("organization"));
    m_analytics = m_cache->loadObject(QStringLiteral("profile"), QStringLiteral("analytics"));
    m_notifications = m_cache->loadList(QStringLiteral("profile"), QStringLiteral("notifications"));
    m_recentActivity = m_cache->loadList(QStringLiteral("profile"), QStringLiteral("recent_activity"));
    emit profileChanged();
}

void ProfileManager::applyBootstrap(const QVariantMap &bootstrap)
{
    m_profile = firstMap(bootstrap, {QStringLiteral("profile"), QStringLiteral("pilot_profile"), QStringLiteral("user")});
    m_organization = firstMap(bootstrap, {QStringLiteral("organization"), QStringLiteral("org")});
    m_analytics = firstMap(bootstrap, {QStringLiteral("analytics_summary"), QStringLiteral("analytics"), QStringLiteral("dashboard_summary")});
    m_notifications = firstList(bootstrap, {QStringLiteral("notifications"), QStringLiteral("alerts")});
    m_recentActivity = firstList(bootstrap, {QStringLiteral("recent_activity"), QStringLiteral("activity")});

    if (m_cache) {
        m_cache->saveObject(QStringLiteral("profile"), QStringLiteral("user"), m_profile);
        m_cache->saveObject(QStringLiteral("profile"), QStringLiteral("organization"), m_organization);
        m_cache->saveObject(QStringLiteral("profile"), QStringLiteral("analytics"), m_analytics);
        m_cache->saveList(QStringLiteral("profile"), QStringLiteral("notifications"), m_notifications);
        m_cache->saveList(QStringLiteral("profile"), QStringLiteral("recent_activity"), m_recentActivity);
    }
    emit profileChanged();
}

QVariantMap ProfileManager::firstMap(const QVariantMap &source, const QStringList &keys) const
{
    for (const QString &key : keys) {
        const QVariantMap value = source.value(key).toMap();
        if (!value.isEmpty()) {
            return value;
        }
    }
    return {};
}

QVariantList ProfileManager::firstList(const QVariantMap &source, const QStringList &keys) const
{
    for (const QString &key : keys) {
        const QVariantList value = source.value(key).toList();
        if (!value.isEmpty()) {
            return value;
        }
    }
    return {};
}
