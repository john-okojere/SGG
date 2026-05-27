#pragma once

#include <QObject>
#include <QString>

#include <functional>
#include <memory>

#include <mavsdk/plugins/action/action.hpp>

class GcsEventSyncManager;
class MavsdkVehicleManager;
class PilotActionSyncManager;
class PreflightChecklistManager;
class SessionManager;

class VehicleActionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY actionChanged)
    Q_PROPERTY(bool takeoffReady READ takeoffReady NOTIFY actionChanged)
    Q_PROPERTY(QString status READ status NOTIFY actionChanged)

public:
    explicit VehicleActionManager(MavsdkVehicleManager *vehicle,
                                  SessionManager *session,
                                  PilotActionSyncManager *pilotActions,
                                  PreflightChecklistManager *preflight,
                                  GcsEventSyncManager *events,
                                  QObject *parent = nullptr);

    bool busy() const;
    bool takeoffReady() const;
    QString status() const;

    Q_INVOKABLE void armAndTakeoff();
    Q_INVOKABLE void arm();
    Q_INVOKABLE void takeoff();
    Q_INVOKABLE void disarm();
    Q_INVOKABLE void land();
    Q_INVOKABLE void returnToLaunch();
    Q_INVOKABLE void holdPosition();
    Q_INVOKABLE void emergencyStop();

signals:
    void actionChanged();

private:
    void setBusy(bool busy);
    void setTakeoffReady(bool ready);
    void setStatus(const QString &status);
    bool canCommandVehicle(const QString &actionName);
    void runAction(const QString &actionName,
                   const QString &eventType,
                   const QString &successStatus,
                   const QString &failureStatus,
                   const std::function<void(std::shared_ptr<mavsdk::Action>,
                                            mavsdk::Action::ResultCallback)> &command);

    MavsdkVehicleManager *m_vehicle = nullptr;
    SessionManager *m_session = nullptr;
    PilotActionSyncManager *m_pilotActions = nullptr;
    PreflightChecklistManager *m_preflight = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    bool m_busy = false;
    bool m_takeoffReady = false;
    QString m_status = "Vehicle action idle";
};
