#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

class LocalSyncCache;

class ProfileManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString displayName READ displayName NOTIFY profileChanged)
    Q_PROPERTY(QString username READ username NOTIFY profileChanged)
    Q_PROPERTY(QString avatarUrl READ avatarUrl NOTIFY profileChanged)
    Q_PROPERTY(QString organizationName READ organizationName NOTIFY profileChanged)
    Q_PROPERTY(int organizationId READ organizationId NOTIFY profileChanged)
    Q_PROPERTY(QVariantMap analyticsSummary READ analyticsSummary NOTIFY profileChanged)
    Q_PROPERTY(QVariantList notifications READ notifications NOTIFY profileChanged)
    Q_PROPERTY(QVariantList recentActivity READ recentActivity NOTIFY profileChanged)

public:
    explicit ProfileManager(LocalSyncCache *cache, QObject *parent = nullptr);

    QString displayName() const;
    QString username() const;
    QString avatarUrl() const;
    QString organizationName() const;
    int organizationId() const;
    QVariantMap analyticsSummary() const;
    QVariantList notifications() const;
    QVariantList recentActivity() const;

    Q_INVOKABLE void loadCachedProfile();
    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);

signals:
    void profileChanged();

private:
    QVariantMap firstMap(const QVariantMap &source, const QStringList &keys) const;
    QVariantList firstList(const QVariantMap &source, const QStringList &keys) const;

    LocalSyncCache *m_cache = nullptr;
    QVariantMap m_profile;
    QVariantMap m_organization;
    QVariantMap m_analytics;
    QVariantList m_notifications;
    QVariantList m_recentActivity;
};
