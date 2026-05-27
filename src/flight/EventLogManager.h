#pragma once

#include <QJsonObject>
#include <QObject>
#include <QVariantList>

class GcsEventSyncManager;
class LocalSyncCache;

class EventLogManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList events READ events NOTIFY eventsChanged)
    Q_PROPERTY(QString status READ status NOTIFY eventsChanged)

public:
    explicit EventLogManager(GcsEventSyncManager *eventSync,
                             LocalSyncCache *cache,
                             QObject *parent = nullptr);

    QVariantList events() const;
    QString status() const;

    Q_INVOKABLE void logEvent(const QString &eventType,
                              const QString &severity,
                              const QString &message,
                              const QJsonObject &payload = {});
    Q_INVOKABLE void clear();

signals:
    void eventsChanged();

private:
    void persist();

    GcsEventSyncManager *m_eventSync = nullptr;
    LocalSyncCache *m_cache = nullptr;
    QVariantList m_events;
    QString m_status = "Event log ready";
};
