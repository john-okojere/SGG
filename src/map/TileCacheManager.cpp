#include "TileCacheManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>
#include <QtMath>

namespace {
constexpr int kTileSize = 256;

QString baseTileUrl(const QString &layer)
{
    if (layer == QStringLiteral("2D")) {
        return QStringLiteral("https://server.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer/tile/");
    }
    if (layer == QStringLiteral("3D")) {
        return QStringLiteral("https://server.arcgisonline.com/ArcGIS/rest/services/World_Terrain_Base/MapServer/tile/");
    }
    return QStringLiteral("https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/");
}
}

TileCacheManager::TileCacheManager(QObject *parent)
    : QObject(parent)
{
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    m_cacheRoot = base + QStringLiteral("/map_tiles");
    QDir().mkpath(m_cacheRoot);
}

bool TileCacheManager::downloading() const { return !m_inFlight.isEmpty(); }
bool TileCacheManager::offlineMode() const { return m_offlineMode; }
int TileCacheManager::queuedTiles() const { return m_queuedTiles; }
int TileCacheManager::downloadedTiles() const { return m_downloadedTiles; }
int TileCacheManager::cacheRevision() const { return m_cacheRevision; }
QString TileCacheManager::status() const { return m_status; }
QString TileCacheManager::cacheRoot() const { return m_cacheRoot; }

void TileCacheManager::setOfflineMode(bool value)
{
    if (m_offlineMode == value) {
        return;
    }
    m_offlineMode = value;
    setStatus(value ? QStringLiteral("Offline map mode: cached tiles only") : QStringLiteral("Online map mode: caching active"));
    emit cacheChanged();
}

QString TileCacheManager::tileSource(const QString &layer,
                                     int zoom,
                                     int tileX,
                                     int tileY,
                                     const QString &remoteUrl,
                                     int)
{
    const QString normalized = normalizedLayer(layer);
    const int count = 1 << qBound(0, zoom, 22);
    if (tileY < 0 || tileY >= count) {
        return QString();
    }
    const int wrappedX = wrappedTileX(tileX, zoom);
    const QString path = tilePath(normalized, zoom, wrappedX, tileY);
    if (QFile::exists(path)) {
        return QUrl::fromLocalFile(path).toString();
    }
    if (!m_offlineMode && !remoteUrl.isEmpty()) {
        queueTile(normalized, zoom, wrappedX, tileY, remoteUrl);
        return remoteUrl;
    }
    return QString();
}

void TileCacheManager::downloadMissionArea(const QVariantList &geometry,
                                           const QString &layer,
                                           double centerLatitude,
                                           double centerLongitude,
                                           int minZoom,
                                           int maxZoom)
{
    double minLat = centerLatitude - 0.04;
    double maxLat = centerLatitude + 0.04;
    double minLon = centerLongitude - 0.04;
    double maxLon = centerLongitude + 0.04;

    if (!geometry.isEmpty()) {
        minLat = 90.0;
        maxLat = -90.0;
        minLon = 180.0;
        maxLon = -180.0;
        for (const QVariant &item : geometry) {
            const QVariantMap point = item.toMap();
            const double lat = point.value(QStringLiteral("latitude")).toDouble();
            const double lon = point.value(QStringLiteral("longitude")).toDouble();
            minLat = qMin(minLat, lat);
            maxLat = qMax(maxLat, lat);
            minLon = qMin(minLon, lon);
            maxLon = qMax(maxLon, lon);
        }
        const double latPad = qMax(0.01, (maxLat - minLat) * 0.18);
        const double lonPad = qMax(0.01, (maxLon - minLon) * 0.18);
        minLat -= latPad;
        maxLat += latPad;
        minLon -= lonPad;
        maxLon += lonPad;
    }

    minZoom = qBound(3, minZoom, 19);
    maxZoom = qBound(minZoom, maxZoom, 19);
    const QString normalized = normalizedLayer(layer);
    int queued = 0;
    constexpr int maxTilesPerRequest = 1400;

    for (int zoom = minZoom; zoom <= maxZoom && queued < maxTilesPerRequest; ++zoom) {
        const int minX = qFloor(longitudeToTileX(minLon, zoom));
        const int maxX = qFloor(longitudeToTileX(maxLon, zoom));
        const int minY = qFloor(latitudeToTileY(maxLat, zoom));
        const int maxY = qFloor(latitudeToTileY(minLat, zoom));
        const int count = 1 << zoom;
        for (int y = qBound(0, minY, count - 1); y <= qBound(0, maxY, count - 1) && queued < maxTilesPerRequest; ++y) {
            for (int x = minX; x <= maxX && queued < maxTilesPerRequest; ++x) {
                const int wrappedX = wrappedTileX(x, zoom);
                if (QFile::exists(tilePath(normalized, zoom, wrappedX, y))) {
                    continue;
                }
                queueTile(normalized, zoom, wrappedX, y, tileUrlFor(normalized, zoom, wrappedX, y));
                ++queued;
            }
        }
    }

    setStatus(queued > 0
                  ? QStringLiteral("Downloading %1 mission-area map tiles").arg(queued)
                  : QStringLiteral("Mission-area map tiles already cached"));
}

