#include "FirmwareUpdateManager.h"

#include "../access/PermissionManager.h"
#include "../autopilot/AutopilotToolsFacade.h"
#include "../auth/SessionManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"
#include "FirmwareFlashAdapter.h"

#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>

#include <QCryptographicHash>
#include <QDateTime>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>
#include <QTimer>
#include <QRegularExpression>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QUrl>
#include <QtGlobal>

#include <array>
#include <functional>
#include <limits>
#include <map>
#include <utility>
#include <thread>

namespace {
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

QString portNameFromDisplay(const QString &display)
{
    const int separator = display.indexOf(QStringLiteral(" - "));
    return separator > 0 ? display.left(separator).trimmed() : display.trimmed();
}

quint32 littleEndian32(const QByteArray &bytes)
{
    if (bytes.size() < 4) {
        return 0;
    }
    return (quint32(quint8(bytes.at(0))))
        | (quint32(quint8(bytes.at(1))) << 8)
        | (quint32(quint8(bytes.at(2))) << 16)
        | (quint32(quint8(bytes.at(3))) << 24);
}

class SerialBootloaderTransport
{
public:
    using ProgressCallback = std::function<void(int, const QString &)>;

    SerialBootloaderTransport(QString portName, int baudRate, ProgressCallback progress)
        : m_portName(std::move(portName)),
          m_baudRate(baudRate),
          m_progress(std::move(progress))
    {
    }

    bool flash(const QByteArray &image, quint32 localCrc, QString *resultMessage)
    {
        if (m_portName.isEmpty()) {
            setResult(resultMessage, QStringLiteral("Select the bootloader COM port before flashing."));
            return false;
        }
        if (image.isEmpty()) {
            setResult(resultMessage, QStringLiteral("Firmware image is empty after package extraction."));
            return false;
        }
        if (!openPort(resultMessage)) {
            return false;
        }
        report(8, QStringLiteral("Waiting for bootloader sync on %1 at %2 baud.").arg(m_portName).arg(m_baudRate));
        if (!sync(resultMessage)) {
            return false;
        }

        quint32 bootloaderRev = 0;
        quint32 boardId = 0;
        quint32 boardRev = 0;
        quint32 flashSize = 0;
        getInfo(InfoBootloaderRevision, &bootloaderRev);
        getInfo(InfoBoardId, &boardId);
        getInfo(InfoBoardRevision, &boardRev);
        getInfo(InfoFlashSize, &flashSize);
        if (flashSize > 0 && quint32(image.size()) > flashSize) {
            setResult(resultMessage,
                      QStringLiteral("Firmware image size %1 exceeds bootloader flash size %2.")
                          .arg(image.size())
                          .arg(flashSize));
            return false;
        }

        report(20,
               QStringLiteral("Bootloader found: rev %1, board %2, board rev %3, flash %4 bytes.")
                   .arg(bootloaderRev)
                   .arg(boardId)
                   .arg(boardRev)
                   .arg(flashSize ? QString::number(flashSize) : QStringLiteral("unknown")));

        report(28, QStringLiteral("Erasing application flash."));
        if (!commandNoPayload(ProtoChipErase, 45000, resultMessage)) {
            return false;
        }

        const int chunkSize = 248;
        for (int offset = 0; offset < image.size(); offset += chunkSize) {
            const QByteArray chunk = image.mid(offset, chunkSize);
            QByteArray frame;
            frame.reserve(chunk.size() + 3);
            frame.append(char(ProtoProgMulti));
            frame.append(char(chunk.size()));
            frame.append(chunk);
            frame.append(char(ProtoEoc));
            if (!writeAll(frame, resultMessage) || !expectSync(3000, resultMessage)) {
                setResult(resultMessage,
                          QStringLiteral("Program failed at byte %1: %2")
                              .arg(offset)
                              .arg(resultMessage ? *resultMessage : QString()));
                return false;
            }
            const int progress = 35 + int((qint64(offset + chunk.size()) * 45) / image.size());
            report(progress, QStringLiteral("Programmed %1 of %2 bytes.").arg(offset + chunk.size()).arg(image.size()));
        }

        report(88, QStringLiteral("Requesting bootloader CRC verification."));
        quint32 bootloaderCrc = 0;
        if (!getCrc(&bootloaderCrc, resultMessage)) {
            return false;
        }
        if (bootloaderCrc != localCrc) {
            setResult(resultMessage,
                      QStringLiteral("Verify failed: bootloader CRC 0x%1 does not match local CRC 0x%2.")
                          .arg(QString::number(bootloaderCrc, 16).rightJustified(8, QLatin1Char('0')))
                          .arg(QString::number(localCrc, 16).rightJustified(8, QLatin1Char('0'))));
            return false;
        }

        report(96, QStringLiteral("Verification passed. Rebooting controller."));
        commandNoPayload(ProtoReboot, 1000, nullptr);
        setResult(resultMessage, QStringLiteral("Firmware erase/program/verify completed and reboot was requested."));
        return true;
    }

private:
    enum : quint8 {
        ProtoOk = 0x10,
        ProtoFailed = 0x11,
        ProtoInsync = 0x12,
        ProtoInvalid = 0x13,
        ProtoEoc = 0x20,
        ProtoGetSync = 0x21,
        ProtoGetDevice = 0x22,
        ProtoChipErase = 0x23,
        ProtoProgMulti = 0x27,
        ProtoGetCrc = 0x29,
        ProtoReboot = 0x30,
        InfoBootloaderRevision = 1,
        InfoBoardId = 2,
        InfoBoardRevision = 3,
        InfoFlashSize = 4
    };

