#include "UsbControllerManager.h"

#include "ManualControlManager.h"

#include <QSettings>
#include <QVariantMap>

#include <algorithm>
#include <cmath>

namespace {
constexpr double kAxisRange = 1000.0;
constexpr int kPollMs = 40;
constexpr int kIdlePollMs = 500;

double clampUnit(double value)
{
    return std::clamp(value, -1.0, 1.0);
}
}

UsbControllerManager::UsbControllerManager(ManualControlManager *manualControl, QObject *parent)
    : QObject(parent),
      m_manualControl(manualControl)
{
    m_axes = {
        {QStringLiteral("X / Roll"), 0.0, 0.0, 0.0},
        {QStringLiteral("Y / Pitch"), 0.0, 0.0, 0.0},
        {QStringLiteral("Z / Throttle"), 0.0, 0.0, 0.0},
        {QStringLiteral("RX"), 0.0, 0.0, 0.0},
        {QStringLiteral("RY"), 0.0, 0.0, 0.0},
        {QStringLiteral("RZ / Yaw"), 0.0, 0.0, 0.0}
    };
    loadSettings();
    connect(&m_pollTimer, &QTimer::timeout, this, &UsbControllerManager::poll);
    m_pollTimer.start(kIdlePollMs);
    refreshDevices();
}

UsbControllerManager::~UsbControllerManager()
{
    setEnabled(false);
    resetDevice();
#ifdef Q_OS_WIN
    if (m_directInput) {
        m_directInput->Release();
        m_directInput = nullptr;
    }
#endif
}

bool UsbControllerManager::supported() const
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

bool UsbControllerManager::connected() const { return m_connected; }
bool UsbControllerManager::enabled() const { return m_enabled; }
bool UsbControllerManager::centerCalibrated() const { return m_centerCalibrated; }
QString UsbControllerManager::status() const { return m_status; }
QString UsbControllerManager::deviceName() const { return m_deviceName; }
QVariantList UsbControllerManager::axes() const { return buildAxisList(); }
double UsbControllerManager::forward() const { return m_forward; }
double UsbControllerManager::lateral() const { return m_lateral; }
double UsbControllerManager::vertical() const { return m_vertical; }
double UsbControllerManager::yaw() const { return m_yaw; }
int UsbControllerManager::forwardAxis() const { return m_forwardAxis; }
int UsbControllerManager::lateralAxis() const { return m_lateralAxis; }
int UsbControllerManager::verticalAxis() const { return m_verticalAxis; }
int UsbControllerManager::yawAxis() const { return m_yawAxis; }
bool UsbControllerManager::forwardInverted() const { return m_forwardInverted; }
bool UsbControllerManager::lateralInverted() const { return m_lateralInverted; }
bool UsbControllerManager::verticalInverted() const { return m_verticalInverted; }
bool UsbControllerManager::yawInverted() const { return m_yawInverted; }

void UsbControllerManager::refreshDevices()
{
    if (!supported()) {
        setStatus(QStringLiteral("USB controller input is only available on Windows builds."));
        return;
    }

#ifdef Q_OS_WIN
    resetDevice();
    if (!initializeDirectInput()) {
        setConnected(false);
        setStatus(QStringLiteral("DirectInput could not start."));
        return;
    }
    if (!openFirstGameController()) {
        setConnected(false);
        setStatus(QStringLiteral("No USB game controller detected."));
        return;
    }
    setConnected(true);
    setStatus(QStringLiteral("USB controller detected. Center sticks and calibrate before enabling."));
    m_pollTimer.setInterval(kPollMs);
    poll();
#endif
}

void UsbControllerManager::setEnabled(bool enabled)
{
    if (enabled == m_enabled) {
        return;
    }
    if (enabled) {
        if (!m_connected) {
            setStatus(QStringLiteral("USB controller not connected."));
            return;
        }
        if (!m_centerCalibrated) {
            setStatus(QStringLiteral("Center sticks and click Calibrate Center before enabling USB control."));
            return;
        }
    }

    m_enabled = enabled;
    if (!m_enabled) {
        m_forward = 0.0;
        m_lateral = 0.0;
        m_vertical = 0.0;
        m_yaw = 0.0;
        if (m_manualControl) {
            m_manualControl->neutral();
        }
        setStatus(m_connected ? QStringLiteral("USB controller standby.") : QStringLiteral("USB controller disconnected."));
    } else {
        setStatus(QStringLiteral("USB controller active. Manual inputs are being sent to the aircraft."));
        applyManualInput();
    }
    emit controllerChanged();
}

