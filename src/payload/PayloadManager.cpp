#include "PayloadManager.h"

#include "VideoStreamManager.h"
#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"
#include "../vehicle/VehicleTelemetryModel.h"

#include <mavsdk/plugins/camera/camera.h>
#include <mavsdk/plugins/gimbal/gimbal.h>

#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>

#include <memory>
#include <thread>

namespace {
QString cameraResultString(mavsdk::Camera::Result result)
{
    switch (result) {
    case mavsdk::Camera::Result::Success: return QStringLiteral("Success");
    case mavsdk::Camera::Result::InProgress: return QStringLiteral("In progress");
    case mavsdk::Camera::Result::Busy: return QStringLiteral("Camera busy");
    case mavsdk::Camera::Result::Denied: return QStringLiteral("Denied");
    case mavsdk::Camera::Result::Error: return QStringLiteral("Error");
    case mavsdk::Camera::Result::Timeout: return QStringLiteral("Timeout");
    case mavsdk::Camera::Result::WrongArgument: return QStringLiteral("Wrong argument");
    case mavsdk::Camera::Result::NoSystem: return QStringLiteral("No system");
    case mavsdk::Camera::Result::ProtocolUnsupported: return QStringLiteral("Protocol unsupported");
    case mavsdk::Camera::Result::Unavailable: return QStringLiteral("Unavailable");
    case mavsdk::Camera::Result::CameraIdInvalid: return QStringLiteral("Camera ID invalid");
    case mavsdk::Camera::Result::ActionUnsupported: return QStringLiteral("Action unsupported");
    default: return QStringLiteral("Unknown");
    }
}

QString gimbalResultString(mavsdk::Gimbal::Result result)
{
    switch (result) {
    case mavsdk::Gimbal::Result::Success: return QStringLiteral("Success");
    case mavsdk::Gimbal::Result::Error: return QStringLiteral("Error");
    case mavsdk::Gimbal::Result::Timeout: return QStringLiteral("Timeout");
    case mavsdk::Gimbal::Result::Unsupported: return QStringLiteral("Unsupported");
    case mavsdk::Gimbal::Result::NoSystem: return QStringLiteral("No system");
    case mavsdk::Gimbal::Result::InvalidArgument: return QStringLiteral("Invalid argument");
    default: return QStringLiteral("Unknown");
    }
}
}

PayloadManager::PayloadManager(VehicleTelemetryModel *telemetry,
                               MavsdkVehicleManager *vehicle,
                               VideoStreamManager *video,
                               AccessManager *access,
                               GcsEventSyncManager *events,
                               QObject *parent)
    : QObject(parent),
      m_telemetry(telemetry),
      m_vehicle(vehicle),
      m_video(video),
      m_access(access),
      m_events(events)
{
    if (m_video) {
        connect(m_video, &VideoStreamManager::videoChanged, this, &PayloadManager::payloadChanged);
    }
}

QVariantList PayloadManager::payloadRows() const
{
    return {
        row(QStringLiteral("Payload Link"), m_telemetry && m_telemetry->connected() ? QStringLiteral("Vehicle connected") : QStringLiteral("No vehicle")),
        row(QStringLiteral("Payload Status"), QStringLiteral("Camera/gimbal adapters permission-gated")),
        row(QStringLiteral("Live Video"), m_video ? m_video->status() : m_videoStatus),
        row(QStringLiteral("Geotagging"), m_geotagStatus),
        row(QStringLiteral("Overlap"), m_overlapStatus)
    };
}

QVariantList PayloadManager::cameraRows() const
{
    return {
        row(QStringLiteral("Camera Trigger"), m_vehicle && m_vehicle->connected()
                ? QStringLiteral("MAVSDK camera command ready")
                : QStringLiteral("Connect vehicle for MAVLink camera command")),
        row(QStringLiteral("Recording"), m_telemetry ? m_telemetry->recordingState() : QStringLiteral("Unknown")),
        row(QStringLiteral("Photo Overlap"), m_overlapStatus)
    };
}

QVariantList PayloadManager::gimbalRows() const
{
    return {
        row(QStringLiteral("Gimbal Control"), m_vehicle && m_vehicle->connected()
                ? QStringLiteral("MAVSDK gimbal command ready")
                : QStringLiteral("Connect vehicle for MAVLink gimbal command")),
        row(QStringLiteral("Mount Mode"), QStringLiteral("Yaw follow / once command")),
        row(QStringLiteral("Pitch Command"), QStringLiteral("--"))
    };
}

