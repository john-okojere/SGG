#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

class ApiClient;
class FlightSessionSyncManager;
class FlightStatsManager;
class LocalSyncCache;
class MissionPlanModel;
class MissionSyncManager;
class ProfileManager;
class SessionManager;
class TelemetrySyncManager;
class VehicleTelemetryModel;

class PostMissionSummaryManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible NOTIFY summaryChanged)
    Q_PROPERTY(QString status READ status NOTIFY summaryChanged)
    Q_PROPERTY(QString syncStatus READ syncStatus NOTIFY summaryChanged)
    Q_PROPERTY(QVariantMap lastSummary READ lastSummary NOTIFY summaryChanged)
    Q_PROPERTY(bool fullLogVisible READ fullLogVisible WRITE setFullLogVisible NOTIFY summaryChanged)

public:
    explicit PostMissionSummaryManager(ApiClient *api,
                                       SessionManager *session,
                                       LocalSyncCache *cache,
                                       MissionPlanModel *plan,
                                       FlightStatsManager *flightStats,
                                       FlightSessionSyncManager *flightSessions,
                                       TelemetrySyncManager *telemetrySync,
                                       MissionSyncManager *missionSync,
                                       ProfileManager *profile,
                                       VehicleTelemetryModel *telemetry,
                                       QObject *parent = nullptr);

    bool visible() const;
    QString status() const;
    QString syncStatus() const;
    QVariantMap lastSummary() const;
    bool fullLogVisible() const;
    void setFullLogVisible(bool visible);

    Q_INVOKABLE void dismiss();
    Q_INVOKABLE void syncNow();
    Q_INVOKABLE void exportReport();
    Q_INVOKABLE void viewFullLog();
    Q_INVOKABLE void returnToDashboard();
    Q_INVOKABLE void startNewMission();

public slots:
    void handleMissionFinished(const QVariantMap &result);
    void handleFlightEnded(const QVariantMap &record);

signals:
    void summaryChanged();
    void returnToDashboardRequested();
    void startNewMissionRequested();

private:
    QVariantMap buildSummary(const QString &kind, const QString &resultStatus, const QString &reason, const QVariantMap &source) const;
    void publishSummary(QVariantMap summary);
    void saveLocalSummary(const QVariantMap &summary);
    void syncSummary(const QVariantMap &summary);
    QVariantMap reportFilePaths(const QString &summaryId) const;
    void writeReportFiles(QVariantMap *summary);
    void setStatus(const QString &status);
    void setSyncStatus(const QString &status);

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    LocalSyncCache *m_cache = nullptr;
    MissionPlanModel *m_plan = nullptr;
    FlightStatsManager *m_flightStats = nullptr;
    FlightSessionSyncManager *m_flightSessions = nullptr;
    TelemetrySyncManager *m_telemetrySync = nullptr;
    MissionSyncManager *m_missionSync = nullptr;
    ProfileManager *m_profile = nullptr;
    VehicleTelemetryModel *m_telemetry = nullptr;
    bool m_visible = false;
    bool m_fullLogVisible = false;
    QString m_status = QStringLiteral("No post-flight summary");
    QString m_syncStatus = QStringLiteral("Not synced");
    QVariantMap m_lastSummary;
};
