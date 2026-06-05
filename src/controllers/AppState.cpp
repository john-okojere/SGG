#include "AppState.h"

#include "../security/AccessManager.h"

AppState::AppState(QObject *parent) : QObject(parent) {}

void AppState::setAccessManager(AccessManager *access)
{
    m_access = access;
}

QString AppState::currentScreen() const { return m_currentScreen; }
QString AppState::currentManufacturerTool() const { return m_currentManufacturerTool; }
QString AppState::currentMissionType() const { return m_currentMissionType; }
QString AppState::operationalMode() const { return m_operationalMode; }
QString AppState::selectedMissionId() const { return m_selectedMissionId; }
QString AppState::selectedTool() const { return m_selectedTool; }
int AppState::selectedWaypointIndex() const { return m_selectedWaypointIndex; }
int AppState::selectedPolygonIndex() const { return m_selectedPolygonIndex; }
bool AppState::rightPanelCollapsed() const { return m_rightPanelCollapsed; }

void AppState::setSelectedTool(const QString &tool)
{
    if (tool != QStringLiteral("select")
        && !authorize(QStringLiteral("mission_planning"),
                      QStringLiteral("Mission tool selection blocked by local permissions."),
                      QVariantMap{{QStringLiteral("tool"), tool}})) {
        return;
    }
    if (m_selectedTool == tool) {
        return;
    }
    m_selectedTool = tool;
    emit toolChanged();
}

void AppState::setOperationalMode(const QString &mode)
{
    const QString next = mode == QStringLiteral("pilot") ? QStringLiteral("pilot") : QStringLiteral("mission");
    const QString action = next == QStringLiteral("pilot") ? QStringLiteral("manual_flight") : QStringLiteral("mission_planning");
    if (!authorize(action,
                   QStringLiteral("Operational mode change blocked by local permissions."),
                   QVariantMap{{QStringLiteral("mode"), next}})) {
        return;
    }
    applyOperationalMode(next);
}

void AppState::applyOperationalMode(const QString &mode)
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
    if (index >= 0) {
        m_selectedPolygonIndex = -1;
        emit selectedGeometryChanged();
    }
    emit selectedWaypointChanged();
}

void AppState::setSelectedPolygonIndex(int index)
{
    if (m_selectedPolygonIndex == index) {
        return;
    }
    m_selectedPolygonIndex = index;
    if (index >= 0) {
        m_selectedWaypointIndex = -1;
        emit selectedWaypointChanged();
    }
    emit selectedGeometryChanged();
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

void AppState::openManufacturerWorkspace()
{
    if (!hasManufacturerAccess()) {
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("manufacturer_tools"),
                                    QStringLiteral("Manufacturer workspace blocked by local permissions."),
                                    {});
        }
        return;
    }
    const QString nextTool = defaultManufacturerTool();
    if (!nextTool.isEmpty() && m_currentManufacturerTool != nextTool) {
        m_currentManufacturerTool = nextTool;
        emit manufacturerToolChanged();
    }
    applyOperationalMode(QStringLiteral("mission"));
    if (m_currentScreen == QStringLiteral("manufacturer")) {
        return;
    }
    m_currentScreen = QStringLiteral("manufacturer");
    emit navigationChanged();
}

void AppState::openManufacturerTool(const QString &tool)
{
    const QString normalized = tool.trimmed();
    const QString action = actionForManufacturerTool(normalized);
    if (action.isEmpty()) {
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("manufacturer_tools"),
                                    QStringLiteral("Unknown manufacturer tool blocked."),
                                    QVariantMap{{QStringLiteral("tool"), normalized}});
        }
        return;
    }
    if (!authorize(action,
                   QStringLiteral("Manufacturer tool blocked by local permissions."),
                   QVariantMap{{QStringLiteral("tool"), normalized}})) {
        return;
    }
    applyOperationalMode(QStringLiteral("mission"));
    if (m_currentManufacturerTool != normalized) {
        m_currentManufacturerTool = normalized;
        emit manufacturerToolChanged();
    }
    if (m_currentScreen != QStringLiteral("manufacturer")) {
        m_currentScreen = QStringLiteral("manufacturer");
        emit navigationChanged();
    }
}

void AppState::resolveWorkspaceForAccess()
{
    if (!m_access || !m_access->accessLoaded()) {
        return;
    }
    if (hasManufacturerAccess()
        && (m_currentScreen == QStringLiteral("home")
            || m_currentScreen == QStringLiteral("missionSelector")
            || m_currentScreen == QStringLiteral("manufacturer"))) {
        openManufacturerWorkspace();
    }
}

void AppState::openMissionSelector()
{
    if (!authorize(QStringLiteral("mission_planning"),
                   QStringLiteral("Mission selector blocked by local permissions."))) {
        return;
    }
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
    if (!authorize(QStringLiteral("mission_planning"),
                   QStringLiteral("Mission start blocked by local permissions."),
                   QVariantMap{{QStringLiteral("mission_type"), missionType}})) {
        return;
    }
    applyOperationalMode(QStringLiteral("mission"));
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
    m_selectedPolygonIndex = -1;
    emit missionChanged();
    emit navigationChanged();
    emit toolChanged();
    emit selectedWaypointChanged();
    emit selectedGeometryChanged();
    emit missionStarted(missionType);
}

