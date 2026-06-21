#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class AccessManager;
class GcsEventSyncManager;

class VideoStreamManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString streamUrl READ streamUrl WRITE setStreamUrl NOTIFY videoChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY videoChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY videoChanged)
    Q_PROPERTY(QString playbackUrl READ playbackUrl NOTIFY videoChanged)
    Q_PROPERTY(QString latencyMode READ latencyMode WRITE setLatencyMode NOTIFY videoChanged)
    Q_PROPERTY(QString protocol READ protocol NOTIFY videoChanged)
    Q_PROPERTY(QString status READ status NOTIFY videoChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY videoChanged)
    Q_PROPERTY(bool recording READ recording NOTIFY videoChanged)
    Q_PROPERTY(bool hasSource READ hasSource NOTIFY videoChanged)
    Q_PROPERTY(QVariantList videoRows READ videoRows NOTIFY videoChanged)

public:
    explicit VideoStreamManager(AccessManager *access,
                                GcsEventSyncManager *events,
                                QObject *parent = nullptr);

    QString streamUrl() const;
    QString username() const;
    QString password() const;
    QString playbackUrl() const;
    QString latencyMode() const;
    QString protocol() const;
    QString status() const;
    bool connected() const;
    bool recording() const;
    bool hasSource() const;
    QVariantList videoRows() const;

    void setStreamUrl(const QString &url);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setLatencyMode(const QString &mode);

    Q_INVOKABLE void configure(const QString &url,
                               const QString &username,
                               const QString &password,
                               const QString &latencyMode);
    Q_INVOKABLE void connectStream();
    Q_INVOKABLE void disconnectStream();
    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();
    Q_INVOKABLE void reportPlaybackError(const QString &error);
    Q_INVOKABLE void reportPlaybackReady();

signals:
    void videoChanged();

private:
    QVariantMap row(const QString &name, const QString &value) const;
    bool authorize(const QString &action, const QString &label);
    bool validateSource(QString *reason = nullptr) const;
    QString buildPlaybackUrl() const;
    QString detectedProtocol(const QString &url) const;
    void setStatus(const QString &status);
    void audit(const QString &eventType,
               const QString &severity,
               const QString &message) const;

    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QString m_streamUrl;
    QString m_username;
    QString m_password;
    QString m_latencyMode = "Low latency";
    QString m_status = "RTSP/H264 video idle.";
    bool m_connected = false;
    bool m_recording = false;
};
