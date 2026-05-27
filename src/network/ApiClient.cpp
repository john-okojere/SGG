#include "ApiClient.h"

#include "../auth/TokenManager.h"
#include "../security/BackendTrustManager.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcessEnvironment>
#include <QDebug>

ApiClient::ApiClient(BackendTrustManager *backend, TokenManager *tokens, QObject *parent)
    : QObject(parent), m_backend(backend), m_tokens(tokens)
{
}

bool ApiClient::busy() const { return m_pending > 0; }

void ApiClient::get(const QString &path, bool auth, bool deviceTrust, Callback callback)
{
    send(QStringLiteral("GET"), path, {}, auth, deviceTrust, true, std::move(callback));
}

void ApiClient::post(const QString &path, const QJsonObject &payload, bool auth, bool deviceTrust, Callback callback)
{
    send(QStringLiteral("POST"), path, payload, auth, deviceTrust, true, std::move(callback));
}

void ApiClient::patch(const QString &path, const QJsonObject &payload, bool auth, bool deviceTrust, Callback callback)
{
    send(QStringLiteral("PATCH"), path, payload, auth, deviceTrust, true, std::move(callback));
}

void ApiClient::send(const QString &method,
                     const QString &path,
                     const QJsonObject &payload,
                     bool auth,
                     bool deviceTrust,
                     bool retryOnUnauthorized,
                     Callback callback)
{
    QNetworkRequest request = requestFor(path, auth, deviceTrust);
    QNetworkReply *reply = nullptr;
    const QByteArray data = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    setBusyDelta(1);

    if (method == QStringLiteral("GET")) {
        reply = m_network.get(request);
    } else if (method == QStringLiteral("PATCH")) {
        reply = m_network.sendCustomRequest(request, QByteArray("PATCH"), data);
    } else {
        reply = m_network.post(request, data);
    }

    connect(reply, &QNetworkReply::finished, this, [=, cb = std::move(callback)]() mutable {
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QByteArray raw = reply->readAll();
        const QJsonDocument doc = QJsonDocument::fromJson(raw);
        QJsonObject body = doc.isObject() ? doc.object() : QJsonObject{};
        if (doc.isArray()) {
            body.insert(QStringLiteral("results"), doc.array());
        }
        const QString error = reply->error() == QNetworkReply::NoError
            ? QString()
            : reply->errorString();
        reply->deleteLater();
        setBusyDelta(-1);

        if (QProcessEnvironment::systemEnvironment().value(QStringLiteral("SKYGRID_VERBOSE_COMMS")) == QStringLiteral("true")) {
            qInfo().noquote()
                << "SkyGrid API"
                << method
                << path
                << "status=" << status
                << "bytes=" << raw.size();
        }

        if (status == 400) {
            qWarning().noquote()
                << "SkyGrid API 400"
                << "endpoint=" << path
                << "method=" << method
                << "request=" << QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact))
                << "response=" << QString::fromUtf8(raw);
        }

        if (status == 401 && retryOnUnauthorized && m_tokens && !m_tokens->refreshToken().isEmpty()) {
            refreshAccessToken([=, cb = std::move(cb)](bool ok) mutable {
                if (ok) {
                    send(method, path, payload, auth, deviceTrust, false, std::move(cb));
                } else {
                    emit securityFailure(QStringLiteral("Session expired. Please sign in again."));
                    cb(status, body, QStringLiteral("Token refresh failed."));
                }
            });
            return;
        }
        if (isDeviceSecurityFailure(status, body)) {
            if (m_tokens) {
                m_tokens->clear();
            }
            emit securityFailure(QStringLiteral("This GCS device has been revoked or disabled by SkyGrid Control Center."));
        }
        cb(status, body, error);
    });
}

QNetworkRequest ApiClient::requestFor(const QString &path, bool auth, bool deviceTrust) const
{
    QNetworkRequest request(QUrl(m_backend->makeUrl(path)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Accept", "application/json");
    if (auth && m_tokens && !m_tokens->accessToken().isEmpty()) {
        request.setRawHeader("Authorization", QByteArray("Bearer ") + m_tokens->accessToken().toUtf8());
    }
    if (deviceTrust && m_tokens) {
        request.setRawHeader("X-Device-UUID", m_tokens->deviceUuid().toUtf8());
        request.setRawHeader("X-Device-Trust-Token", m_tokens->deviceTrustToken().toUtf8());
    }
    return request;
}

void ApiClient::refreshAccessToken(std::function<void(bool)> done)
{
    if (!m_tokens || m_tokens->refreshToken().isEmpty()) {
        done(false);
        return;
    }
    QJsonObject payload{{QStringLiteral("refresh"), m_tokens->refreshToken()}};
    QNetworkRequest request = requestFor(QStringLiteral("/api/auth/refresh/"), false, false);
    setBusyDelta(1);
    auto *reply = m_network.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply, done = std::move(done)]() mutable {
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QJsonObject body = QJsonDocument::fromJson(reply->readAll()).object();
        reply->deleteLater();
        setBusyDelta(-1);
        const QString access = body.value(QStringLiteral("access")).toString();
        if (status >= 200 && status < 300 && !access.isEmpty()) {
            m_tokens->updateAccessToken(access);
            done(true);
            return;
        }
        m_tokens->clear();
        done(false);
    });
}

bool ApiClient::isDeviceSecurityFailure(int status, const QJsonObject &body) const
{
    if (status != 403) {
        return false;
    }
    const QString reason = body.value(QStringLiteral("reason")).toString().toLower();
    const QString detail = body.value(QStringLiteral("detail")).toString().toLower();
    const QString message = body.value(QStringLiteral("message")).toString().toLower();
    const QString text = reason + QStringLiteral(" ") + detail + QStringLiteral(" ") + message;
    static const QStringList hardLogoutReasons{
        QStringLiteral("device_revoked"),
        QStringLiteral("device_disabled"),
        QStringLiteral("device_blocked"),
        QStringLiteral("device_suspicious"),
        QStringLiteral("force_logout_required"),
        QStringLiteral("device_not_trusted")
    };
    if (body.value(QStringLiteral("logout_required")).toBool(false)
        || body.value(QStringLiteral("force_logout")).toBool(false)) {
        return true;
    }
    for (const QString &needle : hardLogoutReasons) {
        if (text.contains(needle)) {
            return true;
        }
    }
    return false;
}

void ApiClient::setBusyDelta(int delta)
{
    const bool wasBusy = busy();
    m_pending = qMax(0, m_pending + delta);
    if (wasBusy != busy()) {
        emit busyChanged();
    }
}