void UsbControllerManager::calibrateCenter()
{
    if (!m_connected) {
        setStatus(QStringLiteral("Connect a USB controller before calibrating center."));
        return;
    }
    poll();
    for (AxisState &axis : m_axes) {
        axis.center = axis.raw;
        axis.normalized = 0.0;
    }
    m_forward = 0.0;
    m_lateral = 0.0;
    m_vertical = 0.0;
    m_yaw = 0.0;
    m_centerCalibrated = true;
    saveSettings();
    setStatus(QStringLiteral("Center calibrated. Enable USB control when ready."));
    emit controllerChanged();
}

void UsbControllerManager::setAxisMapping(const QString &control, int axisIndex, bool inverted)
{
    const int maxAxis = std::max(0, static_cast<int>(m_axes.size()) - 1);
    const int clampedAxis = std::clamp(axisIndex, 0, maxAxis);
    const QString key = control.trimmed().toLower();
    if (key == QStringLiteral("forward")) {
        m_forwardAxis = clampedAxis;
        m_forwardInverted = inverted;
    } else if (key == QStringLiteral("lateral")) {
        m_lateralAxis = clampedAxis;
        m_lateralInverted = inverted;
    } else if (key == QStringLiteral("vertical")) {
        m_verticalAxis = clampedAxis;
        m_verticalInverted = inverted;
    } else if (key == QStringLiteral("yaw")) {
        m_yawAxis = clampedAxis;
        m_yawInverted = inverted;
    } else {
        return;
    }
    saveSettings();
    emit mappingChanged();
    applyManualInput();
}

void UsbControllerManager::poll()
{
    if (!supported()) {
        return;
    }

#ifdef Q_OS_WIN
    if (!m_device) {
        refreshDevices();
        return;
    }
    if (!updateDirectInputState()) {
        resetDevice();
        setConnected(false);
        setEnabled(false);
        setStatus(QStringLiteral("USB controller disconnected."));
        m_pollTimer.setInterval(kIdlePollMs);
        emit controllerChanged();
        return;
    }
#endif

    if (m_centerCalibrated) {
        for (AxisState &axis : m_axes) {
            double normalized = (axis.raw - axis.center) / kAxisRange;
            if (std::abs(normalized) < m_deadzone) {
                normalized = 0.0;
            }
            axis.normalized = clampUnit(normalized);
        }
    } else {
        for (AxisState &axis : m_axes) {
            axis.normalized = clampUnit(axis.raw / kAxisRange);
        }
    }

    m_forward = mappedAxis(m_forwardAxis, m_forwardInverted);
    m_lateral = mappedAxis(m_lateralAxis, m_lateralInverted);
    m_vertical = mappedAxis(m_verticalAxis, m_verticalInverted);
    m_yaw = mappedAxis(m_yawAxis, m_yawInverted);
    applyManualInput();
    emit controllerChanged();
}

void UsbControllerManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit controllerChanged();
}

void UsbControllerManager::setConnected(bool connected)
{
    if (m_connected == connected) {
        return;
    }
    m_connected = connected;
    emit controllerChanged();
}

void UsbControllerManager::applyManualInput()
{
    if (!m_enabled || !m_manualControl) {
        return;
    }
    m_manualControl->setInput(m_forward, m_lateral, m_vertical, m_yaw);
}

double UsbControllerManager::mappedAxis(int axisIndex, bool inverted) const
{
    if (axisIndex < 0 || axisIndex >= m_axes.size()) {
        return 0.0;
    }
    const double value = m_axes.at(axisIndex).normalized;
    return inverted ? -value : value;
}

void UsbControllerManager::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("usbController"));
    m_forwardAxis = settings.value(QStringLiteral("forwardAxis"), m_forwardAxis).toInt();
    m_lateralAxis = settings.value(QStringLiteral("lateralAxis"), m_lateralAxis).toInt();
    m_verticalAxis = settings.value(QStringLiteral("verticalAxis"), m_verticalAxis).toInt();
    m_yawAxis = settings.value(QStringLiteral("yawAxis"), m_yawAxis).toInt();
    m_forwardInverted = settings.value(QStringLiteral("forwardInverted"), m_forwardInverted).toBool();
    m_lateralInverted = settings.value(QStringLiteral("lateralInverted"), m_lateralInverted).toBool();
    m_verticalInverted = settings.value(QStringLiteral("verticalInverted"), m_verticalInverted).toBool();
    m_yawInverted = settings.value(QStringLiteral("yawInverted"), m_yawInverted).toBool();
    settings.endGroup();
}

