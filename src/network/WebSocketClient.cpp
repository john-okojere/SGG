#include "WebSocketClient.h"

#include "../auth/TokenManager.h"
#include "../security/BackendTrustManager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QUrlQuery>
#include <QWebSocketProtocol>

WebSocketClient::WebSocketClient(BackendTrustManager *backend, TokenManager *tokens, QObject *parent)
    : QObject(parent), m_backend(backend), m_tokens(tokens)
{
    connect(&m_socket, &QWebSocket::connected, this, [this]() {
        setStatus(QStringLiteral("WebSocket telemetry online"), true);
    });
    connect(&m_socket, &QWebSocket::disconnected, this, [this]() {
        setStatus(QStringLiteral("WebSocket telemetry disconnected"), false);
    });
    connect(&m_socket, &QWebSocket::textMessageReceived, this, [this](const QString &message) {
        const QJsonObject body = QJsonDocument::fromJson(message.toUtf8()).object();
        const QString reason = body.value(QStringLiteral("reason")).toString();
        if (body.value(QStringLiteral("logout_required")).toBool(false)
            || reason.contains(QStringLiteral("device_revoked"), Qt::CaseInsensitive)
            || reason.contains(QStringLiteral("device_disabled"), Qt::CaseInsensitive)
            || reason.contains(QStringLiteral("device_not_trusted"), Qt::CaseInsensitive)) {
            if (m_tokens) {
                m_tokens->clear();
            }
            disconnectTelemetry();
            setStatus(QStringLiteral("WebSocket closed: device revoked or untrusted"), false);
        }
        emit messageReceived(message);
    });
    connect(&m_socket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error), this, [this](QAbstractSocket::SocketError) {
        setStatus(QStringLiteral("WebSocket error: %1").arg(m_socket.errorString()), false);
    });
}

bool WebSocketClient::connected() const { return m_connected; }
int WebSocketClient::liveStreamCount() const { return m_connectedStreams.size(); }
QString WebSocketClient::liveStreamSummary() const
{
    if (m_connectedStreams.size() >= 4) {
        return QStringLiteral("Live WS 4/4");
    }
    if (!m_connectedStreams.isEmpty()) {
        return QStringLiteral("Live WS %1/4").arg(m_connectedStreams.size());
    }
    return QStringLiteral("Fallback sync");
}
QString WebSocketClient::status() const { return m_status; }

void WebSocketClient::connectTelemetry()
{
    if (!m_backend || !m_tokens || !m_tokens->deviceTrusted()) {
        setStatus(QStringLiteral("WebSocket blocked until device approval."), false);
        return;
    }
    if (m_socket.state() == QAbstractSocket::ConnectedState || m_socket.state() == QAbstractSocket::ConnectingState) {
        return;
    }

    QUrl url(m_backend->makeWebSocketUrl(QStringLiteral("/ws/telemetry/")));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("access_token"), m_tokens->accessToken());
    query.addQueryItem(QStringLiteral("device_uuid"), m_tokens->deviceUuid());
    query.addQueryItem(QStringLiteral("device_trust_token"), m_tokens->deviceTrustToken());
    url.setQuery(query);

    m_socket.open(url);
    setStatus(QStringLiteral("Connecting telemetry WebSocket..."), false);
}

void WebSocketClient::connectLiveStreams()
{
    openStream(QStringLiteral("operations"), QStringLiteral("/ws/operations/"));
    openStream(QStringLiteral("flight_sessions"), QStringLiteral("/ws/flight-sessions/"));
    openStream(QStringLiteral("weather"), QStringLiteral("/ws/weather/"));
    openStream(QStringLiteral("missions"), QStringLiteral("/ws/missions/"));
}

void WebSocketClient::disconnectTelemetry()
{
    if (m_socket.state() != QAbstractSocket::UnconnectedState) {
        m_socket.close();
    }
    setStatus(QStringLiteral("WebSocket disconnected"), false);
}

void WebSocketClient::disconnectLiveStreams()
{
    const QStringList streams = m_streamSockets.keys();
    for (const QString &stream : streams) {
        closeStream(stream);
    }
    setStatus(QStringLiteral("Live backend streams disconnected"), false);
}

void WebSocketClient::sendTelemetryMessage(const QString &message)
{
    if (m_socket.state() == QAbstractSocket::ConnectedState) {
        m_socket.sendTextMessage(message);
        setStatus(QStringLiteral("WebSocket telemetry sent"), true);
        return;
    }
    setStatus(QStringLiteral("WebSocket unavailable; telemetry using HTTPS retry queue"), false);
}

void WebSocketClient::openStream(const QString &stream, const QString &path)
{
    if (!m_backend || !m_tokens || !m_tokens->deviceTrusted()) {
        setStatus(QStringLiteral("WebSocket blocked until device approval."), false);
        return;
    }
    QWebSocket *existing = m_streamSockets.value(stream, nullptr);
    if (existing && (existing->state() == QAbstractSocket::ConnectedState || existing->state() == QAbstractSocket::ConnectingState)) {
        return;
    }
    if (existing) {
        existing->deleteLater();
        m_streamSockets.remove(stream);
    }

    auto *socket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    m_streamSockets.insert(stream, socket);

    connect(socket, &QWebSocket::connected, this, [this, stream]() {
        m_connectedStreams.insert(stream);
        setStatus(QStringLiteral("%1 stream online").arg(stream), true);
    });
    connect(socket, &QWebSocket::disconnected, this, [this, stream, socket]() {
        if (m_streamSockets.value(stream) == socket) {
            m_streamSockets.remove(stream);
        }
        m_connectedStreams.remove(stream);
        socket->deleteLater();
        setStatus(QStringLiteral("%1 stream disconnected").arg(stream), m_socket.state() == QAbstractSocket::ConnectedState || !m_connectedStreams.isEmpty());
    });
    connect(socket, &QWebSocket::textMessageReceived, this, [this](const QString &message) {
        emit messageReceived(message);
    });
    connect(socket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error), this, [this, stream, socket](QAbstractSocket::SocketError) {
        setStatus(QStringLiteral("%1 stream error: %2").arg(stream, socket->errorString()), !m_streamSockets.isEmpty());
    });

    QUrl url(m_backend->makeWebSocketUrl(path));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("access_token"), m_tokens->accessToken());
    query.addQueryItem(QStringLiteral("device_uuid"), m_tokens->deviceUuid());
    query.addQueryItem(QStringLiteral("device_trust_token"), m_tokens->deviceTrustToken());
    url.setQuery(query);
    socket->open(url);
}

void WebSocketClient::closeStream(const QString &stream)
{
    QWebSocket *socket = m_streamSockets.take(stream);
    m_connectedStreams.remove(stream);
    if (!socket) {
        return;
    }
    socket->close();
    socket->deleteLater();
}

void WebSocketClient::setStatus(const QString &status, bool connected)
{
    if (m_status == status && m_connected == connected) {
        return;
    }
    m_status = status;
    m_connected = connected;
    emit stateChanged();
}
