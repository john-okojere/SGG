#include "VehicleTelemetryModel.h"

#include <QDateTime>
#include <QVariantMap>
#include <QtMath>

VehicleTelemetryModel::VehicleTelemetryModel(QObject *parent) : QObject(parent)
{
    m_uiNotifyTimer.setSingleShot(true);
    m_uiNotifyTimer.setInterval(125);
    connect(&m_uiNotifyTimer, &QTimer::timeout, this, [this]() {
        m_notifyPending = false;
        emit uiTelemetryChanged();
        emit telemetryChanged();
    });
}

QString VehicleTelemetryModel::aircraftId() const { return m_aircraftId; }
bool VehicleTelemetryModel::connected() const { return m_connected; }
bool VehicleTelemetryModel::armed() const { return m_armed; }
bool VehicleTelemetryModel::inAir() const { return m_inAir; }
int VehicleTelemetryModel::battery() const { return m_battery; }
int VehicleTelemetryModel::satellites() const { return m_satellites; }
int VehicleTelemetryModel::rcSignal() const { return m_rcSignal; }
int VehicleTelemetryModel::transmission() const { return m_transmission; }
double VehicleTelemetryModel::speed() const { return m_speed; }
double VehicleTelemetryModel::altitude() const { return m_altitude; }
double VehicleTelemetryModel::verticalSpeed() const { return m_verticalSpeed; }
double VehicleTelemetryModel::latitude() const { return m_latitude; }
double VehicleTelemetryModel::longitude() const { return m_longitude; }
double VehicleTelemetryModel::heading() const { return m_heading; }
double VehicleTelemetryModel::roll() const { return m_roll; }
double VehicleTelemetryModel::pitch() const { return m_pitch; }
double VehicleTelemetryModel::yaw() const { return m_yaw; }
double VehicleTelemetryModel::batteryVoltage() const { return m_batteryVoltage; }
double VehicleTelemetryModel::hdop() const { return m_hdop; }
int VehicleTelemetryModel::latency() const { return m_latency; }
QString VehicleTelemetryModel::gpsMode() const
{
    if (!m_connected) {
        return QStringLiteral("NO GPS");
    }
    if (m_satellites >= 14) {
        return QStringLiteral("RTK FIX");
    }
    if (m_satellites >= 10) {
        return QStringLiteral("3D FIX");
    }
    return QStringLiteral("GPS WAIT");
}
QString VehicleTelemetryModel::flightMode() const { return m_flightMode; }
QString VehicleTelemetryModel::recordingState() const { return m_connected ? QStringLiteral("STBY") : QStringLiteral("NO CAM"); }
bool VehicleTelemetryModel::recording() const { return false; }
QString VehicleTelemetryModel::obstacleState() const { return m_healthOk ? QStringLiteral("OK") : QStringLiteral("CHECK"); }
QString VehicleTelemetryModel::operatorName() const { return QStringLiteral("Operator"); }
QString VehicleTelemetryModel::gpsQuality() const { return m_satellites >= 12 ? QStringLiteral("High") : QStringLiteral("Degraded"); }
QString VehicleTelemetryModel::rcQuality() const { return m_rcSignal >= 80 ? QStringLiteral("Strong") : QStringLiteral("Weak"); }
bool VehicleTelemetryModel::aircraftReady() const
{
    return m_connected && m_healthOk && m_battery > 25 && m_satellites >= 8;
}
QString VehicleTelemetryModel::aircraftReadiness() const
{
    if (!m_connected) {
        return QStringLiteral("No Connected Aircraft");
    }
    return aircraftReady() ? QStringLiteral("Ready") : QStringLiteral("Preflight Required");
}
double VehicleTelemetryModel::batteryTrend() const { return m_batteryTrend; }
QVariantList VehicleTelemetryModel::livePath() const { return m_livePath; }

void VehicleTelemetryModel::setConnection(bool connected, const QString &aircraftId)
{
    if (m_connected == connected && m_aircraftId == (connected ? aircraftId : QStringLiteral("No Connected Aircraft"))) {
        return;
    }
    m_connected = connected;
    m_aircraftId = connected ? aircraftId : QStringLiteral("No Connected Aircraft");
    if (!connected) {
        m_armed = false;
        m_inAir = false;
        m_flightMode = QStringLiteral("DISCONNECTED");
    }
    notifyTelemetryChanged(true);
}

