#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>

class ApiClient;
class LocalSyncCache;
class SessionManager;

class GcsEventSyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY eventSyncChanged)

public:
    explicit GcsEventSyncManager(ApiClient *api,
                                 SessionManager *session,
                                 LocalSyncCache *cache,
                                 QObject *parent = nullptr);

    QString status() const;

    Q_INVOKABLE void recordEvent(const QString &eventType,
                                 const QString &severity,
                                 const QString &message,
                                 const QJsonObject &payload = {});

signals:
    void eventSyncChanged();

private:
    struct QueuedEvent {
        QString eventType;
        QJsonObject payload;
    };

    bool isCriticalEvent(const QString &eventType, const QString &severity, const QJsonObject &payload) const;
    void enqueueOrSend(const QString &eventType, const QJsonObject &event, bool immediate);
    void flushBatch();
    void postEvent(const QString &eventType, const QJsonObject &event);
    QJsonObject normalizePayload(const QJsonObject &payload) const;
    QJsonValue normalizeValue(const QString &key, const QJsonValue &value) const;
    static double roundNumber(double value, int decimals);
    void setStatus(const QString &status);

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    LocalSyncCache *m_cache = nullptr;
    QTimer m_flushTimer;
    QVector<QueuedEvent> m_pendingEvents;
    QString m_status = "GCS event sync idle";
};
