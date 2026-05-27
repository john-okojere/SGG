#include "MapState.h"

#include <algorithm>

MapState::MapState(QObject *parent) : QObject(parent) {}

double MapState::centerLatitude() const { return m_centerLatitude; }
double MapState::centerLongitude() const { return m_centerLongitude; }
double MapState::zoomLevel() const { return m_zoomLevel; }
QString MapState::layerMode() const { return m_layerMode; }
bool MapState::followAircraft() const { return m_followAircraft; }
QString MapState::activeOverlayMode() const { return m_activeOverlayMode; }
int MapState::overlayRevision() const { return m_overlayRevision; }

void MapState::setCenterLatitude(double value)
{
    if (qFuzzyCompare(m_centerLatitude, value)) {
        return;
    }
    m_centerLatitude = value;
    emit centerChanged();
}

void MapState::setCenterLongitude(double value)
{
    if (qFuzzyCompare(m_centerLongitude, value)) {
        return;
    }
    m_centerLongitude = value;
    emit centerChanged();
}

void MapState::setZoomLevel(double value)
{
    const double next = std::clamp(value, 3.0, 20.0);
    if (qFuzzyCompare(m_zoomLevel, next)) {
        return;
    }
    m_zoomLevel = next;
    emit zoomChanged();
}

void MapState::setLayerMode(const QString &mode)
{
    if (m_layerMode == mode) {
        return;
    }
    m_layerMode = mode;
    emit layerChanged();
}

void MapState::setFollowAircraft(bool follow)
{
    if (m_followAircraft == follow) {
        return;
    }
    m_followAircraft = follow;
    emit followAircraftChanged();
}

void MapState::zoomIn() { setZoomLevel(m_zoomLevel + 0.8); }
void MapState::zoomOut() { setZoomLevel(m_zoomLevel - 0.8); }

void MapState::setOverlayMode(const QString &mode)
{
    if (m_activeOverlayMode == mode) {
        return;
    }
    m_activeOverlayMode = mode;
    refreshOverlay();
}

void MapState::refreshOverlay()
{
    ++m_overlayRevision;
    emit overlayChanged();
}
