#include "OptionalHardwareManager.h"

#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"

#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/rtk/rtk.h>
#include <mavsdk/plugins/shell/shell.h>

#include <QDateTime>
#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QVariantMap>

#include <memory>
#include <thread>

namespace {
QString rtkResultString(mavsdk::Rtk::Result result)
{
    switch (result) {
    case mavsdk::Rtk::Result::Success: return QStringLiteral("Success");
    case mavsdk::Rtk::Result::TooLong: return QStringLiteral("RTCM payload too long");
    case mavsdk::Rtk::Result::NoSystem: return QStringLiteral("No system");
    case mavsdk::Rtk::Result::ConnectionError: return QStringLiteral("Connection error");
    default: return QStringLiteral("Unknown");
    }
}

QString shellResultString(mavsdk::Shell::Result result)
{
    switch (result) {
    case mavsdk::Shell::Result::Success: return QStringLiteral("Success");
    case mavsdk::Shell::Result::NoSystem: return QStringLiteral("No system");
    case mavsdk::Shell::Result::ConnectionError: return QStringLiteral("Connection error");
    case mavsdk::Shell::Result::NoResponse: return QStringLiteral("No response");
    case mavsdk::Shell::Result::Busy: return QStringLiteral("Shell busy");
    default: return QStringLiteral("Unknown");
    }
}

QString mavlinkResultString(mavsdk::MavlinkPassthrough::Result result)
{
    switch (result) {
    case mavsdk::MavlinkPassthrough::Result::Success: return QStringLiteral("Success");
    case mavsdk::MavlinkPassthrough::Result::ConnectionError: return QStringLiteral("Connection error");
    case mavsdk::MavlinkPassthrough::Result::CommandNoSystem: return QStringLiteral("No system");
    case mavsdk::MavlinkPassthrough::Result::CommandBusy: return QStringLiteral("Vehicle busy");
    case mavsdk::MavlinkPassthrough::Result::CommandDenied: return QStringLiteral("Denied");
    case mavsdk::MavlinkPassthrough::Result::CommandUnsupported: return QStringLiteral("Unsupported");
    case mavsdk::MavlinkPassthrough::Result::CommandTimeout: return QStringLiteral("Timeout");
    case mavsdk::MavlinkPassthrough::Result::CommandTemporarilyRejected: return QStringLiteral("Temporarily rejected");
    case mavsdk::MavlinkPassthrough::Result::CommandFailed: return QStringLiteral("Command failed");
    default: return QStringLiteral("Unknown");
    }
}

QString firstPortNameFromDisplay(const QString &display)
{
    QString port = display.trimmed();
    const int separator = port.indexOf(QStringLiteral(" - "));
    if (separator > 0) {
        port = port.left(separator).trimmed();
    }
    if (port.startsWith(QStringLiteral("\\\\.\\"))) {
        port = port.mid(4);
    }
    return port;
}

QString readSerial(QSerialPort *serial, int timeoutMs)
{
    QByteArray response;
    if (serial->waitForReadyRead(timeoutMs)) {
        response += serial->readAll();
        while (serial->waitForReadyRead(80)) {
            response += serial->readAll();
        }
    }
    return QString::fromLatin1(response).trimmed();
}
}