void VehicleTelemetryModel::setPosition(double latitude, double longitude, double altitude)
{
    const bool changed = qAbs(m_latitude - latitude) > 0.000003
        || qAbs(m_longitude - longitude) > 0.000003
        || qAbs(m_altitude - altitude) > 0.15;
    if (!changed) {
        return;
    }
    m_latitude = latitude;
    m_longitude = longitude;
    m_altitude = altitude;
    if (m_connected && qAbs(latitude) > 0.000001 && qAbs(longitude) > 0.000001) {
        m_livePath << QVariantMap{
            {QStringLiteral("latitude"), latitude},
            {QStringLiteral("longitude"), longitude},
            {QStringLiteral("altitude"), altitude},
            {QStringLiteral("recorded_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
        };
        while (m_livePath.size() > 300) {
            m_livePath.removeFirst();
        }
    }
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::setBattery(double batteryPercent)
{
    const int previous = m_battery;
    const int battery = qBound(0, qRound(batteryPercent), 100);
    if (previous == battery) {
        return;
    }
    m_battery = battery;
    if (previous > 0) {
        m_batteryTrend = m_battery - previous;
    }
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::setBatteryVoltage(double voltage)
{
    const double value = qMax(0.0, voltage);
    if (qAbs(m_batteryVoltage - value) < 0.05) {
        return;
    }
    m_batteryVoltage = value;
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::setSatellites(int satellites)
{
    const int value = qMax(0, satellites);
    if (m_satellites == value) {
        return;
    }
    m_satellites = value;
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::setVelocity(double groundSpeedMps)
{
    const double value = qMax(0.0, groundSpeedMps);
    const bool changed = qAbs(m_speed - value) >= 0.05
        || m_rcSignal != (m_connected ? 100 : 0)
        || m_transmission != (m_connected ? 100 : 0);
    if (!changed) {
        return;
    }
    m_speed = value;
    m_rcSignal = m_connected ? 100 : 0;
    m_transmission = m_connected ? 100 : 0;
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::setVerticalSpeed(double verticalSpeedMps)
{
    if (qAbs(m_verticalSpeed - verticalSpeedMps) < 0.05) {
        return;
    }
    m_verticalSpeed = verticalSpeedMps;
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::setHeading(double headingDeg)
{
    if (qAbs(m_heading - headingDeg) < 0.5) {
        return;
    }
    m_heading = headingDeg;
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::setAttitude(double rollDeg, double pitchDeg, double yawDeg)
{
    const bool changed = qAbs(m_roll - rollDeg) >= 0.4
        || qAbs(m_pitch - pitchDeg) >= 0.4
        || qAbs(m_yaw - yawDeg) >= 0.5;
    if (!changed) {
        return;
    }
    m_roll = rollDeg;
    m_pitch = pitchDeg;
    m_yaw = yawDeg;
    m_heading = yawDeg;
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::setHdop(double hdop)
{
    const double value = qMax(0.0, hdop);
    if (qAbs(m_hdop - value) < 0.05) {
        return;
    }
    m_hdop = value;
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::setArmed(bool armed)
{
    if (m_armed == armed) {
        return;
    }
    m_armed = armed;
    notifyTelemetryChanged(true);
}

void VehicleTelemetryModel::setInAir(bool inAir)
{
    if (m_inAir == inAir) {
        return;
    }
    m_inAir = inAir;
    notifyTelemetryChanged(true);
}

void VehicleTelemetryModel::setFlightMode(const QString &mode)
{
    const QString value = mode.isEmpty() ? QStringLiteral("UNKNOWN") : mode;
    if (m_flightMode == value) {
        return;
    }
    m_flightMode = value;
    notifyTelemetryChanged(true);
}

void VehicleTelemetryModel::setHealth(bool allOk)
{
    if (m_healthOk == allOk) {
        return;
    }
    m_healthOk = allOk;
    notifyTelemetryChanged();
}

void VehicleTelemetryModel::reset()
{
    m_aircraftId = QStringLiteral("No Connected Aircraft");
    m_connected = false;
    m_armed = false;
    m_inAir = false;
    m_battery = 0;
    m_satellites = 0;
    m_rcSignal = 0;
    m_transmission = 0;
    m_speed = 0.0;
    m_altitude = 0.0;
    m_verticalSpeed = 0.0;
    m_latitude = 0.0;
    m_longitude = 0.0;
    m_heading = 0.0;
    m_roll = 0.0;
    m_pitch = 0.0;
    m_yaw = 0.0;
    m_batteryVoltage = 0.0;
    m_hdop = 0.0;
    m_latency = 0;
    m_batteryTrend = 0.0;
    m_healthOk = false;
    m_flightMode = QStringLiteral("DISCONNECTED");
    m_livePath.clear();
    notifyTelemetryChanged(true);
}

void VehicleTelemetryModel::notifyTelemetryChanged(bool immediate)
{
    if (immediate) {
        m_uiNotifyTimer.stop();
        m_notifyPending = false;
        emit uiTelemetryChanged();
        emit telemetryChanged();
        return;
    }
    if (m_notifyPending) {
        return;
    }
    m_notifyPending = true;
    m_uiNotifyTimer.start();
}
