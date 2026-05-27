#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>

class VehicleTelemetryModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString aircraftId READ aircraftId NOTIFY telemetryChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY telemetryChanged)
    Q_PROPERTY(bool armed READ armed NOTIFY telemetryChanged)
    Q_PROPERTY(bool inAir READ inAir NOTIFY telemetryChanged)
    Q_PROPERTY(int battery READ battery NOTIFY telemetryChanged)
    Q_PROPERTY(int satellites READ satellites NOTIFY telemetryChanged)
    Q_PROPERTY(int rcSignal READ rcSignal NOTIFY telemetryChanged)
    Q_PROPERTY(int transmission READ transmission NOTIFY telemetryChanged)
    Q_PROPERTY(double speed READ speed NOTIFY telemetryChanged)
    Q_PROPERTY(double altitude READ altitude NOTIFY telemetryChanged)
    Q_PROPERTY(double verticalSpeed READ verticalSpeed NOTIFY telemetryChanged)
    Q_PROPERTY(double latitude READ latitude NOTIFY telemetryChanged)
    Q_PROPERTY(double longitude READ longitude NOTIFY telemetryChanged)
    Q_PROPERTY(double heading READ heading NOTIFY telemetryChanged)
    Q_PROPERTY(double roll READ roll NOTIFY telemetryChanged)
    Q_PROPERTY(double pitch READ pitch NOTIFY telemetryChanged)
    Q_PROPERTY(double yaw READ yaw NOTIFY telemetryChanged)
    Q_PROPERTY(double batteryVoltage READ batteryVoltage NOTIFY telemetryChanged)
    Q_PROPERTY(double hdop READ hdop NOTIFY telemetryChanged)
    Q_PROPERTY(int latency READ latency NOTIFY telemetryChanged)
    Q_PROPERTY(QString gpsMode READ gpsMode NOTIFY telemetryChanged)
    Q_PROPERTY(QString flightMode READ flightMode NOTIFY telemetryChanged)
    Q_PROPERTY(QString recordingState READ recordingState NOTIFY telemetryChanged)
    Q_PROPERTY(bool recording READ recording NOTIFY telemetryChanged)
    Q_PROPERTY(QString obstacleState READ obstacleState NOTIFY telemetryChanged)
    Q_PROPERTY(QString operatorName READ operatorName CONSTANT)
    Q_PROPERTY(QString gpsQuality READ gpsQuality NOTIFY telemetryChanged)
    Q_PROPERTY(QString rcQuality READ rcQuality NOTIFY telemetryChanged)
    Q_PROPERTY(bool aircraftReady READ aircraftReady NOTIFY telemetryChanged)
    Q_PROPERTY(QString aircraftReadiness READ aircraftReadiness NOTIFY telemetryChanged)
    Q_PROPERTY(double batteryTrend READ batteryTrend NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList livePath READ livePath NOTIFY telemetryChanged)

public:
    explicit VehicleTelemetryModel(QObject *parent = nullptr);

    QString aircraftId() const;
    bool connected() const;
    bool armed() const;
    bool inAir() const;
    int battery() const;
    int satellites() const;
    int rcSignal() const;
    int transmission() const;
    double speed() const;
    double altitude() const;
    double verticalSpeed() const;
    double latitude() const;
    double longitude() const;
    double heading() const;
    double roll() const;
    double pitch() const;
    double yaw() const;
    double batteryVoltage() const;
    double hdop() const;
    int latency() const;
    QString gpsMode() const;
    QString flightMode() const;
    QString recordingState() const;
    bool recording() const;
    QString obstacleState() const;
    QString operatorName() const;
    QString gpsQuality() const;
    QString rcQuality() const;
    bool aircraftReady() const;
    QString aircraftReadiness() const;
    double batteryTrend() const;
    QVariantList livePath() const;

    void setConnection(bool connected, const QString &aircraftId);
    void setPosition(double latitude, double longitude, double altitude);
    void setBattery(double batteryPercent);
    void setBatteryVoltage(double voltage);
    void setSatellites(int satellites);
    void setVelocity(double groundSpeedMps);
    void setVerticalSpeed(double verticalSpeedMps);
    void setHeading(double headingDeg);
    void setAttitude(double rollDeg, double pitchDeg, double yawDeg);
    void setHdop(double hdop);
    void setArmed(bool armed);
    void setInAir(bool inAir);
    void setFlightMode(const QString &mode);
    void setHealth(bool allOk);
    void reset();

signals:
    void telemetryChanged();
    void uiTelemetryChanged();

private:
    void notifyTelemetryChanged(bool immediate = false);

    QString m_aircraftId = "No Connected Aircraft";
    bool m_connected = false;
    bool m_armed = false;
    bool m_inAir = false;
    int m_battery = 0;
    int m_satellites = 0;
    int m_rcSignal = 0;
    int m_transmission = 0;
    double m_speed = 0.0;
    double m_altitude = 0.0;
    double m_verticalSpeed = 0.0;
    double m_latitude = 0.0;
    double m_longitude = 0.0;
    double m_heading = 0.0;
    double m_roll = 0.0;
    double m_pitch = 0.0;
    double m_yaw = 0.0;
    double m_batteryVoltage = 0.0;
    double m_hdop = 0.0;
    int m_latency = 0;
    double m_batteryTrend = 0.0;
    bool m_healthOk = false;
    QString m_flightMode = "DISCONNECTED";
    QVariantList m_livePath;
    QTimer m_uiNotifyTimer;
    bool m_notifyPending = false;
};