    bool openPort(QString *reason)
    {
        m_serial.setPortName(m_portName);
        m_serial.setBaudRate(m_baudRate > 0 ? m_baudRate : 921600);
        m_serial.setDataBits(QSerialPort::Data8);
        m_serial.setParity(QSerialPort::NoParity);
        m_serial.setStopBits(QSerialPort::OneStop);
        m_serial.setFlowControl(QSerialPort::NoFlowControl);
        if (!m_serial.open(QIODevice::ReadWrite)) {
            setResult(reason, QStringLiteral("Could not open bootloader port %1: %2").arg(m_portName, m_serial.errorString()));
            return false;
        }
        m_serial.clear();
        return true;
    }

    bool sync(QString *reason)
    {
        for (int attempt = 0; attempt < 60; ++attempt) {
            m_serial.readAll();
            QByteArray frame;
            frame.append(char(ProtoGetSync));
            frame.append(char(ProtoEoc));
            if (writeAll(frame, reason) && expectSync(120, nullptr)) {
                return true;
            }
            QThread::msleep(100);
        }
        setResult(reason, QStringLiteral("No PX4/ArduPilot bootloader response. Unplug/replug the controller or request bootloader mode, then retry."));
        return false;
    }

    bool getInfo(quint8 infoId, quint32 *value)
    {
        QByteArray frame;
        frame.append(char(ProtoGetDevice));
        frame.append(char(infoId));
        frame.append(char(ProtoEoc));
        QString reason;
        if (!writeAll(frame, &reason)) {
            return false;
        }
        const QByteArray valueBytes = readExact(4, 1000);
        if (valueBytes.size() != 4) {
            return false;
        }
        if (!expectSync(1000, &reason)) {
            return false;
        }
        if (value) {
            *value = littleEndian32(valueBytes);
        }
        return true;
    }

    bool getCrc(quint32 *value, QString *reason)
    {
        QByteArray frame;
        frame.append(char(ProtoGetCrc));
        frame.append(char(ProtoEoc));
        if (!writeAll(frame, reason)) {
            return false;
        }
        const QByteArray valueBytes = readExact(4, 10000);
        if (valueBytes.size() != 4) {
            setResult(reason, QStringLiteral("Timed out waiting for bootloader CRC."));
            return false;
        }
        if (!expectSync(5000, reason)) {
            return false;
        }
        if (value) {
            *value = littleEndian32(valueBytes);
        }
        return true;
    }

    bool commandNoPayload(quint8 command, int timeoutMs, QString *reason)
    {
        QByteArray frame;
        frame.append(char(command));
        frame.append(char(ProtoEoc));
        return writeAll(frame, reason) && expectSync(timeoutMs, reason);
    }

    bool writeAll(const QByteArray &bytes, QString *reason)
    {
        if (m_serial.write(bytes) != bytes.size()) {
            setResult(reason, QStringLiteral("Serial write failed on %1: %2").arg(m_portName, m_serial.errorString()));
            return false;
        }
        if (!m_serial.waitForBytesWritten(3000)) {
            setResult(reason, QStringLiteral("Timed out writing to %1: %2").arg(m_portName, m_serial.errorString()));
            return false;
        }
        return true;
    }

    QByteArray readExact(int count, int timeoutMs)
    {
        QByteArray out;
        QElapsedTimer timer;
        timer.start();
        while (out.size() < count && timer.elapsed() < timeoutMs) {
            if (m_serial.bytesAvailable() > 0 || m_serial.waitForReadyRead(40)) {
                out.append(m_serial.read(count - out.size()));
            }
        }
        return out;
    }

    bool expectSync(int timeoutMs, QString *reason)
    {
        QElapsedTimer timer;
        timer.start();
        bool sawInsync = false;
        while (timer.elapsed() < timeoutMs) {
            if (m_serial.bytesAvailable() <= 0 && !m_serial.waitForReadyRead(40)) {
                continue;
            }
            const QByteArray bytes = m_serial.readAll();
            for (char c : bytes) {
                const quint8 value = quint8(c);
                if (!sawInsync) {
                    if (value == ProtoInsync) {
                        sawInsync = true;
                    } else if (value == ProtoFailed || value == ProtoInvalid) {
                        setResult(reason, QStringLiteral("Bootloader rejected command: 0x%1")
                                              .arg(QString::number(value, 16)));
                        return false;
                    }
                    continue;
                }
                if (value == ProtoOk) {
                    return true;
                }
                setResult(reason, QStringLiteral("Bootloader response was not OK: 0x%1")
                                      .arg(QString::number(value, 16)));
                return false;
            }
        }
        setResult(reason, QStringLiteral("Timed out waiting for bootloader sync/OK."));
        return false;
    }

    void report(int progress, const QString &message)
    {
        if (m_progress) {
            m_progress(qBound(0, progress, 100), message);
        }
    }

    void setResult(QString *target, const QString &message)
    {
        if (target) {
            *target = message;
        }
    }

    QString m_portName;
    int m_baudRate = 921600;
    QSerialPort m_serial;
    ProgressCallback m_progress;
};

class UnsupportedFirmwareAdapter final : public FirmwareFlashAdapter
{
public:
    explicit UnsupportedFirmwareAdapter(QString adapterName, QString reason)
        : m_name(std::move(adapterName)), m_reason(std::move(reason))
    {
    }

    QString name() const override { return m_name; }
    bool flashingSupported() const override { return false; }
    QString unsupportedReason() const override { return m_reason; }

    bool validatesPackage(const FirmwarePackageInfo &package,
                          const FirmwareBoardInfo &board,
                          QString *reason) const override
    {
        if (!board.connected) {
            if (reason) {
                *reason = QStringLiteral("Connect a flight controller before validating firmware target.");
            }
            return false;
        }
        const QString stack = board.stack.trimmed();
        if (package.packageType == QStringLiteral("PX4") && !stack.isEmpty() && stack != QStringLiteral("PX4") && stack != QStringLiteral("Unknown")) {
            if (reason) {
                *reason = QStringLiteral("PX4 package does not match detected %1 controller.").arg(stack);
            }
            return false;
        }
        if (package.packageType == QStringLiteral("ArduPilot") && !stack.isEmpty() && stack != QStringLiteral("ArduPilot") && stack != QStringLiteral("Unknown")) {
            if (reason) {
                *reason = QStringLiteral("ArduPilot package does not match detected %1 controller.").arg(stack);
            }
            return false;
        }
        if (reason) {
            *reason = unsupportedReason();
        }
        return true;
    }

private:
    QString m_name;
    QString m_reason;
};
}

