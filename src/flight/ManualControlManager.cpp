#include "ManualControlManager.h"

#include "../auth/SessionManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"

#include <mavsdk/plugins/manual_control/manual_control.hpp>
#include <mavsdk/system.hpp>

#include <QMetaObject>
#include <QJsonObject>

#include <algorithm>

ManualControlManager::ManualControlManager(MavsdkVehicleManager *vehicle,
                                           SessionManager *session,
                                           GcsEventSyncManager *events,
                                           QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_session(session),
      m_events(events)
{
    m_publishTimer.setInterval(80);
    m_eventThrottle.start();
    connect(&m_publishTimer, &QTimer::timeout, this, &ManualControlManager::publishInput);
    if (m_vehicle) {
        connect(m_vehicle, &MavsdkVehicleManager::vehicleChanged, this, [this]() {
            if ((!m_vehicle->connected() || !m_vehicle->system() || !m_vehicle->system()->is_connected()) && m_active) {
                stopManualControl();
                setStatus(QStringLiteral("Manual control stopped: aircraft link lost"));
            }
        });
    }
}

bool ManualControlManager::active() const { return m_active; }
QString ManualControlManager::status() const { return m_status; }
double ManualControlManager::forward() const { return m_forward; }
double ManualControlManager::lateral() const { return m_lateral; }
double ManualControlManager::vertical() const { return m_vertical; }
double ManualControlManager::yaw() const { return m_yaw; }

void ManualControlManager::startManualControl()
{
    if (!canControl()) {
        return;
    }
    if (!m_manualControl) {
        m_manualControl = std::make_shared<mavsdk::ManualControl>(m_vehicle->system());
    }
    m_manualControl->set_manual_control_input(0.0f, 0.0f, 0.5f, 0.0f);
    m_manualControl->start_position_control_async([this](mavsdk::ManualControl::Result result) {
        QMetaObject::invokeMethod(this, [this, result]() {
            m_active = result == mavsdk::ManualControl::Result::Success;
            if (m_active) {
                m_publishTimer.start();
                setStatus(QStringLiteral("Pilot manual control active"));
                if (m_events) {
                    m_events->recordEvent(QStringLiteral("pilot_mode_manual_control"), QStringLiteral("info"), QStringLiteral("Manual control started"));
                }
            } else {
                setStatus(QStringLiteral("Manual control start failed"));
            }
            emit manualControlChanged();
        }, Qt::QueuedConnection);
    });
}

void ManualControlManager::stopManualControl()
{
    neutral();
    m_publishTimer.stop();
    m_active = false;
    m_manualControl.reset();
    setStatus(QStringLiteral("Manual control stopped"));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("pilot_mode_manual_control"), QStringLiteral("info"), QStringLiteral("Manual control stopped"));
    }
    emit manualControlChanged();
}

void ManualControlManager::setInput(double forward, double lateral, double vertical, double yaw)
{
    m_forward = std::clamp(forward, -1.0, 1.0);
    m_lateral = std::clamp(lateral, -1.0, 1.0);
    m_vertical = std::clamp(vertical, -1.0, 1.0);
    m_yaw = std::clamp(yaw, -1.0, 1.0);
    if (!m_active) {
        startManualControl();
    } else {
        publishInput();
    }
    if (m_events && m_eventThrottle.elapsed() > 600) {
        m_eventThrottle.restart();
        m_events->recordEvent(QStringLiteral("pilot_manual_control_input"),
                              QStringLiteral("info"),
                              QStringLiteral("Manual flight control input"),
                              QJsonObject{
                                  {QStringLiteral("forward"), m_forward},
                                  {QStringLiteral("lateral"), m_lateral},
                                  {QStringLiteral("vertical"), m_vertical},
                                  {QStringLiteral("yaw"), m_yaw},
                              });
    }
    emit manualControlChanged();
}

void ManualControlManager::neutral()
{
    m_forward = 0.0;
    m_lateral = 0.0;
    m_vertical = 0.0;
    m_yaw = 0.0;
    publishInput();
    emit manualControlChanged();
}

void ManualControlManager::goToCoordinate(double latitude, double longitude, double altitudeMeters, double speedMps)
{
    const bool valid = latitude >= -90.0 && latitude <= 90.0
        && longitude >= -180.0 && longitude <= 180.0
        && altitudeMeters >= 0.0
        && speedMps > 0.0;
    if (!valid) {
        setStatus(QStringLiteral("Go-to coordinate rejected: invalid target"));
        if (m_events) {
            m_events->recordEvent(QStringLiteral("go_to_coordinate_rejected"),
                                  QStringLiteral("warning"),
                                  QStringLiteral("Invalid go-to coordinate requested"),
                                  QJsonObject{
                                      {QStringLiteral("latitude"), latitude},
                                      {QStringLiteral("longitude"), longitude},
                                      {QStringLiteral("altitude_m"), altitudeMeters},
                                      {QStringLiteral("speed_mps"), speedMps},
                                  });
        }
        return;
    }

    setStatus(canControl()
                  ? QStringLiteral("Go-to coordinate queued: guided mode pending")
                  : QStringLiteral("Go-to coordinate queued locally"));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("go_to_coordinate_requested"),
                              QStringLiteral("info"),
                              QStringLiteral("Pilot requested guided go-to coordinate"),
                              QJsonObject{
                                  {QStringLiteral("latitude"), latitude},
                                  {QStringLiteral("longitude"), longitude},
                                  {QStringLiteral("altitude_m"), altitudeMeters},
                                  {QStringLiteral("speed_mps"), speedMps},
                                  {QStringLiteral("implemented"), false},
                              });
    }
}

bool ManualControlManager::canControl() const
{
    if (!m_session || !m_session->operationsAllowed()) {
        const_cast<ManualControlManager *>(this)->setStatus(QStringLiteral("Pilot mode blocked: device approval required"));
        return false;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system() || !m_vehicle->system()->is_connected()) {
        const_cast<ManualControlManager *>(this)->setStatus(QStringLiteral("Pilot mode blocked: no connected aircraft"));
        return false;
    }
    return true;
}

void ManualControlManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit manualControlChanged();
}

void ManualControlManager::publishInput()
{
    if (!m_active || !m_manualControl || !m_vehicle || !m_vehicle->connected() || !m_vehicle->system() || !m_vehicle->system()->is_connected()) {
        if (m_active) {
            m_publishTimer.stop();
            m_active = false;
            emit manualControlChanged();
        }
        return;
    }
    const float z = static_cast<float>(std::clamp(0.5 + (m_vertical * 0.35), 0.0, 1.0));
    const auto result = m_manualControl->set_manual_control_input(static_cast<float>(m_forward),
                                                                  static_cast<float>(m_lateral),
                                                                  z,
                                                                  static_cast<float>(m_yaw));
    if (result != mavsdk::ManualControl::Result::Success) {
        setStatus(QStringLiteral("Manual input rejected"));
    }
}
