#pragma once

#include <functional>

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>

class BackendTrustManager;
class TokenManager;

class ApiClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    using Callback = std::function<void(int statusCode, const QJsonObject &body, const QString &error)>;

    explicit ApiClient(BackendTrustManager *backend, TokenManager *tokens, QObject *parent = nullptr);

    bool busy() const;
    void get(const QString &path, bool auth, bool deviceTrust, Callback callback);
    void post(const QString &path, const QJsonObject &payload, bool auth, bool deviceTrust, Callback callback);
    void patch(const QString &path, const QJsonObject &payload, bool auth, bool deviceTrust, Callback callback);

signals:
    void busyChanged();
    void securityFailure(const QString &message);

private:
    void send(const QString &method,
              const QString &path,
              const QJsonObject &payload,
              bool auth,
              bool deviceTrust,
              bool retryOnUnauthorized,
              Callback callback);
    QNetworkRequest requestFor(const QString &path, bool auth, bool deviceTrust) const;
    void refreshAccessToken(std::function<void(bool)> done);
    bool isDeviceSecurityFailure(int status, const QJsonObject &body) const;
    void setBusyDelta(int delta);

    BackendTrustManager *m_backend = nullptr;
    TokenManager *m_tokens = nullptr;
    QNetworkAccessManager m_network;
    int m_pending = 0;
};
