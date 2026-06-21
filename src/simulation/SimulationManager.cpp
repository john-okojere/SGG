#include "SimulationManager.h"

#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"
#include "../vehicle/VehicleConfigManager.h"

#include <QJsonObject>

SimulationManager::SimulationManager(VehicleConfigManager *vehicleConfig,
                                     MavsdkVehicleManager *vehicle,
                                     AccessManager *access,
                                     GcsEventSyncManager *events,
                                     QObject *parent)
    : QObject(parent),
      m_vehicleConfig(vehicleConfig),
      m_vehicle(vehicle),
      m_access(access),
      m_events(events)
{
    if (m_vehicle) {
        connect(m_vehicle, &MavsdkVehicleManager::vehicleChanged, this, [this]() {
            m_running = m_vehicle->connected() && m_vehicle->connectionUrl().contains(QStringLiteral("145"));
            emit simulationChanged();
        });
    }
}

QVariantList SimulationManager::profiles() const
{
    return {
        QVariantMap{{QStringLiteral("key"), QStringLiteral("px4-gazebo-udp")},
                    {QStringLiteral("name"), QStringLiteral("PX4 / Gazebo UDP")},
                    {QStringLiteral("stack"), QStringLiteral("PX4")},
                    {QStringLiteral("mode"), QStringLiteral("UDP")},
                    {QStringLiteral("host"), QStringLiteral("0.0.0.0")},
                    {QStringLiteral("port"), 14540},
                    {QStringLiteral("launchSupported"), false}},
        QVariantMap{{QStringLiteral("key"), QStringLiteral("ardupilot-sitl-udp")},
                    {QStringLiteral("name"), QStringLiteral("ArduPilot SITL UDP")},
                    {QStringLiteral("stack"), QStringLiteral("ArduPilot")},
                    {QStringLiteral("mode"), QStringLiteral("UDP")},
                    {QStringLiteral("host"), QStringLiteral("0.0.0.0")},
                    {QStringLiteral("port"), 14550},
                    {QStringLiteral("launchSupported"), false}},
        QVariantMap{{QStringLiteral("key"), QStringLiteral("ardupilot-sitl-tcp")},
                    {QStringLiteral("name"), QStringLiteral("ArduPilot SITL TCP")},
                    {QStringLiteral("stack"), QStringLiteral("ArduPilot")},
                    {QStringLiteral("mode"), QStringLiteral("TCP")},
                    {QStringLiteral("host"), QStringLiteral("127.0.0.1")},
                    {QStringLiteral("port"), 5760},
                    {QStringLiteral("launchSupported"), false}}
    };
}

QString SimulationManager::activeProfile() const { return m_activeProfile; }
bool SimulationManager::running() const { return m_running; }
QString SimulationManager::status() const { return m_status; }

void SimulationManager::selectProfile(const QString &key)
{
    const QVariantMap profile = profileForKey(key);
    if (profile.isEmpty()) {
        setStatus(QStringLiteral("Unknown simulation profile."));
        return;
    }
    m_activeProfile = key;
    setStatus(QStringLiteral("Simulation profile selected: %1.").arg(profile.value(QStringLiteral("name")).toString()));
    emit simulationChanged();
}

void SimulationManager::connectSelected()
{
    if (m_access && !m_access->authorizeAction(QStringLiteral("simulation"),
                                                QVariantMap{{QStringLiteral("profile"), m_activeProfile}},
                                                QStringLiteral("Simulation connection blocked by local permissions."))) {
        setStatus(QStringLiteral("Simulation connection blocked by RBAC."));
        return;
    }
    const QVariantMap profile = profileForKey(m_activeProfile);
    if (profile.isEmpty() || !m_vehicleConfig) {
        setStatus(QStringLiteral("Simulation profile is unavailable."));
        return;
    }
    const QString mode = profile.value(QStringLiteral("mode")).toString();
    m_vehicleConfig->setConnectionMode(mode);
    if (mode == QStringLiteral("TCP")) {
        m_vehicleConfig->setTcpHost(profile.value(QStringLiteral("host")).toString());
        m_vehicleConfig->setTcpPort(profile.value(QStringLiteral("port")).toInt());
    } else {
        m_vehicleConfig->setUdpHost(profile.value(QStringLiteral("host")).toString());
        m_vehicleConfig->setUdpPort(profile.value(QStringLiteral("port")).toInt());
    }
    m_vehicleConfig->connectVehicle();
    setStatus(QStringLiteral("Connecting GCS to simulation endpoint: %1.").arg(m_vehicleConfig->connectionUrl()));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("simulation_connect_requested"),
                              QStringLiteral("info"),
                              QStringLiteral("Simulation endpoint connection requested"),
                              QJsonObject::fromVariantMap(profile));
    }
}

void SimulationManager::launchSelected()
{
    const QVariantMap profile = profileForKey(m_activeProfile);
    setStatus(QStringLiteral("Local simulator launch is unsupported for %1 until an adapter is installed.")
                  .arg(profile.value(QStringLiteral("name")).toString()));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("simulation_launch_unsupported"),
                              QStringLiteral("warning"),
                              QStringLiteral("Local simulator launch unsupported"),
                              QJsonObject::fromVariantMap(profile));
    }
}

void SimulationManager::stopSimulation()
{
    m_running = false;
    setStatus(QStringLiteral("Simulation session marked stopped. External SITL processes are not terminated by Foundation v1."));
    emit simulationChanged();
}

void SimulationManager::restartSimulation()
{
    stopSimulation();
    connectSelected();
}

QVariantMap SimulationManager::profileForKey(const QString &key) const
{
    for (const QVariant &entry : profiles()) {
        const QVariantMap profile = entry.toMap();
        if (profile.value(QStringLiteral("key")).toString() == key) {
            return profile;
        }
    }
    return {};
}

void SimulationManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit simulationChanged();
}
