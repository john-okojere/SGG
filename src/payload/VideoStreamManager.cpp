#include "VideoStreamManager.h"

#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"

#include <QJsonObject>
#include <QUrl>
#include <QVariantMap>

VideoStreamManager::VideoStreamManager(AccessManager *access,
                                       GcsEventSyncManager *events,
                                       QObject *parent)
    : QObject(parent),
      m_access(access),
      m_events(events)
{
}

QString VideoStreamManager::streamUrl() const { return m_streamUrl; }
QString VideoStreamManager::username() const { return m_username; }
QString VideoStreamManager::password() const { return m_password; }
QString VideoStreamManager::playbackUrl() const { return m_connected ? buildPlaybackUrl() : QString(); }
QString VideoStreamManager::latencyMode() const { return m_latencyMode; }
QString VideoStreamManager::protocol() const { return detectedProtocol(m_streamUrl); }
QString VideoStreamManager::status() const { return m_status; }
bool VideoStreamManager::connected() const { return m_connected; }
bool VideoStreamManager::recording() const { return m_recording; }
bool VideoStreamManager::hasSource() const { return !m_streamUrl.trimmed().isEmpty(); }

QVariantList VideoStreamManager::videoRows() const
{
    return {
        row(QStringLiteral("Protocol"), protocol().isEmpty() ? QStringLiteral("--") : protocol()),
        row(QStringLiteral("Stream"), hasSource() ? m_streamUrl : QStringLiteral("No RTSP/H264 source configured")),
        row(QStringLiteral("Latency"), m_latencyMode),
        row(QStringLiteral("Playback"), m_connected ? QStringLiteral("Connected to Qt Multimedia") : QStringLiteral("Disconnected")),
        row(QStringLiteral("Recording"), m_recording ? QStringLiteral("Metadata recording active") : QStringLiteral("Off")),
        row(QStringLiteral("Status"), m_status)
    };
}

void VideoStreamManager::setStreamUrl(const QString &url)
{
    const QString next = url.trimmed();
    if (m_streamUrl == next) {
        return;
    }
    m_streamUrl = next;
    if (m_connected) {
        m_connected = false;
        m_recording = false;
        setStatus(QStringLiteral("Video source changed. Reconnect stream."));
    }
    emit videoChanged();
}

void VideoStreamManager::setUsername(const QString &username)
{
    const QString next = username.trimmed();
    if (m_username == next) {
        return;
    }
    m_username = next;
    emit videoChanged();
}

void VideoStreamManager::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    emit videoChanged();
}

void VideoStreamManager::setLatencyMode(const QString &mode)
{
    const QString next = mode.trimmed().isEmpty() ? QStringLiteral("Low latency") : mode.trimmed();
    if (m_latencyMode == next) {
        return;
    }
    m_latencyMode = next;
    emit videoChanged();
}

void VideoStreamManager::configure(const QString &url,
                                   const QString &username,
                                   const QString &password,
                                   const QString &latencyMode)
{
    if (!authorize(QStringLiteral("video_payload_configuration"), QStringLiteral("video payload configuration"))) {
        return;
    }
    setStreamUrl(url);
    setUsername(username);
    setPassword(password);
    setLatencyMode(latencyMode);
    setStatus(QStringLiteral("Video profile saved locally. Connect to open RTSP/H264 preview."));
    audit(QStringLiteral("video_profile_configured"), QStringLiteral("info"), QStringLiteral("Video payload profile configured"));
}

void VideoStreamManager::connectStream()
{
    if (!authorize(QStringLiteral("video_stream"), QStringLiteral("video stream view"))) {
        return;
    }
    QString reason;
    if (!validateSource(&reason)) {
        m_connected = false;
        m_recording = false;
        setStatus(reason);
        emit videoChanged();
        return;
    }
    m_connected = true;
    setStatus(QStringLiteral("Opening RTSP/H264 stream with Qt Multimedia."));
    audit(QStringLiteral("video_stream_opened"), QStringLiteral("info"), QStringLiteral("RTSP/H264 video stream opened"));
    emit videoChanged();
}

void VideoStreamManager::disconnectStream()
{
    if (!m_connected && !m_recording) {
        return;
    }
    m_connected = false;
    m_recording = false;
    setStatus(QStringLiteral("Video stream disconnected."));
    audit(QStringLiteral("video_stream_disconnected"), QStringLiteral("info"), QStringLiteral("Video stream disconnected"));
    emit videoChanged();
}

