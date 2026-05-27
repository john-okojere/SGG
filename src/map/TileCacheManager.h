#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QSet>
#include <QString>
#include <QVariantList>

class TileCacheManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool downloading READ downloading NOTIFY cacheChanged)
    Q_PROPERTY(bool offlineMode READ offlineMode WRITE setOfflineMode NOTIFY cacheChanged)
    Q_PROPERTY(int queuedTiles READ queuedTiles NOTIFY cacheChanged)
    Q_PROPERTY(int downloadedTiles READ downloadedTiles NOTIFY cacheChanged)
    Q_PROPERTY(int cacheRevision READ cacheRevision NOTIFY cacheChanged)
    Q_PROPERTY(QString status READ status NOTIFY cacheChanged)
    Q_PROPERTY(QString cacheRoot READ cacheRoot CONSTANT)

public:
    explicit TileCacheManager(QObject *parent = nullptr);

    bool downloading() const;
    bool offlineMode() const;
    int queuedTiles() const;
    int downloadedTiles() const;
    int cacheRevision() const;
    QString status() const;
    QString cacheRoot() const;

    void setOfflineMode(bool value);

    Q_INVOKABLE QString tileSource(const QString &layer,
                                   int zoom,
                                   int tileX,
                                   int tileY,
                                   const QString &remoteUrl,
                                   int revision);
    Q_INVOKABLE void downloadMissionArea(const QVariantList &geometry,
                                         const QString &layer,
                                         double centerLatitude,
                                         double centerLongitude,
                                         int minZoom,
                                         int maxZoom);
    Q_INVOKABLE void clearCache();

signals:
    void cacheChanged();

private:
    QString normalizedLayer(const QString &layer) const;
    QString tilePath(const QString &layer, int zoom, int tileX, int tileY) const;
    QString tileUrlFor(const QString &layer, int zoom, int tileX, int tileY) const;
    void queueTile(const QString &layer, int zoom, int tileX, int tileY, const QString &remoteUrl);
    void setStatus(const QString &status);
    static int wrappedTileX(int tileX, int zoom);
    static double longitudeToTileX(double longitude, int zoom);
    static double latitudeToTileY(double latitude, int zoom);

    QNetworkAccessManager m_network;
    QSet<QString> m_inFlight;
    QString m_cacheRoot;
    bool m_offlineMode = false;
    int m_queuedTiles = 0;
    int m_downloadedTiles = 0;
    int m_cacheRevision = 0;
    QString m_status = "Tile cache ready";
};
