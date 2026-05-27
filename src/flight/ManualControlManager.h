#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QString>
#include <QTimer>

#include <memory>

class GcsEventSyncManager;
class MavsdkVehicleManager;
class SessionManager;

namespace mavsdk {
class ManualControl;
}

class ManualControlManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active NOTIFY manualControlChanged)
    Q_PROPERTY(QString status READ status NOTIFY manualControlChanged)
    Q_PROPERTY(double forward READ forward NOTIFY manualControlChanged)
    Q_PROPERTY(double lateral READ lateral NOTIFY manualControlChanged)
    Q_PROPERTY(double vertical READ vertical NOTIFY manualControlChanged)
    Q_PROPERTY(double yaw READ yaw NOTIFY manualControlChanged)

public:
    explicit ManualControlManager(MavsdkVehicleManager *vehicle,
                                  SessionManager *session,
                                  GcsEventSyncManager *events,
                                  QObject *parent = nullptr);

    bool active() const;
    QString status() const;
    double forward() const;
    double lateral() const;
    double vertical() const;
    double yaw() const;

    Q_INVOKABLE void startManualControl();
    Q_INVOKABLE void stopManualControl();
    Q_INVOKABLE void setInput(double forward, double lateral, double vertical, double yaw);
    Q_INVOKABLE void neutral();
    Q_INVOKABLE void goToCoordinate(double latitude, double longitude, double altitudeMeters, double speedMps);

signals:
    void manualControlChanged();

private:
    bool canControl() const;
    void setStatus(const QString &status);
    void publishInput();

    MavsdkVehicleManager *m_vehicle = nullptr;
    SessionManager *m_session = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    std::shared_ptr<mavsdk::ManualControl> m_manualControl;
    QTimer m_publishTimer;
    QElapsedTimer m_eventThrottle;
    bool m_active = false;
    QString m_status = "Manual control idle";
    double m_forward = 0.0;
    double m_lateral = 0.0;
    double m_vertical = 0.0;
    double m_yaw = 0.0;
};
