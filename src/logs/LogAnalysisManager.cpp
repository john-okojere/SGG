#include "LogAnalysisManager.h"

#include "../auth/SessionManager.h"
#include "../flight/EventLogManager.h"
#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"

#include <mavsdk/plugins/log_files/log_files.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>
#include <QStandardPaths>
#include <QTextStream>
#include <QUrl>
#include <QVariantMap>

#include <algorithm>
#include <array>
#include <memory>
#include <thread>
#include <vector>

namespace {
QString logResultString(mavsdk::LogFiles::Result result)
{
    switch (result) {
    case mavsdk::LogFiles::Result::Success: return QStringLiteral("Success");
    case mavsdk::LogFiles::Result::Next: return QStringLiteral("Progress");
    case mavsdk::LogFiles::Result::NoLogfiles: return QStringLiteral("No log files");
    case mavsdk::LogFiles::Result::Timeout: return QStringLiteral("Timeout");
    case mavsdk::LogFiles::Result::InvalidArgument: return QStringLiteral("Invalid argument");
    case mavsdk::LogFiles::Result::FileOpenFailed: return QStringLiteral("File open failed");
    case mavsdk::LogFiles::Result::NoSystem: return QStringLiteral("No system");
    default: return QStringLiteral("Log operation failed");
    }
}

struct TrackPoint {
    double latitude = 0.0;
    double longitude = 0.0;
    double altitude = 0.0;
    QString timestamp;
    QString label;
};

bool numberFromKeys(const QVariantMap &map, const QStringList &keys, double *value)
{
    for (const QString &key : keys) {
        const QVariant candidate = map.value(key);
        if (candidate.isValid() && !candidate.toString().trimmed().isEmpty()) {
            bool ok = false;
            const double parsed = candidate.toDouble(&ok);
            if (ok) {
                *value = parsed;
                return true;
            }
        }
    }
    return false;
}

bool trackPointFromMap(const QVariantMap &map, TrackPoint *point)
{
    double latitude = 0.0;
    double longitude = 0.0;
    const bool hasLatitude = numberFromKeys(map,
                                            {QStringLiteral("latitude"),
                                             QStringLiteral("lat"),
                                             QStringLiteral("latitude_deg"),
                                             QStringLiteral("lat_deg")},
                                            &latitude);
    const bool hasLongitude = numberFromKeys(map,
                                             {QStringLiteral("longitude"),
                                              QStringLiteral("lon"),
                                              QStringLiteral("lng"),
                                              QStringLiteral("longitude_deg"),
                                              QStringLiteral("lon_deg")},
                                             &longitude);
    if (hasLatitude && hasLongitude) {
        point->latitude = latitude;
        point->longitude = longitude;
        numberFromKeys(map,
                       {QStringLiteral("altitude"),
                        QStringLiteral("alt"),
                        QStringLiteral("relative_altitude"),
                        QStringLiteral("altitude_m")},
                       &point->altitude);
        point->timestamp = map.value(QStringLiteral("recorded_at"),
                                     map.value(QStringLiteral("timestamp"))).toString();
        point->label = map.value(QStringLiteral("message"),
                                 map.value(QStringLiteral("event_type"))).toString();
        return true;
    }

    const QVariant payload = map.value(QStringLiteral("payload"));
    if (payload.canConvert<QVariantMap>() && trackPointFromMap(payload.toMap(), point)) {
        if (point->timestamp.isEmpty()) {
            point->timestamp = map.value(QStringLiteral("recorded_at")).toString();
        }
        if (point->label.isEmpty()) {
            point->label = map.value(QStringLiteral("message"),
                                     map.value(QStringLiteral("event_type"))).toString();
        }
        return true;
    }
    return false;
}

std::vector<TrackPoint> collectTrackPoints(const QVariantList &events)
{
    std::vector<TrackPoint> points;
    for (const QVariant &event : events) {
        TrackPoint point;
        if (trackPointFromMap(event.toMap(), &point)) {
            points.push_back(point);
        }
    }
    return points;
}

QVariantMap numericSummary(const QVariantList &events, const QString &field)
{
    bool found = false;
    double minimum = 0.0;
    double maximum = 0.0;
    double last = 0.0;
    int count = 0;
    for (const QVariant &event : events) {
        QVariant value = event.toMap().value(field);
        if (!value.isValid()) {
            value = event.toMap().value(QStringLiteral("payload")).toMap().value(field);
        }
        if (!value.isValid() || value.toString().trimmed().isEmpty()) {
            continue;
        }
        bool ok = false;
        const double number = value.toDouble(&ok);
        if (!ok) {
            continue;
        }
        if (!found) {
            minimum = maximum = number;
            found = true;
        } else {
            minimum = std::min(minimum, number);
            maximum = std::max(maximum, number);
        }
        last = number;
        ++count;
    }
    return QVariantMap{{QStringLiteral("name"), field},
                       {QStringLiteral("value"), found
                            ? QStringLiteral("last %1, min %2, max %3 (%4 samples)")
                                  .arg(QString::number(last, 'f', 2),
                                       QString::number(minimum, 'f', 2),
                                       QString::number(maximum, 'f', 2),
                                       QString::number(count))
                            : QStringLiteral("No numeric samples")}};
}

QVariantList tlogSummaryRows(const QByteArray &bytes)
{
    int mavlink1 = 0;
    int mavlink2 = 0;
    int malformed = 0;
    int i = 0;
    while (i < bytes.size()) {
        const quint8 magic = quint8(bytes.at(i));
        if (magic == 0xFE && i + 8 <= bytes.size()) {
            const int payloadLen = quint8(bytes.at(i + 1));
            const int frameLen = 8 + payloadLen;
            if (i + frameLen <= bytes.size()) {
                ++mavlink1;
                i += frameLen;
                continue;
            }
        } else if (magic == 0xFD && i + 12 <= bytes.size()) {
            const int payloadLen = quint8(bytes.at(i + 1));
            const bool signedPacket = (quint8(bytes.at(i + 2)) & 0x01) != 0;
            const int frameLen = 12 + payloadLen + (signedPacket ? 13 : 0);
            if (i + frameLen <= bytes.size()) {
                ++mavlink2;
                i += frameLen;
                continue;
            }
        }
        ++malformed;
        ++i;
    }
    return QVariantList{
        QVariantMap{{QStringLiteral("name"), QStringLiteral("MAVLink v1 packets")}, {QStringLiteral("value"), QString::number(mavlink1)}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("MAVLink v2 packets")}, {QStringLiteral("value"), QString::number(mavlink2)}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Skipped bytes")}, {QStringLiteral("value"), QString::number(malformed)}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Bytes")}, {QStringLiteral("value"), QString::number(bytes.size())}}
    };
}
}