FirmwareUpdateManager::FirmwareUpdateManager(MavsdkVehicleManager *vehicle,
                                             SessionManager *session,
                                             PermissionManager *permissions,
                                             AutopilotToolsFacade *autopilotTools,
                                             GcsEventSyncManager *events,
                                             QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_session(session),
      m_permissions(permissions),
      m_autopilotTools(autopilotTools),
      m_events(events)
{
    refreshBootloaderPorts();
    if (m_vehicle) {
        connect(m_vehicle, &MavsdkVehicleManager::vehicleChanged, this, [this]() {
            detectBoard();
        });
    }
}

QString FirmwareUpdateManager::filePath() const { return m_filePath; }
QString FirmwareUpdateManager::fileName() const { return m_fileName; }
QString FirmwareUpdateManager::packageType() const { return m_packageType; }
QString FirmwareUpdateManager::version() const { return m_version; }
QString FirmwareUpdateManager::target() const { return m_target; }
QString FirmwareUpdateManager::checksum() const { return m_checksum; }
QString FirmwareUpdateManager::fileSize() const { return m_fileSize; }
QString FirmwareUpdateManager::status() const { return m_status; }
QStringList FirmwareUpdateManager::logs() const { return m_logs; }
bool FirmwareUpdateManager::fileLoaded() const { return !m_filePath.isEmpty(); }
bool FirmwareUpdateManager::uploading() const { return m_uploading; }
int FirmwareUpdateManager::progress() const { return m_progress; }
QString FirmwareUpdateManager::state() const { return m_state; }
QString FirmwareUpdateManager::adapterName() const { return m_adapterName; }
QString FirmwareUpdateManager::boardSummary() const { return m_boardSummary; }
QString FirmwareUpdateManager::bootloaderStatus() const { return m_bootloaderStatus; }
QStringList FirmwareUpdateManager::bootloaderPorts() const { return m_bootloaderPorts; }
QString FirmwareUpdateManager::bootloaderPort() const { return m_bootloaderPort; }
int FirmwareUpdateManager::bootloaderBaud() const { return m_bootloaderBaud; }
QString FirmwareUpdateManager::validationStatus() const { return m_validationStatus; }
bool FirmwareUpdateManager::supported() const { return m_supported; }
bool FirmwareUpdateManager::canStartUpload() const
{
    return fileLoaded() && !m_uploading && m_state != QStringLiteral("unsupported") && m_state != QStringLiteral("failed");
}

void FirmwareUpdateManager::setPackageType(const QString &packageType)
{
    const QString next = packageType.trimmed().isEmpty() ? QStringLiteral("Auto") : packageType.trimmed();
    if (m_packageType == next) {
        return;
    }
    m_packageType = next;
    m_target = inferTarget(m_packageType);
    setAdapterForPackage();
    appendLog(QStringLiteral("Firmware package type set to %1.").arg(m_packageType));
    emit firmwareChanged();
}

void FirmwareUpdateManager::setBootloaderPort(const QString &port)
{
    const QString next = portNameFromDisplay(port);
    if (m_bootloaderPort == next) {
        return;
    }
    m_bootloaderPort = next;
    m_bootloaderStatus = m_bootloaderPort.isEmpty()
        ? QStringLiteral("Select the bootloader COM port used for erase/program/verify.")
        : QStringLiteral("Bootloader port set to %1.").arg(m_bootloaderPort);
    emit firmwareChanged();
}

void FirmwareUpdateManager::setBootloaderBaud(int baud)
{
    const int next = qBound(57600, baud, 1500000);
    if (m_bootloaderBaud == next) {
        return;
    }
    m_bootloaderBaud = next;
    m_bootloaderStatus = QStringLiteral("Bootloader flash baud set to %1.").arg(m_bootloaderBaud);
    emit firmwareChanged();
}

void FirmwareUpdateManager::loadFirmwareFile(const QString &pathOrUrl)
{
    m_uploading = false;
    const QString path = normalizePath(pathOrUrl);
    QFileInfo info(path);
    if (!info.exists() || !info.isFile()) {
        resetSelection();
        fail(QStringLiteral("Firmware file not found."));
        return;
    }

    const QString suffix = info.suffix().toLower();
    const QString inferredType = inferPackageType(suffix);
    if (inferredType == QStringLiteral("Unsupported")) {
        resetSelection();
        fail(QStringLiteral("Unsupported firmware format. Use PX4 .px4, ArduPilot .apj/.hex, SkyGrid .sgfw, or custom .bin packages."));
        return;
    }

    m_filePath = info.absoluteFilePath();
    m_fileName = info.fileName();
    m_packageType = inferredType;
    m_version = inferVersion(info.completeBaseName());
    m_target = inferTarget(m_packageType);
    m_fileSize = formatSize(info.size());
    m_checksum = sha256ForFile(m_filePath);
    parsePackageMetadata(m_filePath);
    setAdapterForPackage();
    setProgress(0);
    setState(QStringLiteral("fileLoaded"));
    m_validationStatus = QStringLiteral("Package metadata loaded. Validate against connected board before flashing.");
    setStatus(QStringLiteral("Firmware package loaded. Review target and connection before upload."));
    appendLog(QStringLiteral("Loaded %1 (%2, %3).").arg(m_fileName, m_packageType, m_fileSize));
    audit(QStringLiteral("firmware_package_loaded"), QStringLiteral("info"), QStringLiteral("Firmware package metadata loaded"));
    emit firmwareChanged();
}

void FirmwareUpdateManager::clear()
{
    m_uploading = false;
    m_filePath.clear();
    m_fileName = QStringLiteral("No firmware selected");
    m_packageType = QStringLiteral("Auto");
    m_version = QStringLiteral("--");
    m_target = QStringLiteral("--");
    m_checksum = QStringLiteral("--");
    m_fileSize = QStringLiteral("--");
    setProgress(0);
    setState(QStringLiteral("idle"));
    m_supported = false;
    m_adapterName = QStringLiteral("Unsupported adapter");
    m_bootloaderStatus = QStringLiteral("Bootloader not requested.");
    refreshBootloaderPorts();
    m_validationStatus = QStringLiteral("No package loaded.");
    setStatus(QStringLiteral("Select a signed firmware package."));
    appendLog(QStringLiteral("Firmware selection cleared."));
    emit firmwareChanged();
}

void FirmwareUpdateManager::clearLogs()
{
    m_logs.clear();
    emit firmwareChanged();
}

void FirmwareUpdateManager::validatePackage()
{
    if (m_filePath.isEmpty()) {
        fail(QStringLiteral("Select a firmware file before validation."));
        return;
    }
    setState(QStringLiteral("validating"));
    setProgress(20);
    appendLog(QStringLiteral("Validating package metadata and target."));
    QString reason;
    const bool valid = validateAgainstBoard(&reason);
    m_validationStatus = valid ? QStringLiteral("Package target validation passed. %1").arg(reason)
                               : reason;
    setProgress(valid ? 55 : 25);
    setState(valid ? QStringLiteral("ready") : QStringLiteral("failed"));
    setStatus(valid ? QStringLiteral("Firmware package ready for supported adapter check.") : reason);
    appendLog(m_validationStatus);
    emit firmwareChanged();
}

void FirmwareUpdateManager::detectBoard()
{
    if (m_autopilotTools) {
        const QVariantMap board = m_autopilotTools->boardInfo();
        if (board.value(QStringLiteral("connected")).toBool()) {
            m_boardSummary = QStringLiteral("%1 system %2 via %3")
                .arg(board.value(QStringLiteral("stack")).toString(),
                     board.value(QStringLiteral("systemId")).toString(),
                     board.value(QStringLiteral("connectionUrl")).toString());
            if (m_state == QStringLiteral("idle") || m_state == QStringLiteral("fileLoaded")) {
                setState(m_filePath.isEmpty() ? QStringLiteral("idle") : QStringLiteral("boardDetected"));
            }
            emit firmwareChanged();
            return;
        }
    }
    if (m_vehicle && m_vehicle->connected()) {
        m_boardSummary = QStringLiteral("%1 system %2 via %3")
            .arg(m_vehicle->autopilot(), m_vehicle->systemId(), m_vehicle->connectionUrl());
        if (m_state == QStringLiteral("idle") || m_state == QStringLiteral("fileLoaded")) {
            setState(m_filePath.isEmpty() ? QStringLiteral("idle") : QStringLiteral("boardDetected"));
        }
    } else {
        m_boardSummary = QStringLiteral("No controller detected");
    }
    emit firmwareChanged();
}

void FirmwareUpdateManager::refreshBootloaderPorts()
{
    m_bootloaderPorts.clear();
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        QString label = port.portName();
        const QString description = port.description();
        const QString manufacturer = port.manufacturer();
        if (!description.isEmpty()) {
            label += QStringLiteral(" - %1").arg(description);
        }
        if (!manufacturer.isEmpty()) {
            label += QStringLiteral(" (%1)").arg(manufacturer);
        }
        m_bootloaderPorts << label;
    }
    if (m_bootloaderPort.isEmpty() && !m_bootloaderPorts.isEmpty()) {
        m_bootloaderPort = portNameFromDisplay(m_bootloaderPorts.first());
    }
    m_bootloaderStatus = m_bootloaderPorts.isEmpty()
        ? QStringLiteral("No bootloader serial ports detected. Replug the controller in bootloader mode and refresh.")
        : QStringLiteral("%1 bootloader candidate port(s) detected.").arg(m_bootloaderPorts.size());
    emit firmwareChanged();
}

