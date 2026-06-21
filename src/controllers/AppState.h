#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

class AccessManager;

class AppState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentScreen READ currentScreen NOTIFY navigationChanged)
    Q_PROPERTY(QString currentManufacturerTool READ currentManufacturerTool NOTIFY manufacturerToolChanged)
    Q_PROPERTY(QString currentGcsTool READ currentGcsTool NOTIFY gcsToolChanged)
    Q_PROPERTY(QString currentMissionType READ currentMissionType NOTIFY missionChanged)
    Q_PROPERTY(QString operationalMode READ operationalMode WRITE setOperationalMode NOTIFY operationalModeChanged)
    Q_PROPERTY(QString selectedMissionId READ selectedMissionId NOTIFY missionChanged)
    Q_PROPERTY(QString selectedTool READ selectedTool WRITE setSelectedTool NOTIFY toolChanged)
    Q_PROPERTY(int selectedWaypointIndex READ selectedWaypointIndex WRITE setSelectedWaypointIndex NOTIFY selectedWaypointChanged)
    Q_PROPERTY(int selectedPolygonIndex READ selectedPolygonIndex WRITE setSelectedPolygonIndex NOTIFY selectedGeometryChanged)
    Q_PROPERTY(bool rightPanelCollapsed READ rightPanelCollapsed WRITE setRightPanelCollapsed NOTIFY panelChanged)

public:
    explicit AppState(QObject *parent = nullptr);
    void setAccessManager(AccessManager *access);

    QString currentScreen() const;
    QString currentManufacturerTool() const;
    QString currentGcsTool() const;
    QString currentMissionType() const;
    QString operationalMode() const;
    QString selectedMissionId() const;
    QString selectedTool() const;
    int selectedWaypointIndex() const;
    int selectedPolygonIndex() const;
    bool rightPanelCollapsed() const;

    void setSelectedTool(const QString &tool);
    void setOperationalMode(const QString &mode);
    void setSelectedWaypointIndex(int index);
    void setSelectedPolygonIndex(int index);
    void setRightPanelCollapsed(bool collapsed);

    Q_INVOKABLE void goHome();
    Q_INVOKABLE void openMissionSelector();
    Q_INVOKABLE void closeMissionSelector();
    Q_INVOKABLE void openVehicleConfiguration();
    Q_INVOKABLE void openManufacturerTestFlight();
    Q_INVOKABLE void openDefaultWorkspace(const QString &workspace);
    Q_INVOKABLE void startMission(const QString &missionType);
    Q_INVOKABLE void openExistingMission(const QString &missionType, const QString &missionId);
    Q_INVOKABLE void startPilotMode();
    Q_INVOKABLE void openGcsTools();
    Q_INVOKABLE void openGcsTool(const QString &tool);
    Q_INVOKABLE void openManufacturerWorkspace();
    Q_INVOKABLE void openManufacturerTool(const QString &tool);
    Q_INVOKABLE void openHelpCenter();
    Q_INVOKABLE void resolveWorkspaceForAccess();
    Q_INVOKABLE QString missionTitle() const;

signals:
    void navigationChanged();
    void missionChanged();
    void operationalModeChanged();
    void toolChanged();
    void manufacturerToolChanged();
    void gcsToolChanged();
    void selectedWaypointChanged();
    void selectedGeometryChanged();
    void panelChanged();
    void missionStarted(const QString &missionType);

private:
    bool authorize(const QString &action, const QString &message, const QVariantMap &context = {});
    void applyOperationalMode(const QString &mode);
    bool hasManufacturerAccess() const;
    QString actionForManufacturerTool(const QString &tool) const;
    QString actionForGcsTool(const QString &tool) const;
    QString defaultManufacturerTool() const;
    QString defaultGcsTool() const;

    AccessManager *m_access = nullptr;
    QString m_currentScreen = "home";
    QString m_currentManufacturerTool = "vehicleConfiguration";
    QString m_currentGcsTool = "connect";
    QString m_currentMissionType;
    QString m_operationalMode = "mission";
    QString m_selectedMissionId;
    QString m_selectedTool = "select";
    int m_selectedWaypointIndex = -1;
    int m_selectedPolygonIndex = -1;
    bool m_rightPanelCollapsed = false;
};
