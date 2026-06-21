#include "BackendTrustManager.h"

#include <QProcessEnvironment>
#include <QUrl>
 
BackendTrustManager::BackendTrustManager(QObject *parent) : QObject(parent)
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString configuredApi = env.value(QStringLiteral("SKYGRID_API_BASE_URL"),
                                            env.value(QStringLiteral("SKYGRID_BACKEND_URL"),
                                                      defaultBaseUrl()));
    m_baseUrl = normalizedBaseUrl(configuredApi);
    const QString configuredWs = env.value(QStringLiteral("SKYGRID_WS_BASE_URL"),
                                           env.value(QStringLiteral("SKYGRID_WEBSOCKET_URL")));
    m_webSocketUrl = normalizedBaseUrl(configuredWs.isEmpty() ? websocketUrlFromApiUrl(m_baseUrl) : configuredWs);
}

QString BackendTrustManager::baseUrl() const { return m_baseUrl; }

QString BackendTrustManager::websocketUrl() const
{
    return m_webSocketUrl;
}

bool BackendTrustManager::productionSecure() const
{
    const QUrl url(m_baseUrl);
    return url.scheme() == QStringLiteral("https")
        && (url.host() == QStringLiteral("sgg-api.up.railway.app")
            || !url.host().isEmpty());
}

QString BackendTrustManager::makeUrl(const QString &path) const
{
    const QString cleanPath = path.startsWith('/') ? path : QStringLiteral("/") + path;
    return m_baseUrl + cleanPath;
}

QString BackendTrustManager::makeWebSocketUrl(const QString &pathWithQuery) const
{
    const QString cleanPath = pathWithQuery.startsWith('/') ? pathWithQuery : QStringLiteral("/") + pathWithQuery;
    return websocketUrl() + cleanPath;
}

void BackendTrustManager::setBaseUrl(const QString &baseUrl)
{
    const QString next = normalizedBaseUrl(baseUrl);
    if (m_baseUrl == next) {
        return;
    }
    m_baseUrl = next;
    if (QProcessEnvironment::systemEnvironment().value(QStringLiteral("SKYGRID_WS_BASE_URL")).trimmed().isEmpty()
        && QProcessEnvironment::systemEnvironment().value(QStringLiteral("SKYGRID_WEBSOCKET_URL")).trimmed().isEmpty()) {
        m_webSocketUrl = normalizedBaseUrl(websocketUrlFromApiUrl(m_baseUrl));
    }
    emit backendChanged();
}

void BackendTrustManager::setWebSocketUrl(const QString &webSocketUrl)
{
    const QString next = normalizedBaseUrl(webSocketUrl);
    if (m_webSocketUrl == next) {
        return;
    }
    m_webSocketUrl = next;
    emit backendChanged();
}

QString BackendTrustManager::normalizedBaseUrl(const QString &value) const
{
    QString trimmed = value.trimmed();
    while (trimmed.endsWith('/')) {
        trimmed.chop(1);
    }
    if (trimmed.isEmpty()) {
        return defaultBaseUrl();
    }
    QUrl url(trimmed);
    if (url.scheme().isEmpty()) {
        url.setScheme(QStringLiteral("https"));
    }
    const bool localhost = url.host() == QStringLiteral("localhost")
        || url.host() == QStringLiteral("127.0.0.1")
        || url.host() == QStringLiteral("::1");
    const bool allowInsecure = QProcessEnvironment::systemEnvironment()
                                   .value(QStringLiteral("SKYGRID_ALLOW_INSECURE_BACKEND"))
                                   .trimmed()
                                   .toLower()
                               == QStringLiteral("true");
    if (url.scheme() != QStringLiteral("https") && !localhost && !allowInsecure) {
        return QStringLiteral("https://sgg-api.up.railway.app");
    }
    QString normalized = url.toString();
    while (normalized.endsWith('/')) {
        normalized.chop(1);
    }
    return normalized;
}

QString BackendTrustManager::defaultBaseUrl() const
{
    return QProcessEnvironment::systemEnvironment().value(QStringLiteral("DEV_BUILD")).toLower() == QStringLiteral("true")
        ? QStringLiteral("http://localhost:8000")
        : QStringLiteral("https://sgg-api.up.railway.app");
}

QString BackendTrustManager::websocketUrlFromApiUrl(const QString &apiUrl) const
{
    QUrl url(apiUrl);
    url.setScheme(url.scheme() == QStringLiteral("https") ? QStringLiteral("wss") : QStringLiteral("ws"));
    QString value = url.toString();
    while (value.endsWith('/')) {
        value.chop(1);
    }
    return value;
}
