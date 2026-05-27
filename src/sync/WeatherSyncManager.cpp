#include "WeatherSyncManager.h"

#include "../auth/SessionManager.h"
#include "../cache/LocalSyncCache.h"
#include "../network/ApiClient.h"
#include "../telemetry/WeatherManager.h"

#include <QJsonObject>
#include <QProcessEnvironment>
#include <QUrlQuery>
#include <QtGlobal>

WeatherSyncManager::WeatherSyncManager(ApiClient *api,
                                       SessionManager *session,
                                       WeatherManager *weather,
                                       LocalSyncCache *cache,
                                       QObject *parent)
    : QObject(parent), m_api(api), m_session(session), m_weather(weather), m_cache(cache)
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    m_minRefreshIntervalMs = qBound(2000,
                                    env.value(QStringLiteral("SKYGRID_WEATHER_CHECK_INTERVAL_MS"),
                                              env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true")
                                                  ? QStringLiteral("5000")
                                                  : QStringLiteral("8000")).toInt(),
                                    60000);
}

QString WeatherSyncManager::status() const { return m_status; }
bool WeatherSyncManager::syncing() const { return m_syncing; }

void WeatherSyncManager::refreshForCoordinate(double latitude, double longitude)
{
    refreshForMission(latitude, longitude, 0.0, QString(), QStringLiteral("MISSION"));
}

void WeatherSyncManager::refreshForMission(double latitude,
                                           double longitude,
                                           double altitudeMeters,
                                           const QString &aircraftId,
                                           const QString &mode)
{
    const QDateTime now = QDateTime::currentDateTimeUtc();
    if (m_lastRefreshAt.isValid() && m_lastRefreshAt.msecsTo(now) < m_minRefreshIntervalMs) {
        setStatus(QStringLiteral("Weather check recently updated."));
        return;
    }
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        setSyncing(false);
        setStatus(QStringLiteral("Weather check blocked: trusted Control Center session required"));
        loadCachedWeather(QStringLiteral("Trusted session required for weather check"));
        return;
    }

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("lat"), QString::number(latitude, 'f', 7));
    query.addQueryItem(QStringLiteral("lon"), QString::number(longitude, 'f', 7));
    query.addQueryItem(QStringLiteral("altitude_m"), QString::number(altitudeMeters, 'f', 2));
    query.addQueryItem(QStringLiteral("mode"), mode.isEmpty() ? QStringLiteral("MISSION") : mode);
    if (!aircraftId.isEmpty()) {
        query.addQueryItem(QStringLiteral("aircraft_id"), aircraftId);
    }

    setSyncing(true);
    m_lastRefreshAt = now;
    setStatus(QStringLiteral("Checking mission weather envelope..."));
    const QString path = QStringLiteral("/api/weather/check/?%1").arg(query.toString(QUrl::FullyEncoded));
    m_api->get(path, true, true, [this](int statusCode, const QJsonObject &body, const QString &error) {
        setSyncing(false);
        if (statusCode < 200 || statusCode >= 300) {
            const QString reason = error.isEmpty() ? QStringLiteral("Weather source unavailable") : error;
            setStatus(reason);
            loadCachedWeather(reason);
            return;
        }

        const QVariantMap weather = body.value(QStringLiteral("weather")).isObject()
            ? body.value(QStringLiteral("weather")).toObject().toVariantMap()
            : body.toVariantMap();
        if (m_weather) {
            m_weather->applyWeather(weather);
        }
        if (m_cache) {
            m_cache->saveObject(QStringLiteral("weather"), QStringLiteral("latest"), weather);
        }
        setStatus(QStringLiteral("Weather check synchronized."));
    });
}

void WeatherSyncManager::loadCachedWeather(const QString &fallbackReason)
{
    const QVariantMap cached = m_cache ? m_cache->loadObject(QStringLiteral("weather"), QStringLiteral("latest")) : QVariantMap{};
    if (!cached.isEmpty() && m_weather) {
        m_weather->applyWeather(cached);
        setStatus(QStringLiteral("%1; using cached weather.").arg(fallbackReason));
        return;
    }
    if (m_weather) {
        m_weather->markUnavailable(fallbackReason);
    }
}

void WeatherSyncManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit weatherSyncChanged();
}

void WeatherSyncManager::setSyncing(bool syncing)
{
    if (m_syncing == syncing) {
        return;
    }
    m_syncing = syncing;
    emit weatherSyncChanged();
}
