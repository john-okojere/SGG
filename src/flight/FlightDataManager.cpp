#include "FlightDataManager.h"

#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"
#include "../vehicle/VehicleTelemetryModel.h"

#include <QJsonObject>

FlightDataManager::FlightDataManager(VehicleTelemetryModel *telemetry,
                                     MavsdkVehicleManager *vehicle,
                                     AccessManager *access,
                                     GcsEventSyncManager *events,
                                     QObject *parent)
    : QObject(parent),
      m_telemetry(telemetry),
      m_vehicle(vehicle),
      m_access(access),
      m_events(events)
{
    if (m_telemetry) {
        connect(m_telemetry, &VehicleTelemetryModel::uiTelemetryChanged, this, &FlightDataManager::flightDataChanged);
    }
    if (m_vehicle) {
        connect(m_vehicle, &MavsdkVehicleManager::vehicleChanged, this, &FlightDataManager::flightDataChanged);
    }
}

QVariantList FlightDataManager::telemetryRows() const
{
    if (!m_telemetry) {
        return {};
    }
    return {
        row(QStringLiteral("Aircraft"), m_telemetry->aircraftId()),
        row(QStringLiteral("Connection"), m_telemetry->connected() ? QStringLiteral("Connected") : QStringLiteral("Disconnected")),
        row(QStringLiteral("Armed"), m_telemetry->armed() ? QStringLiteral("Yes") : QStringLiteral("No")),
        row(QStringLiteral("In Air"), m_telemetry->inAir() ? QStringLiteral("Yes") : QStringLiteral("No")),
        row(QStringLiteral("Flight Mode"), m_telemetry->flightMode()),
        row(QStringLiteral("Ground Speed"), value(m_telemetry->speed()), QStringLiteral("m/s")),
        row(QStringLiteral("Altitude"), value(m_telemetry->altitude()), QStringLiteral("m")),
        row(QStringLiteral("Vertical Speed"), value(m_telemetry->verticalSpeed()), QStringLiteral("m/s")),
        row(QStringLiteral("Heading"), value(m_telemetry->heading(), 0), QStringLiteral("deg"))
    };
}

QVariantList FlightDataManager::gpsRows() const
{
    if (!m_telemetry) {
        return {};
    }
    return {
        row(QStringLiteral("GPS Mode"), m_telemetry->gpsMode()),
        row(QStringLiteral("GPS Quality"), m_telemetry->gpsQuality()),
        row(QStringLiteral("Satellites"), QString::number(m_telemetry->satellites())),
        row(QStringLiteral("HDOP"), value(m_telemetry->hdop(), 2)),
        row(QStringLiteral("Latitude"), value(m_telemetry->latitude(), 7)),
        row(QStringLiteral("Longitude"), value(m_telemetry->longitude(), 7))
    };
}

QVariantList FlightDataManager::powerRows() const
{
    if (!m_telemetry) {
        return {};
    }
    return {
        row(QStringLiteral("Battery"), QString::number(m_telemetry->battery()), QStringLiteral("%")),
        row(QStringLiteral("Voltage"), value(m_telemetry->batteryVoltage(), 2), QStringLiteral("V")),
        row(QStringLiteral("Trend"), value(m_telemetry->batteryTrend(), 1), QStringLiteral("%")),
        row(QStringLiteral("Readiness"), m_telemetry->aircraftReadiness())
    };
}

QVariantList FlightDataManager::linkRows() const
{
    if (!m_telemetry || !m_vehicle) {
        return {};
    }
    return {
        row(QStringLiteral("Telemetry Link"), m_telemetry->connected() ? QStringLiteral("Online") : QStringLiteral("Offline")),
        row(QStringLiteral("RC Link"), m_telemetry->rcQuality()),
        row(QStringLiteral("Transmission"), QString::number(m_telemetry->transmission()), QStringLiteral("%")),
        row(QStringLiteral("Latency"), QString::number(m_telemetry->latency()), QStringLiteral("ms")),
        row(QStringLiteral("MAVSDK Status"), m_vehicle->status()),
        row(QStringLiteral("Connection URL"), m_vehicle->connectionUrl())
    };
}

