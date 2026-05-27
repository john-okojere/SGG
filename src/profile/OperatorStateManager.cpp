#include "OperatorStateManager.h"

#include "../cache/LocalSyncCache.h"

OperatorStateManager::OperatorStateManager(LocalSyncCache *cache, QObject *parent)
    : QObject(parent), m_cache(cache)
{
    loadCached();
}

QString OperatorStateManager::activeAircraftName() const
{
    return m_activeAircraft.value(QStringLiteral("name"), QStringLiteral("No aircraft selected")).toString();
}

QString OperatorStateManager::activeAircraftId() const
{
    return m_activeAircraft.value(QStringLiteral("id"), m_activeAircraft.value(QStringLiteral("aircraft_id"))).toString();
}

QString OperatorStateManager::workspaceMode() const { return m_workspaceMode; }
QVariantMap OperatorStateManager::layoutState() const { return m_layoutState; }

void OperatorStateManager::applyBootstrap(const QVariantMap &bootstrap)
{
    const QVariantList aircraft = bootstrap.value(QStringLiteral("assigned_aircraft")).toList();
    if (!aircraft.isEmpty()) {
        m_activeAircraft = aircraft.first().toMap();
    }
    const QVariantMap dashboard = bootstrap.value(QStringLiteral("dashboard_configuration"),
                                  bootstrap.value(QStringLiteral("dashboard_config"))).toMap();
    const QVariantMap layout = bootstrap.value(QStringLiteral("layout"),
                               bootstrap.value(QStringLiteral("preferred_layout"))).toMap();
    m_layoutState = layout.isEmpty() ? dashboard : layout;
    m_workspaceMode = m_layoutState.value(QStringLiteral("workspace_mode"), QStringLiteral("Mission Operations")).toString();
    persist();
    emit operatorStateChanged();
}

void OperatorStateManager::setActiveAircraft(const QVariantMap &aircraft)
{
    m_activeAircraft = aircraft;
    persist();
    emit operatorStateChanged();
}

void OperatorStateManager::loadCached()
{
    if (!m_cache) {
        return;
    }
    m_activeAircraft = m_cache->loadObject(QStringLiteral("operator"), QStringLiteral("active_aircraft"));
    m_layoutState = m_cache->loadObject(QStringLiteral("operator"), QStringLiteral("layout_state"));
    m_workspaceMode = m_layoutState.value(QStringLiteral("workspace_mode"), m_workspaceMode).toString();
    emit operatorStateChanged();
}

void OperatorStateManager::persist()
{
    if (!m_cache) {
        return;
    }
    m_cache->saveObject(QStringLiteral("operator"), QStringLiteral("active_aircraft"), m_activeAircraft);
    m_cache->saveObject(QStringLiteral("operator"), QStringLiteral("layout_state"), m_layoutState);
}
