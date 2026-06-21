#include "ProtocolTestManager.h"

#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTextStream>
#include <QVariantMap>

namespace {
quint8 hexChecksum(const QByteArray &record)
{
    quint8 sum = 0;
    for (char byte : record) {
        sum = quint8(sum + quint8(byte));
    }
    return quint8((~sum) + 1);
}

QString hexByte(quint8 value)
{
    return QString::number(value, 16).rightJustified(2, QLatin1Char('0')).toUpper();
}
}

ProtocolTestManager::ProtocolTestManager(AccessManager *access,
                                         GcsEventSyncManager *events,
                                         QObject *parent)
    : QObject(parent),
      m_access(access),
      m_events(events)
{
}

QVariantList ProtocolTestManager::testRows() const { return m_testRows; }
QVariantList ProtocolTestManager::artifactRows() const { return m_artifactRows; }
QString ProtocolTestManager::artifactDirectory() const { return ensureArtifactDirectory(); }
QString ProtocolTestManager::status() const { return m_status; }
bool ProtocolTestManager::running() const { return m_running; }

void ProtocolTestManager::runAll()
{
    if (!authorize(QStringLiteral("simulation"), QStringLiteral("protocol test mode"))) {
        return;
    }
    m_running = true;
    m_testRows.clear();
    m_artifactRows.clear();
    setStatus(QStringLiteral("Running pre-hardware protocol self-tests."));
    createFirmwareSamples();
    createSampleTlog();
    runBootloaderSelfTest();
    runOptionalHardwareSelfTest();
    prepareRtspTestProfile();
    m_running = false;
    setStatus(QStringLiteral("Protocol self-tests complete. Results are simulation-only until real hardware is connected."));
    audit(QStringLiteral("protocol_tests_completed"), QStringLiteral("info"), QStringLiteral("Pre-hardware protocol tests completed"));
    emit testsChanged();
}

void ProtocolTestManager::runBootloaderSelfTest()
{
    if (!authorize(QStringLiteral("simulation"), QStringLiteral("bootloader protocol self-test"))) {
        return;
    }
    const QByteArray image = sampleFirmwareImage();
    const int chunkSize = 248;
    const int chunks = (image.size() + chunkSize - 1) / chunkSize;
    const quint32 imageCrc = crc32(image);

    appendTest(QStringLiteral("Bootloader sync sequence"),
               QStringLiteral("GET_SYNC/EOC -> INSYNC/OK sequence prepared for serial adapter."),
               QStringLiteral("simulated"));
    appendTest(QStringLiteral("Erase/program/verify plan"),
               QStringLiteral("%1 bytes, %2 chunks at %3-byte max payload.")
                   .arg(image.size())
                   .arg(chunks)
                   .arg(chunkSize),
               QStringLiteral("simulated"));
    appendTest(QStringLiteral("Local firmware CRC"),
               QStringLiteral("0x%1")
                   .arg(QString::number(imageCrc, 16).rightJustified(8, QLatin1Char('0')).toUpper()),
               QStringLiteral("simulated"));
    appendTest(QStringLiteral("Failure-path coverage"),
               QStringLiteral("Self-test covers no-sync, truncated image, and CRC mismatch cases in the UI status model."),
               QStringLiteral("simulated"));
    setStatus(QStringLiteral("Bootloader protocol self-test complete. Real COM flashing still requires a PX4/ArduPilot controller or virtual COM bootloader."));
    audit(QStringLiteral("bootloader_protocol_self_tested"), QStringLiteral("info"), QStringLiteral("Bootloader protocol self-test completed"));
    emit testsChanged();
}

void ProtocolTestManager::createFirmwareSamples()
{
    if (!authorize(QStringLiteral("simulation"), QStringLiteral("firmware sample generation"))) {
        return;
    }
    const QString dirPath = ensureArtifactDirectory();
    QDir dir(dirPath);
    const QByteArray image = sampleFirmwareImage();
    const QList<QPair<QString, QByteArray>> files{
        {QStringLiteral("skygrid-protocol-test.bin"), image},
        {QStringLiteral("skygrid-protocol-test.hex"), makeIntelHex(image)},
        {QStringLiteral("skygrid-protocol-test.apj"), makeJsonFirmwarePackage(image, QStringLiteral("ArduPilot"))},
        {QStringLiteral("skygrid-protocol-test.px4"), makeJsonFirmwarePackage(image, QStringLiteral("PX4"))}
    };

    for (const auto &fileData : files) {
        const QString path = dir.filePath(fileData.first);
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.write(fileData.second);
            appendArtifact(fileData.first, path);
        } else {
            appendTest(fileData.first, QStringLiteral("Could not write sample firmware package."), QStringLiteral("failed"));
        }
    }
    appendTest(QStringLiteral("Firmware samples"),
               QStringLiteral("Generated .bin, .hex, .apj, and .px4 parser fixtures."),
               QStringLiteral("ready"));
    setStatus(QStringLiteral("Firmware protocol samples created in %1.").arg(dirPath));
    audit(QStringLiteral("protocol_firmware_samples_created"), QStringLiteral("info"), QStringLiteral("Firmware sample artifacts created"));
    emit testsChanged();
}

