#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVector>

class ManualControlManager;

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <windows.h>
#include <dinput.h>
#endif

class UsbControllerManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool supported READ supported CONSTANT)
    Q_PROPERTY(bool connected READ connected NOTIFY controllerChanged)
    Q_PROPERTY(bool enabled READ enabled NOTIFY controllerChanged)
    Q_PROPERTY(bool centerCalibrated READ centerCalibrated NOTIFY controllerChanged)
    Q_PROPERTY(QString status READ status NOTIFY controllerChanged)
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY controllerChanged)
    Q_PROPERTY(QVariantList axes READ axes NOTIFY controllerChanged)
    Q_PROPERTY(double forward READ forward NOTIFY controllerChanged)
    Q_PROPERTY(double lateral READ lateral NOTIFY controllerChanged)
    Q_PROPERTY(double vertical READ vertical NOTIFY controllerChanged)
    Q_PROPERTY(double yaw READ yaw NOTIFY controllerChanged)
    Q_PROPERTY(int forwardAxis READ forwardAxis NOTIFY mappingChanged)
    Q_PROPERTY(int lateralAxis READ lateralAxis NOTIFY mappingChanged)
    Q_PROPERTY(int verticalAxis READ verticalAxis NOTIFY mappingChanged)
    Q_PROPERTY(int yawAxis READ yawAxis NOTIFY mappingChanged)
    Q_PROPERTY(bool forwardInverted READ forwardInverted NOTIFY mappingChanged)
    Q_PROPERTY(bool lateralInverted READ lateralInverted NOTIFY mappingChanged)
    Q_PROPERTY(bool verticalInverted READ verticalInverted NOTIFY mappingChanged)
    Q_PROPERTY(bool yawInverted READ yawInverted NOTIFY mappingChanged)

public:
    explicit UsbControllerManager(ManualControlManager *manualControl, QObject *parent = nullptr);
    ~UsbControllerManager() override;

    bool supported() const;
    bool connected() const;
    bool enabled() const;
    bool centerCalibrated() const;
    QString status() const;
    QString deviceName() const;
    QVariantList axes() const;
    double forward() const;
    double lateral() const;
    double vertical() const;
    double yaw() const;
    int forwardAxis() const;
    int lateralAxis() const;
    int verticalAxis() const;
    int yawAxis() const;
    bool forwardInverted() const;
    bool lateralInverted() const;
    bool verticalInverted() const;
    bool yawInverted() const;

    Q_INVOKABLE void refreshDevices();
    Q_INVOKABLE void setEnabled(bool enabled);
    Q_INVOKABLE void calibrateCenter();
    Q_INVOKABLE void setAxisMapping(const QString &control, int axisIndex, bool inverted);

signals:
    void controllerChanged();
    void mappingChanged();

private:
    struct AxisState {
        QString name;
        double raw = 0.0;
        double center = 0.0;
        double normalized = 0.0;
    };

    void poll();
    void setStatus(const QString &status);
    void setConnected(bool connected);
    void applyManualInput();
    double mappedAxis(int axisIndex, bool inverted) const;
    void loadSettings();
    void saveSettings() const;
    QVariantList buildAxisList() const;
    void resetDevice();

#ifdef Q_OS_WIN
    bool initializeDirectInput();
    bool openFirstGameController();
    bool updateDirectInputState();
    static BOOL CALLBACK enumDevicesCallback(const DIDEVICEINSTANCE *instance, VOID *context);
#endif

    ManualControlManager *m_manualControl = nullptr;
    QTimer m_pollTimer;
    QVector<AxisState> m_axes;
    bool m_connected = false;
    bool m_enabled = false;
    bool m_centerCalibrated = false;
    QString m_status = QStringLiteral("USB controller idle");
    QString m_deviceName;
    double m_forward = 0.0;
    double m_lateral = 0.0;
    double m_vertical = 0.0;
    double m_yaw = 0.0;
    int m_forwardAxis = 1;
    int m_lateralAxis = 0;
    int m_verticalAxis = 2;
    int m_yawAxis = 5;
    bool m_forwardInverted = true;
    bool m_lateralInverted = false;
    bool m_verticalInverted = false;
    bool m_yawInverted = false;
    double m_deadzone = 0.08;

#ifdef Q_OS_WIN
    IDirectInput8 *m_directInput = nullptr;
    IDirectInputDevice8 *m_device = nullptr;
    GUID m_pendingDeviceGuid{};
    QString m_pendingDeviceName;
#endif
};
