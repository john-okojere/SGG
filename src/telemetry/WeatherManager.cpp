#include "WeatherManager.h"

WeatherManager::WeatherManager(QObject *parent) : QObject(parent) {}

bool WeatherManager::available() const { return m_available; }
QString WeatherManager::sourceState() const { return m_sourceState; }
QString WeatherManager::status() const { return m_status; }
double WeatherManager::windSpeedMps() const { return m_windSpeedMps; }
double WeatherManager::windDirectionDeg() const { return m_windDirectionDeg; }
double WeatherManager::windGustMps() const { return m_windGustMps; }
double WeatherManager::visibilityKm() const { return m_visibilityKm; }
double WeatherManager::precipitationProbability() const { return m_precipitationProbability; }
double WeatherManager::cloudCover() const { return m_cloudCover; }
double WeatherManager::temperatureC() const { return m_temperatureC; }

void WeatherManager::markUnavailable(const QString &reason)
{
    m_available = false;
    m_sourceState = QStringLiteral("unavailable");
    m_status = reason.isEmpty() ? QStringLiteral("Waiting for weather source") : reason;
    emit weatherChanged();
}

void WeatherManager::applyWeather(const QVariantMap &weather)
{
    const QVariantMap wind = weather.value(QStringLiteral("wind")).toMap();
    const QVariantMap conditions = weather.value(QStringLiteral("conditions")).toMap();
    const QVariantMap current = weather.value(QStringLiteral("current")).toMap();
    const QVariantMap assessment = weather.value(QStringLiteral("assessment")).toMap();

    m_available = true;
    m_sourceState = weather.value(QStringLiteral("source_state"),
                                  weather.value(QStringLiteral("source"),
                                                QStringLiteral("control_center"))).toString();
    m_status = assessment.value(QStringLiteral("status"),
                                weather.value(QStringLiteral("status"),
                                              weather.value(QStringLiteral("result"),
                                                            weather.value(QStringLiteral("flight_status"),
                                                                          QStringLiteral("Weather check received"))))).toString();
    m_windSpeedMps = weather.value(QStringLiteral("wind_speed_mps"),
                                   current.value(QStringLiteral("wind_speed_mps"),
                                   wind.value(QStringLiteral("speed_mps"),
                                              wind.value(QStringLiteral("speed"), 0.0)))).toDouble();
    m_windDirectionDeg = weather.value(QStringLiteral("wind_direction_deg"),
                                       current.value(QStringLiteral("wind_direction_deg"),
                                       wind.value(QStringLiteral("direction_deg"),
                                                  wind.value(QStringLiteral("direction"), 0.0)))).toDouble();
    m_windGustMps = weather.value(QStringLiteral("wind_gust_mps"),
                                  current.value(QStringLiteral("gust_speed_mps"),
                                  wind.value(QStringLiteral("gust_mps"),
                                             wind.value(QStringLiteral("gust"), 0.0)))).toDouble();
    const QVariant visibilityValue = weather.value(QStringLiteral("visibility_km"),
                                     current.value(QStringLiteral("visibility_km"),
                                     current.value(QStringLiteral("visibility_m"),
                                     conditions.value(QStringLiteral("visibility_km"),
                                     conditions.value(QStringLiteral("visibility"), 0.0)))));
    m_visibilityKm = visibilityValue.toDouble();
    if (current.contains(QStringLiteral("visibility_m")) && !current.contains(QStringLiteral("visibility_km"))) {
        m_visibilityKm /= 1000.0;
    }
    m_precipitationProbability = weather.value(QStringLiteral("precipitation_probability"),
                                               current.value(QStringLiteral("precip_probability_percent"),
                                               weather.value(QStringLiteral("precip_probability"),
                                                             conditions.value(QStringLiteral("precipitation_probability"), 0.0)))).toDouble();
    m_cloudCover = weather.value(QStringLiteral("cloud_cover"),
                                 current.value(QStringLiteral("cloud_cover_percent"),
                                               conditions.value(QStringLiteral("cloud_cover"), 0.0))).toDouble();
    m_temperatureC = weather.value(QStringLiteral("temperature_c"),
                                   current.value(QStringLiteral("temperature_c"),
                                   conditions.value(QStringLiteral("temperature_c"),
                                                    conditions.value(QStringLiteral("temperature"), 0.0)))).toDouble();
    emit weatherChanged();
}
