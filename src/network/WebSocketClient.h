#pragma once

#include <QObject>
#include <QHash>
#include <QSet>
#include <QString>
#include <QWebSocket>

class BackendTrustManager;
class TokenManager;

class WebSocketClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY stateChanged)
    Q_PROPERTY(int liveStreamCount READ liveStreamCount NOTIFY stateChanged)
    Q_PROPERTY(QString liveStreamSummary READ liveStreamSummary NOTIFY stateChanged)
    Q_PROPERTY(QString status READ status NOTIFY stateChanged)

public:
    explicit WebSocketClient(BackendTrustManager *backend, TokenManager *tokens, QObject *parent = nullptr);

    bool connected() const;
    int liveStreamCount() const;
    QString liveStreamSummary() const;
    QString status() const;

    Q_INVOKABLE void connectTelemetry();
    Q_INVOKABLE void connectLiveStreams();
    Q_INVOKABLE void disconnectTelemetry();
    Q_INVOKABLE void disconnectLiveStreams();
    Q_INVOKABLE void sendTelemetryMessage(const QString &message);

signals:
    void stateChanged();
    void messageReceived(const QString &message);

private:
    void setStatus(const QString &status, bool connected);
    void openStream(const QString &stream, const QString &path);
    void closeStream(const QString &stream);

    BackendTrustManager *m_backend = nullptr;
    TokenManager *m_tokens = nullptr;
    QWebSocket m_socket;
    QHash<QString, QWebSocket *> m_streamSockets;
    QSet<QString> m_connectedStreams;
    bool m_connected = false;
    QString m_status = "WebSocket idle";
};