QVariantList FlightDataManager::attitudeRows() const
{
    if (!m_telemetry) {
        return {};
    }
    return {
        row(QStringLiteral("Roll"), value(m_telemetry->roll(), 1), QStringLiteral("deg")),
        row(QStringLiteral("Pitch"), value(m_telemetry->pitch(), 1), QStringLiteral("deg")),
        row(QStringLiteral("Yaw"), value(m_telemetry->yaw(), 1), QStringLiteral("deg")),
        row(QStringLiteral("Obstacle"), m_telemetry->obstacleState())
    };
}

QVariantList FlightDataManager::vehicleMessages() const
{
    if (!m_telemetry || !m_vehicle) {
        return {};
    }
    return {
        QVariantMap{{QStringLiteral("severity"), QStringLiteral("info")},
                    {QStringLiteral("message"), m_vehicle->status()}},
        QVariantMap{{QStringLiteral("severity"), m_telemetry->aircraftReady() ? QStringLiteral("info") : QStringLiteral("warning")},
                    {QStringLiteral("message"), m_telemetry->aircraftReadiness()}},
        QVariantMap{{QStringLiteral("severity"), QStringLiteral("info")},
                    {QStringLiteral("message"), QStringLiteral("Autopilot: %1").arg(m_vehicle->autopilot())}}
    };
}

QVariantList FlightDataManager::modeOptions() const
{
    return {QStringLiteral("Hold"), QStringLiteral("Mission"), QStringLiteral("RTL"), QStringLiteral("Land"), QStringLiteral("Loiter"), QStringLiteral("Manual")};
}

QVariantList FlightDataManager::actionOptions() const
{
    return {QStringLiteral("Hold"), QStringLiteral("Return to Launch"), QStringLiteral("Land"), QStringLiteral("Pause Mission"), QStringLiteral("Resume Mission")};
}

QString FlightDataManager::status() const { return m_status; }

void FlightDataManager::refreshSnapshot()
{
    if (m_access && !m_access->canPerform(QStringLiteral("flight_data"))) {
        setStatus(QStringLiteral("Flight data blocked by RBAC."));
        return;
    }
    setStatus(QStringLiteral("Flight data snapshot refreshed from current telemetry state."));
    emit flightDataChanged();
}

void FlightDataManager::setFlightMode(const QString &mode)
{
    setStatus(QStringLiteral("Mode selection is prepared for adapter integration: %1").arg(mode));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("flight_data_mode_selection_unsupported"),
                              QStringLiteral("warning"),
                              QStringLiteral("Flight mode adapter operation unsupported"),
                              QJsonObject{{QStringLiteral("mode"), mode}});
    }
}

void FlightDataManager::runAction(const QString &action)
{
    setStatus(QStringLiteral("Flight action is prepared for guarded adapter integration: %1").arg(action));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("flight_data_action_unsupported"),
                              QStringLiteral("warning"),
                              QStringLiteral("Flight action adapter operation unsupported"),
                              QJsonObject{{QStringLiteral("action"), action}});
    }
}

void FlightDataManager::sendServoRelayOverride(const QString &channel, int value)
{
    setStatus(QStringLiteral("Servo/relay override is unsupported until a guarded MAVLink adapter is enabled."));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("mavlink_servo_relay_override_unsupported"),
                              QStringLiteral("warning"),
                              QStringLiteral("Servo relay override unsupported"),
                              QJsonObject{{QStringLiteral("channel"), channel},
                                          {QStringLiteral("value"), value}});
    }
}

QVariantMap FlightDataManager::row(const QString &name, const QString &value, const QString &unit) const
{
    return QVariantMap{{QStringLiteral("name"), name},
                       {QStringLiteral("value"), value},
                       {QStringLiteral("unit"), unit}};
}

QString FlightDataManager::value(double number, int precision) const
{
    return QString::number(number, 'f', precision);
}

void FlightDataManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit flightDataChanged();
}
