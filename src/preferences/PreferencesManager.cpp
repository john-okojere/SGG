#include "PreferencesManager.h"

#include "../cache/LocalSyncCache.h"

PreferencesManager::PreferencesManager(LocalSyncCache *cache, QObject *parent)
    : QObject(parent), m_cache(cache)
{
    loadCached();
}

QString PreferencesManager::units() const
{
    return m_preferences.value(QStringLiteral("units"), QStringLiteral("metric")).toString();
}

QString PreferencesManager::mapLayer() const
{
    return m_preferences.value(QStringLiteral("map_layer"), QStringLiteral("SAT")).toString();
}

QVariantMap PreferencesManager::missionDefaults() const { return m_missionDefaults; }
QVariantMap PreferencesManager::telemetryConfig() const { return m_telemetryConfig; }
QVariantMap PreferencesManager::dashboardConfig() const { return m_dashboardConfig; }
QVariantMap PreferencesManager::preferredOverlays() const { return m_preferredOverlays; }
bool PreferencesManager::joystickVisible() const { return m_preferences.value(QStringLiteral("pilot_joystick_visible"), true).toBool(); }
QString PreferencesManager::pilotViewMode() const { return m_preferences.value(QStringLiteral("pilot_view_mode"), QStringLiteral("MAP")).toString(); }

void PreferencesManager::applyBootstrap(const QVariantMap &bootstrap)
{
    m_preferences = bootstrap.value(QStringLiteral("preferences")).toMap();
    if (m_preferences.isEmpty()) {
        m_preferences = bootstrap.value(QStringLiteral("user_preferences")).toMap();
    }
    m_missionDefaults = bootstrap.value(QStringLiteral("mission_defaults")).toMap();
    m_telemetryConfig = bootstrap.value(QStringLiteral("telemetry_config")).toMap();
    m_dashboardConfig = bootstrap.value(QStringLiteral("dashboard_configuration"),
                        bootstrap.value(QStringLiteral("dashboard_config"))).toMap();
    m_preferredOverlays = bootstrap.value(QStringLiteral("preferred_overlays"),
                          m_preferences.value(QStringLiteral("overlays"))).toMap();
    persist();
    emit preferencesChanged();
}

void PreferencesManager::setMapLayer(const QString &layer)
{
    m_preferences[QStringLiteral("map_layer")] = layer;
    persist();
    emit preferencesChanged();
}

void PreferencesManager::setJoystickVisible(bool visible)
{
    m_preferences[QStringLiteral("pilot_joystick_visible")] = visible;
    persist();
    emit preferencesChanged();
}

void PreferencesManager::setPilotViewMode(const QString &mode)
{
    m_preferences[QStringLiteral("pilot_view_mode")] = mode;
    persist();
    emit preferencesChanged();
}

void PreferencesManager::loadCached()
{
    if (!m_cache) {
        return;
    }
    m_preferences = m_cache->loadObject(QStringLiteral("preferences"), QStringLiteral("user"));
    m_missionDefaults = m_cache->loadObject(QStringLiteral("preferences"), QStringLiteral("mission_defaults"));
    m_telemetryConfig = m_cache->loadObject(QStringLiteral("preferences"), QStringLiteral("telemetry_config"));
    m_dashboardConfig = m_cache->loadObject(QStringLiteral("preferences"), QStringLiteral("dashboard_config"));
    m_preferredOverlays = m_cache->loadObject(QStringLiteral("preferences"), QStringLiteral("preferred_overlays"));
    emit preferencesChanged();
}

void PreferencesManager::persist()
{
    if (!m_cache) {
        return;
    }
    m_cache->saveObject(QStringLiteral("preferences"), QStringLiteral("user"), m_preferences);
    m_cache->saveObject(QStringLiteral("preferences"), QStringLiteral("mission_defaults"), m_missionDefaults);
    m_cache->saveObject(QStringLiteral("preferences"), QStringLiteral("telemetry_config"), m_telemetryConfig);
    m_cache->saveObject(QStringLiteral("preferences"), QStringLiteral("dashboard_config"), m_dashboardConfig);
    m_cache->saveObject(QStringLiteral("preferences"), QStringLiteral("preferred_overlays"), m_preferredOverlays);
}
