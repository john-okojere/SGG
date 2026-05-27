#include "VehicleActionManager.h"

#include "MavsdkVehicleManager.h"
#include "../auth/SessionManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../flight/PreflightChecklistManager.h"
#include "../sync/PilotActionSyncManager.h"

#include <mavsdk/plugins/action/action.hpp>
#include <mavsdk/system.hpp>

#include <QJsonObject>
#include <QMetaObject>

VehicleActionManager::VehicleActionManager(MavsdkVehicleManager *vehicle,
                                           SessionManager *session,
                                           PilotActionSyncManager *pilotActions,
                                           PreflightChecklistManager *preflight,
                                           GcsEventSyncManager *events,
                                           QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_session(session),
      m_pilotActions(pilotActions),
      m_preflight(preflight),
      m_events(events)
{
}

namespace {

void recordVehicleAction(PilotActionSyncManager *pilotActions,
                         GcsEventSyncManager *events,
                         const QString &eventType,
                         const QString &severity,
                         const QString &message)
{
    if (pilotActions) {
        pilotActions->recordAction(eventType, message);
        return;
    }
    if (events) {
        events->recordEvent(eventType, severity, message);
    }
}

} // namespace

bool VehicleActionManager::busy() const { return m_busy; }
bool VehicleActionManager::takeoffReady() const { return m_takeoffReady; }
QString VehicleActionManager::status() const { return m_status; }

void VehicleActionManager::armAndTakeoff()
{
    if (!canCommandVehicle(QStringLiteral("arm and take off"))) {
        return;
    }
    setBusy(true);
    setStatus(QStringLiteral("Arming aircraft..."));
    auto action = std::make_shared<mavsdk::Action>(m_vehicle->system());
    action->arm_async([this, action](mavsdk::Action::Result result) {
        QMetaObject::invokeMethod(this, [this, action, result]() {
            if (result != mavsdk::Action::Result::Success) {
                setBusy(false);
                setStatus(QStringLiteral("Arm failed"));
                recordVehicleAction(m_pilotActions, m_events, QStringLiteral("takeoff_started"), QStringLiteral("error"), QStringLiteral("Aircraft arm failed"));
                return;
            }
            setStatus(QStringLiteral("Takeoff command sent..."));
            recordVehicleAction(m_pilotActions, m_events, QStringLiteral("takeoff_started"), QStringLiteral("info"), QStringLiteral("Aircraft armed; takeoff requested"));
            action->takeoff_async([this, action](mavsdk::Action::Result takeoffResult) {
                QMetaObject::invokeMethod(this, [this, takeoffResult]() {
                    setBusy(false);
                    const bool ok = takeoffResult == mavsdk::Action::Result::Success;
                    setTakeoffReady(ok);
                    setStatus(ok ? QStringLiteral("Aircraft takeoff in progress") : QStringLiteral("Takeoff failed"));
                    recordVehicleAction(m_pilotActions,
                                        m_events,
                                        ok ? QStringLiteral("takeoff_started") : QStringLiteral("mission_failed"),
                                        ok ? QStringLiteral("info") : QStringLiteral("error"),
                                        ok ? QStringLiteral("Takeoff accepted by vehicle") : QStringLiteral("Takeoff command failed"));
                }, Qt::QueuedConnection);
            });
        }, Qt::QueuedConnection);
    });
}

void VehicleActionManager::arm()
{
    if (!canCommandVehicle(QStringLiteral("arm"))) {
        return;
    }
    setBusy(true);
    setStatus(QStringLiteral("Arming aircraft..."));
    auto action = std::make_shared<mavsdk::Action>(m_vehicle->system());
    action->arm_async([this, action](mavsdk::Action::Result result) {
        QMetaObject::invokeMethod(this, [this, result]() {
            setBusy(false);
            const bool ok = result == mavsdk::Action::Result::Success;
            setTakeoffReady(ok);
            setStatus(ok ? QStringLiteral("Aircraft armed") : QStringLiteral("Arm failed"));
            recordVehicleAction(m_pilotActions, m_events, QStringLiteral("vehicle_arm"), ok ? QStringLiteral("info") : QStringLiteral("error"), m_status);
        }, Qt::QueuedConnection);
    });
}

void VehicleActionManager::takeoff()
{
    runAction(QStringLiteral("take off"),
              QStringLiteral("takeoff_started"),
              QStringLiteral("Aircraft takeoff in progress"),
              QStringLiteral("Takeoff failed"),
              [](std::shared_ptr<mavsdk::Action> action, mavsdk::Action::ResultCallback callback) {
                  action->takeoff_async(callback);
              });
}

void VehicleActionManager::disarm()
{
    runAction(QStringLiteral("disarm"),
              QStringLiteral("vehicle_disarm"),
              QStringLiteral("Aircraft disarmed"),
              QStringLiteral("Disarm failed"),
              [](std::shared_ptr<mavsdk::Action> action, mavsdk::Action::ResultCallback callback) {
                  action->disarm_async(callback);
              });
}

void VehicleActionManager::land()
{
    runAction(QStringLiteral("land"),
              QStringLiteral("land_requested"),
              QStringLiteral("Land command accepted"),
              QStringLiteral("Land command failed"),
              [](std::shared_ptr<mavsdk::Action> action, mavsdk::Action::ResultCallback callback) {
                  action->land_async(callback);
              });
}

void VehicleActionManager::returnToLaunch()
{
    runAction(QStringLiteral("return to home"),
              QStringLiteral("return_to_home_requested"),
              QStringLiteral("Return-to-home command accepted"),
              QStringLiteral("Return-to-home command failed"),
              [](std::shared_ptr<mavsdk::Action> action, mavsdk::Action::ResultCallback callback) {
                  action->return_to_launch_async(callback);
              });
}

void VehicleActionManager::holdPosition()
{
    runAction(QStringLiteral("hold position"),
              QStringLiteral("vehicle_hold"),
              QStringLiteral("Hold position command accepted"),
              QStringLiteral("Hold position command failed"),
              [](std::shared_ptr<mavsdk::Action> action, mavsdk::Action::ResultCallback callback) {
                  action->hold_async(callback);
              });
}

void VehicleActionManager::emergencyStop()
{
    runAction(QStringLiteral("emergency stop"),
              QStringLiteral("emergency_stop_requested"),
              QStringLiteral("Emergency stop accepted"),
              QStringLiteral("Emergency stop failed"),
              [](std::shared_ptr<mavsdk::Action> action, mavsdk::Action::ResultCallback callback) {
                  action->kill_async(callback);
              });
}

void VehicleActionManager::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit actionChanged();
}

