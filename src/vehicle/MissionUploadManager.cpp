#include "MissionUploadManager.h"

#include "MavsdkVehicleManager.h"
#include "VehicleTelemetryModel.h"
#include "../auth/SessionManager.h"
#include "../access/PermissionManager.h"
#include "../models/MissionPlanModel.h"
#include "../network/ApiClient.h"
#include "../flight/PreflightChecklistManager.h"
#include "../mission/AdvancedMissionManager.h"
#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"

#include <mavsdk/plugins/mission/mission.hpp>
#include <mavsdk/plugins/mission_raw/mission_raw.h>

#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>

#include <cmath>
#include <memory>
#include <sstream>
#include <thread>

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

QString rawUploadFailureMessage(mavsdk::MissionRaw::Result result)
{
    switch (result) {
    case mavsdk::MissionRaw::Result::Success:
        return QStringLiteral("Raw mission uploaded successfully.");
    case mavsdk::MissionRaw::Result::Busy:
        return QStringLiteral("Raw mission upload failed because the aircraft is busy.");
    case mavsdk::MissionRaw::Result::Timeout:
        return QStringLiteral("Raw mission upload timed out.");
    case mavsdk::MissionRaw::Result::NoSystem:
        return QStringLiteral("Aircraft not connected.");
    case mavsdk::MissionRaw::Result::Denied:
        return QStringLiteral("Raw mission upload was denied by the aircraft.");
    case mavsdk::MissionRaw::Result::InvalidArgument:
        return QStringLiteral("Raw mission upload failed because one or more mission rows are invalid.");
    case mavsdk::MissionRaw::Result::TooManyMissionItems:
        return QStringLiteral("Raw mission upload failed because there are too many mission items.");
    case mavsdk::MissionRaw::Result::IntMessagesNotSupported:
        return QStringLiteral("Raw mission upload failed: MISSION_INT is not supported by this autopilot path.");
    default:
        return QStringLiteral("Raw mission upload failed: %1").arg(enumString(result));
    }
}

int32_t coordinateToRawInt(double value)
{
    if (std::abs(value) <= 180.0) {
        return int32_t(std::llround(value * 10000000.0));
    }
    return int32_t(std::llround(value));
}

mavsdk::MissionRaw::MissionItem rawMapToItem(const QVariantMap &map)
{
    mavsdk::MissionRaw::MissionItem item{};
    item.seq = uint32_t(map.value(QStringLiteral("seq")).toInt());
    item.frame = uint32_t(map.value(QStringLiteral("frame"), 3).toInt());
    item.command = uint32_t(map.value(QStringLiteral("command"), 16).toInt());
    item.current = uint32_t(map.value(QStringLiteral("current")).toInt());
    item.autocontinue = uint32_t(map.value(QStringLiteral("autocontinue"), 1).toInt());
    item.param1 = float(map.value(QStringLiteral("param1")).toDouble());
    item.param2 = float(map.value(QStringLiteral("param2")).toDouble());
    item.param3 = float(map.value(QStringLiteral("param3")).toDouble());
    item.param4 = float(map.value(QStringLiteral("param4")).toDouble());
    item.x = map.contains(QStringLiteral("latitude"))
        ? coordinateToRawInt(map.value(QStringLiteral("latitude")).toDouble())
        : int32_t(map.value(QStringLiteral("x")).toInt());
    item.y = map.contains(QStringLiteral("longitude"))
        ? coordinateToRawInt(map.value(QStringLiteral("longitude")).toDouble())
        : int32_t(map.value(QStringLiteral("y")).toInt());
    item.z = float(map.value(QStringLiteral("altitude"), map.value(QStringLiteral("z"))).toDouble());
    item.mission_type = uint32_t(map.value(QStringLiteral("missionType")).toInt());
    return item;
}
}

