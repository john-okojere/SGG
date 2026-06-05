#include "PilotActionSyncManager.h"

#include "FlightSessionSyncManager.h"
#include "GcsEventSyncManager.h"
#include "../security/AccessManager.h"

#include <QVariantMap>

PilotActionSyncManager::PilotActionSyncManager(FlightSessionSyncManager *flightSessions,
                                               GcsEventSyncManager *events,
                                               LocalSyncCache *cache,
                                               AccessManager *access,
                                               QObject *parent)
    : QObject(parent),
      m_access(access),
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
    const QString accessAction = accessActionForPilotAction(actionType);
    if (!m_access || !m_access->authorizeAction(accessAction,
                                                QVariantMap{{QStringLiteral("action_type"), actionType}},
                                                QStringLiteral("Pilot action record blocked by local permissions."))) {
        setStatus(QStringLiteral("Pilot action record blocked by local permissions."));
        return;
    }
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

QString PilotActionSyncManager::accessActionForPilotAction(const QString &actionType) const
{
    const QString action = actionType.trimmed().toLower();
    if (action.contains(QStringLiteral("emergency_stop")) || action.contains(QStringLiteral("kill"))) {
        return QStringLiteral("emergency_stop");
    }
    if (action.contains(QStringLiteral("mission"))) {
        return QStringLiteral("mission_start");
    }
    return QStringLiteral("manual_flight");
}