LogAnalysisManager::LogAnalysisManager(MavsdkVehicleManager *vehicle,
                                       AccessManager *access,
                                       SessionManager *session,
                                       GcsEventSyncManager *events,
                                       EventLogManager *eventLog,
                                       QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_access(access),
      m_session(session),
      m_events(events),
      m_eventLog(eventLog)
{
    m_graphFields = {QStringLiteral("altitude"), QStringLiteral("speed"), QStringLiteral("battery"), QStringLiteral("satellites"), QStringLiteral("link_quality")};
}

QVariantList LogAnalysisManager::logEntries() const { return m_logEntries; }
QVariantList LogAnalysisManager::replayEvents() const { return m_replayEvents; }
QVariantList LogAnalysisManager::graphFields() const { return m_graphFields; }
QVariantList LogAnalysisManager::graphRows() const { return m_graphRows; }
bool LogAnalysisManager::downloading() const { return m_downloading; }
bool LogAnalysisManager::tlogRecording() const { return m_tlogRecording; }
int LogAnalysisManager::downloadProgress() const { return m_downloadProgress; }
QString LogAnalysisManager::tlogPath() const { return m_tlogPath; }
QString LogAnalysisManager::status() const { return m_status; }

void LogAnalysisManager::listOnboardLogs()
{
    if (m_access && !m_access->authorizeAction(QStringLiteral("logs_analysis"),
                                                {},
                                                QStringLiteral("Log listing blocked by local permissions."))) {
        setStatus(QStringLiteral("Log listing blocked by RBAC."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a flight controller before listing onboard logs."));
        return;
    }
    setStatus(QStringLiteral("Requesting onboard log list."));

    const auto system = m_vehicle->system();
    QPointer<LogAnalysisManager> self(this);
    std::thread([self, system]() {
        mavsdk::LogFiles logFiles(system);
        const auto [result, entries] = logFiles.get_entries();
        QVariantList rows;
        for (const mavsdk::LogFiles::Entry &entry : entries) {
            rows << QVariantMap{{QStringLiteral("id"), int(entry.id)},
                                {QStringLiteral("date"), QString::fromStdString(entry.date)},
                                {QStringLiteral("sizeBytes"), int(entry.size_bytes)},
                                {QStringLiteral("size"), QStringLiteral("%1 KB").arg(QString::number(entry.size_bytes / 1024.0, 'f', 1))}};
        }
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, rows]() {
            if (!self) {
                return;
            }
            self->m_logEntries = rows;
            self->setStatus(result == mavsdk::LogFiles::Result::Success
                                ? QStringLiteral("Loaded %1 onboard logs.").arg(rows.size())
                                : QStringLiteral("Onboard log list unavailable: %1").arg(logResultString(result)));
            emit self->logsChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void LogAnalysisManager::downloadLog(int id, const QString &pathOrUrl)
{
    if (m_access && !m_access->authorizeAction(QStringLiteral("log_download"),
                                                QVariantMap{{QStringLiteral("log_id"), id}},
                                                QStringLiteral("Log download blocked by local permissions."))) {
        setStatus(QStringLiteral("Log download blocked by RBAC."));
        return;
    }
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Log download blocked: trusted session required."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a flight controller before downloading logs."));
        return;
    }

    QVariantMap selected;
    for (const QVariant &entry : m_logEntries) {
        const QVariantMap row = entry.toMap();
        if (row.value(QStringLiteral("id")).toInt() == id) {
            selected = row;
            break;
        }
    }
    if (selected.isEmpty()) {
        setStatus(QStringLiteral("Select a listed onboard log before downloading."));
        return;
    }

    const QString path = normalizePath(pathOrUrl);
    if (path.isEmpty()) {
        setStatus(QStringLiteral("Choose a download path for the log file."));
        return;
    }

    auto logFiles = std::make_shared<mavsdk::LogFiles>(m_vehicle->system());
    mavsdk::LogFiles::Entry entry{};
    entry.id = uint32_t(id);
    entry.date = selected.value(QStringLiteral("date")).toString().toStdString();
    entry.size_bytes = uint32_t(selected.value(QStringLiteral("sizeBytes")).toInt());

    setDownloading(true);
    setDownloadProgress(0);
    setStatus(QStringLiteral("Downloading onboard log %1.").arg(id));

    QPointer<LogAnalysisManager> self(this);
    logFiles->download_log_file_async(entry, path.toStdString(), [self, logFiles, id](mavsdk::LogFiles::Result result,
                                                                                     mavsdk::LogFiles::ProgressData progress) {
        Q_UNUSED(logFiles)
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, progress, id]() {
            if (!self) {
                return;
            }
            if (result == mavsdk::LogFiles::Result::Next) {
                self->setDownloadProgress(qBound(0, int(progress.progress * 100.0f), 100));
                return;
            }
            self->setDownloading(false);
            self->setDownloadProgress(result == mavsdk::LogFiles::Result::Success ? 100 : self->m_downloadProgress);
            self->setStatus(result == mavsdk::LogFiles::Result::Success
                                ? QStringLiteral("Downloaded onboard log %1.").arg(id)
                                : QStringLiteral("Log download failed: %1").arg(logResultString(result)));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("log_download_completed"),
                                            result == mavsdk::LogFiles::Result::Success ? QStringLiteral("info") : QStringLiteral("warning"),
                                            QStringLiteral("Onboard log download completed"),
                                            QJsonObject{{QStringLiteral("log_id"), id},
                                                        {QStringLiteral("result"), logResultString(result)}});
            }
        }, Qt::QueuedConnection);
    });
}

