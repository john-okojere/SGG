#pragma once

#include <QObject>
#include <QDateTime>
#include <QString>

#include <memory>

class ApiClient;
class FlightSessionSyncManager;
class GcsEventSyncManager;
class PreflightChecklistManager;
class MissionPlanModel;
class MavsdkVehicleManager;
class SessionManager;

namespace mavsdk {
class Mission;
}

class MissionExecutionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool executing READ executing NOTIFY executionChanged)
    Q_PROPERTY(int activeWaypoint READ activeWaypoint NOTIFY executionChanged)
    Q_PROPERTY(int progress READ progress NOTIFY executionChanged)
    Q_PROPERTY(QString status READ status NOTIFY executionChanged)

public:
    explicit MissionExecutionManager(MavsdkVehicleManager *vehicle,
                                     MissionPlanModel *plan,
                                     ApiClient *api,
                                     SessionManager *session,
                                     FlightSessionSyncManager *flightSessions,
                                     PreflightChecklistManager *preflight,
                                     GcsEventSyncManager *events,
                                     QObject *parent = nullptr);

    bool executing() const;
    int activeWaypoint() const;
    int progress() const;
    QString status() const;

    Q_INVOKABLE void startMission();

signals:
    void executionChanged();
    void missionStarted();
    void missionStartFailed(const QString &message);

private:
    void setStatus(const QString &status);
    void postExecutionAction(const QString &action, const QJsonObject &payload);

    MavsdkVehicleManager *m_vehicle = nullptr;
    MissionPlanModel *m_plan = nullptr;
    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    FlightSessionSyncManager *m_flightSessions = nullptr;
    PreflightChecklistManager *m_preflight = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    bool m_executing = false;
    int m_activeWaypoint = -1;
    int m_progress = 0;
    int m_progressSyncIntervalMs = 1500;
    QDateTime m_lastProgressSyncAt;
    QString m_status = "Mission idle";
    std::shared_ptr<mavsdk::Mission> m_activeMission;
};
