#include "HomePositionManager.h"

#include "VehicleTelemetryModel.h"

#include <QtMath>

#include <algorithm>

HomePositionManager::HomePositionManager(VehicleTelemetryModel *telemetry, QObject *parent)
    : QObject(parent), m_telemetry(telemetry)
{
}

bool HomePositionManager::hasHome() const { return m_hasHome; }
double HomePositionManager::homeLatitude() const { return m_homeLatitude; }
double HomePositionManager::homeLongitude() const { return m_homeLongitude; }
QString HomePositionManager::source() const { return m_source; }
bool HomePositionManager::locked() const { return m_locked; }

void HomePositionManager::setLocked(bool locked)
{
    if (m_locked == locked) {
        return;
    }
    m_locked = locked;
    emit homeChanged();
}

void HomePositionManager::setHome(double latitude, double longitude, const QString &source)
{
    if (!validCoordinate(latitude, longitude)) {
        return;
    }
    const double clampedLat = std::clamp(latitude, -90.0, 90.0);
    const double clampedLon = std::clamp(longitude, -180.0, 180.0);
    if (m_hasHome
        && qFuzzyCompare(m_homeLatitude, clampedLat)
        && qFuzzyCompare(m_homeLongitude, clampedLon)
        && m_source == source) {
        return;
    }
    m_hasHome = true;
    m_homeLatitude = clampedLat;
    m_homeLongitude = clampedLon;
    m_source = source;
    m_locked = true;
    emit homeChanged();
}

void HomePositionManager::clearHome()
{
    if (!m_hasHome) {
        return;
    }
    m_hasHome = false;
    m_homeLatitude = 0.0;
    m_homeLongitude = 0.0;
    m_source.clear();
    emit homeChanged();
}

bool HomePositionManager::updateHomeToCurrentTelemetry(const QString &source)
{
    if (!m_telemetry || !m_telemetry->connected()) {
        return false;
    }
    if (!validCoordinate(m_telemetry->latitude(), m_telemetry->longitude())) {
        return false;
    }
    setHome(m_telemetry->latitude(), m_telemetry->longitude(), source);
    return true;
}

double HomePositionManager::distanceFromHomeMeters(double latitude, double longitude) const
{
    if (!m_hasHome || !validCoordinate(latitude, longitude)) {
        return 0.0;
    }
    const double phiA = qDegreesToRadians(m_homeLatitude);
    const double phiB = qDegreesToRadians(latitude);
    const double dPhi = qDegreesToRadians(latitude - m_homeLatitude);
    const double dLambda = qDegreesToRadians(longitude - m_homeLongitude);
    const double a = qSin(dPhi / 2.0) * qSin(dPhi / 2.0)
        + qCos(phiA) * qCos(phiB) * qSin(dLambda / 2.0) * qSin(dLambda / 2.0);
    return 6371000.0 * 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));
}

bool HomePositionManager::validCoordinate(double latitude, double longitude) const
{
    return std::abs(latitude) <= 90.0
        && std::abs(longitude) <= 180.0
        && (std::abs(latitude) > 0.000001 || std::abs(longitude) > 0.000001);
}
