#include "MavsdkVehicleManager.h"

#include "VehicleTelemetryModel.h"

#include <mavsdk/component_type.hpp>
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/system.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>

#include <QMetaObject>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QDebug>
#include <QtMath>

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

bool envEnabled(const QString &value)
{
    const QString normalized = value.trimmed().toLower();
    return normalized == QStringLiteral("1")
        || normalized == QStringLiteral("true")
        || normalized == QStringLiteral("yes")
        || normalized == QStringLiteral("on");
}
}

struct MavsdkVehicleManager::Impl
{
    Impl()
        : sdk(mavsdk::Mavsdk::Configuration(mavsdk::ComponentType::GroundStation))
    {
    }

    mavsdk::Mavsdk sdk;
    mavsdk::Mavsdk::NewSystemHandle newSystemHandle{};
    std::shared_ptr<mavsdk::System> system;
    std::unique_ptr<mavsdk::Telemetry> telemetry;
    mavsdk::System::IsConnectedHandle connectedHandle{};
    mavsdk::Telemetry::PositionHandle positionHandle{};
    mavsdk::Telemetry::BatteryHandle batteryHandle{};
    mavsdk::Telemetry::GpsInfoHandle gpsHandle{};
    mavsdk::Telemetry::VelocityNedHandle velocityHandle{};
    mavsdk::Telemetry::HeadingHandle headingHandle{};
    mavsdk::Telemetry::AttitudeEulerHandle attitudeHandle{};
    mavsdk::Telemetry::ArmedHandle armedHandle{};
    mavsdk::Telemetry::InAirHandle inAirHandle{};
    mavsdk::Telemetry::FlightModeHandle flightModeHandle{};
    mavsdk::Telemetry::HealthHandle healthHandle{};
};

MavsdkVehicleManager::MavsdkVehicleManager(VehicleTelemetryModel *telemetry, QObject *parent)
    : QObject(parent), m_impl(std::make_unique<Impl>()), m_telemetry(telemetry)
{
    m_discoveryTimer.setInterval(1500);
    connect(&m_discoveryTimer, &QTimer::timeout, this, &MavsdkVehicleManager::refreshSystems);
}

MavsdkVehicleManager::~MavsdkVehicleManager()
{
    stopDiscovery();
}

bool MavsdkVehicleManager::connected() const { return m_connected; }
bool MavsdkVehicleManager::discoveryActive() const { return m_discoveryActive; }
QString MavsdkVehicleManager::status() const { return m_status; }
QString MavsdkVehicleManager::connectionUrl() const { return m_connectionUrl; }
QString MavsdkVehicleManager::systemId() const { return m_systemId; }
QString MavsdkVehicleManager::autopilot() const { return m_autopilot; }
bool MavsdkVehicleManager::armed() const { return m_armed; }
bool MavsdkVehicleManager::inAir() const { return m_inAir; }
QString MavsdkVehicleManager::flightMode() const { return m_flightMode; }
QString MavsdkVehicleManager::health() const { return m_health; }
std::shared_ptr<mavsdk::System> MavsdkVehicleManager::system() const { return m_impl->system; }

void MavsdkVehicleManager::startDiscovery()
{
    if (m_discoveryActive) {
        refreshSystems();
        return;
    }
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList urls;
    const QString configured = env.value(QStringLiteral("SKYGRID_MAVSDK_URLS"),
                                         env.value(QStringLiteral("SKYGRID_MAVSDK_URL")))
                                   .trimmed();
    if (!configured.isEmpty()) {
        urls << configured.split(QRegularExpression(QStringLiteral("[,;\\s]+")), Qt::SkipEmptyParts);
    }
    if (urls.isEmpty()) {
        urls << QStringLiteral("udpin://0.0.0.0:14540")
             << QStringLiteral("udpin://0.0.0.0:14550");
    }
    urls.removeDuplicates();

    const bool allowMultipleLinks = envEnabled(env.value(QStringLiteral("SKYGRID_MAVSDK_ALLOW_MULTIPLE_URLS")));
    QStringList accepted;
    QStringList rejected;
    for (const QString &url : urls) {
        const auto result = m_impl->sdk.add_any_connection(url.toStdString());
        if (result == mavsdk::ConnectionResult::Success) {
            accepted << url;
            if (!allowMultipleLinks) {
                break;
            }
        } else {
            rejected << QStringLiteral("%1 (%2)").arg(url, enumString(result));
        }
    }
    m_connectionUrl = accepted.join(QStringLiteral(", "));
    m_status = accepted.isEmpty()
        ? QStringLiteral("MAVSDK UDP bind failed")
        : QStringLiteral("Searching for Gazebo/PX4 aircraft");
    m_discoveryActive = true;

    m_impl->newSystemHandle = m_impl->sdk.subscribe_on_new_system([this]() {
        QMetaObject::invokeMethod(this, &MavsdkVehicleManager::refreshSystems, Qt::QueuedConnection);
    });
    if (!m_discoveryTimer.isActive()) {
        m_discoveryTimer.start();
    }
    qInfo() << "MAVSDK discovery active. Accepted:" << accepted << "Rejected:" << rejected
            << "Multiple links:" << allowMultipleLinks;
    emit vehicleChanged();
    refreshSystems();
}