void LogAnalysisManager::playbackLocalEvents()
{
    if (!m_eventLog) {
        setStatus(QStringLiteral("Local event log is unavailable."));
        return;
    }
    m_replayEvents = m_eventLog->events();
    rebuildGraphRows();
    setStatus(QStringLiteral("Loaded %1 local GCS events for replay.").arg(m_replayEvents.size()));
    emit logsChanged();
}

void LogAnalysisManager::startTlogRecording()
{
    if (m_tlogRecording) {
        setStatus(QStringLiteral("TLog recording already active: %1").arg(m_tlogPath));
        return;
    }
    if (m_access && !m_access->authorizeAction(QStringLiteral("logs_analysis"),
                                                {},
                                                QStringLiteral("TLog recording blocked by local permissions."))) {
        setStatus(QStringLiteral("TLog recording blocked by RBAC."));
        return;
    }
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("TLog recording blocked: trusted session required."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a flight controller before TLog recording."));
        return;
    }
    QString root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (root.isEmpty()) {
        root = QDir::currentPath();
    }
    QDir dir(root);
    dir.mkpath(QStringLiteral("logs"));
    m_tlogPath = dir.filePath(QStringLiteral("logs/skygrid-%1.tlog")
                                  .arg(QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyyMMdd-HHmmss"))));
    m_tlogFile.setFileName(m_tlogPath);
    if (!m_tlogFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        setStatus(QStringLiteral("Could not open TLog file for writing."));
        return;
    }

    m_tlogPassthrough = std::make_shared<mavsdk::MavlinkPassthrough>(m_vehicle->system());
    const std::array<int, 12> messageIds{
        MAVLINK_MSG_ID_HEARTBEAT,
        MAVLINK_MSG_ID_SYS_STATUS,
        MAVLINK_MSG_ID_GPS_RAW_INT,
        MAVLINK_MSG_ID_GLOBAL_POSITION_INT,
        MAVLINK_MSG_ID_ATTITUDE,
        MAVLINK_MSG_ID_VFR_HUD,
        MAVLINK_MSG_ID_BATTERY_STATUS,
        MAVLINK_MSG_ID_RC_CHANNELS,
        MAVLINK_MSG_ID_SERVO_OUTPUT_RAW,
        MAVLINK_MSG_ID_STATUSTEXT,
        MAVLINK_MSG_ID_RADIO_STATUS,
        MAVLINK_MSG_ID_OPTICAL_FLOW
    };
    QPointer<LogAnalysisManager> self(this);
    for (int messageId : messageIds) {
        m_tlogPassthrough->subscribe_message(uint16_t(messageId), [self](const mavlink_message_t &message) {
            uint8_t buffer[MAVLINK_MAX_PACKET_LEN]{};
            const uint16_t len = mavlink_msg_to_send_buffer(buffer, &message);
            QByteArray packet(reinterpret_cast<const char *>(buffer), int(len));
            if (!self) {
                return;
            }
            QMetaObject::invokeMethod(self, [self, packet]() {
                if (self) {
                    self->appendTlogPacket(packet);
                }
            }, Qt::QueuedConnection);
        });
    }
    m_tlogRecording = true;
    setStatus(QStringLiteral("TLog recording active: %1").arg(m_tlogPath));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("tlog_recording_started"),
                              QStringLiteral("info"),
                              QStringLiteral("TLog recording started"),
                              QJsonObject{{QStringLiteral("path"), m_tlogPath}});
    }
    emit logsChanged();
}