QString PayloadManager::videoStatus() const
{
    return m_video ? m_video->status() : m_videoStatus;
}
QString PayloadManager::geotagStatus() const { return m_geotagStatus; }
QString PayloadManager::overlapStatus() const { return m_overlapStatus; }
QString PayloadManager::status() const { return m_status; }

void PayloadManager::triggerCamera()
{
    if (!authorizePayloadAction(QStringLiteral("camera trigger"))) {
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a vehicle before triggering a MAVLink camera."));
        return;
    }
    setStatus(QStringLiteral("Sending MAVLink camera trigger command."));
    auto camera = std::make_shared<mavsdk::Camera>(m_vehicle->system());
    QPointer<PayloadManager> self(this);
    camera->take_photo_async(0, [self, camera](mavsdk::Camera::Result result) {
        Q_UNUSED(camera)
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result]() {
            if (!self) {
                return;
            }
            const bool ok = result == mavsdk::Camera::Result::Success;
            self->setStatus(ok
                                ? QStringLiteral("Camera trigger accepted.")
                                : QStringLiteral("Camera trigger failed: %1").arg(cameraResultString(result)));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("payload_camera_trigger"),
                                            ok ? QStringLiteral("info") : QStringLiteral("warning"),
                                            self->m_status,
                                            QJsonObject{{QStringLiteral("result"), cameraResultString(result)}});
            }
        }, Qt::QueuedConnection);
    });
}

void PayloadManager::validateCamera()
{
    if (!authorizePayloadAction(QStringLiteral("camera validation"))) {
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a vehicle before validating a MAVLink camera."));
        return;
    }
    setStatus(QStringLiteral("Reading MAVLink camera mode, video stream, and storage status."));
    const auto system = m_vehicle->system();
    QPointer<PayloadManager> self(this);
    std::thread([self, system]() {
        mavsdk::Camera camera(system);
        const auto [modeResult, mode] = camera.get_mode(0);
        const auto [streamResult, stream] = camera.get_video_stream_info(0);
        const auto [storageResult, storage] = camera.get_storage(0);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, modeResult, mode, streamResult, stream, storageResult, storage]() {
            if (!self) {
                return;
            }
            const bool ok = modeResult == mavsdk::Camera::Result::Success
                || streamResult == mavsdk::Camera::Result::Success
                || storageResult == mavsdk::Camera::Result::Success;
            self->m_videoStatus = streamResult == mavsdk::Camera::Result::Success
                ? QStringLiteral("Camera stream %1: %2x%3 %4 fps, %5")
                      .arg(stream.stream_id)
                      .arg(stream.settings.horizontal_resolution_pix)
                      .arg(stream.settings.vertical_resolution_pix)
                      .arg(QString::number(stream.settings.frame_rate_hz, 'f', 1),
                           QString::fromStdString(stream.settings.uri))
                : QStringLiteral("Camera stream status unavailable: %1").arg(cameraResultString(streamResult));
            self->setStatus(ok
                                ? QStringLiteral("Camera validation read completed. Mode result: %1, storage: %2 MiB available.")
                                      .arg(cameraResultString(modeResult),
                                           QString::number(storage.available_storage_mib, 'f', 1))
                                : QStringLiteral("Camera validation failed: mode %1, stream %2, storage %3.")
                                      .arg(cameraResultString(modeResult),
                                           cameraResultString(streamResult),
                                           cameraResultString(storageResult)));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("payload_camera_validated"),
                                            ok ? QStringLiteral("info") : QStringLiteral("warning"),
                                            self->m_status,
                                            QJsonObject{{QStringLiteral("mode_result"), cameraResultString(modeResult)},
                                                        {QStringLiteral("mode"), int(mode)},
                                                        {QStringLiteral("stream_result"), cameraResultString(streamResult)},
                                                        {QStringLiteral("stream_uri"), QString::fromStdString(stream.settings.uri)},
                                                        {QStringLiteral("storage_result"), cameraResultString(storageResult)},
                                                        {QStringLiteral("storage_available_mib"), storage.available_storage_mib}});
            }
            emit self->payloadChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void PayloadManager::validateGimbal()
{
    if (!authorizePayloadAction(QStringLiteral("gimbal validation"))) {
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a vehicle before validating a MAVLink gimbal."));
        return;
    }
    setStatus(QStringLiteral("Reading MAVLink gimbal control and attitude status."));
    const auto system = m_vehicle->system();
    QPointer<PayloadManager> self(this);
    std::thread([self, system]() {
        mavsdk::Gimbal gimbal(system);
        const auto [controlResult, control] = gimbal.get_control_status(0);
        const auto [attitudeResult, attitude] = gimbal.get_attitude(0);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, controlResult, control, attitudeResult, attitude]() {
            if (!self) {
                return;
            }
            const bool ok = controlResult == mavsdk::Gimbal::Result::Success
                || attitudeResult == mavsdk::Gimbal::Result::Success;
            self->setStatus(ok
                                ? QStringLiteral("Gimbal validation read completed. Pitch %1 deg, yaw %2 deg.")
                                      .arg(QString::number(attitude.euler_angle_forward.pitch_deg, 'f', 1),
                                           QString::number(attitude.euler_angle_forward.yaw_deg, 'f', 1))
                                : QStringLiteral("Gimbal validation failed: control %1, attitude %2.")
                                      .arg(gimbalResultString(controlResult),
                                           gimbalResultString(attitudeResult)));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("payload_gimbal_validated"),
                                            ok ? QStringLiteral("info") : QStringLiteral("warning"),
                                            self->m_status,
                                            QJsonObject{{QStringLiteral("control_result"), gimbalResultString(controlResult)},
                                                        {QStringLiteral("control_mode"), int(control.control_mode)},
                                                        {QStringLiteral("attitude_result"), gimbalResultString(attitudeResult)},
                                                        {QStringLiteral("pitch_deg"), attitude.euler_angle_forward.pitch_deg},
                                                        {QStringLiteral("yaw_deg"), attitude.euler_angle_forward.yaw_deg}});
            }
            emit self->payloadChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void PayloadManager::setGimbalPitch(double pitch)
{
    if (!authorizePayloadAction(QStringLiteral("gimbal pitch"))) {
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a vehicle before commanding a MAVLink gimbal."));
        return;
    }
    setStatus(QStringLiteral("Sending gimbal pitch command."));
    auto gimbal = std::make_shared<mavsdk::Gimbal>(m_vehicle->system());
    QPointer<PayloadManager> self(this);
    gimbal->set_angles_async(0,
                             0.0f,
                             float(pitch),
                             0.0f,
                             mavsdk::Gimbal::GimbalMode::YawFollow,
                             mavsdk::Gimbal::SendMode::Once,
                             [self, gimbal, pitch](mavsdk::Gimbal::Result result) {
        Q_UNUSED(gimbal)
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, pitch]() {
            if (!self) {
                return;
            }
            const bool ok = result == mavsdk::Gimbal::Result::Success;
            self->setStatus(ok
                                ? QStringLiteral("Gimbal pitch command accepted: %1 deg.").arg(QString::number(pitch, 'f', 1))
                                : QStringLiteral("Gimbal pitch command failed: %1").arg(gimbalResultString(result)));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("payload_gimbal_pitch"),
                                            ok ? QStringLiteral("info") : QStringLiteral("warning"),
                                            self->m_status,
                                            QJsonObject{{QStringLiteral("pitch_deg"), pitch},
                                                        {QStringLiteral("result"), gimbalResultString(result)}});
            }
        }, Qt::QueuedConnection);
    });
}