void TileCacheManager::clearCache()
{
    QDir dir(m_cacheRoot);
    dir.removeRecursively();
    QDir().mkpath(m_cacheRoot);
    ++m_cacheRevision;
    setStatus(QStringLiteral("Map tile cache cleared"));
    emit cacheChanged();
}

QString TileCacheManager::normalizedLayer(const QString &layer) const
{
    if (layer == QStringLiteral("2D") || layer == QStringLiteral("3D")) {
        return layer;
    }
    return QStringLiteral("SAT");
}

QString TileCacheManager::tilePath(const QString &layer, int zoom, int tileX, int tileY) const
{
    return QStringLiteral("%1/%2/%3/%4/%5.jpg")
        .arg(m_cacheRoot, normalizedLayer(layer), QString::number(zoom), QString::number(tileX), QString::number(tileY));
}

QString TileCacheManager::tileUrlFor(const QString &layer, int zoom, int tileX, int tileY) const
{
    return baseTileUrl(normalizedLayer(layer)) + QString::number(zoom) + QStringLiteral("/") + QString::number(tileY) + QStringLiteral("/") + QString::number(wrappedTileX(tileX, zoom));
}

void TileCacheManager::queueTile(const QString &layer, int zoom, int tileX, int tileY, const QString &remoteUrl)
{
    const QString path = tilePath(layer, zoom, tileX, tileY);
    if (QFile::exists(path) || m_inFlight.contains(path)) {
        return;
    }
    m_inFlight.insert(path);
    ++m_queuedTiles;
    emit cacheChanged();

    QNetworkRequest request((QUrl(remoteUrl)));
    request.setRawHeader("User-Agent", "DigiSkyGrid-GCS/0.1 map-cache");
    QNetworkReply *reply = m_network.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, path]() {
        const QByteArray data = reply->readAll();
        const bool ok = reply->error() == QNetworkReply::NoError && !data.isEmpty();
        reply->deleteLater();

        if (ok) {
            QDir().mkpath(QFileInfo(path).absolutePath());
            QFile file(path);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
                ++m_downloadedTiles;
                ++m_cacheRevision;
            }
        }
        m_inFlight.remove(path);
        setStatus(ok
                      ? QStringLiteral("Map cache updated: %1 tiles").arg(m_downloadedTiles)
                      : QStringLiteral("Map tile download failed; using online fallback"));
        emit cacheChanged();
    });
}

void TileCacheManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
}

int TileCacheManager::wrappedTileX(int tileX, int zoom)
{
    const int count = 1 << qBound(0, zoom, 22);
    return ((tileX % count) + count) % count;
}

double TileCacheManager::longitudeToTileX(double longitude, int zoom)
{
    const double count = static_cast<double>(1 << qBound(0, zoom, 22));
    return ((longitude + 180.0) / 360.0) * count;
}

double TileCacheManager::latitudeToTileY(double latitude, int zoom)
{
    const double clamped = qBound(-85.05112878, latitude, 85.05112878);
    const double sinLat = qSin(qDegreesToRadians(clamped));
    const double count = static_cast<double>(1 << qBound(0, zoom, 22));
    return (0.5 - qLn((1 + sinLat) / (1 - sinLat)) / (4 * M_PI)) * count;
}
