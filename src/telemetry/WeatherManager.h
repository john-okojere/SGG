#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

class WeatherManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY weatherChanged)
    Q_PROPERTY(QString sourceState READ sourceState NOTIFY weatherChanged)
    Q_PROPERTY(QString status READ status NOTIFY weatherChanged)
    Q_PROPERTY(double windSpeedMps READ windSpeedMps NOTIFY weatherChanged)
    Q_PROPERTY(double windDirectionDeg READ windDirectionDeg NOTIFY weatherChanged)
    Q_PROPERTY(double windGustMps READ windGustMps NOTIFY weatherChanged)
    Q_PROPERTY(double visibilityKm READ visibilityKm NOTIFY weatherChanged)
    Q_PROPERTY(double precipitationProbability READ precipitationProbability NOTIFY weatherChanged)
    Q_PROPERTY(double cloudCover READ cloudCover NOTIFY weatherChanged)
    Q_PROPERTY(double temperatureC READ temperatureC NOTIFY weatherChanged)

public:
    explicit WeatherManager(QObject *parent = nullptr);

    bool available() const;
    QString sourceState() const;
    QString status() const;
    double windSpeedMps() const;
    double windDirectionDeg() const;
    double windGustMps() const;
    double visibilityKm() const;
    double precipitationProbability() const;
    double cloudCover() const;
    double temperatureC() const;

    Q_INVOKABLE void markUnavailable(const QString &reason = QStringLiteral("Waiting for weather source"));
    Q_INVOKABLE void applyWeather(const QVariantMap &weather);

signals:
    void weatherChanged();

private:
    bool m_available = false;
    QString m_sourceState = QStringLiteral("unavailable");
    QString m_status = QStringLiteral("Waiting for weather source");
    double m_windSpeedMps = 0.0;
    double m_windDirectionDeg = 0.0;
    double m_windGustMps = 0.0;
    double m_visibilityKm = 0.0;
    double m_precipitationProbability = 0.0;
    double m_cloudCover = 0.0;
    double m_temperatureC = 0.0;
};
