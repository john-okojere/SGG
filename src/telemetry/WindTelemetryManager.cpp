#include "WindTelemetryManager.h"

#include <QtMath>

WindTelemetryManager::WindTelemetryManager(QObject *parent) : QObject(parent) {}

bool WindTelemetryManager::available() const { return m_available; }
double WindTelemetryManager::windSpeed() const { return m_windSpeed; }
double WindTelemetryManager::windDirection() const { return m_windDirection; }
double WindTelemetryManager::gust() const { return m_gust; }
QString WindTelemetryManager::directionLabel() const { return m_available ? directionFor(m_windDirection) : QStringLiteral("--"); }
QString WindTelemetryManager::status() const { return m_available ? QStringLiteral("Wind telemetry available") : QStringLiteral("Wind unavailable"); }

void WindTelemetryManager::clear()
{
    m_available = false;
    m_windSpeed = 0.0;
    m_windDirection = 0.0;
    m_gust = 0.0;
    emit windChanged();
}

void WindTelemetryManager::setDevelopmentWind(double speedMps, double directionDeg, double gustMps)
{
    m_available = true;
    m_windSpeed = qMax(0.0, speedMps);
    m_windDirection = std::fmod(std::fmod(directionDeg, 360.0) + 360.0, 360.0);
    m_gust = qMax(m_windSpeed, gustMps);
    emit windChanged();
}

QString WindTelemetryManager::directionFor(double degrees)
{
    static const QStringList labels{
        QStringLiteral("N"), QStringLiteral("NNE"), QStringLiteral("NE"), QStringLiteral("ENE"),
        QStringLiteral("E"), QStringLiteral("ESE"), QStringLiteral("SE"), QStringLiteral("SSE"),
        QStringLiteral("S"), QStringLiteral("SSW"), QStringLiteral("SW"), QStringLiteral("WSW"),
        QStringLiteral("W"), QStringLiteral("WNW"), QStringLiteral("NW"), QStringLiteral("NNW")
    };
    const int index = qRound(degrees / 22.5) % labels.size();
    return labels.at(index);
}
