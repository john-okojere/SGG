#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVariantMap>

class ApiClient;
class AppState;
class GcsEventSyncManager;
class LocalSyncCache;
class MissionPlanModel;
class MissionSyncManager;
class SessionManager;
class VehicleTelemetryModel;

class FlightSessionSyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active NOTIFY sessionChanged)
    Q_PROPERTY(QString serverSessionId READ serverSessionId NOTIFY sessionChanged)
    Q_PROPERTY(QString clientSessionId READ clientSessionId NOTIFY sessionChanged)
    Q_PROPERTY(QString mode READ mode NOTIFY sessionChanged)
    Q_PROPERTY(QString status READ status NOTIFY sessionChanged)

public:
    explicit FlightSessionSyncManager(ApiClient *api,
                                      SessionManager *session,
                                      MissionSyncManager *missionSync,
                                      MissionPlanModel *plan,
                                      AppState *appState,
                                      VehicleTelemetryModel *telemetry,
                                      GcsEventSyncManager *events,
                                      LocalSyncCache *cache,
                                      QObject *parent = nullptr);

    bool active() const;
    QString serverSessionId() const;
    QString clientSessionId() const;
    QString mode() const;
    QString status() const;

    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);
    Q_INVOKABLE void beginPilotSession(const QString &clientSessionId);
    Q_INVOKABLE void beginMissionSession(const QString &clientSessionId, const QString &missionId);
    Q_INVOKABLE void endActiveSession(const QString &endStatus = QStringLiteral("completed"),
                                      const QString &reason = QString(),
                                      const QVariantMap &summary = {});
    Q_INVOKABLE void recordPilotAction(const QString &actionType,
                                       const QJsonObject &actionData = {},
                                       const QString &message = QString());

signals:
    void sessionChanged();

private:
    void startSessionOnServer(const QString &mode,
                              const QString &clientSessionId,
                              const QString &missionId);
    void adoptServerSession(const QVariantMap &session);
    void clearSession();
    int selectedAircraftId() const;
    QJsonObject positionPayload() const;
    void setStatus(const QString &status);
    QString normalizedActionType(const QString &actionType) const;

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    MissionSyncManager *m_missionSync = nullptr;
    MissionPlanModel *m_plan = nullptr;
    AppState *m_appState = nullptr;
    VehicleTelemetryModel *m_telemetry = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    LocalSyncCache *m_cache = nullptr;
    bool m_active = false;
    bool m_starting = false;
    QString m_serverSessionId;
    QString m_clientSessionId;
    QString m_mode;
    QString m_status = QStringLiteral("No active Control Center flight session");
};