OptionalHardwareManager::OptionalHardwareManager(MavsdkVehicleManager *vehicle,
                                                 AccessManager *access,
                                                 GcsEventSyncManager *events,
                                                 QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_access(access),
      m_events(events)
{
    refresh();
}

QVariantList OptionalHardwareManager::tools() const
{
    const QString serialSummary = m_serialPorts.isEmpty()
        ? QStringLiteral("No serial devices detected")
        : QStringLiteral("%1 serial device(s) detected").arg(m_serialPorts.size());
    return {
        tool(QStringLiteral("rtkGpsInject"), QStringLiteral("RTK/GPS Inject"),
             QStringLiteral("NTRIP/manual RTCM correction injection through MAVLink GPS_RTCM_DATA."),
             QStringLiteral("Adapter-ready. Requires correction source and connected vehicle."), true),
        tool(QStringLiteral("sikRadio"), QStringLiteral("SiK Radio"),
             QStringLiteral("Detect radio serial ports and prepare AT command configuration."),
             serialSummary, true),
        tool(QStringLiteral("droneCan"), QStringLiteral("DroneCAN/UAVCAN"),
             QStringLiteral("Node list, health, and diagnostics shell for CAN peripherals."),
             QStringLiteral("MAVLink UAVCAN_NODE_STATUS probe available."), true),
        tool(QStringLiteral("joystick"), QStringLiteral("Joystick"),
             QStringLiteral("USB controller mapping, calibration, deadzone, and manual-control binding."),
             QStringLiteral("Uses the existing USB/manual-control manager."), true),
        tool(QStringLiteral("px4flow"), QStringLiteral("PX4Flow"),
             QStringLiteral("Optical-flow status and parameter-backed setup."),
             QStringLiteral("MAVLink OPTICAL_FLOW probe available."), true),
        tool(QStringLiteral("bluetoothSetup"), QStringLiteral("Bluetooth Setup"),
             QStringLiteral("Bluetooth scan/connect workflow."),
             QStringLiteral("Qt Bluetooth runtime is not enabled in this build."), false),
        tool(QStringLiteral("antennaTracker"), QStringLiteral("Antenna Tracker"),
             QStringLiteral("Tracker connection profile and target/pointing status."),
             QStringLiteral("Local target profile and MAVLink status probe."), true),
        tool(QStringLiteral("terminal"), QStringLiteral("Terminal"),
             QStringLiteral("MAVLink SERIAL_CONTROL shell where supported by the autopilot."),
             QStringLiteral("MAVSDK Shell command path available."), true)
    };
}

QVariantList OptionalHardwareManager::serialRows() const
{
    QVariantList rows;
    if (m_serialPorts.isEmpty()) {
        rows << row(QStringLiteral("Serial"), QStringLiteral("No COM/USB serial devices detected"));
        return rows;
    }
    for (const QString &port : m_serialPorts) {
        rows << row(QStringLiteral("Port"), port);
    }
    return rows;
}

QVariantList OptionalHardwareManager::hardwareRows() const { return m_hardwareRows; }
QString OptionalHardwareManager::status() const { return m_status; }
QString OptionalHardwareManager::activeTool() const { return m_activeTool; }
QString OptionalHardwareManager::sikPort() const { return m_sikPort; }
int OptionalHardwareManager::sikBaud() const { return m_sikBaud; }

void OptionalHardwareManager::setSikPort(const QString &port)
{
    const QString next = firstPortNameFromDisplay(port);
    if (m_sikPort == next) {
        return;
    }
    m_sikPort = next;
    emit hardwareChanged();
}

void OptionalHardwareManager::setSikBaud(int baud)
{
    const int next = baud > 0 ? baud : 57600;
    if (m_sikBaud == next) {
        return;
    }
    m_sikBaud = next;
    emit hardwareChanged();
}

void OptionalHardwareManager::refresh()
{
    m_serialPorts.clear();
    for (const QSerialPortInfo &port : QSerialPortInfo::availablePorts()) {
        QString detail = port.portName();
        const QString description = port.description().trimmed();
        const QString manufacturer = port.manufacturer().trimmed();
        if (!description.isEmpty()) {
            detail += QStringLiteral(" - ") + description;
        }
        if (!manufacturer.isEmpty()) {
            detail += QStringLiteral(" (") + manufacturer + QStringLiteral(")");
        }
        m_serialPorts << detail;
        if (m_sikPort.isEmpty()) {
            const QString lower = detail.toLower();
            if (lower.contains(QStringLiteral("sik"))
                || lower.contains(QStringLiteral("radio"))
                || lower.contains(QStringLiteral("ftdi"))
                || lower.contains(QStringLiteral("silicon labs"))) {
                m_sikPort = port.portName();
            }
        }
    }
    if (m_sikPort.isEmpty() && !m_serialPorts.isEmpty()) {
        m_sikPort = firstPortNameFromDisplay(m_serialPorts.first());
    }
    m_hardwareRows = {
        row(QStringLiteral("Serial devices"), QString::number(m_serialPorts.size())),
        row(QStringLiteral("Selected SiK port"), m_sikPort.isEmpty() ? QStringLiteral("--") : m_sikPort),
        row(QStringLiteral("SiK baud"), QString::number(m_sikBaud)),
        row(QStringLiteral("Vehicle link"), m_vehicle && m_vehicle->connected() ? m_vehicle->connectionUrl() : QStringLiteral("No MAVLink vehicle connected")),
        row(QStringLiteral("Last refresh"), QDateTime::currentDateTime().toString(Qt::ISODate))
    };
    setStatus(QStringLiteral("Optional hardware refreshed: %1 serial device(s).").arg(m_serialPorts.size()));
    emit hardwareChanged();
}

void OptionalHardwareManager::selectTool(const QString &toolKey)
{
    const QString next = toolKey.trimmed();
    if (next.isEmpty() || m_activeTool == next) {
        return;
    }
    m_activeTool = next;
    emit hardwareChanged();
}

void OptionalHardwareManager::configureTool(const QString &toolKey)
{
    const QString key = toolKey.trimmed();
    if (!authorize(key == QStringLiteral("terminal") ? QStringLiteral("terminal") : QStringLiteral("optional_hardware"),
                   key)) {
        return;
    }
    m_activeTool = key;
    if (key == QStringLiteral("sikRadio")) {
        setStatus(m_serialPorts.isEmpty()
                      ? QStringLiteral("SiK radio setup needs a detected serial radio port.")
                      : QStringLiteral("SiK radio serial port selected. Use Configure SiK to enter AT mode and write basic settings."));
    } else if (key == QStringLiteral("rtkGpsInject")) {
        setStatus(QStringLiteral("RTK/GPS injection ready for NTRIP/manual RTCM source; vehicle injection adapter must be connected."));
    } else if (key == QStringLiteral("joystick")) {
        setStatus(QStringLiteral("Joystick setup uses the existing USB controller panel and manual-control safety guards."));
    } else if (key == QStringLiteral("antennaTracker")) {
        setStatus(QStringLiteral("Antenna tracker target can be updated from active/manual coordinates. Live pointing requires a tracker endpoint."));
    } else if (key == QStringLiteral("droneCan")) {
        setStatus(QStringLiteral("DroneCAN probe requests UAVCAN_NODE_STATUS over MAVLink."));
    } else if (key == QStringLiteral("px4flow")) {
        setStatus(QStringLiteral("PX4Flow probe requests OPTICAL_FLOW over MAVLink."));
    } else if (key == QStringLiteral("bluetoothSetup")) {
        scanBluetooth();
        return;
    } else {
        setStatus(QStringLiteral("%1 is visible but unsupported until the required adapter is enabled.").arg(key));
    }
    audit(QStringLiteral("optional_hardware_configure_requested"),
          key == QStringLiteral("terminal") ? QStringLiteral("warning") : QStringLiteral("info"),
          m_status,
          key);
    emit hardwareChanged();
}

void OptionalHardwareManager::configureSikRadio(const QString &portName, int baudRate, int netId, int airSpeed)
{
    if (!authorize(QStringLiteral("optional_hardware"), QStringLiteral("sikRadio"))) {
        return;
    }
    const QString port = firstPortNameFromDisplay(portName.isEmpty() ? m_sikPort : portName);
    const int baud = baudRate > 0 ? baudRate : m_sikBaud;
    if (port.isEmpty()) {
        setStatus(QStringLiteral("Select a SiK radio COM port before configuration."));
        return;
    }
    setSikPort(port);
    setSikBaud(baud);
    m_activeTool = QStringLiteral("sikRadio");
    setStatus(QStringLiteral("Opening %1 at %2 for SiK AT configuration.").arg(port).arg(baud));

    QPointer<OptionalHardwareManager> self(this);
    std::thread([self, port, baud, netId, airSpeed]() {
        QSerialPort serial;
        serial.setPortName(port);
        serial.setBaudRate(baud);
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity(QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);

        QString status;
        QString response;
        bool ok = false;
        if (!serial.open(QIODevice::ReadWrite)) {
            status = QStringLiteral("SiK radio open failed on %1: %2").arg(port, serial.errorString());
        } else {
            serial.clear();
            QThread::msleep(1100);
            serial.write("+++");
            serial.waitForBytesWritten(500);
            QThread::msleep(1100);
            response += readSerial(&serial, 900);
            serial.write("ATI\r\n");
            serial.waitForBytesWritten(500);
            response += QStringLiteral("\n") + readSerial(&serial, 1000);
            if (netId >= 0) {
                serial.write(QStringLiteral("ATS3=%1\r\n").arg(netId).toLatin1());
                serial.waitForBytesWritten(500);
                response += QStringLiteral("\n") + readSerial(&serial, 800);
            }
            if (airSpeed > 0) {
                serial.write(QStringLiteral("ATS2=%1\r\n").arg(airSpeed).toLatin1());
                serial.waitForBytesWritten(500);
                response += QStringLiteral("\n") + readSerial(&serial, 800);
            }
            serial.write("AT&W\r\n");
            serial.waitForBytesWritten(500);
            response += QStringLiteral("\n") + readSerial(&serial, 900);
            serial.close();
            ok = response.contains(QStringLiteral("OK"), Qt::CaseInsensitive)
                || response.contains(QStringLiteral("SiK"), Qt::CaseInsensitive)
                || response.contains(QStringLiteral("HM-TRP"), Qt::CaseInsensitive);
            status = ok
                ? QStringLiteral("SiK radio AT configuration completed on %1.").arg(port)
                : QStringLiteral("SiK radio did not return an expected AT response on %1.").arg(port);
        }

        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, ok, status, response, port, baud, netId, airSpeed]() {
            if (!self) {
                return;
            }
            self->m_hardwareRows = {
                self->row(QStringLiteral("Tool"), QStringLiteral("SiK Radio")),
                self->row(QStringLiteral("Port"), port),
                self->row(QStringLiteral("Baud"), QString::number(baud)),
                self->row(QStringLiteral("Net ID"), netId >= 0 ? QString::number(netId) : QStringLiteral("unchanged")),
                self->row(QStringLiteral("Air speed"), airSpeed > 0 ? QString::number(airSpeed) : QStringLiteral("unchanged")),
                self->row(QStringLiteral("Response"), response.isEmpty() ? QStringLiteral("No response") : response.left(600))
            };
            self->setStatus(status);
            self->audit(QStringLiteral("sik_radio_config_attempted"),
                        ok ? QStringLiteral("info") : QStringLiteral("warning"),
                        status,
                        QStringLiteral("sikRadio"));
            emit self->hardwareChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void OptionalHardwareManager::probeDroneCan()
{
    requestMavlinkMessage(MAVLINK_MSG_ID_UAVCAN_NODE_STATUS,
                          QStringLiteral("DroneCAN/UAVCAN node status"),
                          QStringLiteral("dronecan_probe_requested"),
                          QStringLiteral("droneCan"));
}

void OptionalHardwareManager::probePx4Flow()
{
    requestMavlinkMessage(MAVLINK_MSG_ID_OPTICAL_FLOW,
                          QStringLiteral("PX4Flow optical flow"),
                          QStringLiteral("px4flow_probe_requested"),
                          QStringLiteral("px4flow"));
}

void OptionalHardwareManager::probeRadioStatus()
{
    requestMavlinkMessage(MAVLINK_MSG_ID_RADIO_STATUS,
                          QStringLiteral("radio link status"),
                          QStringLiteral("radio_status_probe_requested"),
                          QStringLiteral("sikRadio"));
}

void OptionalHardwareManager::scanBluetooth()
{
    if (!authorize(QStringLiteral("optional_hardware"), QStringLiteral("bluetoothSetup"))) {
        return;
    }
    m_activeTool = QStringLiteral("bluetoothSetup");
    setStatus(QStringLiteral("Bluetooth setup is not enabled in this Qt build. Use serial/USB, UDP, TCP, or rebuild with Qt Bluetooth."));
    m_hardwareRows = {
        row(QStringLiteral("Tool"), QStringLiteral("Bluetooth Setup")),
        row(QStringLiteral("Runtime"), QStringLiteral("Qt Bluetooth unavailable")),
        row(QStringLiteral("Fallback"), QStringLiteral("Use COM/USB serial or MAVLink network endpoints"))
    };
    audit(QStringLiteral("bluetooth_scan_unavailable"),
          QStringLiteral("warning"),
          m_status,
          QStringLiteral("bluetoothSetup"));
    emit hardwareChanged();
}

void OptionalHardwareManager::updateAntennaTrackerTarget(double latitude, double longitude, double altitude)
{
    if (!authorize(QStringLiteral("optional_hardware"), QStringLiteral("antennaTracker"))) {
        return;
    }
    m_activeTool = QStringLiteral("antennaTracker");
    const bool valid = qAbs(latitude) <= 90.0 && qAbs(longitude) <= 180.0;
    if (!valid) {
        setStatus(QStringLiteral("Antenna tracker target rejected: latitude/longitude are invalid."));
        return;
    }
    m_hardwareRows = {
        row(QStringLiteral("Tool"), QStringLiteral("Antenna Tracker")),
        row(QStringLiteral("Target latitude"), QString::number(latitude, 'f', 7)),
        row(QStringLiteral("Target longitude"), QString::number(longitude, 'f', 7)),
        row(QStringLiteral("Target altitude"), QStringLiteral("%1 m").arg(QString::number(altitude, 'f', 1))),
        row(QStringLiteral("Tracker link"), m_vehicle && m_vehicle->connected()
                ? QStringLiteral("Active vehicle target staged")
                : QStringLiteral("No live vehicle; target stored locally"))
    };
    setStatus(QStringLiteral("Antenna tracker target updated. Connect a tracker MAVLink endpoint to drive live pointing."));
    audit(QStringLiteral("antenna_tracker_target_updated"),
          QStringLiteral("info"),
          m_status,
          QStringLiteral("antennaTracker"));
    emit hardwareChanged();
}

void OptionalHardwareManager::injectRtcmSample()
{
    if (!authorize(QStringLiteral("optional_hardware"), QStringLiteral("rtkGpsInject"))) {
        return;
    }
    setStatus(QStringLiteral("RTCM injection requires a live correction stream; no sample data was sent."));
    audit(QStringLiteral("rtcm_injection_blocked_no_source"),
          QStringLiteral("warning"),
          QStringLiteral("RTCM injection requested without correction source"),
          QStringLiteral("rtkGpsInject"));
}

void OptionalHardwareManager::injectRtcmBase64(const QString &base64Data)
{
    if (!authorize(QStringLiteral("optional_hardware"), QStringLiteral("rtkGpsInject"))) {
        return;
    }
    const QString trimmed = base64Data.trimmed();
    if (trimmed.isEmpty()) {
        setStatus(QStringLiteral("Enter base64 RTCM correction data or connect an NTRIP source before injection."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a vehicle before injecting RTK corrections."));
        return;
    }

    setStatus(QStringLiteral("Sending RTCM correction data to vehicle."));
    auto rtk = std::make_shared<mavsdk::Rtk>(m_vehicle->system());
    QPointer<OptionalHardwareManager> self(this);
    std::thread([self, rtk, trimmed]() {
        mavsdk::Rtk::RtcmData data{};
        data.data_base64 = trimmed.toStdString();
        const mavsdk::Rtk::Result result = rtk->send_rtcm_data(data);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result]() {
            if (!self) {
                return;
            }
            const bool ok = result == mavsdk::Rtk::Result::Success;
            self->setStatus(ok
                                ? QStringLiteral("RTCM correction data sent to vehicle.")
                                : QStringLiteral("RTCM injection failed: %1").arg(rtkResultString(result)));
            self->audit(QStringLiteral("rtcm_injection_attempted"),
                        ok ? QStringLiteral("info") : QStringLiteral("warning"),
                        self->m_status,
                        QStringLiteral("rtkGpsInject"));
        }, Qt::QueuedConnection);
    }).detach();
}

void OptionalHardwareManager::sendTerminalCommand(const QString &command)
{
    if (!authorize(QStringLiteral("terminal"), QStringLiteral("terminal"))) {
        return;
    }
    const QString trimmed = command.trimmed();
    if (trimmed.isEmpty()) {
        setStatus(QStringLiteral("Enter a terminal command first."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a vehicle before sending terminal commands."));
        return;
    }
    setStatus(QStringLiteral("Sending terminal command through MAVSDK shell."));
    auto shell = std::make_shared<mavsdk::Shell>(m_vehicle->system());
    QPointer<OptionalHardwareManager> self(this);
    std::thread([self, shell, trimmed]() {
        const mavsdk::Shell::Result result = shell->send(trimmed.toStdString());
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result]() {
            if (!self) {
                return;
            }
            const bool ok = result == mavsdk::Shell::Result::Success;
            self->setStatus(ok
                                ? QStringLiteral("Terminal command sent through MAVSDK shell.")
                                : QStringLiteral("Terminal command failed: %1").arg(shellResultString(result)));
            self->audit(QStringLiteral("terminal_command_sent"),
                        ok ? QStringLiteral("info") : QStringLiteral("warning"),
                        self->m_status,
                        QStringLiteral("terminal"));
        }, Qt::QueuedConnection);
    }).detach();
}

QVariantMap OptionalHardwareManager::tool(const QString &key,
                                          const QString &name,
                                          const QString &description,
                                          const QString &status,
                                          bool supported) const
{
    return QVariantMap{{QStringLiteral("key"), key},
                       {QStringLiteral("name"), name},
                       {QStringLiteral("description"), description},
                       {QStringLiteral("status"), status},
                       {QStringLiteral("supported"), supported}};
}

QVariantMap OptionalHardwareManager::row(const QString &name, const QString &value) const
{
    return QVariantMap{{QStringLiteral("name"), name}, {QStringLiteral("value"), value}};
}

bool OptionalHardwareManager::authorize(const QString &action, const QString &label)
{
    if (m_access && !m_access->authorizeAction(action,
                                                QVariantMap{{QStringLiteral("hardware_tool"), label}},
                                                QStringLiteral("Optional hardware action blocked by local permissions."))) {
        setStatus(QStringLiteral("Optional hardware action blocked by RBAC."));
        return false;
    }
    return true;
}

bool OptionalHardwareManager::ensureVehicleLink(const QString &label)
{
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a vehicle before probing %1.").arg(label));
        return false;
    }
    return true;
}

void OptionalHardwareManager::requestMavlinkMessage(int messageId,
                                                    const QString &label,
                                                    const QString &eventType,
                                                    const QString &toolKey)
{
    if (!authorize(QStringLiteral("optional_hardware"), toolKey)) {
        return;
    }
    if (!ensureVehicleLink(label)) {
        return;
    }
    m_activeTool = toolKey;
    setStatus(QStringLiteral("Requesting %1 over MAVLink.").arg(label));
    const auto system = m_vehicle->system();
    QPointer<OptionalHardwareManager> self(this);
    std::thread([self, system, messageId, label, eventType, toolKey]() {
        mavsdk::MavlinkPassthrough passthrough(system);
        mavsdk::MavlinkPassthrough::CommandLong command{};
        command.target_sysid = passthrough.get_target_sysid();
        command.target_compid = passthrough.get_target_compid();
        command.command = MAV_CMD_REQUEST_MESSAGE;
        command.param1 = float(messageId);
        const mavsdk::MavlinkPassthrough::Result result = passthrough.send_command_long(command);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, messageId, label, eventType, toolKey]() {
            if (!self) {
                return;
            }
            const bool ok = result == mavsdk::MavlinkPassthrough::Result::Success;
            self->m_hardwareRows = {
                self->row(QStringLiteral("Tool"), toolKey),
                self->row(QStringLiteral("Requested message"), QStringLiteral("%1 (%2)").arg(label).arg(messageId)),
                self->row(QStringLiteral("Result"), mavlinkResultString(result)),
                self->row(QStringLiteral("Note"), ok
                              ? QStringLiteral("Request accepted. Watch live MAVLink/telemetry for the response message.")
                              : QStringLiteral("Autopilot did not accept the request or does not support this message."))
            };
            self->setStatus(ok
                                ? QStringLiteral("%1 request sent. Response depends on connected hardware/autopilot support.").arg(label)
                                : QStringLiteral("%1 request failed: %2").arg(label, mavlinkResultString(result)));
            self->audit(eventType,
                        ok ? QStringLiteral("info") : QStringLiteral("warning"),
                        self->m_status,
                        toolKey);
            emit self->hardwareChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void OptionalHardwareManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit hardwareChanged();
}

void OptionalHardwareManager::audit(const QString &eventType,
                                    const QString &severity,
                                    const QString &message,
                                    const QString &toolKey) const
{
    if (!m_events) {
        return;
    }
    m_events->recordEvent(eventType,
                          severity,
                          message,
                          QJsonObject{{QStringLiteral("tool"), toolKey},
                                      {QStringLiteral("serial_ports"), m_serialPorts.size()}});
}
