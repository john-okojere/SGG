#pragma once

#include <QObject>
#include <QString>

class VehicleTelemetryModel;

class HomePositionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasHome READ hasHome NOTIFY homeChanged)
    Q_PROPERTY(double homeLatitude READ homeLatitude NOTIFY homeChanged)
    Q_PROPERTY(double homeLongitude READ homeLongitude NOTIFY homeChanged)
    Q_PROPERTY(QString source READ source NOTIFY homeChanged)
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY homeChanged)

public:
    explicit HomePositionManager(VehicleTelemetryModel *telemetry, QObject *parent = nullptr);

    bool hasHome() const;
    double homeLatitude() const;
    double homeLongitude() const;
    QString source() const;
    bool locked() const;
    void setLocked(bool locked);

    Q_INVOKABLE void setHome(double latitude, double longitude, const QString &source = QStringLiteral("operator"));
    Q_INVOKABLE void clearHome();
    Q_INVOKABLE bool updateHomeToCurrentTelemetry(const QString &source = QStringLiteral("operator"));
    Q_INVOKABLE double distanceFromHomeMeters(double latitude, double longitude) const;

signals:
    void homeChanged();

private:
    bool validCoordinate(double latitude, double longitude) const;

    VehicleTelemetryModel *m_telemetry = nullptr;
    bool m_hasHome = false;
    double m_homeLatitude = 0.0;
    double m_homeLongitude = 0.0;
    QString m_source;
    bool m_locked = true;
};