void FirmwareUpdateManager::requestBootloaderMode()
{
    if (!firmwareActionAllowed(QStringLiteral("Bootloader request"))) {
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        fail(QStringLiteral("Connect a flight controller before requesting bootloader mode."));
        return;
    }
    if (m_vehicle->armed() || m_vehicle->inAir()) {
        fail(QStringLiteral("Bootloader request blocked while vehicle is armed or in air."));
        return;
    }

    m_bootloaderStatus = QStringLiteral("Requesting reboot to bootloader over MAVLink.");
    setState(QStringLiteral("bootloaderRequested"));
    setStatus(m_bootloaderStatus);
    appendLog(m_bootloaderStatus);
    emit firmwareChanged();

    const auto system = m_vehicle->system();
    QPointer<FirmwareUpdateManager> self(this);
    std::thread([self, system]() {
        mavsdk::MavlinkPassthrough passthrough(system);
        mavsdk::MavlinkPassthrough::CommandLong command{};
        command.target_sysid = passthrough.get_target_sysid();
        command.target_compid = passthrough.get_target_compid();
        command.command = MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN;
        command.param1 = float(REBOOT_SHUTDOWN_ACTION_REBOOT_TO_BOOTLOADER);
        command.param6 = float(REBOOT_SHUTDOWN_CONDITIONS_SAFETY_INTERLOCKED);
        const mavsdk::MavlinkPassthrough::Result result = passthrough.send_command_long(command);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result]() {
            if (!self) {
                return;
            }
            const bool ok = result == mavsdk::MavlinkPassthrough::Result::Success;
            self->m_bootloaderStatus = ok
                ? QStringLiteral("Bootloader reboot command accepted. Reconnect to the bootloader serial device before erase/program/verify.")
                : QStringLiteral("Bootloader reboot command failed: %1").arg(mavlinkResultString(result));
            self->setStatus(self->m_bootloaderStatus);
            self->appendLog(self->m_bootloaderStatus);
            self->audit(ok ? QStringLiteral("firmware_bootloader_requested") : QStringLiteral("firmware_bootloader_request_failed"),
                        ok ? QStringLiteral("info") : QStringLiteral("warning"),
                        self->m_bootloaderStatus);
            emit self->firmwareChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void FirmwareUpdateManager::retryRecovery()
{
    if (m_filePath.isEmpty()) {
        setState(QStringLiteral("idle"));
        setStatus(QStringLiteral("Select a firmware package to retry."));
        return;
    }
    m_uploading = false;
    setProgress(0);
    setState(QStringLiteral("fileLoaded"));
    m_bootloaderStatus = QStringLiteral("Bootloader not requested.");
    setStatus(QStringLiteral("Firmware recovery reset. Validate package and board again."));
    appendLog(QStringLiteral("Recovery reset requested."));
    emit firmwareChanged();
}

void FirmwareUpdateManager::startUpload()
{
    if (m_uploading) {
        return;
    }
    if (!m_permissions
        || (!m_permissions->hasPermission(QStringLiteral("can_flash_firmware"))
            && !m_permissions->hasPermission(QStringLiteral("can_configure_vehicle")))) {
        fail(QStringLiteral("Firmware upload blocked by local permissions."));
        return;
    }
    if (!m_session || !m_session->operationsAllowed()) {
        fail(QStringLiteral("Firmware upload blocked: trusted Control Center session required."));
        return;
    }
    if (m_filePath.isEmpty()) {
        fail(QStringLiteral("Select a firmware file before upload."));
        return;
    }
    if (m_bootloaderPort.isEmpty()) {
        fail(QStringLiteral("Select the bootloader serial COM port before firmware upload."));
        return;
    }

    setState(QStringLiteral("validating"));
    setProgress(20);
    setStatus(QStringLiteral("Firmware upload preflight started."));
    const QString targetSummary = (m_vehicle && m_vehicle->connected())
        ? QStringLiteral("system %1 (%2)").arg(m_vehicle->systemId(), m_vehicle->autopilot())
        : QStringLiteral("bootloader port %1").arg(m_bootloaderPort);
    appendLog(QStringLiteral("Upload requested for %1 on %2.").arg(m_fileName, targetSummary));
    emit firmwareChanged();

    QTimer::singleShot(150, this, [this]() {
        QString reason;
        if (!validateAgainstBoard(&reason)) {
            fail(reason);
            return;
        }
        m_validationStatus = QStringLiteral("Package validated against detected board. %1").arg(reason);
        setState(QStringLiteral("ready"));
        setProgress(55);
        appendLog(m_validationStatus);

        QTimer::singleShot(150, this, [this]() {
            QString imageReason;
            const QByteArray image = firmwareImageBytes(&imageReason);
            if (image.isEmpty()) {
                fail(imageReason.isEmpty() ? QStringLiteral("Firmware image extraction failed.") : imageReason);
                return;
            }
            const quint32 localCrc = firmwareCrc32(image);
            setState(QStringLiteral("running"));
            m_uploading = true;
            setProgress(8);
            appendLog(QStringLiteral("Selected firmware adapter: %1.").arg(m_adapterName));
            appendLog(QStringLiteral("Prepared %1 bytes for erase/program/verify. Local CRC: 0x%2.")
                          .arg(image.size())
                          .arg(QString::number(localCrc, 16).rightJustified(8, QLatin1Char('0'))));
            audit(QStringLiteral("firmware_flash_started"),
                  QStringLiteral("info"),
                  QStringLiteral("Firmware serial bootloader flash started"));
            emit firmwareChanged();

            runSerialBootloaderFlash(image, m_bootloaderPort, m_bootloaderBaud, localCrc);
        });
    });
}

QString FirmwareUpdateManager::normalizePath(const QString &pathOrUrl) const
{
    const QUrl url(pathOrUrl);
    if (url.isValid() && url.isLocalFile()) {
        return url.toLocalFile();
    }
    QString path = pathOrUrl;
    if (path.startsWith(QStringLiteral("file:///"))) {
        path = QUrl(path).toLocalFile();
    }
    return path.trimmed();
}

QString FirmwareUpdateManager::inferPackageType(const QString &suffix) const
{
    if (suffix == QStringLiteral("px4")) {
        return QStringLiteral("PX4");
    }
    if (suffix == QStringLiteral("apj") || suffix == QStringLiteral("hex")) {
        return QStringLiteral("ArduPilot");
    }
    if (suffix == QStringLiteral("sgfw")) {
        return QStringLiteral("SkyGrid");
    }
    if (suffix == QStringLiteral("bin")) {
        return QStringLiteral("Custom");
    }
    return QStringLiteral("Unsupported");
}

QString FirmwareUpdateManager::inferVersion(const QString &baseName) const
{
    const QRegularExpression versionPattern(QStringLiteral("(v?\\d+(?:\\.\\d+){1,3}(?:[-_][A-Za-z0-9.]+)?)"));
    const QRegularExpressionMatch match = versionPattern.match(baseName);
    return match.hasMatch() ? match.captured(1) : QStringLiteral("Unknown");
}

QString FirmwareUpdateManager::inferTarget(const QString &packageType) const
{
    if (packageType == QStringLiteral("PX4")) {
        return QStringLiteral("PX4 flight controllers");
    }
    if (packageType == QStringLiteral("ArduPilot")) {
        return QStringLiteral("ArduPilot flight controllers");
    }
    if (packageType == QStringLiteral("SkyGrid")) {
        return QStringLiteral("SkyGrid signed controller packages");
    }
    if (packageType == QStringLiteral("Custom")) {
        return QStringLiteral("Custom controller packages");
    }
    return QStringLiteral("Auto-detect from file");
}

void FirmwareUpdateManager::parsePackageMetadata(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    const QByteArray prefix = file.read(256 * 1024);
    const QJsonDocument doc = QJsonDocument::fromJson(prefix);
    if (!doc.isObject()) {
        return;
    }
    const QJsonObject object = doc.object();
    const QString version = object.value(QStringLiteral("version")).toString(object.value(QStringLiteral("firmware_version")).toString());
    const QString target = object.value(QStringLiteral("target")).toString(object.value(QStringLiteral("board_id")).toString());
    const QString stack = object.value(QStringLiteral("stack")).toString(object.value(QStringLiteral("autopilot")).toString());
    if (!version.isEmpty()) {
        m_version = version;
    }
    if (!target.isEmpty()) {
        m_target = target;
    }
    if (!stack.isEmpty() && m_packageType == QStringLiteral("Custom")) {
        const QString lower = stack.toLower();
        if (lower.contains(QStringLiteral("px4"))) {
            m_packageType = QStringLiteral("PX4");
        } else if (lower.contains(QStringLiteral("ardu"))) {
            m_packageType = QStringLiteral("ArduPilot");
        } else if (lower.contains(QStringLiteral("skygrid"))) {
            m_packageType = QStringLiteral("SkyGrid");
        }
    }
}

QString FirmwareUpdateManager::formatSize(qint64 bytes) const
{
    if (bytes < 1024) {
        return QStringLiteral("%1 B").arg(bytes);
    }
    if (bytes < 1024 * 1024) {
        return QStringLiteral("%1 KB").arg(QString::number(bytes / 1024.0, 'f', 1));
    }
    return QStringLiteral("%1 MB").arg(QString::number(bytes / (1024.0 * 1024.0), 'f', 1));
}

QString FirmwareUpdateManager::sha256ForFile(const QString &path) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QStringLiteral("--");
    }
    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (!hash.addData(&file)) {
        return QStringLiteral("--");
    }
    return QString::fromLatin1(hash.result().toHex());
}

QByteArray FirmwareUpdateManager::firmwareImageBytes(QString *reason) const
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (reason) {
            *reason = QStringLiteral("Could not open firmware file for flashing.");
        }
        return {};
    }
    const QByteArray fileBytes = file.readAll();
    const QString suffix = QFileInfo(m_filePath).suffix().toLower();
    if (suffix == QStringLiteral("bin")) {
        if (reason) {
            *reason = QStringLiteral("Raw binary image loaded.");
        }
        return fileBytes;
    }
    if (suffix == QStringLiteral("hex")) {
        return firmwareImageFromIntelHex(fileBytes, reason);
    }
    if (suffix == QStringLiteral("apj")
        || suffix == QStringLiteral("px4")
        || suffix == QStringLiteral("sgfw")) {
        return firmwareImageFromJsonPackage(fileBytes, reason);
    }
    if (reason) {
        *reason = QStringLiteral("Unsupported firmware image format for bootloader flashing.");
    }
    return {};
}

