import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"
import "../map"

Item {
    id: root
    objectName: "missionWorkspace"
    focus: true
    property bool planningPanelExpanded: true
    property bool keyForward: false
    property bool keyBack: false
    property bool keyYawLeft: false
    property bool keyYawRight: false
    property bool keyLeft: false
    property bool keyRight: false
    property bool keyUp: false
    property bool keyDown: false
    property bool preflightModalOpen: false
    property bool elevationProfileOpen: true
    property int vehicleRetryAttempts: 0
    property bool pilotAutoStartPending: false
    property bool waitingForUploadBeforePilotStart: false
    property bool startFlowActive: false
    property string startFlowStep: "idle"
    property string pilotWarningText: ""
    property bool pilotWarningVisible: false
    readonly property string activeMissionType: missionStore.plan.missionType.length > 0 ? missionStore.plan.missionType : appState.currentMissionType
    readonly property bool compactLayout: width < 1180 || height < 780
    readonly property real sideInset: compactLayout ? 18 : 34
    readonly property real sidebarWidth: planningPanelExpanded ? Math.max(compactLayout ? 312 : 350, Math.min(compactLayout ? 360 : 420, width * (compactLayout ? 0.32 : 0.34))) : 0

    function recordPilotFlow(actionType, message) {
        var payload = {
            mission_id: missionStore.plan.missionId,
            upload_state: missionStore.plan.uploadState,
            vehicle_connected: vehicleManager.connected,
            retry_attempt: root.vehicleRetryAttempts
        }
        if (typeof gcsEventSyncManager !== "undefined") {
            gcsEventSyncManager.recordEvent(actionType,
                                            actionType.indexOf("failed") >= 0 || actionType.indexOf("blocked") >= 0 ? "warning" : "info",
                                            message,
                                            payload)
        }
        if (typeof pilotActionSyncManager !== "undefined") {
            pilotActionSyncManager.recordAction(actionType, message, payload)
        } else if (typeof eventLogManager !== "undefined") {
            eventLogManager.logEvent(actionType, "info", message, payload)
        }
    }

    function showWarning(message) {
        pilotWarningText = message
        pilotWarningVisible = true
        warningTimer.restart()
    }

    function focusMissionStart() {
        if (missionStore.plan.hasTakeoffPoint) {
            mapState.centerLatitude = missionStore.plan.takeoffPoint.latitude
            mapState.centerLongitude = missionStore.plan.takeoffPoint.longitude
            mapState.zoomLevel = Math.max(mapState.zoomLevel, 17)
            return
        }
        var first = missionStore.plan.waypointAt(0)
        if (first && first.latitude !== undefined && first.longitude !== undefined) {
            mapState.centerLatitude = first.latitude
            mapState.centerLongitude = first.longitude
            mapState.zoomLevel = Math.max(mapState.zoomLevel, 16)
        }
    }

    function setStartFlowMessage(message) {
        showWarning(message)
        recordPilotFlow("prepare_start_status", message)
    }

    function failPrepareAndStart(message) {
        startFlowActive = false
        startFlowStep = "idle"
        pilotAutoStartPending = false
        waitingForUploadBeforePilotStart = false
        vehicleRetryTimer.stop()
        vehicleRetryAttempts = 0
        showWarning(message)
        recordPilotFlow("prepare_start_failed", message)
    }

    function prepareAndStartMission() {
        if (startFlowActive) {
            setStartFlowMessage(pilotWarningText.length > 0 ? pilotWarningText : "Preparing mission start.")
            return
        }
        if (missionStore.plan.boundaryOnly) {
            showWarning("Virtual Fence is a Control Center safety boundary and cannot start as a route mission.")
            recordPilotFlow("pilot_mode_start_blocked", "Pilot Mode start blocked for boundary-only mission")
            return
        }
        if (missionExecutionManager.executing) {
            showWarning("Mission is already executing.")
            return
        }
        if (!sessionManager.operationsAllowed) {
            failPrepareAndStart("Device approval required before aircraft upload.")
            return
        }

        startFlowActive = true
        startFlowStep = "localValidation"
        pilotAutoStartPending = false
        waitingForUploadBeforePilotStart = false
        vehicleRetryAttempts = 0
        focusMissionStart()
        setStartFlowMessage("Mission ready for validation.")
        missionStore.plan.validateMission()

        if (!missionStore.plan.hasTakeoffPoint) {
            failPrepareAndStart("Set a takeoff point before starting the mission.")
            return
        }
        if (missionStore.plan.serializeForMavsdkMission().length < 2) {
            failPrepareAndStart("Add at least two route points before starting.")
            return
        }
        if (!missionStore.plan.missionReady) {
            failPrepareAndStart(missionStore.plan.operationStatus.length > 0
                                ? missionStore.plan.operationStatus
                                : "Resolve mission validation issues before starting.")
            return
        }

        startFlowStep = "saveSync"
        continuePrepareAndStart()
    }

    function continuePrepareAndStart() {
        if (!startFlowActive) {
            return
        }
        if (startFlowStep === "saveSync") {
            setStartFlowMessage("Saving mission to Control Center.")
            missionSyncManager.saveActiveMission()
            return
        }
        if (startFlowStep === "backendValidation") {
            setStartFlowMessage("Backend validation required.")
            missionSyncManager.validateActiveMission()
            return
        }
        if (startFlowStep === "preflight") {
            setStartFlowMessage("Running preflight checks.")
            preflightModalOpen = true
            return
        }
        if (startFlowStep === "aircraftConnection") {
            if (!vehicleManager.connected) {
                if (vehicleRetryAttempts >= 3) {
                    failPrepareAndStart("Aircraft not connected. Please connect Gazebo/PX4 before starting.")
                    return
                }

                vehicleRetryAttempts += 1
                setStartFlowMessage("Aircraft not connected. Please connect Gazebo/PX4 before starting. Retrying (" + vehicleRetryAttempts + "/3).")
                vehicleManager.connectRetry()
                vehicleRetryTimer.restart()
                return
            }

            vehicleRetryTimer.stop()
            vehicleRetryAttempts = 0
            startFlowStep = missionStore.plan.uploadState === "uploaded" ? "pilotMode" : "upload"
            continuePrepareAndStart()
            return
        }
        if (startFlowStep === "upload") {
            waitingForUploadBeforePilotStart = true
            setStartFlowMessage("Uploading mission to aircraft.")
            missionUploadManager.uploadActiveMission()
            return
        }
        if (startFlowStep === "pilotMode") {
            setStartFlowMessage("Starting autonomous flight.")
            appState.operationalMode = "pilot"
            startFlowStep = "execute"
            missionExecutionManager.startMission()
        }
    }

    function enterPilotMode() {
        prepareAndStartMission()
    }

    function updateManualInput() {
        if (appState.operationalMode !== "pilot") {
            return
        }
        var forward = (keyForward ? 1 : 0) + (keyBack ? -1 : 0)
        var lateral = (keyRight ? 1 : 0) + (keyLeft ? -1 : 0)
        var vertical = (keyUp ? 1 : 0) + (keyDown ? -1 : 0)
        var yaw = (keyYawRight ? 1 : 0) + (keyYawLeft ? -1 : 0)
        manualControlManager.setInput(forward, lateral, vertical, yaw)
    }

    function importMissionFile(fileUrl) {
        if (missionStore.plan.importMissionFile(String(fileUrl))) {
            appState.selectedTool = "select"
            missionStore.plan.validateMission()
        }
    }

    Component.onCompleted: forceActiveFocus()

    Timer {
        id: vehicleRetryTimer
        interval: 2000
        repeat: false
        onTriggered: {
            if (root.startFlowActive && root.startFlowStep === "aircraftConnection") {
                root.continuePrepareAndStart()
            }
        }
    }

    Timer {
        id: warningTimer
        interval: 4200
        repeat: false
        onTriggered: root.pilotWarningVisible = false
    }

    Connections {
        target: missionUploadManager
        function onMissionUploaded() {
            if (root.startFlowActive && root.startFlowStep === "upload") {
                root.waitingForUploadBeforePilotStart = false
                root.setStartFlowMessage("Mission uploaded successfully.")
                root.startFlowStep = "pilotMode"
                root.continuePrepareAndStart()
            }
        }

        function onMissionUploadFailed(message) {
            if (root.startFlowActive && root.startFlowStep === "upload") {
                root.failPrepareAndStart(message)
            }
        }
    }

    Connections {
        target: missionSyncManager
        function onActiveMissionSaved(success, message) {
            if (!root.startFlowActive || root.startFlowStep !== "saveSync") {
                return
            }
            if (!success) {
                root.failPrepareAndStart(message)
                return
            }
            root.startFlowStep = "backendValidation"
            root.continuePrepareAndStart()
        }

        function onActiveMissionValidated(success, message) {
            if (!root.startFlowActive || root.startFlowStep !== "backendValidation") {
                return
            }
            if (!success) {
                root.failPrepareAndStart(message)
                return
            }
            root.setStartFlowMessage(message.length > 0 ? message : "Mission validated by Control Center.")
            root.startFlowStep = "preflight"
            root.continuePrepareAndStart()
        }
    }

    Connections {
        target: preflightChecklistManager
        function onChecklistCompleted(success, message) {
            if (!root.startFlowActive || root.startFlowStep !== "preflight") {
                return
            }
            if (!success) {
                root.preflightModalOpen = true
                root.setStartFlowMessage(message.length > 0 ? message : "Preflight failed.")
                return
            }
            root.preflightModalOpen = false
            root.startFlowStep = "aircraftConnection"
            root.continuePrepareAndStart()
        }
    }

    Connections {
        target: missionExecutionManager
        function onMissionStarted() {
            if (root.startFlowActive && root.startFlowStep === "execute") {
                root.startFlowActive = false
                root.startFlowStep = "idle"
                root.setStartFlowMessage("Starting autonomous flight.")
            }
        }

        function onMissionStartFailed(message) {
            if (root.startFlowActive && root.startFlowStep === "execute") {
                root.failPrepareAndStart(message)
            }
        }
    }

    Connections {
        target: vehicleManager
        function onSystemReady() {
            if (root.startFlowActive && root.startFlowStep === "aircraftConnection" && !vehicleRetryTimer.running) {
                root.continuePrepareAndStart()
            }
        }
    }

    Keys.onPressed: function(event) {
        if (appState.operationalMode !== "pilot" || event.isAutoRepeat) {
            return
        }
        if (event.key === Qt.Key_W) keyForward = true
        else if (event.key === Qt.Key_S) keyBack = true
        else if (event.key === Qt.Key_A) keyYawLeft = true
        else if (event.key === Qt.Key_D) keyYawRight = true
        else if (event.key === Qt.Key_Left) keyLeft = true
        else if (event.key === Qt.Key_Right) keyRight = true
        else if (event.key === Qt.Key_Q) keyDown = true
        else if (event.key === Qt.Key_E) keyUp = true
        else if (event.key === Qt.Key_H) {
            manualControlManager.neutral()
            vehicleActionManager.holdPosition()
            event.accepted = true
            return
        } else {
            return
        }
        updateManualInput()
        event.accepted = true
    }
    Keys.onReleased: function(event) {
        if (appState.operationalMode !== "pilot" || event.isAutoRepeat) {
            return
        }
        if (event.key === Qt.Key_W) keyForward = false
        else if (event.key === Qt.Key_S) keyBack = false
        else if (event.key === Qt.Key_A) keyYawLeft = false
        else if (event.key === Qt.Key_D) keyYawRight = false
        else if (event.key === Qt.Key_Left) keyLeft = false
        else if (event.key === Qt.Key_Right) keyRight = false
        else if (event.key === Qt.Key_Q) keyDown = false
        else if (event.key === Qt.Key_E) keyUp = false
        else {
            return
        }
        updateManualInput()
        event.accepted = true
    }

    MissionMap {
        id: map
        objectName: "missionPlanningMap"
        anchors.fill: parent
    }

    DropArea {
        id: importDropArea
        objectName: "missionImportDropArea"
        anchors.fill: parent
        keys: ["text/uri-list"]
        z: 2
        onDropped: function(drop) {
            if (drop.hasUrls && drop.urls.length > 0) {
                root.importMissionFile(drop.urls[0])
                drop.acceptProposedAction()
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        visible: importDropArea.containsDrag
        z: 30
        color: "#4b128b33"
        border.color: Theme.amber
        border.width: 2
        Text {
            anchors.centerIn: parent
            text: "Drop KML or GeoJSON to import mission geometry"
            color: Theme.white
            font.pixelSize: root.compactLayout ? 14 : 18
            font.bold: true
        }
    }

    Rectangle {
        id: importButton
        objectName: "missionImportButton"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: root.sideInset
        anchors.topMargin: root.compactLayout ? 18 : 28
        width: Math.max(176, importButtonText.implicitWidth + 44)
        height: root.compactLayout ? 32 : 36
        radius: 8
        color: importMouse.containsMouse ? Theme.purple2 : Theme.purple
        border.color: "#ffffff33"
        border.width: 1
        z: 9
        Row {
            anchors.centerIn: parent
            spacing: 8
            Text { text: "+"; color: Theme.white; font.pixelSize: 16; font.bold: true }
            Text {
                id: importButtonText
                text: "Import KML / GeoJSON"
                color: Theme.white
                font.pixelSize: 12
                font.bold: true
            }
        }
        MouseArea {
            id: importMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                var path = missionStore.plan.chooseImportFile()
                if (path.length > 0) {
                    root.importMissionFile(path)
                }
            }
        }
        ToolTip.visible: importMouse.containsMouse
        ToolTip.text: "Import waypoints, routes, polygons, or POIs"
    }

    GlassPanel {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: root.sideInset
        anchors.topMargin: root.compactLayout ? 58 : 70
        width: root.compactLayout ? 184 : 204
        height: root.compactLayout ? 104 : 126
        color: Theme.glassSoft
        z: 8
        Column {
            anchors.fill: parent
            anchors.margins: root.compactLayout ? 10 : 14
            spacing: root.compactLayout ? 5 : 8
            Text { text: appState.operationalMode === "pilot" ? "Mode: Pilot / Direct Flight" : "Mission: " + catalog.titleFor(root.activeMissionType); color: Theme.white; font.pixelSize: root.compactLayout ? 12 : 14; font.bold: true }
            Text { text: appState.operationalMode === "pilot" ? "Aircraft: " + telemetryStore.aircraftId : "Area: " + Number(missionStore.plan.missionAreaHa).toFixed(2) + " ha"; color: "#efe7f7"; font.pixelSize: root.compactLayout ? 12 : 14; elide: Text.ElideRight; width: parent.width }
            Text { text: appState.operationalMode === "pilot" ? "Flight: " + telemetryStore.flightMode + " · " + (telemetryStore.armed ? "ARMED" : "DISARMED") : "Alt: " + Number(missionStore.plan.altitude).toFixed(0) + "m AGL"; color: "#efe7f7"; font.pixelSize: root.compactLayout ? 12 : 14; elide: Text.ElideRight; width: parent.width }
            Text { text: missionStore.plan.importStatus; visible: appState.operationalMode !== "pilot"; color: "#efe7f7"; font.pixelSize: 11; elide: Text.ElideRight; width: parent.width }
        }
    }

    MissionCatalog { id: catalog }

    Rectangle {
        id: toolRail
        objectName: "missionToolRail"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: root.sideInset
        anchors.topMargin: root.compactLayout ? 190 : 260
        width: root.compactLayout ? 68 : 84
        height: root.compactLayout ? 440 : 520
        radius: 8
        color: "#ffffff"
        z: 8
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: root.compactLayout ? 8 : 12
            spacing: root.compactLayout ? 6 : 8
            MissionToolButton { Layout.fillWidth: true; iconSource: AssetRegistry.icons.boxicons_cursor_pointer; tool: "select" }
            MissionToolButton { Layout.fillWidth: true; iconSource: AssetRegistry.icons.plane; tool: "takeoff"; label: "Takeoff" }
            MissionToolButton { Layout.fillWidth: true; iconSource: AssetRegistry.icons.pin_location; tool: "point" }
            MissionToolButton { Layout.fillWidth: true; iconSource: AssetRegistry.icons.lucide_waypoints; tool: "poi" }
            MissionToolButton { Layout.fillWidth: true; iconSource: AssetRegistry.icons.lucide_route; tool: "route" }
            MissionToolButton { Layout.fillWidth: true; iconSource: AssetRegistry.icons.lucide_grid_3x3; tool: "polygon" }
            IconButton {
                Layout.fillWidth: true
                iconSource: AssetRegistry.icons.boxicons_undo
                onClicked: missionStore.plan.undoLastGeometry()
                ToolTip.visible: hovered
                ToolTip.text: "Undo last geometry"
            }
            IconButton {
                Layout.fillWidth: true
                iconSource: AssetRegistry.icons.boxicons_trash
                onClicked: {
                    if (appState.selectedWaypointIndex >= 0) {
                        missionStore.plan.deleteWaypoint(appState.selectedWaypointIndex)
                        appState.selectedWaypointIndex = -1
                    } else {
                        missionStore.plan.clearDraftGeometry()
                    }
                }
                ToolTip.visible: hovered
                ToolTip.text: appState.selectedWaypointIndex >= 0 ? "Delete selected waypoint" : "Clear mission geometry"
            }
            IconButton {
                Layout.fillWidth: true
                iconSource: AssetRegistry.icons.lucide_mountain
                active: root.elevationProfileOpen
                iconOnAccent: root.elevationProfileOpen
                onClicked: root.elevationProfileOpen = !root.elevationProfileOpen
                ToolTip.visible: hovered
                ToolTip.text: root.elevationProfileOpen ? "Hide elevation profile" : "Show elevation profile"
            }
        }
    }

    MissionElevationProfile {
        y: 924
        objectName: "missionElevationProfile"
        anchors.left: parent.left
        anchors.right: root.planningPanelExpanded ? panel.left : parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 34
        anchors.rightMargin: 128
        anchors.bottomMargin: 24
        height: root.elevationProfileOpen ? (root.compactLayout ? 108 : 132) : 0
        visible: root.elevationProfileOpen
        z: 7
    }

    MapControls {
        objectName: "missionMapControls"
        anchors.right: parent.right
        anchors.rightMargin: root.planningPanelExpanded ? panel.width + (root.compactLayout ? 42 : 64) : root.sideInset
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.compactLayout ? 54 : 76
        z: 8
    }

    IconButton {
        id: panelToggle
        objectName: "missionConfigPanelToggle"
        anchors.right: root.planningPanelExpanded ? panel.left : parent.right
        anchors.rightMargin: root.planningPanelExpanded ? 12 : 24
        anchors.top: parent.top
        anchors.topMargin: root.compactLayout ? 22 : 32
        z: 12
        implicitWidth: root.compactLayout ? 36 : 42
        implicitHeight: root.compactLayout ? 36 : 42
        iconText: root.planningPanelExpanded ? "›" : "‹"
        active: true
        iconOnAccent: root.planningPanelExpanded
        onClicked: root.planningPanelExpanded = !root.planningPanelExpanded
        ToolTip.visible: hovered
        ToolTip.text: root.planningPanelExpanded ? "Collapse mission panel" : "Expand mission panel"
    }

    MissionConfigPanel {
        id: panel
        width:500
        objectName: "missionConfigPanel"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: root.compactLayout ? 16 : 28
        anchors.rightMargin: root.compactLayout ? 16 : 24
        anchors.bottomMargin: root.compactLayout ? 16 : 24
        visible: root.planningPanelExpanded
        enabled: root.planningPanelExpanded
        z: 10
        onRequestPreflight: root.preflightModalOpen = true
        onRequestPrepareAndStart: root.prepareAndStartMission()
    }

    Rectangle {
        id: pilotWarningToast
        objectName: "pilotModeWarningToast"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.elevationProfileOpen ? 190 : 56
        width: Math.min(parent.width - 72, Math.max(360, pilotWarningTextItem.implicitWidth + 48))
        height: Math.max(46, pilotWarningTextItem.implicitHeight + 22)
        radius: 8
        color: "#fff6d8"
        border.color: "#efd06c"
        border.width: 1
        visible: root.pilotWarningVisible
        opacity: visible ? 1 : 0
        z: 60

        Text {
            id: pilotWarningTextItem
            anchors.fill: parent
            anchors.margins: 12
            text: root.pilotWarningText
            color: Theme.ink
            font.pixelSize: 13
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
        }
    }

    PreflightChecklistModal {
        id: preflightModal
        objectName: "preflightChecklistModal"
        anchors.fill: parent
        visible: root.preflightModalOpen
        onDismissed: {
            root.preflightModalOpen = false
            if (root.startFlowActive && root.startFlowStep === "preflight") {
                root.failPrepareAndStart("Preflight checklist is required before starting.")
            }
        }
        onChecklistCompleted: {
            root.preflightModalOpen = false
            if (root.startFlowActive && root.startFlowStep === "preflight") {
                root.startFlowStep = "aircraftConnection"
                root.continuePrepareAndStart()
            }
        }
    }
}
