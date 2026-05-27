#include "MissionUploadManager.h"

#include "MavsdkVehicleManager.h"
#include "VehicleTelemetryModel.h"
#include "../auth/SessionManager.h"
#include "../models/MissionPlanModel.h"
#include "../network/ApiClient.h"
#include "../flight/PreflightChecklistManager.h"
#include "../sync/GcsEventSyncManager.h"

#include <mavsdk/plugins/mission/mission.hpp>

#include <QJsonObject>
#include <QMetaObject>

#include <cmath>
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

QString uploadFailureMessage(mavsdk::Mission::Result result)
{
    switch (result) {
    case mavsdk::Mission::Result::ProtocolError:
        return QStringLiteral("Mission upload failed because PX4 rejected the mission transfer. Close other GCS/MAVSDK clients or restart Gazebo/PX4, then try again.");
    case mavsdk::Mission::Result::Busy:
        return QStringLiteral("Mission upload failed because the aircraft is busy. Wait a moment and try Start Flying again.");
    case mavsdk::Mission::Result::Timeout:
        return QStringLiteral("Mission upload timed out. Check the Gazebo/PX4 connection and try again.");
    case mavsdk::Mission::Result::NoSystem:
        return QStringLiteral("Aircraft not connected. Please connect Gazebo/PX4 before starting.");
    case mavsdk::Mission::Result::TooManyMissionItems:
        return QStringLiteral("Mission upload failed because the route has too many mission items for the aircraft.");
    case mavsdk::Mission::Result::InvalidArgument:
        return QStringLiteral("Mission upload failed because one or more route points are invalid.");
    case mavsdk::Mission::Result::Denied:
        return QStringLiteral("Mission upload was denied by the aircraft. Check PX4 preflight state and safety settings.");
    default:
        return QStringLiteral("Mission upload failed: %1").arg(enumString(result));
    }
}
}