void MavsdkVehicleManager::stopDiscovery()
{
    m_discoveryTimer.stop();
    if (m_impl->newSystemHandle.valid()) {
        m_impl->sdk.unsubscribe_on_new_system(m_impl->newSystemHandle);
    }
    if (m_impl->system && m_impl->connectedHandle.valid()) {
        m_impl->system->unsubscribe_is_connected(m_impl->connectedHandle);
    }
    m_impl->telemetry.reset();
    m_impl->system.reset();
    m_discoveryActive = false;
    m_connected = false;
    m_systemId.clear();
    m_status = QStringLiteral("Aircraft discovery stopped");
    if (m_telemetry) {
        m_telemetry->reset();
    }
    emit vehicleChanged();
}

void MavsdkVehicleManager::connectRetry()
{
    m_status = QStringLiteral("Retrying MAVSDK aircraft connection");
    emit vehicleChanged();

    if (!m_discoveryActive) {
        startDiscovery();
        return;
    }

    if (!m_discoveryTimer.isActive()) {
        m_discoveryTimer.start();
    }
    refreshSystems();
}

void MavsdkVehicleManager::refreshSystems()
{
    if (m_impl->system && m_impl->system->is_connected()) {
        return;
    }
    for (const auto &system : m_impl->sdk.systems()) {
        if (!system || !system->has_autopilot()) {
            continue;
        }
        m_discoveryTimer.stop();
        m_impl->system = system;
        m_impl->telemetry = std::make_unique<mavsdk::Telemetry>(system);
        m_connected = system->is_connected();
        m_systemId = QString::number(system->get_system_id());
        m_autopilot = enumString(system->autopilot_type());
        m_status = m_connected ? QStringLiteral("Aircraft connected") : QStringLiteral("Aircraft discovered");
        qInfo() << "MAVSDK vehicle selected. System:" << m_systemId << "Autopilot:" << m_autopilot << "Connected:" << m_connected;
        if (m_telemetry) {
            const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            const bool fastMode = env.value(QStringLiteral("SKYGRID_PERFORMANCE_MODE")).trimmed().toLower() == QStringLiteral("fast");
            const double fastRate = env.value(QStringLiteral("SKYGRID_MAVSDK_FAST_RATE_HZ"),
                                              env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true")
                                                  ? (fastMode ? QStringLiteral("2.5") : QStringLiteral("3.5"))
                                                  : (fastMode ? QStringLiteral("3.0") : QStringLiteral("4.0"))).toDouble();
            const double slowRate = env.value(QStringLiteral("SKYGRID_MAVSDK_SLOW_RATE_HZ"),
                                              env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true")
                                                  ? QStringLiteral("1.0")
                                                  : QStringLiteral("2.0")).toDouble();
            const double statusRate = env.value(QStringLiteral("SKYGRID_MAVSDK_STATUS_RATE_HZ"),
                                                fastMode ? QStringLiteral("0.5") : QStringLiteral("1.0")).toDouble();
            m_impl->telemetry->set_rate_position(qBound(1.0, fastRate, 10.0));
            m_impl->telemetry->set_rate_velocity_ned(qBound(1.0, fastRate, 10.0));
            m_impl->telemetry->set_rate_attitude_euler(qBound(1.0, fastRate, 10.0));
            m_impl->telemetry->set_rate_battery(qBound(0.5, slowRate, 5.0));
            m_impl->telemetry->set_rate_gps_info(qBound(0.5, slowRate, 5.0));
            m_impl->telemetry->set_rate_health(qBound(0.2, statusRate, 2.0));
            m_impl->telemetry->set_rate_in_air(qBound(0.2, statusRate, 2.0));

            m_impl->positionHandle = m_impl->telemetry->subscribe_position([this](mavsdk::Telemetry::Position position) {
                QMetaObject::invokeMethod(this, [this, position]() {
                    if (m_telemetry) {
                        m_telemetry->setPosition(position.latitude_deg, position.longitude_deg, position.relative_altitude_m);
                    }
                }, Qt::QueuedConnection);
            });
            m_impl->batteryHandle = m_impl->telemetry->subscribe_battery([this](mavsdk::Telemetry::Battery battery) {
                QMetaObject::invokeMethod(this, [this, battery]() {
                    if (m_telemetry) {
                        m_telemetry->setBattery(battery.remaining_percent * 100.0);
                        m_telemetry->setBatteryVoltage(battery.voltage_v);
                    }
                }, Qt::QueuedConnection);
            });
            m_impl->gpsHandle = m_impl->telemetry->subscribe_gps_info([this](mavsdk::Telemetry::GpsInfo gps) {
                QMetaObject::invokeMethod(this, [this, gps]() {
                    if (m_telemetry) {
                        m_telemetry->setSatellites(gps.num_satellites);
                    }
                }, Qt::QueuedConnection);
            });
            m_impl->velocityHandle = m_impl->telemetry->subscribe_velocity_ned([this](mavsdk::Telemetry::VelocityNed velocity) {
                QMetaObject::invokeMethod(this, [this, velocity]() {
                    const double speed = std::sqrt(velocity.north_m_s * velocity.north_m_s + velocity.east_m_s * velocity.east_m_s);
                    if (m_telemetry) {
                        m_telemetry->setVelocity(speed);
                        m_telemetry->setVerticalSpeed(-velocity.down_m_s);
                    }
                }, Qt::QueuedConnection);
            });
            m_impl->attitudeHandle = m_impl->telemetry->subscribe_attitude_euler([this](mavsdk::Telemetry::EulerAngle attitude) {
                QMetaObject::invokeMethod(this, [this, attitude]() {
                    if (m_telemetry) {
                        m_telemetry->setAttitude(attitude.roll_deg, attitude.pitch_deg, attitude.yaw_deg);
                    }
                }, Qt::QueuedConnection);
            });
            m_impl->armedHandle = m_impl->telemetry->subscribe_armed([this](bool armed) {
                QMetaObject::invokeMethod(this, [this, armed]() {
                    m_armed = armed;
                    if (m_telemetry) {
                        m_telemetry->setArmed(armed);
                    }
                    emit vehicleChanged();
                }, Qt::QueuedConnection);
            });
            m_impl->inAirHandle = m_impl->telemetry->subscribe_in_air([this](bool inAir) {
                QMetaObject::invokeMethod(this, [this, inAir]() {
                    m_inAir = inAir;
                    if (m_telemetry) {
                        m_telemetry->setInAir(inAir);
                    }
                    emit vehicleChanged();
                }, Qt::QueuedConnection);
            });
            m_impl->flightModeHandle = m_impl->telemetry->subscribe_flight_mode([this](mavsdk::Telemetry::FlightMode mode) {
                QMetaObject::invokeMethod(this, [this, mode]() {
                    m_flightMode = enumString(mode);
                    if (m_telemetry) {
                        m_telemetry->setFlightMode(m_flightMode);
                    }
                    emit vehicleChanged();
                }, Qt::QueuedConnection);
            });
            m_impl->healthHandle = m_impl->telemetry->subscribe_health([this](mavsdk::Telemetry::Health health) {
                QMetaObject::invokeMethod(this, [this, health]() {
                    const bool ok = health.is_global_position_ok && health.is_home_position_ok;
                    m_health = ok ? QStringLiteral("Ready") : QStringLiteral("Preflight Required");
                    if (m_telemetry) {
                        m_telemetry->setHealth(ok);
                    }
                    emit vehicleChanged();
                }, Qt::QueuedConnection);
            });
        }
        m_impl->connectedHandle = system->subscribe_is_connected([this](bool connected) {
            QMetaObject::invokeMethod(this, [this, connected]() {
                m_connected = connected;
                m_status = m_connected ? QStringLiteral("Aircraft connected") : QStringLiteral("Aircraft disconnected");
                if (m_telemetry) {
                    m_telemetry->setConnection(m_connected, m_connected ? QStringLiteral("SYS-%1").arg(m_systemId) : QString());
                }
                emit vehicleChanged();
            }, Qt::QueuedConnection);
        });
        if (m_telemetry) {
            m_telemetry->setConnection(m_connected, QStringLiteral("SYS-%1").arg(m_systemId));
        }
        emit vehicleChanged();
        emit systemReady();
        return;
    }
    m_connected = false;
    m_status = m_discoveryActive
        ? QStringLiteral("Searching for Gazebo/PX4 aircraft")
        : QStringLiteral("No Connected Aircraft");
    if (m_telemetry) {
        m_telemetry->setConnection(false, QString());
    }
    emit vehicleChanged();
}
