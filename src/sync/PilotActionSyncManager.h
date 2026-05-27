#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>

class FlightSessionSyncManager;
class GcsEventSyncManager;
class LocalSyncCache;

class PilotActionSyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY pilotActionChanged)

public:
    explicit PilotActionSyncManager(FlightSessionSyncManager *flightSessions,
                                    GcsEventSyncManager *events,
                                    LocalSyncCache *cache,
                                    QObject *parent = nullptr);

    QString status() const;

    Q_INVOKABLE void recordAction(const QString &actionType,
                                  const QString &message,
                                  const QJsonObject &payload = {});

signals:
    void pilotActionChanged();

private:
    void setStatus(const QString &status);

    FlightSessionSyncManager *m_flightSessions = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    LocalSyncCache *m_cache = nullptr;
    QString m_status = "Pilot action sync ready";
};
