#include "BackendTrustManager.h"

#include <QProcessEnvironment>
#include <QUrl>
 
BackendTrustManager::BackendTrustManager(QObject *parent) : QObject(parent)
{
    const QString configured = QProcessEnvironment::systemEnvironment()
                                   .value(QStringLiteral("SKYGRID_BACKEND_URL"),
                                          QStringLiteral("http://127.0.0.1:8000"));
    m_baseUrl = normalizedBaseUrl(configured);
}

QString BackendTrustManager::baseUrl() const { return m_baseUrl; }

QString BackendTrustManager::websocketUrl() const
{
    QUrl url(m_baseUrl);
    url.setScheme(url.scheme() == QStringLiteral("https") ? QStringLiteral("wss") : QStringLiteral("ws"));
    QString value = url.toString();
    while (value.endsWith('/')) {
        value.chop(1);
    }
    return value;
}

bool BackendTrustManager::productionSecure() const
{
    const QUrl url(m_baseUrl);
    return url.host() == QStringLiteral("127.0.0.1")
        || url.host() == QStringLiteral("localhost")
        || url.scheme() == QStringLiteral("https");
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
    emit backendChanged();
}

QString BackendTrustManager::normalizedBaseUrl(const QString &value) const
{
    QString trimmed = value.trimmed();
    while (trimmed.endsWith('/')) {
        trimmed.chop(1);
    }
    return trimmed.isEmpty() ? QStringLiteral("http://127.0.0.1:8000") : trimmed;
}
