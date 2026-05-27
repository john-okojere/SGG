#include "PilotActionSyncManager.h"

#include "FlightSessionSyncManager.h"
#include "GcsEventSyncManager.h"

PilotActionSyncManager::PilotActionSyncManager(FlightSessionSyncManager *flightSessions,
                                               GcsEventSyncManager *events,
                                               LocalSyncCache *cache,
                                               QObject *parent)
    : QObject(parent),
      m_flightSessions(flightSessions),
      m_events(events),
      m_cache(cache)
{
    Q_UNUSED(m_events)
    Q_UNUSED(m_cache)
}

QString PilotActionSyncManager::status() const { return m_status; }

void PilotActionSyncManager::recordAction(const QString &actionType,
                                          const QString &message,
                                          const QJsonObject &payload)
{
    if (m_flightSessions) {
        m_flightSessions->recordPilotAction(actionType, payload, message);
        setStatus(m_flightSessions->status());
        return;
    }
    setStatus(QStringLiteral("Pilot action recorded locally: %1").arg(actionType));
}

void PilotActionSyncManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit pilotActionChanged();
}
