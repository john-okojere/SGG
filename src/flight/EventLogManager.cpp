#include "EventLogManager.h"

#include "../cache/LocalSyncCache.h"
#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"

#include <QDateTime>
#include <QVariantMap>

EventLogManager::EventLogManager(GcsEventSyncManager *eventSync,
                                 LocalSyncCache *cache,
                                 AccessManager *access,
                                 QObject *parent)
    : QObject(parent),
      m_eventSync(eventSync),
      m_access(access),
      m_cache(cache)
{
    if (m_cache) {
        m_events = m_cache->loadList(QStringLiteral("event_log"), QStringLiteral("recent"));
    }
}

QVariantList EventLogManager::events() const { return m_events; }
QString EventLogManager::status() const { return m_status; }

void EventLogManager::logEvent(const QString &eventType,
                               const QString &severity,
                               const QString &message,
                               const QJsonObject &payload)
{
    QVariantMap event{
        {QStringLiteral("event_type"), eventType},
        {QStringLiteral("severity"), severity.isEmpty() ? QStringLiteral("info") : severity},
        {QStringLiteral("message"), message},
        {QStringLiteral("payload"), payload.toVariantMap()},
        {QStringLiteral("recorded_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
    };
    m_events.prepend(event);
    while (m_events.size() > 120) {
        m_events.removeLast();
    }
    m_status = QStringLiteral("%1: %2").arg(event.value(QStringLiteral("severity")).toString(), message);
    persist();
    if (m_eventSync) {
        m_eventSync->recordEvent(eventType, event.value(QStringLiteral("severity")).toString(), message, payload);
    }
    emit eventsChanged();
}

bool EventLogManager::authorizeLogView()
{
    if (!m_access) {
        m_status = QStringLiteral("Event log blocked: access manager unavailable");
        emit eventsChanged();
        return false;
    }
    const bool allowed = m_access->authorizeAction(QStringLiteral("security_audit"),
                                                   {},
                                                   QStringLiteral("Event log blocked by local permissions."));
    if (!allowed) {
        m_status = QStringLiteral("Event log blocked by local permissions");
        emit eventsChanged();
    }
    return allowed;
}

void EventLogManager::clear()
{
    if (!m_access || !m_access->authorizeAction(QStringLiteral("security_audit"),
                                                {},
                                                QStringLiteral("Event log clear blocked by local permissions."))) {
        m_status = QStringLiteral("Event log clear blocked by local permissions");
        emit eventsChanged();
        return;
    }
    m_events.clear();
    m_status = QStringLiteral("Event log cleared");
    persist();
    emit eventsChanged();
}

void EventLogManager::persist()
{
    if (m_cache) {
        m_cache->saveList(QStringLiteral("event_log"), QStringLiteral("recent"), m_events);
    }
}