QByteArray FirmwareUpdateManager::firmwareImageFromJsonPackage(const QByteArray &fileBytes, QString *reason) const
{
    const QJsonDocument doc = QJsonDocument::fromJson(fileBytes);
    if (!doc.isObject()) {
        if (reason) {
            *reason = QStringLiteral("Firmware package is not valid JSON. Use .bin or .hex for raw flashing.");
        }
        return {};
    }
    const QJsonObject object = doc.object();
    const QString imageText = object.value(QStringLiteral("image")).toString(
        object.value(QStringLiteral("firmware")).toString(
            object.value(QStringLiteral("binary")).toString()));
    if (imageText.isEmpty()) {
        if (reason) {
            *reason = QStringLiteral("Firmware package does not contain an image payload.");
        }
        return {};
    }

    QByteArray decoded = QByteArray::fromBase64(imageText.toLatin1());
    if (decoded.isEmpty()) {
        if (reason) {
            *reason = QStringLiteral("Firmware package image payload is not valid base64.");
        }
        return {};
    }

    const int imageSize = object.value(QStringLiteral("image_size")).toInt(
        object.value(QStringLiteral("imageSize")).toInt(
            object.value(QStringLiteral("flash_size")).toInt(0)));
    const QString format = object.value(QStringLiteral("format")).toString().toLower();
    const QString compression = object.value(QStringLiteral("compression")).toString().toLower();
    const bool looksCompressed = decoded.size() > 2
        && quint8(decoded.at(0)) == 0x78
        && (quint8(decoded.at(1)) == 0x01
            || quint8(decoded.at(1)) == 0x5e
            || quint8(decoded.at(1)) == 0x9c
            || quint8(decoded.at(1)) == 0xda);
    if (compression.contains(QStringLiteral("zlib"))
        || format.contains(QStringLiteral("zlib"))
        || looksCompressed) {
        QByteArray prefixed;
        const quint32 expected = imageSize > 0 ? quint32(imageSize) : quint32(decoded.size() * 8);
        prefixed.append(char((expected >> 24) & 0xff));
        prefixed.append(char((expected >> 16) & 0xff));
        prefixed.append(char((expected >> 8) & 0xff));
        prefixed.append(char(expected & 0xff));
        prefixed.append(decoded);
        const QByteArray inflated = qUncompress(prefixed);
        if (inflated.isEmpty()) {
            if (reason) {
                *reason = QStringLiteral("Firmware package image is compressed, but decompression failed.");
            }
            return {};
        }
        if (reason) {
            *reason = QStringLiteral("Compressed JSON firmware image expanded to %1 bytes.").arg(inflated.size());
        }
        return inflated;
    }

    if (reason) {
        *reason = QStringLiteral("JSON firmware image decoded to %1 bytes.").arg(decoded.size());
    }
    return decoded;
}

