#pragma once

#include <QObject>
#include <QDateTime>
#include <QString>

class ApiClient;
class LocalSyncCache;
class SessionManager;
class WeatherManager;

class WeatherSyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY weatherSyncChanged)
    Q_PROPERTY(bool syncing READ syncing NOTIFY weatherSyncChanged)

public:
    explicit WeatherSyncManager(ApiClient *api,
                                SessionManager *session,
                                WeatherManager *weather,
                                LocalSyncCache *cache = nullptr,
                                QObject *parent = nullptr);

    QString status() const;
    bool syncing() const;

    Q_INVOKABLE void refreshForCoordinate(double latitude, double longitude);
    Q_INVOKABLE void refreshForMission(double latitude,
                                       double longitude,
                                       double altitudeMeters,
                                       const QString &aircraftId,
                                       const QString &mode);

signals:
    void weatherSyncChanged();

private:
    void setStatus(const QString &status);
    void setSyncing(bool syncing);
    void loadCachedWeather(const QString &fallbackReason);

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    WeatherManager *m_weather = nullptr;
    LocalSyncCache *m_cache = nullptr;
    QString m_status = QStringLiteral("Weather source unavailable");
    bool m_syncing = false;
    int m_minRefreshIntervalMs = 5000;
    QDateTime m_lastRefreshAt;
};