void AppState::openExistingMission(const QString &missionType, const QString &missionId)
{
    if (missionType.isEmpty()) {
        return;
    }
    const QString trimmedMissionId = missionId.trimmed();
    if (!authorize(QStringLiteral("mission_open"),
                   QStringLiteral("Mission open blocked by local permissions."),
                   QVariantMap{{QStringLiteral("mission_type"), missionType},
                               {QStringLiteral("mission_id"), trimmedMissionId}})
        || !m_access->canAccessMission(trimmedMissionId)) {
        if (m_access) {
            m_access->recordBlocked(QStringLiteral("mission_open"),
                                    QStringLiteral("Mission is outside the local need-to-know scope."),
                                    QVariantMap{{QStringLiteral("mission_id"), trimmedMissionId}});
        }
        return;
    }
    applyOperationalMode(QStringLiteral("mission"));
    m_currentMissionType = missionType;
    m_selectedMissionId = trimmedMissionId;
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
    m_selectedPolygonIndex = -1;
    emit missionChanged();
    emit navigationChanged();
    emit toolChanged();
    emit selectedWaypointChanged();
    emit selectedGeometryChanged();
}

void AppState::startPilotMode()
{
    if (!authorize(QStringLiteral("manual_flight"),
                   QStringLiteral("Pilot mode blocked by local permissions."))) {
        return;
    }
    applyOperationalMode(QStringLiteral("pilot"));
    m_currentMissionType.clear();
    m_selectedMissionId.clear();
    m_currentScreen = QStringLiteral("planner");
    m_selectedTool = QStringLiteral("select");
    m_selectedWaypointIndex = -1;
    m_selectedPolygonIndex = -1;
    emit missionChanged();
    emit navigationChanged();
    emit toolChanged();
    emit selectedWaypointChanged();
    emit selectedGeometryChanged();
}

bool AppState::authorize(const QString &action, const QString &message, const QVariantMap &context)
{
    if (!m_access) {
        return false;
    }
    return m_access->authorizeAction(action, context, message);
}

bool AppState::hasManufacturerAccess() const
{
    if (!m_access || !m_access->accessLoaded()) {
        return false;
    }
    return m_access->canModule(QStringLiteral("manufacturer_dashboard"))
        || m_access->canModule(QStringLiteral("manufacturer_test_flight"))
        || m_access->canModule(QStringLiteral("vehicle_profiles"))
        || m_access->can(QStringLiteral("can_configure_vehicle"))
        || m_access->can(QStringLiteral("can_register_vehicle"))
        || m_access->can(QStringLiteral("can_edit_vehicle_profile"))
        || m_access->can(QStringLiteral("can_bind_flight_controller"))
        || m_access->can(QStringLiteral("can_configure_rc"))
        || m_access->can(QStringLiteral("can_read_vehicle_parameters"))
        || m_access->can(QStringLiteral("can_write_vehicle_parameters"))
        || m_access->can(QStringLiteral("can_run_manufacturer_test_flight"))
        || m_access->can(QStringLiteral("can_fly_manual_test"))
        || m_access->can(QStringLiteral("can_release_vehicle_to_organization"));
}

QString AppState::actionForManufacturerTool(const QString &tool) const
{
    if (tool == QStringLiteral("vehicleConfiguration")) {
        return QStringLiteral("vehicle_configuration");
    }
    if (tool == QStringLiteral("vehicleProfile")) {
        return QStringLiteral("vehicle_profile_setup");
    }
    if (tool == QStringLiteral("flightControllerBinding")) {
        return QStringLiteral("flight_controller_binding");
    }
    if (tool == QStringLiteral("vehicleParameters")) {
        return QStringLiteral("vehicle_parameter_read");
    }
    if (tool == QStringLiteral("rcMapping")) {
        return QStringLiteral("rc_mapping");
    }
    if (tool == QStringLiteral("manufacturerTestFlight")) {
        return QStringLiteral("manufacturer_test_flight");
    }
    if (tool == QStringLiteral("manualTestMode")) {
        return QStringLiteral("manual_test_mode");
    }
    if (tool == QStringLiteral("vehicleReleaseLock")) {
        return QStringLiteral("vehicle_release_lock");
    }
    if (tool == QStringLiteral("firmwareManager")) {
        return QStringLiteral("firmware_manager");
    }
    return {};
}

QString AppState::defaultManufacturerTool() const
{
    const QStringList candidates{
        QStringLiteral("vehicleConfiguration"),
        QStringLiteral("vehicleProfile"),
        QStringLiteral("flightControllerBinding"),
        QStringLiteral("vehicleParameters"),
        QStringLiteral("rcMapping"),
        QStringLiteral("manufacturerTestFlight"),
        QStringLiteral("manualTestMode"),
        QStringLiteral("vehicleReleaseLock"),
        QStringLiteral("firmwareManager")
    };
    for (const QString &tool : candidates) {
        const QString action = actionForManufacturerTool(tool);
        if (!action.isEmpty() && m_access && m_access->canPerform(action)) {
            return tool;
        }
    }
    return QStringLiteral("vehicleConfiguration");
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
