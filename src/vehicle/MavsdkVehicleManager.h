#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVariantList>

#include <memory>

namespace mavsdk {
class System;
}

class VehicleTelemetryModel;
class AccessManager;

class MavsdkVehicleManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY vehicleChanged)
    Q_PROPERTY(bool discoveryActive READ discoveryActive NOTIFY vehicleChanged)
    Q_PROPERTY(QString status READ status NOTIFY vehicleChanged)
    Q_PROPERTY(QString connectionUrl READ connectionUrl NOTIFY vehicleChanged)
    Q_PROPERTY(QString systemId READ systemId NOTIFY vehicleChanged)
    Q_PROPERTY(QString autopilot READ autopilot NOTIFY vehicleChanged)
    Q_PROPERTY(bool armed READ armed NOTIFY vehicleChanged)
    Q_PROPERTY(bool inAir READ inAir NOTIFY vehicleChanged)
    Q_PROPERTY(QString flightMode READ flightMode NOTIFY vehicleChanged)
    Q_PROPERTY(QString health READ health NOTIFY vehicleChanged)
    Q_PROPERTY(QStringList availableSerialPorts READ availableSerialPorts NOTIFY serialPortsChanged)
    Q_PROPERTY(QVariantList serialPortOptions READ serialPortOptions NOTIFY serialPortsChanged)

public:
    explicit MavsdkVehicleManager(VehicleTelemetryModel *telemetry,
                                  AccessManager *access = nullptr,
                                  QObject *parent = nullptr);
    ~MavsdkVehicleManager() override;

    bool connected() const;
    bool discoveryActive() const;
    QString status() const;
    QString connectionUrl() const;
    QString systemId() const;
    QString autopilot() const;
    bool armed() const;
    bool inAir() const;
    QString flightMode() const;
    QString health() const;
    QStringList availableSerialPorts() const;
    QVariantList serialPortOptions() const;

    std::shared_ptr<mavsdk::System> system() const;

    Q_INVOKABLE void startDiscovery();
    Q_INVOKABLE void connectToUrl(const QString &url, bool allowMultipleLinks = false);
    Q_INVOKABLE void connectToUrls(const QStringList &urls, bool allowMultipleLinks = false);
    Q_INVOKABLE void stopDiscovery();
    Q_INVOKABLE void connectRetry();
    Q_INVOKABLE void refreshSystems();
    Q_INVOKABLE void refreshSerialPorts();

signals:
    void vehicleChanged();
    void systemReady();
    void serialPortsChanged();

private:
    struct Impl;
    void startDiscoveryWithUrls(QStringList urls, bool allowMultipleLinks);

    std::unique_ptr<Impl> m_impl;
    VehicleTelemetryModel *m_telemetry = nullptr;
    AccessManager *m_access = nullptr;
    QTimer m_discoveryTimer;
    bool m_discoveryActive = false;
    bool m_connected = false;
    QString m_status = "No Connected Aircraft";
    QString m_connectionUrl;
    QString m_systemId;
    QString m_autopilot = "Unknown";
    bool m_armed = false;
    bool m_inAir = false;
    QString m_flightMode = "DISCONNECTED";
    QString m_health = "Unknown";
    QStringList m_availableSerialPorts;
    QVariantList m_serialPortOptions;
};