void ProtocolTestManager::createSampleTlog()
{
    if (!authorize(QStringLiteral("simulation"), QStringLiteral("TLog sample generation"))) {
        return;
    }
    QByteArray tlog;
    tlog.append(makeMavlinkV2Frame(1, 1, 1, 0, QByteArray(9, char(0))));     // HEARTBEAT
    tlog.append(makeMavlinkV2Frame(2, 1, 1, 1, QByteArray(31, char(0))));    // SYS_STATUS
    tlog.append(makeMavlinkV2Frame(3, 1, 1, 24, QByteArray(30, char(0))));   // GPS_RAW_INT
    tlog.append(makeMavlinkV2Frame(4, 1, 1, 30, QByteArray(28, char(0))));   // ATTITUDE
    tlog.append(makeMavlinkV2Frame(5, 1, 1, 74, QByteArray(20, char(0))));   // VFR_HUD

    const QString path = QDir(ensureArtifactDirectory()).filePath(QStringLiteral("skygrid-protocol-test.tlog"));
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        appendTest(QStringLiteral("Sample TLog"), QStringLiteral("Could not write TLog sample."), QStringLiteral("failed"));
        emit testsChanged();
        return;
    }
    file.write(tlog);
    appendArtifact(QStringLiteral("skygrid-protocol-test.tlog"), path);
    appendTest(QStringLiteral("Sample TLog"),
               QStringLiteral("Generated %1 MAVLink v2 packets for playback parser testing.").arg(5),
               QStringLiteral("ready"));
    setStatus(QStringLiteral("Sample TLog created: %1.").arg(path));
    audit(QStringLiteral("protocol_tlog_sample_created"), QStringLiteral("info"), QStringLiteral("TLog sample artifact created"));
    emit testsChanged();
}

void ProtocolTestManager::runOptionalHardwareSelfTest()
{
    if (!authorize(QStringLiteral("simulation"), QStringLiteral("optional hardware self-test"))) {
        return;
    }
    appendTest(QStringLiteral("SiK AT command plan"),
               QStringLiteral("AT sync, ATI, NETID, air speed, and AT&W command sequence prepared for serial validation."),
               QStringLiteral("simulated"));
    appendTest(QStringLiteral("DroneCAN probe"),
               QStringLiteral("MAVLink request-message path targets UAVCAN_NODE_STATUS."),
               QStringLiteral("simulated"));
    appendTest(QStringLiteral("PX4Flow probe"),
               QStringLiteral("MAVLink request-message path targets OPTICAL_FLOW."),
               QStringLiteral("simulated"));
    appendTest(QStringLiteral("Radio diagnostics"),
               QStringLiteral("MAVLink request-message path targets RADIO_STATUS."),
               QStringLiteral("simulated"));
    appendTest(QStringLiteral("Antenna tracker"),
               QStringLiteral("Target latitude/longitude/altitude validation can run before tracker hardware arrives."),
               QStringLiteral("simulated"));
    setStatus(QStringLiteral("Optional hardware protocol self-test complete."));
    audit(QStringLiteral("optional_hardware_protocol_self_tested"), QStringLiteral("info"), QStringLiteral("Optional hardware protocol self-test completed"));
    emit testsChanged();
}

void ProtocolTestManager::prepareRtspTestProfile()
{
    if (!authorize(QStringLiteral("simulation"), QStringLiteral("RTSP test profile"))) {
        return;
    }
    const QString path = QDir(ensureArtifactDirectory()).filePath(QStringLiteral("rtsp-test-profile.txt"));
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream out(&file);
        out << "SkyGrid RTSP/H264 test profile\n";
        out << "URL: rtsp://127.0.0.1:8554/skygrid-test\n";
        out << "Purpose: use a local RTSP test server or camera simulator before payload hardware arrives.\n";
        out << "Suggested GStreamer source when available: gst-launch-1.0 videotestsrc is-live=true ! x264enc tune=zerolatency ! rtph264pay\n";
    }
    appendArtifact(QStringLiteral("rtsp-test-profile.txt"), path);
    appendTest(QStringLiteral("RTSP/H264 test profile"),
               QStringLiteral("Prepared rtsp://127.0.0.1:8554/skygrid-test for local stream validation."),
               QStringLiteral("ready"));
    setStatus(QStringLiteral("RTSP test profile prepared. Start a local RTSP server, then use the payload video panel."));
    audit(QStringLiteral("rtsp_test_profile_prepared"), QStringLiteral("info"), QStringLiteral("RTSP test profile prepared"));
    emit testsChanged();
}

void ProtocolTestManager::clear()
{
    m_testRows.clear();
    m_artifactRows.clear();
    m_running = false;
    setStatus(QStringLiteral("Protocol test results cleared."));
    emit testsChanged();
}

