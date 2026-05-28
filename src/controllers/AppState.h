#pragma once

#include <QObject>
#include <QString>

class AppState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentScreen READ currentScreen NOTIFY navigationChanged)
    Q_PROPERTY(QString currentMissionType READ currentMissionType NOTIFY missionChanged)
    Q_PROPERTY(QString operationalMode READ operationalMode WRITE setOperationalMode NOTIFY operationalModeChanged)
    Q_PROPERTY(QString selectedMissionId READ selectedMissionId NOTIFY missionChanged)
    Q_PROPERTY(QString selectedTool READ selectedTool WRITE setSelectedTool NOTIFY toolChanged)
    Q_PROPERTY(int selectedWaypointIndex READ selectedWaypointIndex WRITE setSelectedWaypointIndex NOTIFY selectedWaypointChanged)
    Q_PROPERTY(int selectedPolygonIndex READ selectedPolygonIndex WRITE setSelectedPolygonIndex NOTIFY selectedGeometryChanged)
    Q_PROPERTY(bool rightPanelCollapsed READ rightPanelCollapsed WRITE setRightPanelCollapsed NOTIFY panelChanged)

public:
    explicit AppState(QObject *parent = nullptr);

    QString currentScreen() const;
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
    Q_INVOKABLE void startMission(const QString &missionType);
    Q_INVOKABLE void openExistingMission(const QString &missionType, const QString &missionId);
    Q_INVOKABLE void startPilotMode();
    Q_INVOKABLE QString missionTitle() const;

signals:
    void navigationChanged();
    void missionChanged();
    void operationalModeChanged();
    void toolChanged();
    void selectedWaypointChanged();
    void selectedGeometryChanged();
    void panelChanged();
    void missionStarted(const QString &missionType);

private:
    QString m_currentScreen = "home";
    QString m_currentMissionType;
    QString m_operationalMode = "mission";
    QString m_selectedMissionId;
    QString m_selectedTool = "select";
    int m_selectedWaypointIndex = -1;
    int m_selectedPolygonIndex = -1;
    bool m_rightPanelCollapsed = false;
};