void PayloadManager::configurePayload(const QVariantMap &config)
{
    if (!authorizePayloadAction(QStringLiteral("payload configuration"))) {
        return;
    }
    setStatus(QStringLiteral("Payload configuration saved locally for adapter handoff."));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("payload_configuration_saved"),
                              QStringLiteral("info"),
                              QStringLiteral("Payload configuration staged"),
                              QJsonObject::fromVariantMap(config));
    }
}

void PayloadManager::verifyOverlap()
{
    if (!authorizePayloadAction(QStringLiteral("overlap verification"))) {
        return;
    }
    m_overlapStatus = QStringLiteral("Overlap verification placeholder ready; requires camera FOV and mission grid data.");
    setStatus(m_overlapStatus);
    emit payloadChanged();
}

void PayloadManager::startGeotagWorkflow()
{
    if (!authorizePayloadAction(QStringLiteral("geotagging"))) {
        return;
    }
    m_geotagStatus = QStringLiteral("Geotagging placeholder ready; requires photo log and GPS time correlation.");
    setStatus(m_geotagStatus);
    emit payloadChanged();
}

QVariantMap PayloadManager::row(const QString &name, const QString &value) const
{
    return QVariantMap{{QStringLiteral("name"), name},
                       {QStringLiteral("value"), value}};
}

bool PayloadManager::authorizePayloadAction(const QString &label)
{
    if (m_access && !m_access->authorizeAction(QStringLiteral("payload_configuration"),
                                                QVariantMap{{QStringLiteral("payload_action"), label}},
                                                QStringLiteral("Payload action blocked by local permissions."))) {
        setStatus(QStringLiteral("Payload action blocked by RBAC."));
        return false;
    }
    return true;
}

void PayloadManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit payloadChanged();
}