void VideoStreamManager::startRecording()
{
    if (!authorize(QStringLiteral("video_payload_configuration"), QStringLiteral("video recording"))) {
        return;
    }
    if (!m_connected) {
        setStatus(QStringLiteral("Connect video before starting recording."));
        return;
    }
    m_recording = true;
    setStatus(QStringLiteral("Recording metadata active. File capture adapter is not enabled in this build."));
    audit(QStringLiteral("video_recording_requested"), QStringLiteral("warning"), QStringLiteral("Video recording requested without file capture adapter"));
    emit videoChanged();
}

void VideoStreamManager::stopRecording()
{
    if (!m_recording) {
        return;
    }
    m_recording = false;
    setStatus(QStringLiteral("Video recording stopped."));
    audit(QStringLiteral("video_recording_stopped"), QStringLiteral("info"), QStringLiteral("Video recording stopped"));
    emit videoChanged();
}

void VideoStreamManager::reportPlaybackError(const QString &error)
{
    m_connected = false;
    m_recording = false;
    setStatus(error.trimmed().isEmpty()
                  ? QStringLiteral("Video playback failed. Check stream URL, credentials, and H264 support.")
                  : QStringLiteral("Video playback failed: %1").arg(error.trimmed()));
    audit(QStringLiteral("video_playback_failed"), QStringLiteral("warning"), m_status);
    emit videoChanged();
}

void VideoStreamManager::reportPlaybackReady()
{
    if (!m_connected) {
        return;
    }
    setStatus(QStringLiteral("RTSP/H264 video stream is live."));
    emit videoChanged();
}

QVariantMap VideoStreamManager::row(const QString &name, const QString &value) const
{
    return QVariantMap{{QStringLiteral("name"), name}, {QStringLiteral("value"), value}};
}

bool VideoStreamManager::authorize(const QString &action, const QString &label)
{
    if (m_access && !m_access->authorizeAction(action,
                                                QVariantMap{{QStringLiteral("video_action"), label}},
                                                QStringLiteral("Video action blocked by local permissions."))) {
        setStatus(QStringLiteral("Video action blocked by RBAC."));
        return false;
    }
    return true;
}

bool VideoStreamManager::validateSource(QString *reason) const
{
    const QUrl url(m_streamUrl.trimmed());
    if (!url.isValid() || url.scheme().isEmpty() || url.host().isEmpty()) {
        if (reason) {
            *reason = QStringLiteral("Enter a valid RTSP URL such as rtsp://camera.local:8554/live.");
        }
        return false;
    }
    const QString scheme = url.scheme().toLower();
    if (scheme != QStringLiteral("rtsp") && scheme != QStringLiteral("rtsps")) {
        if (reason) {
            *reason = QStringLiteral("Only RTSP/H264 is enabled in this build. Use an rtsp:// or rtsps:// source.");
        }
        return false;
    }
    return true;
}

QString VideoStreamManager::buildPlaybackUrl() const
{
    QUrl url(m_streamUrl.trimmed());
    if (!m_username.isEmpty()) {
        url.setUserName(m_username);
        url.setPassword(m_password);
    }
    return url.toString(QUrl::FullyEncoded);
}

QString VideoStreamManager::detectedProtocol(const QString &url) const
{
    const QString scheme = QUrl(url.trimmed()).scheme().toLower();
    if (scheme == QStringLiteral("rtsp") || scheme == QStringLiteral("rtsps")) {
        return QStringLiteral("RTSP/H264");
    }
    if (scheme.isEmpty()) {
        return {};
    }
    return QStringLiteral("Unsupported: %1").arg(scheme.toUpper());
}

void VideoStreamManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit videoChanged();
}

void VideoStreamManager::audit(const QString &eventType,
                               const QString &severity,
                               const QString &message) const
{
    if (!m_events) {
        return;
    }
    m_events->recordEvent(eventType,
                          severity,
                          message,
                          QJsonObject{{QStringLiteral("protocol"), protocol()},
                                      {QStringLiteral("latency_mode"), m_latencyMode},
                                      {QStringLiteral("has_credentials"), !m_username.isEmpty()}});
}
