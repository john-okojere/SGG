#pragma once

#include <QObject>
#include <QVariantList>

class AccessManager;
class GcsEventSyncManager;
class MavsdkVehicleManager;
class VehicleConfigManager;

class SimulationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList profiles READ profiles NOTIFY simulationChanged)
    Q_PROPERTY(QString activeProfile READ activeProfile NOTIFY simulationChanged)
    Q_PROPERTY(bool running READ running NOTIFY simulationChanged)
    Q_PROPERTY(QString status READ status NOTIFY simulationChanged)

public:
    explicit SimulationManager(VehicleConfigManager *vehicleConfig,
                               MavsdkVehicleManager *vehicle,
                               AccessManager *access,
                               GcsEventSyncManager *events,
                               QObject *parent = nullptr);

    QVariantList profiles() const;
    QString activeProfile() const;
    bool running() const;
    QString status() const;

    Q_INVOKABLE void selectProfile(const QString &key);
    Q_INVOKABLE void connectSelected();
    Q_INVOKABLE void launchSelected();
    Q_INVOKABLE void stopSimulation();
    Q_INVOKABLE void restartSimulation();

signals:
    void simulationChanged();

private:
    QVariantMap profileForKey(const QString &key) const;
    void setStatus(const QString &status);

    VehicleConfigManager *m_vehicleConfig = nullptr;
    MavsdkVehicleManager *m_vehicle = nullptr;
    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QString m_activeProfile = "px4-gazebo-udp";
    bool m_running = false;
    QString m_status = "Simulation manager idle";
};
