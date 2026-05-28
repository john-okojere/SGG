#pragma once

#include <QObject>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>

class ApiClient;
class AppState;
class FlightSessionSyncManager;
class FlightStatsManager;
class HomePositionManager;
class LocalSyncCache;
class ManualControlManager;
class MissionPlanModel;
class MissionSyncManager;
class SessionManager;
class VehicleTelemetryModel;
class WebSocketClient;
class WindTelemetryManager;

class TelemetrySyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool uploading READ uploading NOTIFY telemetrySyncChanged)
    Q_PROPERTY(bool gazeboMode READ gazeboMode WRITE setGazeboMode NOTIFY telemetrySyncChanged)
    Q_PROPERTY(QString status READ status NOTIFY telemetrySyncChanged)
    Q_PROPERTY(QString syncState READ syncState NOTIFY telemetrySyncChanged)
    Q_PROPERTY(QString lastAckAt READ lastAckAt NOTIFY telemetrySyncChanged)
    Q_PROPERTY(int pendingQueueCount READ pendingQueueCount NOTIFY telemetrySyncChanged)

public:
    explicit TelemetrySyncManager(ApiClient *api,
                                  SessionManager *session,
                                  MissionSyncManager *missionSync,
                                  VehicleTelemetryModel *telemetry,
                                  MissionPlanModel *plan,
                                  AppState *appState,
                                  ManualControlManager *manualControl,
                                  WindTelemetryManager *wind,
                                  FlightStatsManager *flightStats,
                                  HomePositionManager *homePosition,
                                  FlightSessionSyncManager *flightSessions,
                                  WebSocketClient *websocket,
                                  LocalSyncCache *cache,
                                  QObject *parent = nullptr);

    bool uploading() const;
    bool gazeboMode() const;
    QString status() const;
    QString syncState() const;
    QString lastAckAt() const;
    int pendingQueueCount() const;
    void setGazeboMode(bool enabled);

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void uploadNow();

signals:
    void telemetrySyncChanged();

private:
    QJsonObject telemetryPayload() const;
    QJsonObject missionMetricsPayload() const;
    QJsonObject manualControlPayload() const;
    QJsonObject windPayload() const;
    bool meaningfulTelemetryChanged(const QJsonObject &payload) const;
    QJsonObject stateForComparison(const QJsonObject &payload) const;
    QJsonValue nullableString(const QString &value) const;
    QJsonValue backendIdValue(const QString &value) const;
    int selectedAircraftId() const;
    double roundNumber(double value, int decimals) const;
    double distanceFromHomeMeters() const;
    void drainQueuedSync();
    void postQueuedItem(const QVariantMap &item);
    void pollOutboundMessages();
    void setUploading(bool uploading);
    void setStatus(const QString &status);
    void setSyncState(const QString &state);
    void setPendingQueueCount(int count);

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    MissionSyncManager *m_missionSync = nullptr;
    VehicleTelemetryModel *m_telemetry = nullptr;
    MissionPlanModel *m_plan = nullptr;
    AppState *m_appState = nullptr;
    ManualControlManager *m_manualControl = nullptr;
    WindTelemetryManager *m_wind = nullptr;
    FlightStatsManager *m_flightStats = nullptr;
    HomePositionManager *m_homePosition = nullptr;
    FlightSessionSyncManager *m_flightSessions = nullptr;
    WebSocketClient *m_websocket = nullptr;
    LocalSyncCache *m_cache = nullptr;
    QTimer m_timer;
    bool m_uploading = false;
    bool m_gazeboMode = true;
    int m_retryBackoffMs = 1000;
    int m_uploadIntervalMs = 1500;
    int m_queueDrainIntervalMs = 5000;
    QDateTime m_nextRetryAt;
    QDateTime m_lastUploadAt;
    QDateTime m_lastQueueDrainAt;
    QJsonObject m_lastSentState;
    QString m_status = "Telemetry upload idle";
    QString m_syncState = "offline";
    QString m_lastAckAt;
    int m_pendingQueueCount = 0;
};
