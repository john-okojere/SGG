#include "MissionExecutionManager.h"

#include "MavsdkVehicleManager.h"
#include "../auth/SessionManager.h"
#include "../models/MissionPlanModel.h"
#include "../network/ApiClient.h"
#include "../flight/PreflightChecklistManager.h"
#include "../sync/FlightSessionSyncManager.h"
#include "../sync/GcsEventSyncManager.h"

#include <mavsdk/plugins/mission/mission.hpp>
#include <mavsdk/plugins/action/action.hpp>

#include <QJsonObject>
#include <QMetaObject>
#include <QProcessEnvironment>
#include <QTimer>
#include <QUuid>
#include <QtGlobal>

#include <functional>
#include <memory>
#include <sstream>

namespace {
template <typename T>
QString enumString(T value)
{
    std::ostringstream stream;
    stream << value;
    return QString::fromStdString(stream.str());
}
}

MissionExecutionManager::MissionExecutionManager(MavsdkVehicleManager *vehicle,
                                                 MissionPlanModel *plan,
                                                 ApiClient *api,
                                                 SessionManager *session,
                                                 FlightSessionSyncManager *flightSessions,
                                                 PreflightChecklistManager *preflight,
                                                 GcsEventSyncManager *events,
                                                 QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_plan(plan),
      m_api(api),
      m_session(session),
      m_flightSessions(flightSessions),
      m_preflight(preflight),
      m_events(events)
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    m_progressSyncIntervalMs = qBound(500,
                                      env.value(QStringLiteral("SKYGRID_MISSION_PROGRESS_INTERVAL_MS"),
                                                env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true")
                                                    ? QStringLiteral("1800")
                                                    : QStringLiteral("1500")).toInt(),
                                      10000);
}

bool MissionExecutionManager::executing() const { return m_executing; }
int MissionExecutionManager::activeWaypoint() const { return m_activeWaypoint; }
int MissionExecutionManager::progress() const { return m_progress; }
QString MissionExecutionManager::status() const { return m_status; }