void LogAnalysisManager::stopTlogRecording()
{
    if (!m_tlogRecording) {
        setStatus(QStringLiteral("TLog recording is not active."));
        return;
    }
    m_tlogRecording = false;
    m_tlogPassthrough.reset();
    if (m_tlogFile.isOpen()) {
        m_tlogFile.flush();
        m_tlogFile.close();
    }
    setStatus(QStringLiteral("TLog recording stopped: %1").arg(m_tlogPath));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("tlog_recording_stopped"),
                              QStringLiteral("info"),
                              QStringLiteral("TLog recording stopped"),
                              QJsonObject{{QStringLiteral("path"), m_tlogPath}});
    }
    emit logsChanged();
}

void LogAnalysisManager::playbackTlogFile(const QString &pathOrUrl)
{
    const QString path = normalizePath(pathOrUrl);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        setStatus(QStringLiteral("Could not open TLog file for playback."));
        return;
    }
    const QByteArray bytes = file.readAll();
    m_replayEvents = tlogSummaryRows(bytes);
    m_graphRows = {
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Packets")}, {QStringLiteral("value"), QString::number(m_replayEvents.size())}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Source")}, {QStringLiteral("value"), path}}
    };
    setStatus(QStringLiteral("Loaded TLog playback summary from %1.").arg(path));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("tlog_playback_loaded"),
                              QStringLiteral("info"),
                              QStringLiteral("TLog playback summary loaded"),
                              QJsonObject{{QStringLiteral("path"), path},
                                          {QStringLiteral("bytes"), bytes.size()}});
    }
    emit logsChanged();
}