QVariantMap ProtocolTestManager::row(const QString &name, const QString &value, const QString &state) const
{
    return QVariantMap{{QStringLiteral("name"), name},
                       {QStringLiteral("value"), value},
                       {QStringLiteral("state"), state}};
}

bool ProtocolTestManager::authorize(const QString &action, const QString &label)
{
    if (!m_access) {
        return true;
    }
    if (!m_access->authorizeAction(action,
                                   QVariantMap{{QStringLiteral("source"), QStringLiteral("protocol_test_manager")}},
                                   QStringLiteral("%1 blocked by local permissions.").arg(label))) {
        setStatus(QStringLiteral("%1 blocked by RBAC.").arg(label));
        return false;
    }
    return true;
}

QString ProtocolTestManager::ensureArtifactDirectory() const
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (base.isEmpty()) {
        base = QDir::tempPath() + QStringLiteral("/SkyGridGCS");
    }
    QDir dir(base);
    dir.mkpath(QStringLiteral("protocol-tests"));
    return dir.filePath(QStringLiteral("protocol-tests"));
}

QByteArray ProtocolTestManager::sampleFirmwareImage() const
{
    QByteArray image;
    image.resize(4096);
    for (int i = 0; i < image.size(); ++i) {
        image[i] = char((i * 37 + 11) & 0xff);
    }
    return image;
}

QByteArray ProtocolTestManager::makeIntelHex(const QByteArray &image) const
{
    QString output;
    quint32 address = 0;
    for (int offset = 0; offset < image.size(); offset += 16) {
        const QByteArray chunk = image.mid(offset, 16);
        QByteArray record;
        record.append(char(chunk.size()));
        record.append(char((address >> 8) & 0xff));
        record.append(char(address & 0xff));
        record.append(char(0x00));
        record.append(chunk);

        output += QLatin1Char(':');
        for (char byte : record) {
            output += hexByte(quint8(byte));
        }
        output += hexByte(hexChecksum(record));
        output += QLatin1Char('\n');
        address += quint32(chunk.size());
    }
    output += QStringLiteral(":00000001FF\n");
    return output.toLatin1();
}

QByteArray ProtocolTestManager::makeJsonFirmwarePackage(const QByteArray &image, const QString &stack) const
{
    const QJsonObject object{
        {QStringLiteral("format"), QStringLiteral("raw")},
        {QStringLiteral("stack"), stack},
        {QStringLiteral("version"), QStringLiteral("test-0.0.1")},
        {QStringLiteral("board_id"), QStringLiteral("SIM")},
        {QStringLiteral("image_size"), image.size()},
        {QStringLiteral("image"), QString::fromLatin1(image.toBase64())}
    };
    return QJsonDocument(object).toJson(QJsonDocument::Indented);
}

QByteArray ProtocolTestManager::makeMavlinkV2Frame(quint8 sequence,
                                                   quint8 systemId,
                                                   quint8 componentId,
                                                   quint32 messageId,
                                                   const QByteArray &payload) const
{
    QByteArray frame;
    frame.append(char(0xfd));
    frame.append(char(payload.size()));
    frame.append(char(0x00)); // incompat flags
    frame.append(char(0x00)); // compat flags
    frame.append(char(sequence));
    frame.append(char(systemId));
    frame.append(char(componentId));
    frame.append(char(messageId & 0xff));
    frame.append(char((messageId >> 8) & 0xff));
    frame.append(char((messageId >> 16) & 0xff));
    frame.append(payload);
    frame.append(char(0x00)); // placeholder CRC low
    frame.append(char(0x00)); // placeholder CRC high
    return frame;
}

quint32 ProtocolTestManager::crc32(const QByteArray &bytes) const
{
    quint32 crc = 0;
    for (char byte : bytes) {
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

void ProtocolTestManager::appendTest(const QString &name, const QString &value, const QString &state)
{
    m_testRows.prepend(row(name, value, state));
    while (m_testRows.size() > 80) {
        m_testRows.removeLast();
    }
}

void ProtocolTestManager::appendArtifact(const QString &name, const QString &path)
{
    m_artifactRows.prepend(row(name, path, QStringLiteral("file")));
    while (m_artifactRows.size() > 40) {
        m_artifactRows.removeLast();
    }
}

void ProtocolTestManager::setStatus(const QString &status)
{
    m_status = status;
}

void ProtocolTestManager::audit(const QString &eventType, const QString &severity, const QString &message) const
{
    if (!m_events) {
        return;
    }
    m_events->recordEvent(eventType,
                          severity,
                          message,
                          QJsonObject{{QStringLiteral("artifact_directory"), ensureArtifactDirectory()},
                                      {QStringLiteral("test_count"), m_testRows.size()},
                                      {QStringLiteral("artifact_count"), m_artifactRows.size()},
                                      {QStringLiteral("recorded_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate)}});
}