void MissionExecutionManager::startMission()
{
    if (!m_session || !m_session->operationsAllowed()) {
        const QString message = QStringLiteral("Device approval required before aircraft upload.");
        setStatus(message);
        emit missionStartFailed(message);
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        const QString message = QStringLiteral("Aircraft not connected. Please connect Gazebo/PX4 before starting.");
        setStatus(message);
        emit missionStartFailed(message);
        return;
    }
    if (!m_plan || m_plan->uploadState() != QStringLiteral("uploaded")) {
        const QString message = QStringLiteral("Upload the mission before starting autonomous flight.");
        setStatus(message);
        emit missionStartFailed(message);
        return;
    }
    if (m_preflight) {
        m_preflight->runChecklist(false);
        if (!m_preflight->canStartMission()) {
            const QString reason = m_preflight->blockReason();
            const QString message = reason.isEmpty() ? QStringLiteral("Preflight failed.") : QStringLiteral("Preflight failed: %1").arg(reason);
            setStatus(message);
            emit missionStartFailed(message);
            return;
        }
    }

    m_activeMission = std::make_shared<mavsdk::Mission>(m_vehicle->system());
    m_activeMission->subscribe_mission_progress([this](mavsdk::Mission::MissionProgress progress) {
        QMetaObject::invokeMethod(this, [this, progress]() {
            m_activeWaypoint = progress.current;
            m_progress = progress.total > 0 ? qBound(0, static_cast<int>((progress.current * 100.0) / progress.total), 100) : 0;
            if (m_plan) {
                m_plan->setExecutionProgress(m_activeWaypoint, m_progress);
            }
            const QDateTime now = QDateTime::currentDateTimeUtc();
            const bool shouldSyncProgress = !m_lastProgressSyncAt.isValid()
                || m_lastProgressSyncAt.msecsTo(now) >= m_progressSyncIntervalMs
                || m_progress >= 100;
            if (shouldSyncProgress) {
                postExecutionAction(QStringLiteral("execution-progress"), QJsonObject{
                    {QStringLiteral("active_waypoint"), m_activeWaypoint},
                    {QStringLiteral("progress_percent"), m_progress}
                });
                m_lastProgressSyncAt = now;
            }
                if (m_events && (m_progress == 0 || m_progress == 100 || m_progress % 20 == 0)) {
                    m_events->recordEvent(QStringLiteral("mission_progress"), QStringLiteral("info"), QStringLiteral("Mission progress updated"), QJsonObject{
                        {QStringLiteral("active_waypoint"), m_activeWaypoint},
                        {QStringLiteral("progress_percent"), m_progress}
                    });
                }
            if (progress.total > 0 && progress.current >= progress.total) {
                m_executing = false;
                if (m_plan) {
                    m_plan->markCompleted(true, QString());
                }
                postExecutionAction(QStringLiteral("finish-execution"), QJsonObject{
                    {QStringLiteral("success"), true},
                    {QStringLiteral("reason"), QString()}
                });
                if (m_flightSessions) {
                    m_flightSessions->endActiveSession(QStringLiteral("completed"), QStringLiteral("Mission execution completed"));
                }
                if (m_events) {
                    m_events->recordEvent(QStringLiteral("mission_completed"), QStringLiteral("info"), QStringLiteral("Mission completed"));
                }
                m_activeMission.reset();
                setStatus(QStringLiteral("Mission completed"));
            }
            emit executionChanged();
        }, Qt::QueuedConnection);
    });

    auto startUploadedMission = std::make_shared<std::function<void(int)>>();
    *startUploadedMission = [this, startUploadedMission](int attempt) {
        m_activeMission->start_mission_async([this, startUploadedMission, attempt](mavsdk::Mission::Result result) {
            QMetaObject::invokeMethod(this, [this, result, startUploadedMission, attempt]() {
                if (result == mavsdk::Mission::Result::Success) {
                    m_executing = true;
                    if (m_plan) {
                        m_plan->markExecuting();
                    }
                    postExecutionAction(QStringLiteral("start-execution"), QJsonObject{
                        {QStringLiteral("vehicle_system_id"), m_vehicle ? m_vehicle->systemId() : QString()}
                    });
                    if (m_flightSessions && m_plan) {
                        const QString clientSessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
                        m_flightSessions->beginMissionSession(clientSessionId, m_plan->missionId());
                    }
                    if (m_events) {
                        m_events->recordEvent(QStringLiteral("mission_started"), QStringLiteral("info"), QStringLiteral("Mission execution started"), QJsonObject{
                            {QStringLiteral("vehicle_system_id"), m_vehicle ? m_vehicle->systemId() : QString()}
                        });
                    }
                    setStatus(QStringLiteral("Starting autonomous flight."));
                    emit missionStarted();
                } else if (attempt < 2 && m_activeMission) {
                    if (m_events) {
                        m_events->recordEvent(QStringLiteral("mission_start_retry"), QStringLiteral("warning"), QStringLiteral("Retrying mission start command"), QJsonObject{
                            {QStringLiteral("attempt"), attempt + 2},
                            {QStringLiteral("result"), enumString(result)}
                        });
                    }
                    setStatus(QStringLiteral("Mission start retry %1/3...").arg(attempt + 2));
                    QTimer::singleShot(1000, this, [startUploadedMission, attempt]() {
                        (*startUploadedMission)(attempt + 1);
                    });
                    emit executionChanged();
                    return;
                } else {
                    m_executing = false;
                    m_activeMission.reset();
                    if (m_events) {
                        m_events->recordEvent(QStringLiteral("mission_failed"), QStringLiteral("error"), QStringLiteral("Mission start failed"), QJsonObject{
                            {QStringLiteral("result"), enumString(result)}
                        });
                    }
                    const QString message = QStringLiteral("Mission start failed: %1").arg(enumString(result));
                    setStatus(message);
                    emit missionStartFailed(message);
                }
                emit executionChanged();
            }, Qt::QueuedConnection);
        });
    };

    auto takeoffThenStart = std::make_shared<std::function<void(int)>>();
    *takeoffThenStart = [this, startUploadedMission, takeoffThenStart](int attempt) {
        if (m_vehicle->inAir()) {
            (*startUploadedMission)(0);
            return;
        }
        setStatus(attempt == 0
                      ? QStringLiteral("Takeoff command sent for mission...")
                      : QStringLiteral("Takeoff retry %1/3...").arg(attempt + 1));
        auto action = std::make_shared<mavsdk::Action>(m_vehicle->system());
        action->takeoff_async([this, action, startUploadedMission, takeoffThenStart, attempt](mavsdk::Action::Result result) {
            QMetaObject::invokeMethod(this, [this, action, result, startUploadedMission, takeoffThenStart, attempt]() {
                Q_UNUSED(action)
                if (result != mavsdk::Action::Result::Success) {
                    if (attempt < 2) {
                        if (m_events) {
                            m_events->recordEvent(QStringLiteral("mission_takeoff_retry"), QStringLiteral("warning"), QStringLiteral("Retrying mission takeoff command"), QJsonObject{
                                {QStringLiteral("attempt"), attempt + 2},
                                {QStringLiteral("result"), enumString(result)}
                            });
                        }
                        QTimer::singleShot(1000, this, [takeoffThenStart, attempt]() {
                            (*takeoffThenStart)(attempt + 1);
                        });
                        emit executionChanged();
                        return;
                    }
                    m_activeMission.reset();
                    if (m_events) {
                        m_events->recordEvent(QStringLiteral("mission_failed"), QStringLiteral("error"), QStringLiteral("Mission takeoff failed"), QJsonObject{
                            {QStringLiteral("result"), enumString(result)}
                        });
                    }
                    const QString message = QStringLiteral("Mission takeoff failed: %1").arg(enumString(result));
                    setStatus(message);
                    emit missionStartFailed(message);
                    emit executionChanged();
                    return;
                }
                if (m_events) {
                    m_events->recordEvent(QStringLiteral("takeoff_started"), QStringLiteral("info"), QStringLiteral("Aircraft takeoff accepted before mission start"));
                }
                (*startUploadedMission)(0);
            }, Qt::QueuedConnection);
        });
    };

    if (!m_vehicle->armed()) {
        setStatus(QStringLiteral("Arming aircraft for mission..."));
        auto armThenContinue = std::make_shared<std::function<void(int)>>();
        *armThenContinue = [this, armThenContinue, takeoffThenStart](int attempt) {
            if (attempt > 0) {
                setStatus(QStringLiteral("Arm retry %1/3...").arg(attempt + 1));
            }
            auto action = std::make_shared<mavsdk::Action>(m_vehicle->system());
            action->arm_async([this, action, armThenContinue, takeoffThenStart, attempt](mavsdk::Action::Result result) {
                QMetaObject::invokeMethod(this, [this, action, result, armThenContinue, takeoffThenStart, attempt]() {
                    Q_UNUSED(action)
                    if (result != mavsdk::Action::Result::Success) {
                        if (attempt < 2) {
                            if (m_events) {
                                m_events->recordEvent(QStringLiteral("mission_arm_retry"), QStringLiteral("warning"), QStringLiteral("Retrying mission arm command"), QJsonObject{
                                    {QStringLiteral("attempt"), attempt + 2},
                                    {QStringLiteral("result"), enumString(result)}
                                });
                            }
                            QTimer::singleShot(1000, this, [armThenContinue, attempt]() {
                                (*armThenContinue)(attempt + 1);
                            });
                            emit executionChanged();
                            return;
                        }
                        m_activeMission.reset();
                        if (m_events) {
                            m_events->recordEvent(QStringLiteral("mission_failed"), QStringLiteral("error"), QStringLiteral("Mission arm failed"), QJsonObject{
                                {QStringLiteral("result"), enumString(result)}
                            });
                        }
                        const QString message = QStringLiteral("Mission arm failed: %1").arg(enumString(result));
                        setStatus(message);
                        emit missionStartFailed(message);
                        emit executionChanged();
                        return;
                    }
                    if (m_events) {
                        m_events->recordEvent(QStringLiteral("vehicle_arm"), QStringLiteral("info"), QStringLiteral("Aircraft armed for mission start"));
                    }
                    (*takeoffThenStart)(0);
                }, Qt::QueuedConnection);
            });
        };
        (*armThenContinue)(0);
    } else if (!m_vehicle->inAir()) {
        (*takeoffThenStart)(0);
    } else {
        (*startUploadedMission)(0);
    }
}

void MissionExecutionManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit executionChanged();
}

void MissionExecutionManager::postExecutionAction(const QString &action, const QJsonObject &payload)
{
    if (!m_api || !m_session || !m_session->operationsAllowed() || !m_plan || m_plan->missionId().isEmpty() || m_plan->createdLocally()) {
        return;
    }
    m_api->post(QStringLiteral("/api/missions/%1/%2/").arg(m_plan->missionId(), action), payload, true, true,
                [](int, const QJsonObject &, const QString &) {});
}