QByteArray FirmwareUpdateManager::firmwareImageFromIntelHex(const QByteArray &fileBytes, QString *reason) const
{
    std::map<quint32, quint8> memory;
    quint32 upperAddress = 0;
    quint32 minAddress = std::numeric_limits<quint32>::max();
    quint32 maxAddress = 0;
    const QList<QByteArray> lines = fileBytes.split('\n');
    for (int lineIndex = 0; lineIndex < lines.size(); ++lineIndex) {
        QByteArray line = lines.at(lineIndex).trimmed();
        if (line.isEmpty()) {
            continue;
        }
        if (!line.startsWith(':') || line.size() < 11) {
            if (reason) {
                *reason = QStringLiteral("Invalid Intel HEX line %1.").arg(lineIndex + 1);
            }
            return {};
        }
        line.remove(0, 1);
        QByteArray recordBytes;
        recordBytes.reserve(line.size() / 2);
        bool ok = false;
        for (int i = 0; i + 1 < line.size(); i += 2) {
            const quint8 value = quint8(line.mid(i, 2).toUInt(&ok, 16));
            if (!ok) {
                if (reason) {
                    *reason = QStringLiteral("Invalid HEX byte on line %1.").arg(lineIndex + 1);
                }
                return {};
            }
            recordBytes.append(char(value));
        }
        if (recordBytes.size() < 5) {
            continue;
        }
        quint8 checksum = 0;
        for (char byte : recordBytes) {
            checksum = quint8(checksum + quint8(byte));
        }
        if (checksum != 0) {
            if (reason) {
                *reason = QStringLiteral("Intel HEX checksum failed on line %1.").arg(lineIndex + 1);
            }
            return {};
        }
        const quint8 count = quint8(recordBytes.at(0));
        if (recordBytes.size() < count + 5) {
            if (reason) {
                *reason = QStringLiteral("Intel HEX line %1 is truncated.").arg(lineIndex + 1);
            }
            return {};
        }
        const quint16 offset = (quint16(quint8(recordBytes.at(1))) << 8) | quint16(quint8(recordBytes.at(2)));
        const quint8 type = quint8(recordBytes.at(3));
        if (type == 0x00) {
            const quint32 base = upperAddress + offset;
            for (quint8 i = 0; i < count; ++i) {
                const quint32 address = base + i;
                const quint8 value = quint8(recordBytes.at(4 + i));
                memory[address] = value;
                minAddress = std::min(minAddress, address);
                maxAddress = std::max(maxAddress, address);
            }
        } else if (type == 0x01) {
            break;
        } else if (type == 0x02 && count >= 2) {
            upperAddress = (((quint32(quint8(recordBytes.at(4))) << 8) | quint32(quint8(recordBytes.at(5)))) << 4);
        } else if (type == 0x04 && count >= 2) {
            upperAddress = (((quint32(quint8(recordBytes.at(4))) << 8) | quint32(quint8(recordBytes.at(5)))) << 16);
        }
    }
    if (memory.empty()) {
        if (reason) {
            *reason = QStringLiteral("Intel HEX file did not contain program data.");
        }
        return {};
    }
    QByteArray image;
    image.resize(int(maxAddress - minAddress + 1));
    image.fill(char(0xff));
    for (const auto &entry : memory) {
        image[int(entry.first - minAddress)] = char(entry.second);
    }
    if (reason) {
        *reason = QStringLiteral("Intel HEX image loaded from 0x%1 to 0x%2 (%3 bytes).")
                      .arg(QString::number(minAddress, 16))
                      .arg(QString::number(maxAddress, 16))
                      .arg(image.size());
    }
    return image;
}