void VehicleActionManager::setTakeoffReady(bool ready)
{
    if (m_takeoffReady == ready) {
        return;
    }
    m_takeoffReady = ready;
    emit actionChanged();
}

void VehicleActionManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit actionChanged();
}

bool VehicleActionManager::canCommandVehicle(const QString &actionName)
{
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Cannot %1: device approval required").arg(actionName));
        return false;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system() || !m_vehicle->system()->is_connected()) {
        setStatus(QStringLiteral("Cannot %1: no connected aircraft").arg(actionName));
        return false;
    }
    if (m_preflight) {
        m_preflight->runChecklist(false);
        if (!m_preflight->canArm()) {
            setStatus(QStringLiteral("Cannot %1: %2").arg(actionName, m_preflight->blockReason()));
            return false;
        }
    }
    return true;
}

void VehicleActionManager::runAction(const QString &actionName,
                                     const QString &eventType,
                                     const QString &successStatus,
                                     const QString &failureStatus,
                                     const std::function<void(std::shared_ptr<mavsdk::Action>,
                                                              mavsdk::Action::ResultCallback)> &command)
{
    if (!canCommandVehicle(actionName)) {
        return;
    }
    setBusy(true);
    setStatus(QStringLiteral("%1 command sent...").arg(actionName.left(1).toUpper() + actionName.mid(1)));
    auto action = std::make_shared<mavsdk::Action>(m_vehicle->system());
    command(action, [this, action, eventType, successStatus, failureStatus](mavsdk::Action::Result result) {
        Q_UNUSED(action)
        QMetaObject::invokeMethod(this, [this, eventType, successStatus, failureStatus, result]() {
            setBusy(false);
            const bool ok = result == mavsdk::Action::Result::Success;
            setStatus(ok ? successStatus : failureStatus);
            recordVehicleAction(m_pilotActions,
                                m_events,
                                ok ? eventType : QStringLiteral("mission_failed"),
                                ok ? QStringLiteral("info") : QStringLiteral("error"),
                                ok ? successStatus : failureStatus);
        }, Qt::QueuedConnection);
    });
}
