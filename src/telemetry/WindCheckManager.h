#pragma once

#include <QObject>
#include <QString>

class WeatherManager;
class WindTelemetryManager;

class WindCheckManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString result READ result NOTIFY windCheckChanged)
    Q_PROPERTY(QString status READ status NOTIFY windCheckChanged)
    Q_PROPERTY(QString severity READ severity NOTIFY windCheckChanged)

public:
    explicit WindCheckManager(WeatherManager *weather,
                              WindTelemetryManager *wind,
                              QObject *parent = nullptr);

    QString result() const;
    QString status() const;
    QString severity() const;

    Q_INVOKABLE void evaluate();

signals:
    void windCheckChanged();

private:
    WeatherManager *m_weather = nullptr;
    WindTelemetryManager *m_wind = nullptr;
    QString m_result = QStringLiteral("Source unavailable");
    QString m_status = QStringLiteral("Waiting for weather source");
    QString m_severity = QStringLiteral("info");
};
