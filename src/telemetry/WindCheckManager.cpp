#include "WindCheckManager.h"

#include "WeatherManager.h"
#include "WindTelemetryManager.h"

WindCheckManager::WindCheckManager(WeatherManager *weather,
                                   WindTelemetryManager *wind,
                                   QObject *parent)
    : QObject(parent), m_weather(weather), m_wind(wind)
{
    if (m_weather) {
        connect(m_weather, &WeatherManager::weatherChanged, this, &WindCheckManager::evaluate);
    }
    if (m_wind) {
        connect(m_wind, &WindTelemetryManager::windChanged, this, &WindCheckManager::evaluate);
    }
    evaluate();
}

QString WindCheckManager::result() const { return m_result; }
QString WindCheckManager::status() const { return m_status; }
QString WindCheckManager::severity() const { return m_severity; }

void WindCheckManager::evaluate()
{
    const bool liveWind = m_wind && m_wind->available();
    const bool weatherReady = m_weather && m_weather->available();
    if (!liveWind && !weatherReady) {
        m_result = QStringLiteral("Source unavailable");
        m_status = QStringLiteral("No live wind or forecast source connected");
        m_severity = QStringLiteral("info");
        emit windCheckChanged();
        return;
    }

    const QString controlCenterStatus = weatherReady ? m_weather->status().toLower() : QString();
    if (controlCenterStatus.contains(QStringLiteral("do_not_fly"))
        || controlCenterStatus.contains(QStringLiteral("do not fly"))) {
        m_result = QStringLiteral("Do Not Fly");
        m_status = m_weather->status();
        m_severity = QStringLiteral("critical");
        emit windCheckChanged();
        return;
    }
    if (controlCenterStatus.contains(QStringLiteral("caution"))) {
        m_result = QStringLiteral("Caution");
        m_status = m_weather->status();
        m_severity = QStringLiteral("warning");
        emit windCheckChanged();
        return;
    }
    if (controlCenterStatus.contains(QStringLiteral("good_to_fly"))
        || controlCenterStatus.contains(QStringLiteral("good to fly"))) {
        m_result = QStringLiteral("Good to Fly");
        m_status = m_weather->status();
        m_severity = QStringLiteral("info");
        emit windCheckChanged();
        return;
    }

    const double windSpeed = liveWind ? m_wind->windSpeed() : m_weather->windSpeedMps();
    const double gust = liveWind ? m_wind->gust() : m_weather->windGustMps();
    if (windSpeed >= 12.0 || gust >= 16.0) {
        m_result = QStringLiteral("Do Not Fly");
        m_status = QStringLiteral("Wind or gust exceeds configured safety envelope");
        m_severity = QStringLiteral("critical");
    } else if (windSpeed >= 8.0 || gust >= 11.0) {
        m_result = QStringLiteral("Caution");
        m_status = QStringLiteral("Elevated wind. Review aircraft limits before takeoff");
        m_severity = QStringLiteral("warning");
    } else {
        m_result = QStringLiteral("Good to Fly");
        m_status = QStringLiteral("Wind conditions inside configured envelope");
        m_severity = QStringLiteral("info");
    }
    emit windCheckChanged();
}
