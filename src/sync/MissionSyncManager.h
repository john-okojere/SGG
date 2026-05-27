#pragma once

#include <QObject>
#include <QDateTime>
#include <QVariantList>
#include <QVariantMap>

class ApiClient;
class LocalSyncCache;
class GcsEventSyncManager;
class MissionPlanModel;
class SessionManager;

class MissionSyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool syncing READ syncing NOTIFY syncChanged)
    Q_PROPERTY(QString status READ status NOTIFY syncChanged)
    Q_PROPERTY(QVariantMap organization READ organization NOTIFY syncChanged)
    Q_PROPERTY(QVariantMap pilotProfile READ pilotProfile NOTIFY syncChanged)
    Q_PROPERTY(QVariantMap deviceSummary READ deviceSummary NOTIFY syncChanged)
    Q_PROPERTY(QVariantMap sessionStatus READ sessionStatus NOTIFY syncChanged)
    Q_PROPERTY(QVariantList assignedAircraft READ assignedAircraft NOTIFY syncChanged)
    Q_PROPERTY(QVariantList approvedMissions READ approvedMissions NOTIFY syncChanged)
    Q_PROPERTY(QVariantList activeMissions READ activeMissions NOTIFY syncChanged)
    Q_PROPERTY(QVariantList missionHistory READ missionHistory NOTIFY syncChanged)

public:
    explicit MissionSyncManager(ApiClient *api,
                                SessionManager *session,
                                LocalSyncCache *cache,
                                MissionPlanModel *plan,
                                GcsEventSyncManager *events = nullptr,
                                QObject *parent = nullptr);

    bool syncing() const;
    QString status() const;
    QVariantMap organization() const;
    QVariantMap pilotProfile() const;
    QVariantMap deviceSummary() const;
    QVariantMap sessionStatus() const;
    QVariantList assignedAircraft() const;
    QVariantList approvedMissions() const;
    QVariantList activeMissions() const;
    QVariantList missionHistory() const;

    Q_INVOKABLE void bootstrap();
    Q_INVOKABLE void syncMissions();
    Q_INVOKABLE void saveActiveMission();
    Q_INVOKABLE void validateActiveMission();
    Q_INVOKABLE void openMission(const QVariantMap &mission);

signals:
    void syncChanged();
    void bootstrapReceived(const QVariantMap &bootstrap);
    void missionOpened(const QString &missionType, const QString &missionId);
    void activeMissionSaved(bool success, const QString &message);
    void activeMissionValidated(bool success, const QString &message);

private:
    void loadCached();
    void applyOpenMission(const QVariantMap &mission);
    void setSyncing(bool syncing);
    void setStatus(const QString &status);

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    LocalSyncCache *m_cache = nullptr;
    MissionPlanModel *m_plan = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    bool m_syncing = false;
    QString m_status = "Not synchronized";
    QDateTime m_lastBootstrapAt;
    QVariantMap m_organization;
    QVariantMap m_pilotProfile;
    QVariantMap m_deviceSummary;
    QVariantMap m_sessionStatus;
    QVariantList m_assignedAircraft;
    QVariantList m_approvedMissions;
    QVariantList m_activeMissions;
    QVariantList m_missionHistory;
};
