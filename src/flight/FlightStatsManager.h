#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>

class AppState;
class FlightSessionSyncManager;
class GcsEventSyncManager;
class LocalSyncCache;
class MissionPlanModel;
class VehicleTelemetryModel;

class FlightStatsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active NOTIFY statsChanged)
    Q_PROPERTY(QString flightSessionId READ flightSessionId NOTIFY statsChanged)
    Q_PROPERTY(QString startedAt READ startedAt NOTIFY statsChanged)
    Q_PROPERTY(int flightDurationSeconds READ flightDurationSeconds NOTIFY statsChanged)
    Q_PROPERTY(QString flightTimeText READ flightTimeText NOTIFY statsChanged)
    Q_PROPERTY(double distanceMeters READ distanceMeters NOTIFY statsChanged)
    Q_PROPERTY(double distanceKm READ distanceKm NOTIFY statsChanged)
    Q_PROPERTY(double maxAltitude READ maxAltitude NOTIFY statsChanged)
    Q_PROPERTY(double batteryStartPercent READ batteryStartPercent NOTIFY statsChanged)
    Q_PROPERTY(double batteryEndPercent READ batteryEndPercent NOTIFY statsChanged)
    Q_PROPERTY(QVariantList flightPath READ flightPath NOTIFY statsChanged)
    Q_PROPERTY(QString status READ status NOTIFY statsChanged)

public:
    explicit FlightStatsManager(VehicleTelemetryModel *telemetry,
                                FlightSessionSyncManager *flightSessions,
                                AppState *appState,
                                MissionPlanModel *plan,
                                GcsEventSyncManager *events,
                                LocalSyncCache *cache,
                                QObject *parent = nullptr);

    bool active() const;
    QString flightSessionId() const;
    QString startedAt() const;
    int flightDurationSeconds() const;
    QString flightTimeText() const;
    double distanceMeters() const;
    double distanceKm() const;
    double maxAltitude() const;
    double batteryStartPercent() const;
    double batteryEndPercent() const;
    QVariantList flightPath() const;
    QString status() const;

    Q_INVOKABLE void startSession();
    Q_INVOKABLE void endSession();
    Q_INVOKABLE void reset();

signals:
    void statsChanged();

private:
    static double distanceBetween(double latA, double lonA, double latB, double lonB);
    void ingestTelemetry();
    void tick();
    void setStatus(const QString &status);

    VehicleTelemetryModel *m_telemetry = nullptr;
    FlightSessionSyncManager *m_flightSessions = nullptr;
    AppState *m_appState = nullptr;
    MissionPlanModel *m_plan = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    LocalSyncCache *m_cache = nullptr;
    QTimer m_timer;
    bool m_active = false;
    QString m_flightSessionId;
    QString m_startedAt;
    qint64 m_startedMsec = 0;
    int m_flightDurationSeconds = 0;
    double m_distanceMeters = 0.0;
    double m_maxAltitude = 0.0;
    double m_batteryStartPercent = 0.0;
    double m_batteryEndPercent = 0.0;
    double m_lastLatitude = 0.0;
    double m_lastLongitude = 0.0;
    QVariantList m_flightPath;
    QString m_status = "No active flight session";
};
