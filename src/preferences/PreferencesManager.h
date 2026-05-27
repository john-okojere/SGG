#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

class LocalSyncCache;

class PreferencesManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString units READ units NOTIFY preferencesChanged)
    Q_PROPERTY(QString mapLayer READ mapLayer NOTIFY preferencesChanged)
    Q_PROPERTY(QVariantMap missionDefaults READ missionDefaults NOTIFY preferencesChanged)
    Q_PROPERTY(QVariantMap telemetryConfig READ telemetryConfig NOTIFY preferencesChanged)
    Q_PROPERTY(QVariantMap dashboardConfig READ dashboardConfig NOTIFY preferencesChanged)
    Q_PROPERTY(QVariantMap preferredOverlays READ preferredOverlays NOTIFY preferencesChanged)
    Q_PROPERTY(bool joystickVisible READ joystickVisible NOTIFY preferencesChanged)
    Q_PROPERTY(QString pilotViewMode READ pilotViewMode NOTIFY preferencesChanged)

public:
    explicit PreferencesManager(LocalSyncCache *cache, QObject *parent = nullptr);

    QString units() const;
    QString mapLayer() const;
    QVariantMap missionDefaults() const;
    QVariantMap telemetryConfig() const;
    QVariantMap dashboardConfig() const;
    QVariantMap preferredOverlays() const;
    bool joystickVisible() const;
    QString pilotViewMode() const;

    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);
    Q_INVOKABLE void setMapLayer(const QString &layer);
    Q_INVOKABLE void setJoystickVisible(bool visible);
    Q_INVOKABLE void setPilotViewMode(const QString &mode);

signals:
    void preferencesChanged();

private:
    void loadCached();
    void persist();

    LocalSyncCache *m_cache = nullptr;
    QVariantMap m_preferences;
    QVariantMap m_missionDefaults;
    QVariantMap m_telemetryConfig;
    QVariantMap m_dashboardConfig;
    QVariantMap m_preferredOverlays;
};