quint32 FirmwareUpdateManager::firmwareCrc32(const QByteArray &image) const
{
    quint32 crc = 0;
    for (char byte : image) {
        crc ^= quint32(quint8(byte));
        for (int bit = 0; bit < 8; ++bit) {
            const bool carry = (crc & 1u) != 0;
            crc >>= 1;
            if (carry) {
                crc ^= 0xedb88320u;
            }
        }
    }
    return crc;
}

void FirmwareUpdateManager::appendLog(const QString &message)
{
    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss"));
    m_logs.prepend(QStringLiteral("[%1] %2").arg(timestamp, message));
    while (m_logs.size() > 80) {
        m_logs.removeLast();
    }
}

void FirmwareUpdateManager::fail(const QString &message)
{
    m_uploading = false;
    setState(QStringLiteral("failed"));
    m_status = message;
    appendLog(message);
    audit(QStringLiteral("firmware_flash_failed"), QStringLiteral("warning"), message);
    emit firmwareChanged();
}

void FirmwareUpdateManager::completeFlash(const QString &message)
{
    m_uploading = false;
    setState(QStringLiteral("completed"));
    setProgress(100);
    m_bootloaderStatus = message;
    m_status = message;
    appendLog(message);
    audit(QStringLiteral("firmware_flash_completed"), QStringLiteral("info"), message);
    emit firmwareChanged();
}

void FirmwareUpdateManager::resetSelection()
{
    m_filePath.clear();
    m_fileName = QStringLiteral("No firmware selected");
    m_packageType = QStringLiteral("Auto");
    m_version = QStringLiteral("--");
    m_target = QStringLiteral("--");
    m_checksum = QStringLiteral("--");
    m_fileSize = QStringLiteral("--");
    m_progress = 0;
}

void FirmwareUpdateManager::setStatus(const QString &status)
{
    m_status = status;
}

void FirmwareUpdateManager::setState(const QString &state)
{
    if (m_state == state) {
        return;
    }
    m_state = state;
}

