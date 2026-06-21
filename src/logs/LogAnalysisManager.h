#pragma once

#include <QFile>
#include <QObject>
#include <QVariantList>

#include <memory>

namespace mavsdk {
class MavlinkPassthrough;
}

class AccessManager;
class EventLogManager;
class GcsEventSyncManager;
class MavsdkVehicleManager;
class SessionManager;

class LogAnalysisManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList logEntries READ logEntries NOTIFY logsChanged)
    Q_PROPERTY(QVariantList replayEvents READ replayEvents NOTIFY logsChanged)
    Q_PROPERTY(QVariantList graphFields READ graphFields NOTIFY logsChanged)
    Q_PROPERTY(QVariantList graphRows READ graphRows NOTIFY logsChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY logsChanged)
    Q_PROPERTY(bool tlogRecording READ tlogRecording NOTIFY logsChanged)
    Q_PROPERTY(int downloadProgress READ downloadProgress NOTIFY logsChanged)
    Q_PROPERTY(QString tlogPath READ tlogPath NOTIFY logsChanged)
    Q_PROPERTY(QString status READ status NOTIFY logsChanged)

public:
    explicit LogAnalysisManager(MavsdkVehicleManager *vehicle,
                                AccessManager *access,
                                SessionManager *session,
                                GcsEventSyncManager *events,
                                EventLogManager *eventLog,
                                QObject *parent = nullptr);

    QVariantList logEntries() const;
    QVariantList replayEvents() const;
    QVariantList graphFields() const;
    QVariantList graphRows() const;
    bool downloading() const;
    bool tlogRecording() const;
    int downloadProgress() const;
    QString tlogPath() const;
    QString status() const;

    Q_INVOKABLE void listOnboardLogs();
    Q_INVOKABLE void downloadLog(int id, const QString &pathOrUrl);
    Q_INVOKABLE void playbackLocalEvents();
    Q_INVOKABLE void startTlogRecording();
    Q_INVOKABLE void stopTlogRecording();
    Q_INVOKABLE void playbackTlogFile(const QString &pathOrUrl);
    Q_INVOKABLE void exportKml(const QString &pathOrUrl);
    Q_INVOKABLE void exportGpx(const QString &pathOrUrl);
    Q_INVOKABLE void extractWarnings();

signals:
    void logsChanged();

private:
    QString normalizePath(const QString &pathOrUrl) const;
    void appendTlogPacket(const QByteArray &packet);
    void rebuildGraphRows();
    void setStatus(const QString &status);
    void setDownloading(bool downloading);
    void setDownloadProgress(int progress);

    MavsdkVehicleManager *m_vehicle = nullptr;
    AccessManager *m_access = nullptr;
    SessionManager *m_session = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    EventLogManager *m_eventLog = nullptr;
    QVariantList m_logEntries;
    QVariantList m_replayEvents;
    QVariantList m_graphFields;
    QVariantList m_graphRows;
    bool m_downloading = false;
    bool m_tlogRecording = false;
    int m_downloadProgress = 0;
    QString m_tlogPath;
    QFile m_tlogFile;
    std::shared_ptr<mavsdk::MavlinkPassthrough> m_tlogPassthrough;
    QString m_status = "Logs / analysis idle";
};
