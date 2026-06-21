#pragma once

#include <QObject>
#include <QVariantList>

class AccessManager;
class GcsEventSyncManager;
class MavsdkVehicleManager;
class VehicleTelemetryModel;

class FlightDataManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList telemetryRows READ telemetryRows NOTIFY flightDataChanged)
    Q_PROPERTY(QVariantList gpsRows READ gpsRows NOTIFY flightDataChanged)
    Q_PROPERTY(QVariantList powerRows READ powerRows NOTIFY flightDataChanged)
    Q_PROPERTY(QVariantList linkRows READ linkRows NOTIFY flightDataChanged)
    Q_PROPERTY(QVariantList attitudeRows READ attitudeRows NOTIFY flightDataChanged)
    Q_PROPERTY(QVariantList vehicleMessages READ vehicleMessages NOTIFY flightDataChanged)
    Q_PROPERTY(QVariantList modeOptions READ modeOptions CONSTANT)
    Q_PROPERTY(QVariantList actionOptions READ actionOptions CONSTANT)
    Q_PROPERTY(QString status READ status NOTIFY flightDataChanged)

public:
    explicit FlightDataManager(VehicleTelemetryModel *telemetry,
                               MavsdkVehicleManager *vehicle,
                               AccessManager *access,
                               GcsEventSyncManager *events,
                               QObject *parent = nullptr);

    QVariantList telemetryRows() const;
    QVariantList gpsRows() const;
    QVariantList powerRows() const;
    QVariantList linkRows() const;
    QVariantList attitudeRows() const;
    QVariantList vehicleMessages() const;
    QVariantList modeOptions() const;
    QVariantList actionOptions() const;
    QString status() const;

    Q_INVOKABLE void refreshSnapshot();
    Q_INVOKABLE void setFlightMode(const QString &mode);
    Q_INVOKABLE void runAction(const QString &action);
    Q_INVOKABLE void sendServoRelayOverride(const QString &channel, int value);

signals:
    void flightDataChanged();

private:
    QVariantMap row(const QString &name, const QString &value, const QString &unit = QString()) const;
    QString value(double number, int precision = 1) const;
    void setStatus(const QString &status);

    VehicleTelemetryModel *m_telemetry = nullptr;
    MavsdkVehicleManager *m_vehicle = nullptr;
    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QString m_status = "Flight data ready";
};
