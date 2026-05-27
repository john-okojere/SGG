#pragma once

#include <QObject>
#include <QString>

class WindTelemetryManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY windChanged)
    Q_PROPERTY(double windSpeed READ windSpeed NOTIFY windChanged)
    Q_PROPERTY(double windDirection READ windDirection NOTIFY windChanged)
    Q_PROPERTY(double gust READ gust NOTIFY windChanged)
    Q_PROPERTY(QString directionLabel READ directionLabel NOTIFY windChanged)
    Q_PROPERTY(QString status READ status NOTIFY windChanged)

public:
    explicit WindTelemetryManager(QObject *parent = nullptr);

    bool available() const;
    double windSpeed() const;
    double windDirection() const;
    double gust() const;
    QString directionLabel() const;
    QString status() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void setDevelopmentWind(double speedMps, double directionDeg, double gustMps);

signals:
    void windChanged();

private:
    static QString directionFor(double degrees);

    bool m_available = false;
    double m_windSpeed = 0.0;
    double m_windDirection = 0.0;
    double m_gust = 0.0;
};