void LogAnalysisManager::exportKml(const QString &pathOrUrl)
{
    if (m_replayEvents.isEmpty()) {
        setStatus(QStringLiteral("Load local replay/events before exporting KML."));
        return;
    }
    const std::vector<TrackPoint> points = collectTrackPoints(m_replayEvents);
    if (points.empty()) {
        setStatus(QStringLiteral("KML export skipped: replay events do not contain latitude/longitude data."));
        return;
    }
    const QString path = normalizePath(pathOrUrl);
    if (path.isEmpty()) {
        setStatus(QStringLiteral("Choose a KML export path."));
        return;
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        setStatus(QStringLiteral("Could not open KML export file."));
        return;
    }
    QTextStream out(&file);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
    out << "  <Document>\n";
    out << "    <name>SkyGrid GCS Replay</name>\n";
    out << "    <Placemark>\n";
    out << "      <name>Replay Track</name>\n";
    out << "      <LineString><tessellate>1</tessellate><coordinates>\n";
    for (const TrackPoint &point : points) {
        out << "        " << QString::number(point.longitude, 'f', 7)
            << "," << QString::number(point.latitude, 'f', 7)
            << "," << QString::number(point.altitude, 'f', 2) << "\n";
    }
    out << "      </coordinates></LineString>\n";
    out << "    </Placemark>\n";
    out << "  </Document>\n";
    out << "</kml>\n";
    setStatus(QStringLiteral("Exported %1 replay points to KML.").arg(int(points.size())));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("log_kml_exported"),
                              QStringLiteral("info"),
                              QStringLiteral("Replay KML exported"),
                              QJsonObject{{QStringLiteral("points"), int(points.size())}});
    }
}

