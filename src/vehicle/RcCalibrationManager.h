#pragma once

#include <QObject>
#include <QVariantList>

#include <memory>

class GcsEventSyncManager;
class MavsdkVehicleManager;
class PermissionManager;
class SessionManager;

namespace mavsdk {
class MavlinkPassthrough;
class System;
}

class RcCalibrationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool monitoring READ monitoring NOTIFY calibrationChanged)
    Q_PROPERTY(bool rcAvailable READ rcAvailable NOTIFY calibrationChanged)
    Q_PROPERTY(QString status READ status NOTIFY calibrationChanged)
    Q_PROPERTY(QVariantList channels READ channels NOTIFY calibrationChanged)
    Q_PROPERTY(int sampleCount READ sampleCount NOTIFY calibrationChanged)

public:
    explicit RcCalibrationManager(MavsdkVehicleManager *vehicle,
                                  SessionManager *session,
                                  PermissionManager *permissions,
                                  GcsEventSyncManager *events,
                                  QObject *parent = nullptr);
    ~RcCalibrationManager() override;

    bool monitoring() const;
    bool rcAvailable() const;
    QString status() const;
    QVariantList channels() const;
    int sampleCount() const;

    Q_INVOKABLE void startMonitoring();
    Q_INVOKABLE void stopMonitoring();
    Q_INVOKABLE void resetCalibration();
    Q_INVOKABLE void captureTrim();
    Q_INVOKABLE void saveCalibration();

signals:
    void calibrationChanged();

private:
    struct ChannelState {
        int value = 0;
        int minimum = 0;
        int maximum = 0;
        int trim = 0;
        bool seen = false;
    };

    void setStatus(const QString &status);
    void handleRcChannels(const uint16_t *values, int count, int rssi);
    void unsubscribe();
    bool canWriteCalibration() const;
    QVariantList buildChannels() const;

    MavsdkVehicleManager *m_vehicle = nullptr;
    SessionManager *m_session = nullptr;
    PermissionManager *m_permissions = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    std::shared_ptr<mavsdk::MavlinkPassthrough> m_passthrough;
    bool m_monitoring = false;
    bool m_rcAvailable = false;
    QString m_status = "RC monitor idle.";
    int m_sampleCount = 0;
    int m_rssi = -1;
    ChannelState m_channels[8];
};