void UsbControllerManager::saveSettings() const
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("usbController"));
    settings.setValue(QStringLiteral("forwardAxis"), m_forwardAxis);
    settings.setValue(QStringLiteral("lateralAxis"), m_lateralAxis);
    settings.setValue(QStringLiteral("verticalAxis"), m_verticalAxis);
    settings.setValue(QStringLiteral("yawAxis"), m_yawAxis);
    settings.setValue(QStringLiteral("forwardInverted"), m_forwardInverted);
    settings.setValue(QStringLiteral("lateralInverted"), m_lateralInverted);
    settings.setValue(QStringLiteral("verticalInverted"), m_verticalInverted);
    settings.setValue(QStringLiteral("yawInverted"), m_yawInverted);
    settings.endGroup();
}

QVariantList UsbControllerManager::buildAxisList() const
{
    QVariantList list;
    for (int i = 0; i < m_axes.size(); ++i) {
        const AxisState &axis = m_axes.at(i);
        QVariantMap item;
        item.insert(QStringLiteral("index"), i);
        item.insert(QStringLiteral("name"), axis.name);
        item.insert(QStringLiteral("raw"), axis.raw);
        item.insert(QStringLiteral("center"), axis.center);
        item.insert(QStringLiteral("value"), axis.normalized);
        list.append(item);
    }
    return list;
}

void UsbControllerManager::resetDevice()
{
#ifdef Q_OS_WIN
    if (m_device) {
        m_device->Unacquire();
        m_device->Release();
        m_device = nullptr;
    }
#endif
    m_deviceName.clear();
    m_centerCalibrated = false;
}

#ifdef Q_OS_WIN
bool UsbControllerManager::initializeDirectInput()
{
    if (m_directInput) {
        return true;
    }
    const HRESULT result = DirectInput8Create(GetModuleHandle(nullptr),
                                             DIRECTINPUT_VERSION,
                                             IID_IDirectInput8,
                                             reinterpret_cast<void **>(&m_directInput),
                                             nullptr);
    return SUCCEEDED(result);
}

bool UsbControllerManager::openFirstGameController()
{
    m_pendingDeviceGuid = GUID{};
    m_pendingDeviceName.clear();
    const HRESULT enumResult = m_directInput->EnumDevices(DI8DEVCLASS_GAMECTRL,
                                                         &UsbControllerManager::enumDevicesCallback,
                                                         this,
                                                         DIEDFL_ATTACHEDONLY);
    if (FAILED(enumResult) || m_pendingDeviceName.isEmpty()) {
        return false;
    }

    if (FAILED(m_directInput->CreateDevice(m_pendingDeviceGuid, &m_device, nullptr)) || !m_device) {
        return false;
    }
    if (FAILED(m_device->SetDataFormat(&c_dfDIJoystick2))) {
        resetDevice();
        return false;
    }
    m_device->SetCooperativeLevel(GetDesktopWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

    DIPROPRANGE range{};
    range.diph.dwSize = sizeof(DIPROPRANGE);
    range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    range.diph.dwObj = 0;
    range.diph.dwHow = DIPH_DEVICE;
    range.lMin = -static_cast<LONG>(kAxisRange);
    range.lMax = static_cast<LONG>(kAxisRange);
    m_device->SetProperty(DIPROP_RANGE, &range.diph);
    m_device->Acquire();
    m_deviceName = m_pendingDeviceName;
    return true;
}

bool UsbControllerManager::updateDirectInputState()
{
    if (!m_device) {
        return false;
    }

    HRESULT result = m_device->Poll();
    if (FAILED(result)) {
        result = m_device->Acquire();
        while (result == DIERR_INPUTLOST) {
            result = m_device->Acquire();
        }
    }
    if (FAILED(result)) {
        return false;
    }

    DIJOYSTATE2 state{};
    result = m_device->GetDeviceState(sizeof(DIJOYSTATE2), &state);
    if (FAILED(result)) {
        return false;
    }

    if (m_axes.size() >= 6) {
        m_axes[0].raw = state.lX;
        m_axes[1].raw = state.lY;
        m_axes[2].raw = state.lZ;
        m_axes[3].raw = state.lRx;
        m_axes[4].raw = state.lRy;
        m_axes[5].raw = state.lRz;
    }
    return true;
}

BOOL CALLBACK UsbControllerManager::enumDevicesCallback(const DIDEVICEINSTANCE *instance, VOID *context)
{
    auto *self = static_cast<UsbControllerManager *>(context);
    self->m_pendingDeviceGuid = instance->guidInstance;
#ifdef UNICODE
    self->m_pendingDeviceName = QString::fromWCharArray(instance->tszProductName);
#else
    self->m_pendingDeviceName = QString::fromLocal8Bit(instance->tszProductName);
#endif
    return DIENUM_STOP;
}
#endif
