#pragma once

#include <QObject>
#include <QString>

class MapState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double centerLatitude READ centerLatitude WRITE setCenterLatitude NOTIFY centerChanged)
    Q_PROPERTY(double centerLongitude READ centerLongitude WRITE setCenterLongitude NOTIFY centerChanged)
    Q_PROPERTY(double zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomChanged)
    Q_PROPERTY(QString layerMode READ layerMode WRITE setLayerMode NOTIFY layerChanged)
    Q_PROPERTY(bool followAircraft READ followAircraft WRITE setFollowAircraft NOTIFY followAircraftChanged)
    Q_PROPERTY(QString activeOverlayMode READ activeOverlayMode NOTIFY overlayChanged)
    Q_PROPERTY(int overlayRevision READ overlayRevision NOTIFY overlayChanged)

public:
    explicit MapState(QObject *parent = nullptr);

    double centerLatitude() const;
    double centerLongitude() const;
    double zoomLevel() const;
    QString layerMode() const;
    bool followAircraft() const;
    QString activeOverlayMode() const;
    int overlayRevision() const;

    void setCenterLatitude(double value);
    void setCenterLongitude(double value);
    void setZoomLevel(double value);
    void setLayerMode(const QString &mode);
    void setFollowAircraft(bool follow);

    Q_INVOKABLE void zoomIn();
    Q_INVOKABLE void zoomOut();
    Q_INVOKABLE void setOverlayMode(const QString &mode);
    Q_INVOKABLE void refreshOverlay();

signals:
    void centerChanged();
    void zoomChanged();
    void layerChanged();
    void followAircraftChanged();
    void overlayChanged();

private:
    double m_centerLatitude = 10.531;
    double m_centerLongitude = 7.438;
    double m_zoomLevel = 13.2;
    QString m_layerMode = "SAT";
    bool m_followAircraft = false;
    QString m_activeOverlayMode = "photomap";
    int m_overlayRevision = 0;
};
