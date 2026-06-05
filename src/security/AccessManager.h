#pragma once

#include <QObject>
#include <QDateTime>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

class GcsEventSyncManager;
class LocalSyncCache;

class AccessManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool accessLoaded READ accessLoaded NOTIFY accessChanged)
    Q_PROPERTY(bool sessionTrusted READ sessionTrusted NOTIFY accessChanged)
    Q_PROPERTY(bool offlineAuthorizationValid READ offlineAuthorizationValid NOTIFY accessChanged)
    Q_PROPERTY(QString role READ role NOTIFY accessChanged)
    Q_PROPERTY(QStringList roles READ roles NOTIFY accessChanged)
    Q_PROPERTY(QStringList permissions READ permissions NOTIFY accessChanged)
    Q_PROPERTY(QStringList allowedModules READ allowedModules NOTIFY accessChanged)
    Q_PROPERTY(int organizationId READ organizationId NOTIFY accessChanged)
    Q_PROPERTY(QVariantMap sessionStatus READ sessionStatus NOTIFY accessChanged)
    Q_PROPERTY(QVariantMap deviceSummary READ deviceSummary NOTIFY accessChanged)
    Q_PROPERTY(QString status READ status NOTIFY accessChanged)

public:
    explicit AccessManager(LocalSyncCache *cache = nullptr, QObject *parent = nullptr);

    bool accessLoaded() const;
    bool sessionTrusted() const;
    bool offlineAuthorizationValid() const;
    QString role() const;
    QStringList roles() const;
    QStringList permissions() const;
    QStringList allowedModules() const;
    int organizationId() const;
    QVariantMap sessionStatus() const;
    QVariantMap deviceSummary() const;
    QString status() const;

    void setEventSyncManager(GcsEventSyncManager *events);

    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);
    Q_INVOKABLE void clearAccess(const QString &reason = QString());
    Q_INVOKABLE void setSessionState(bool trusted, bool reachable, const QString &reason = QString());
    Q_INVOKABLE bool can(const QString &permission) const;
    Q_INVOKABLE bool canAny(const QStringList &permissions) const;
    Q_INVOKABLE bool canModule(const QString &module) const;
    Q_INVOKABLE bool canPerform(const QString &action) const;
    Q_INVOKABLE bool canAccessAircraft(const QVariant &aircraftId) const;
    Q_INVOKABLE bool canAccessMission(const QVariant &missionId) const;
    Q_INVOKABLE QVariantList filterAircraft(const QVariantList &aircraft) const;
    Q_INVOKABLE QVariantList filterMissions(const QVariantList &missions) const;
    Q_INVOKABLE bool authorizeAction(const QString &action,
                                     const QVariantMap &context = {},
                                     const QString &message = QString());
    Q_INVOKABLE void recordAllowed(const QString &action, const QVariantMap &context = {});
    Q_INVOKABLE void recordBlocked(const QString &action,
                                   const QString &reason,
                                   const QVariantMap &context = {});

signals:
    void accessChanged();
    void accessRevoked(const QString &reason);

private:
    QString permissionForAction(const QString &action) const;
    QString normalizedAction(const QString &action) const;
    QString idString(const QVariant &value) const;
    QStringList stringListFromVariant(const QVariant &value) const;
    QSet<QString> idsFromList(const QVariantList &items, const QStringList &keys) const;
    QString accessFingerprint(const QVariantMap &bootstrap) const;
    bool actionRequiresTrustedSession(const QString &action) const;
    bool actionAllowsOfflineAuthorization(const QString &action) const;
    void persistSnapshot();
    void loadSnapshot();
    void setStatus(const QString &status);
    void emitAudit(const QString &eventType,
                   const QString &severity,
                   const QString &message,
                   const QVariantMap &context = {});

    LocalSyncCache *m_cache = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    bool m_accessLoaded = false;
    bool m_sessionTrusted = false;
    bool m_backendReachable = false;
    QString m_status = "Access profile not loaded";
    QString m_role;
    QStringList m_roles;
    QStringList m_permissions;
    QStringList m_allowedModules;
    int m_organizationId = 0;
    QVariantMap m_sessionStatus;
    QVariantMap m_deviceSummary;
    QSet<QString> m_aircraftIds;
    QSet<QString> m_missionIds;
    QDateTime m_lastVerifiedAt;
    QString m_fingerprint;
};