MissionUploadManager::MissionUploadManager(MavsdkVehicleManager *vehicle,
                                           VehicleTelemetryModel *telemetry,
                                           MissionPlanModel *plan,
                                           ApiClient *api,
                                           SessionManager *session,
                                           PreflightChecklistManager *preflight,
                                           GcsEventSyncManager *events,
                                           QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_telemetry(telemetry),
      m_plan(plan),
      m_api(api),
      m_session(session),
      m_preflight(preflight),
      m_events(events)
{
}

bool MissionUploadManager::uploading() const { return m_uploading; }
bool MissionUploadManager::uploaded() const { return m_uploaded; }
int MissionUploadManager::progress() const { return m_progress; }
QString MissionUploadManager::status() const { return m_status; }

void MissionUploadManager::uploadActiveMission()
{
    if (!m_session || !m_session->operationsAllowed()) {
        const QString message = QStringLiteral("Device approval required before aircraft upload.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        const QString message = QStringLiteral("Aircraft not connected. Please connect Gazebo/PX4 before starting.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }
    if (m_plan && m_plan->missionType() == QStringLiteral("virtualFence")) {
        const QString message = QStringLiteral("Virtual Fence is a Control Center safety boundary and cannot start as a route mission.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }
    if (m_plan && (m_plan->createdLocally() || !m_plan->backendSyncReady() || !m_plan->backendUploadEligible())) {
        const QString message = QStringLiteral("Backend validation required.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }
    if (m_plan && !m_plan->hasTakeoffPoint()) {
        const QString message = QStringLiteral("Set a takeoff point before starting the mission.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }
    if (m_preflight) {
        m_preflight->runChecklist(false);
        if (!m_preflight->canUpload()) {
            const QString reason = m_preflight->blockReason();
            const QString message = reason.isEmpty() ? QStringLiteral("Preflight failed.") : QStringLiteral("Preflight failed: %1").arg(reason);
            setStatus(message);
            emit missionUploadFailed(message);
            return;
        }
    }
    if (!m_plan || !m_telemetry || !m_plan->validateForUpload(m_telemetry->connected(), m_telemetry->aircraftReady())) {
        const QString message = QStringLiteral("Mission is not ready for aircraft upload.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }

    const QVariantList route = m_plan->serializeForMavsdkMission();
    if (route.size() < 2) {
        const QString message = QStringLiteral("Add at least two route points before starting.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }

    mavsdk::Mission::MissionPlan missionPlan;
    for (int i = 0; i < route.size(); ++i) {
        const QVariantMap point = route.at(i).toMap();
        mavsdk::Mission::MissionItem item{};
        item.latitude_deg = point.value(QStringLiteral("latitude")).toDouble();
        item.longitude_deg = point.value(QStringLiteral("longitude")).toDouble();
        item.relative_altitude_m = static_cast<float>(point.value(QStringLiteral("altitude"), m_plan->altitude()).toDouble());
        item.speed_m_s = static_cast<float>(point.value(QStringLiteral("speed"), m_plan->speed()).toDouble());
        const QString waypointAction = point.value(QStringLiteral("action")).toString();
        const QString cameraMode = point.value(QStringLiteral("camera_mode")).toString();
        item.is_fly_through = waypointAction != QStringLiteral("Hover") && waypointAction != QStringLiteral("Q-loiter");
        item.gimbal_pitch_deg = static_cast<float>(point.value(QStringLiteral("gimbal_pitch"), -45.0).toDouble());
        item.yaw_deg = static_cast<float>(point.value(QStringLiteral("heading"), NAN).toDouble());
        if (waypointAction == QStringLiteral("Take Photo") || cameraMode == QStringLiteral("Take Photo")) {
            item.camera_action = mavsdk::Mission::MissionItem::CameraAction::TakePhoto;
        } else if (waypointAction == QStringLiteral("Start Recording") || cameraMode == QStringLiteral("Start Recording")) {
            item.camera_action = mavsdk::Mission::MissionItem::CameraAction::StartVideo;
        } else if (waypointAction == QStringLiteral("Stop Recording") || cameraMode == QStringLiteral("Stop Recording")) {
            item.camera_action = mavsdk::Mission::MissionItem::CameraAction::StopVideo;
        } else {
            item.camera_action = i == 0
                ? mavsdk::Mission::MissionItem::CameraAction::StartPhotoInterval
                : (i == route.size() - 1 ? mavsdk::Mission::MissionItem::CameraAction::StopPhotoInterval : mavsdk::Mission::MissionItem::CameraAction::None);
        }
        item.camera_photo_interval_s = m_plan->captureInterval();
        missionPlan.mission_items.push_back(item);
    }

    auto mission = std::make_shared<mavsdk::Mission>(m_vehicle->system());
    m_uploading = true;
    if (m_plan) {
        m_plan->markUploading();
        m_plan->setExecutionProgress(-1, 0);
    }
    m_uploaded = false;
    m_progress = 0;
    setStatus(QStringLiteral("Uploading mission to aircraft."));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("mission_upload_started"), QStringLiteral("info"), QStringLiteral("Mission upload started"), QJsonObject{
            {QStringLiteral("upload_item_count"), route.size()},
            {QStringLiteral("vehicle_system_id"), m_vehicle ? m_vehicle->systemId() : QString()}
        });
    }
    emit uploadChanged();

    mission->upload_mission_with_progress_async(missionPlan, [this, mission, count = route.size()](mavsdk::Mission::Result result, mavsdk::Mission::ProgressData progress) {
        QMetaObject::invokeMethod(this, [this, result, progress, count]() {
            m_progress = qBound(0, static_cast<int>(progress.progress * 100.0), 100);
            if (result == mavsdk::Mission::Result::Next) {
                emit uploadChanged();
                return;
            }
            m_uploading = false;
            if (result == mavsdk::Mission::Result::Success) {
                m_uploaded = true;
                m_progress = 100;
                if (m_plan) {
                    m_plan->markUploaded(QStringLiteral("Mission uploaded successfully."));
                }
                markBackendUploaded(count);
                if (m_events) {
                    m_events->recordEvent(QStringLiteral("mission_uploaded"), QStringLiteral("info"), QStringLiteral("Mission uploaded to aircraft"), QJsonObject{
                        {QStringLiteral("uploaded_item_count"), count},
                        {QStringLiteral("vehicle_system_id"), m_vehicle ? m_vehicle->systemId() : QString()}
                    });
                }
                setStatus(QStringLiteral("Mission uploaded successfully."));
                emit missionUploaded();
            } else {
                m_uploaded = false;
                const QString resultText = enumString(result);
                if (m_plan) {
                    m_plan->markUploadFailed(uploadFailureMessage(result));
                }
                if (m_events) {
                    m_events->recordEvent(QStringLiteral("mission_upload_failed"), QStringLiteral("error"), QStringLiteral("Mission upload failed"), QJsonObject{
                        {QStringLiteral("result"), resultText}
                    });
                }
                const QString message = uploadFailureMessage(result);
                setStatus(message);
                emit missionUploadFailed(message);
            }
            emit uploadChanged();
        }, Qt::QueuedConnection);
    });
}

void MissionUploadManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit uploadChanged();
}

void MissionUploadManager::markBackendUploaded(int itemCount)
{
    if (!m_api || !m_session || !m_session->operationsAllowed() || !m_plan || m_plan->missionId().isEmpty() || m_plan->createdLocally()) {
        return;
    }
    const QString path = QStringLiteral("/api/missions/%1/mark-uploaded/").arg(m_plan->missionId());
    m_api->post(path, QJsonObject{
        {QStringLiteral("uploaded_item_count"), itemCount},
        {QStringLiteral("vehicle_system_id"), m_vehicle ? m_vehicle->systemId() : QString()}
    }, true, true, [](int, const QJsonObject &, const QString &) {});
}
