#include "AppState.h"

AppState::AppState(QObject *parent) : QObject(parent) {}

QString AppState::currentScreen() const { return m_currentScreen; }
QString AppState::currentMissionType() const { return m_currentMissionType; }
QString AppState::operationalMode() const { return m_operationalMode; }
QString AppState::selectedMissionId() const { return m_selectedMissionId; }
QString AppState::selectedTool() const { return m_selectedTool; }
int AppState::selectedWaypointIndex() const { return m_selectedWaypointIndex; }
bool AppState::rightPanelCollapsed() const { return m_rightPanelCollapsed; }

void AppState::setSelectedTool(const QString &tool)
{
    if (m_selectedTool == tool) {
        return;
    }
    m_selectedTool = tool;
    emit toolChanged();
}

void AppState::setOperationalMode(const QString &mode)
{
    const QString next = mode == QStringLiteral("pilot") ? QStringLiteral("pilot") : QStringLiteral("mission");
    if (m_operationalMode == next) {
        return;
    }
    m_operationalMode = next;
    emit operationalModeChanged();
}

void AppState::setSelectedWaypointIndex(int index)
{
    if (m_selectedWaypointIndex == index) {
        return;
    }
    m_selectedWaypointIndex = index;
    emit selectedWaypointChanged();
}

void AppState::setRightPanelCollapsed(bool collapsed)
{
    if (m_rightPanelCollapsed == collapsed) {
        return;
    }
    m_rightPanelCollapsed = collapsed;
    emit panelChanged();
}

void AppState::goHome()
{
    if (m_currentScreen == "home") {
        return;
    }
    m_currentScreen = "home";
    emit navigationChanged();
}

void AppState::openMissionSelector()
{
    if (m_currentScreen == "missionSelector") {
        return;
    }
    m_currentScreen = "missionSelector";
    emit navigationChanged();
}

void AppState::closeMissionSelector()
{
    if (m_currentScreen == "missionSelector") {
        m_currentScreen = "home";
        emit navigationChanged();
    }
}

void AppState::startMission(const QString &missionType)
{
    setOperationalMode(QStringLiteral("mission"));
    m_currentMissionType = missionType;
    m_selectedMissionId.clear();
    m_currentScreen = "planner";
    if (missionType == QStringLiteral("waypointRoute")) {
        m_selectedTool = QStringLiteral("route");
    } else if (missionType == QStringLiteral("map3dPoi")) {
        m_selectedTool = QStringLiteral("poi");
    } else if (missionType == QStringLiteral("photomap")
               || missionType == QStringLiteral("map3dArea")
               || missionType == QStringLiteral("towerInspection")
               || missionType == QStringLiteral("virtualFence")) {
        m_selectedTool = QStringLiteral("polygon");
    } else {
        m_selectedTool = "select";
    }
    m_selectedWaypointIndex = -1;
    emit missionChanged();
    emit navigationChanged();
    emit toolChanged();
    emit selectedWaypointChanged();
    emit missionStarted(missionType);
}

void AppState::openExistingMission(const QString &missionType, const QString &missionId)
{
    if (missionType.isEmpty()) {
        return;
    }
    setOperationalMode(QStringLiteral("mission"));
    m_currentMissionType = missionType;
    m_selectedMissionId = missionId.trimmed();
    m_currentScreen = QStringLiteral("planner");
    if (missionType == QStringLiteral("waypointRoute")) {
        m_selectedTool = QStringLiteral("route");
    } else if (missionType == QStringLiteral("map3dPoi")) {
        m_selectedTool = QStringLiteral("poi");
    } else if (missionType == QStringLiteral("photomap")
               || missionType == QStringLiteral("map3dArea")
               || missionType == QStringLiteral("towerInspection")
               || missionType == QStringLiteral("virtualFence")) {
        m_selectedTool = QStringLiteral("polygon");
    } else {
        m_selectedTool = QStringLiteral("select");
    }
    m_selectedWaypointIndex = -1;
    emit missionChanged();
    emit navigationChanged();
    emit toolChanged();
    emit selectedWaypointChanged();
}

void AppState::startPilotMode()
{
    setOperationalMode(QStringLiteral("pilot"));
    m_currentMissionType.clear();
    m_selectedMissionId.clear();
    m_currentScreen = QStringLiteral("planner");
    m_selectedTool = QStringLiteral("select");
    m_selectedWaypointIndex = -1;
    emit missionChanged();
    emit navigationChanged();
    emit toolChanged();
    emit selectedWaypointChanged();
}

QString AppState::missionTitle() const
{
    if (m_operationalMode == QStringLiteral("pilot")) {
        return "Pilot Mode";
    }
    if (m_currentMissionType == "photomap") {
        return "Untitled PhotoMap";
    }
    if (m_currentMissionType == "waypointRoute") {
        return "Untitled Waypoint Route";
    }
    if (m_currentMissionType == "towerInspection") {
        return "Untitled Tower Inspection";
    }
    if (m_currentMissionType == "map3dArea") {
        return "Untitled 3D Area";
    }
    if (m_currentMissionType == "map3dPoi") {
        return "Untitled 3D POI";
    }
    if (m_currentMissionType == "virtualFence") {
        return "Untitled Virtual Fence";
    }
    return "No Mission";
}