MissionUploadManager::MissionUploadManager(MavsdkVehicleManager *vehicle,
                                           VehicleTelemetryModel *telemetry,
                                           MissionPlanModel *plan,
                                           ApiClient *api,
                                           SessionManager *session,
                                           PermissionManager *permissions,
                                           PreflightChecklistManager *preflight,
                                           AccessManager *access,
                                           AdvancedMissionManager *advancedMission,
                                           GcsEventSyncManager *events,
                                           QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_telemetry(telemetry),
      m_plan(plan),
      m_api(api),
      m_session(session),
      m_permissions(permissions),
      m_preflight(preflight),
      m_access(access),
      m_advancedMission(advancedMission),
      m_events(events)
{
}

bool MissionUploadManager::uploading() const { return m_uploading; }
bool MissionUploadManager::uploaded() const { return m_uploaded; }
int MissionUploadManager::progress() const { return m_progress; }
QString MissionUploadManager::status() const { return m_status; }

void MissionUploadManager::uploadActiveMission()
{
    QVariantMap context{
        {QStringLiteral("mission_id"), m_plan ? m_plan->missionId() : QString()},
        {QStringLiteral("vehicle_system_id"), m_vehicle ? m_vehicle->systemId() : QString()}
    };
    if (m_access && !m_access->authorizeAction(QStringLiteral("mission_upload"),
                                               context,
                                               QStringLiteral("Mission upload blocked by local permissions."))) {
        const QString message = QStringLiteral("Mission upload blocked by local permissions.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }
    if (!m_access && (!m_permissions || !m_permissions->hasPermission(QStringLiteral("can_upload_mission")))) {
        const QString message = QStringLiteral("Mission upload blocked: role does not allow aircraft upload.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }
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
    if (useAdvancedRawMissionUpload()) {
        uploadAdvancedRawMission();
        return;
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

bool MissionUploadManager::useAdvancedRawMissionUpload() const
{
    return m_advancedMission
        && m_advancedMission->useForUpload()
        && m_advancedMission->hasUploadableMissionItems();
}

void MissionUploadManager::uploadAdvancedRawMission()
{
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        const QString message = QStringLiteral("Aircraft not connected. Please connect a flight controller before raw mission upload.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }
    if (m_vehicle->armed() || m_vehicle->inAir()) {
        const QString message = QStringLiteral("Raw mission upload blocked while vehicle is armed or in air.");
        setStatus(message);
        if (m_events) {
            m_events->recordEvent(QStringLiteral("mission_upload_raw_blocked"),
                                  QStringLiteral("warning"),
                                  QStringLiteral("Raw MAVLink mission upload blocked by safety interlock"),
                                  QJsonObject{{QStringLiteral("armed"), m_vehicle->armed()},
                                              {QStringLiteral("in_air"), m_vehicle->inAir()}});
        }
        emit missionUploadFailed(message);
        return;
    }
    const QVariantList rows = m_advancedMission->missionItems();
    if (rows.isEmpty()) {
        const QString message = QStringLiteral("Raw mission upload blocked: no MAVLink mission rows.");
        setStatus(message);
        emit missionUploadFailed(message);
        return;
    }
    std::vector<mavsdk::MissionRaw::MissionItem> items;
    items.reserve(size_t(rows.size()));
    for (const QVariant &entry : rows) {
        items.push_back(rawMapToItem(entry.toMap()));
    }

    const auto system = m_vehicle->system();
    m_uploading = true;
    m_uploaded = false;
    m_progress = 0;
    if (m_plan) {
        m_plan->markUploading();
        m_plan->setExecutionProgress(-1, 0);
    }
    setStatus(QStringLiteral("Uploading raw MAVLink mission table to aircraft."));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("mission_upload_raw_started"),
                              QStringLiteral("info"),
                              QStringLiteral("Raw MAVLink mission upload started"),
                              QJsonObject{{QStringLiteral("upload_item_count"), rows.size()},
                                          {QStringLiteral("vehicle_system_id"), m_vehicle->systemId()}});
    }
    emit uploadChanged();

    QPointer<MissionUploadManager> self(this);
    std::thread([self, system, items]() {
        mavsdk::MissionRaw raw(system);
        const mavsdk::MissionRaw::Result result = raw.upload_mission(items);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, count = int(items.size())]() {
            if (!self) {
                return;
            }
            self->m_uploading = false;
            if (result == mavsdk::MissionRaw::Result::Success) {
                self->m_uploaded = true;
                self->m_progress = 100;
                if (self->m_plan) {
                    self->m_plan->markUploaded(QStringLiteral("Raw MAVLink mission uploaded successfully."));
                }
                if (self->m_events) {
                    self->m_events->recordEvent(QStringLiteral("mission_uploaded_raw"),
                                                QStringLiteral("info"),
                                                QStringLiteral("Raw MAVLink mission uploaded to aircraft"),
                                                QJsonObject{{QStringLiteral("uploaded_item_count"), count},
                                                            {QStringLiteral("vehicle_system_id"), self->m_vehicle ? self->m_vehicle->systemId() : QString()}});
                }
                self->setStatus(QStringLiteral("Raw MAVLink mission uploaded successfully."));
                emit self->missionUploaded();
            } else {
                self->m_uploaded = false;
                const QString message = rawUploadFailureMessage(result);
                if (self->m_plan) {
                    self->m_plan->markUploadFailed(message);
                }
                if (self->m_events) {
                    self->m_events->recordEvent(QStringLiteral("mission_upload_raw_failed"),
                                                QStringLiteral("error"),
                                                QStringLiteral("Raw MAVLink mission upload failed"),
                                                QJsonObject{{QStringLiteral("result"), enumString(result)}});
                }
                self->setStatus(message);
                emit self->missionUploadFailed(message);
            }
            emit self->uploadChanged();
        }, Qt::QueuedConnection);
    }).detach();
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
