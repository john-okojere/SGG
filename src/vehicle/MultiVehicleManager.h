#pragma once

#include <QObject>
#include <QVariantList>

class AccessManager;
class GcsEventSyncManager;
class MavsdkVehicleManager;
class VehicleTelemetryModel;

class MultiVehicleManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList connectionProfiles READ connectionProfiles NOTIFY multiVehicleChanged)
    Q_PROPERTY(QVariantList connectedVehicles READ connectedVehicles NOTIFY multiVehicleChanged)
    Q_PROPERTY(QString activeVehicleId READ activeVehicleId NOTIFY multiVehicleChanged)
    Q_PROPERTY(QString status READ status NOTIFY multiVehicleChanged)

public:
    explicit MultiVehicleManager(MavsdkVehicleManager *vehicle,
                                 VehicleTelemetryModel *telemetry,
                                 AccessManager *access,
                                 GcsEventSyncManager *events,
                                 QObject *parent = nullptr);

    QVariantList connectionProfiles() const;
    QVariantList connectedVehicles() const;
    QString activeVehicleId() const;
    QString status() const;

    Q_INVOKABLE void refreshVehicles();
    Q_INVOKABLE void selectVehicle(const QString &vehicleId);
    Q_INVOKABLE void addConnectionProfile(const QVariantMap &profile);
    Q_INVOKABLE void removeConnectionProfile(const QString &profileId);

signals:
    void multiVehicleChanged();

private:
    void rebuildConnectedVehicles();
    void setStatus(const QString &status);

    MavsdkVehicleManager *m_vehicle = nullptr;
    VehicleTelemetryModel *m_telemetry = nullptr;
    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QVariantList m_connectionProfiles;
    QVariantList m_connectedVehicles;
    QString m_activeVehicleId = "primary";
    QString m_status = "Multi-vehicle model ready";
};