void LogAnalysisManager::exportGpx(const QString &pathOrUrl)
{
    if (m_replayEvents.isEmpty()) {
        setStatus(QStringLiteral("Load local replay/events before exporting GPX."));
        return;
    }
    const std::vector<TrackPoint> points = collectTrackPoints(m_replayEvents);
    if (points.empty()) {
        setStatus(QStringLiteral("GPX export skipped: replay events do not contain latitude/longitude data."));
        return;
    }
    const QString path = normalizePath(pathOrUrl);
    if (path.isEmpty()) {
        setStatus(QStringLiteral("Choose a GPX export path."));
        return;
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        setStatus(QStringLiteral("Could not open GPX export file."));
        return;
    }
    QTextStream out(&file);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<gpx version=\"1.1\" creator=\"SkyGrid GCS\" xmlns=\"http://www.topografix.com/GPX/1/1\">\n";
    out << "  <trk><name>SkyGrid GCS Replay</name><trkseg>\n";
    for (const TrackPoint &point : points) {
        out << "    <trkpt lat=\"" << QString::number(point.latitude, 'f', 7)
            << "\" lon=\"" << QString::number(point.longitude, 'f', 7) << "\">\n";
        out << "      <ele>" << QString::number(point.altitude, 'f', 2) << "</ele>\n";
        if (!point.timestamp.isEmpty()) {
            out << "      <time>" << point.timestamp.toHtmlEscaped() << "</time>\n";
        }
        out << "    </trkpt>\n";
    }
    out << "  </trkseg></trk>\n";
    out << "</gpx>\n";
    setStatus(QStringLiteral("Exported %1 replay points to GPX.").arg(int(points.size())));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("log_gpx_exported"),
                              QStringLiteral("info"),
                              QStringLiteral("Replay GPX exported"),
                              QJsonObject{{QStringLiteral("points"), int(points.size())}});
    }
}

void LogAnalysisManager::extractWarnings()
{
    if (!m_eventLog) {
        setStatus(QStringLiteral("No local events available for warning extraction."));
        return;
    }
    QVariantList warnings;
    for (const QVariant &entry : m_eventLog->events()) {
        const QVariantMap row = entry.toMap();
        const QString severity = row.value(QStringLiteral("severity")).toString().toLower();
        if (severity == QStringLiteral("warning") || severity == QStringLiteral("error") || severity == QStringLiteral("critical")) {
            warnings << row;
        }
    }
    m_replayEvents = warnings;
    rebuildGraphRows();
    setStatus(QStringLiteral("Extracted %1 warning/error events.").arg(warnings.size()));
    emit logsChanged();
}

QString LogAnalysisManager::normalizePath(const QString &pathOrUrl) const
{
    const QUrl url(pathOrUrl);
    if (url.isValid() && url.isLocalFile()) {
        return url.toLocalFile();
    }
    QString path = pathOrUrl;
    if (path.startsWith(QStringLiteral("file:///"))) {
        path = QUrl(path).toLocalFile();
    }
    return QFileInfo(path.trimmed()).absoluteFilePath();
}

void LogAnalysisManager::appendTlogPacket(const QByteArray &packet)
{
    if (!m_tlogRecording || !m_tlogFile.isOpen() || packet.isEmpty()) {
        return;
    }
    m_tlogFile.write(packet);
}

void LogAnalysisManager::rebuildGraphRows()
{
    QVariantList rows;
    for (const QVariant &field : m_graphFields) {
        rows << numericSummary(m_replayEvents, field.toString());
    }
    const std::vector<TrackPoint> points = collectTrackPoints(m_replayEvents);
    rows << QVariantMap{{QStringLiteral("name"), QStringLiteral("Coordinate points")},
                        {QStringLiteral("value"), QString::number(int(points.size()))}};
    m_graphRows = rows;
}

void LogAnalysisManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit logsChanged();
}

void LogAnalysisManager::setDownloading(bool downloading)
{
    if (m_downloading == downloading) {
        return;
    }
    m_downloading = downloading;
    emit logsChanged();
}

void LogAnalysisManager::setDownloadProgress(int progress)
{
    const int next = qBound(0, progress, 100);
    if (m_downloadProgress == next) {
        return;
    }
    m_downloadProgress = next;
    emit logsChanged();
}