void FirmwareUpdateManager::setProgress(int progress)
{
    m_progress = qBound(0, progress, 100);
}

void FirmwareUpdateManager::setAdapterForPackage()
{
    if (m_packageType == QStringLiteral("PX4")) {
        m_adapterName = QStringLiteral("PX4 serial bootloader adapter");
    } else if (m_packageType == QStringLiteral("ArduPilot")) {
        m_adapterName = QStringLiteral("ArduPilot/PX4 serial bootloader adapter");
    } else if (m_packageType == QStringLiteral("SkyGrid")) {
        m_adapterName = QStringLiteral("SkyGrid serial bootloader adapter");
    } else {
        m_adapterName = QStringLiteral("Raw binary serial bootloader adapter");
    }
    m_supported = m_packageType == QStringLiteral("PX4")
        || m_packageType == QStringLiteral("ArduPilot")
        || m_packageType == QStringLiteral("SkyGrid")
        || m_packageType == QStringLiteral("Custom");
}

bool FirmwareUpdateManager::validateAgainstBoard(QString *reason)
{
    detectBoard();
    FirmwareBoardInfo board;
    if (m_autopilotTools) {
        const QVariantMap info = m_autopilotTools->boardInfo();
        board.connected = info.value(QStringLiteral("connected")).toBool();
        board.stack = info.value(QStringLiteral("stack")).toString();
        board.autopilot = info.value(QStringLiteral("autopilot")).toString();
        board.systemId = info.value(QStringLiteral("systemId")).toString();
        board.connectionUrl = info.value(QStringLiteral("connectionUrl")).toString();
    } else if (m_vehicle) {
        board.connected = m_vehicle->connected();
        board.autopilot = m_vehicle->autopilot();
        board.systemId = m_vehicle->systemId();
        board.connectionUrl = m_vehicle->connectionUrl();
        const QString lower = board.autopilot.toLower();
        board.stack = lower.contains(QStringLiteral("px4")) ? QStringLiteral("PX4")
                    : lower.contains(QStringLiteral("ardu")) ? QStringLiteral("ArduPilot")
                    : QStringLiteral("Unknown");
    }
    setAdapterForPackage();

    const QString stack = board.stack.trimmed();
    if (board.connected && m_packageType == QStringLiteral("PX4") && !stack.isEmpty()
        && stack != QStringLiteral("PX4") && stack != QStringLiteral("Unknown")) {
        if (reason) {
            *reason = QStringLiteral("PX4 package does not match detected %1 controller.").arg(stack);
        }
        m_supported = false;
        return false;
    }
    if (board.connected && m_packageType == QStringLiteral("ArduPilot") && !stack.isEmpty()
        && stack != QStringLiteral("ArduPilot") && stack != QStringLiteral("Unknown")) {
        if (reason) {
            *reason = QStringLiteral("ArduPilot package does not match detected %1 controller.").arg(stack);
        }
        m_supported = false;
        return false;
    }

    if (m_bootloaderPort.isEmpty()) {
        if (reason) {
            *reason = QStringLiteral("Package metadata is valid, but no bootloader COM port is selected.");
        }
        m_supported = false;
        return true;
    }
    if (reason) {
        *reason = board.connected
            ? QStringLiteral("Package matches detected board; serial bootloader transport will use %1 at %2 baud.")
                  .arg(m_bootloaderPort)
                  .arg(m_bootloaderBaud)
            : QStringLiteral("No MAVLink board is connected; serial bootloader transport will use %1 at %2 baud.")
                  .arg(m_bootloaderPort)
                  .arg(m_bootloaderBaud);
    }
    return true;
}

bool FirmwareUpdateManager::firmwareActionAllowed(const QString &label)
{
    if (!m_permissions
        || (!m_permissions->hasPermission(QStringLiteral("can_flash_firmware"))
            && !m_permissions->hasPermission(QStringLiteral("can_configure_vehicle")))) {
        fail(QStringLiteral("%1 blocked by local permissions.").arg(label));
        return false;
    }
    if (!m_session || !m_session->operationsAllowed()) {
        fail(QStringLiteral("%1 blocked: trusted Control Center session required.").arg(label));
        return false;
    }
    return true;
}

void FirmwareUpdateManager::runSerialBootloaderFlash(const QByteArray &image,
                                                     const QString &port,
                                                     int baud,
                                                     quint32 localCrc)
{
    QPointer<FirmwareUpdateManager> self(this);
    std::thread([self, image, port, baud, localCrc]() {
        SerialBootloaderTransport transport(port, baud, [self](int progress, const QString &message) {
            if (!self) {
                return;
            }
            QMetaObject::invokeMethod(self, [self, progress, message]() {
                if (!self) {
                    return;
                }
                self->setProgress(progress);
                self->m_bootloaderStatus = message;
                self->setStatus(message);
                self->appendLog(message);
                emit self->firmwareChanged();
            }, Qt::QueuedConnection);
        });

        QString result;
        const bool ok = transport.flash(image, localCrc, &result);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, ok, result]() {
            if (!self) {
                return;
            }
            if (ok) {
                self->completeFlash(result);
            } else {
                self->fail(result);
            }
        }, Qt::QueuedConnection);
    }).detach();
}

void FirmwareUpdateManager::audit(const QString &eventType, const QString &severity, const QString &message) const
{
    if (!m_events) {
        return;
    }
    m_events->recordEvent(eventType,
                          severity,
                          message,
                          QJsonObject{{QStringLiteral("file_name"), m_fileName},
                                      {QStringLiteral("package_type"), m_packageType},
                                      {QStringLiteral("version"), m_version},
                                      {QStringLiteral("target"), m_target},
                                      {QStringLiteral("checksum"), m_checksum},
                                      {QStringLiteral("state"), m_state},
                                      {QStringLiteral("adapter"), m_adapterName},
                                      {QStringLiteral("board"), m_boardSummary}});
}
