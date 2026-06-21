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
    property bool advancedMissionPanelOpen: false
    readonly property string activeMissionType: missionStore.plan.missionType.length > 0 ? missionStore.plan.missionType : appState.currentMissionType
    readonly property bool rawMissionUploadMode: advancedMissionManager.useForUpload && advancedMissionManager.missionItems.length > 0
    readonly property bool compactLayout: width < 1180 || height < 780
    readonly property bool crampedPlanningControls: width < 900 && planningPanelExpanded
    readonly property real sideInset: compactLayout ? 18 : 34
    readonly property real sidebarWidth: planningPanelExpanded ? Math.max(compactLayout ? 312 : 350, Math.min(compactLayout ? 360 : 420, width * (compactLayout ? 0.32 : 0.34))) : 0
    readonly property real panelWidth: planningPanelExpanded
                                       ? Math.min(compactLayout ? 340 : 420,
                                                  Math.max(compactLayout ? 300 : 360,
                                                           width - sideInset * 2 - (compactLayout ? 92 : 150)))
                                       : 0

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
        if (!permissionManager.canStartMission) {
            failPrepareAndStart("Your role cannot start autonomous missions.")
            return
        }
        if (!permissionManager.canUploadMission) {
            failPrepareAndStart("Your role cannot upload missions to aircraft.")
            return
        }

        startFlowActive = true
        startFlowStep = "localValidation"
        pilotAutoStartPending = false
        waitingForUploadBeforePilotStart = false
        vehicleRetryAttempts = 0
        focusMissionStart()
        if (root.rawMissionUploadMode) {
            startFlowStep = "preflight"
            setStartFlowMessage("Advanced MAVLink mission table selected for aircraft upload.")
            continuePrepareAndStart()
            return
        }
        setStartFlowMessage("Mission ready for validation.")
        missionStore.plan.validateMission()

        if (!missionStore.plan.hasTakeoffPoint) {
            failPrepareAndStart("Set a takeoff point before starting the mission.")
            return
        }
        if (missionStore.plan.generatedRoute.length < 2) {
            failPrepareAndStart("Add at least two route points before starting.")
            return
        }
        if (typeof homePositionManager !== "undefined" && missionStore.plan.hasTakeoffPoint) {
            homePositionManager.setHome(missionStore.plan.takeoffPoint.latitude,
                                        missionStore.plan.takeoffPoint.longitude,
                                        "mission_takeoff")
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
            if (!accessManager.canPerform("mission_save")) {
                failPrepareAndStart("Mission save blocked by local permissions.")
                return
            }
            setStartFlowMessage("Saving mission to Control Center.")
            missionSyncManager.saveActiveMission()
            return
        }
        if (startFlowStep === "backendValidation") {
            if (!accessManager.canPerform("mission_validation")) {
                failPrepareAndStart("Mission validation blocked by local permissions.")
                return
            }
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
            if (!accessManager.canPerform("aircraft_connection")) {
                failPrepareAndStart("Aircraft connection blocked by local permissions.")
                return
            }
            if (!vehicleManager.connected) {
                if (vehicleRetryAttempts >= 3) {
                    failPrepareAndStart("Aircraft not connected. Connect a flight controller or simulator before starting.")
                    return
                }

                vehicleRetryAttempts += 1
                setStartFlowMessage("Aircraft not connected. Connect a flight controller or simulator before starting. Retrying (" + vehicleRetryAttempts + "/3).")
                vehicleManager.connectRetry()
                vehicleRetryTimer.restart()
                return
            }

            vehicleRetryTimer.stop()
            vehicleRetryAttempts = 0
            startFlowStep = root.rawMissionUploadMode || missionStore.plan.uploadState !== "uploaded" ? "upload" : "pilotMode"
            continuePrepareAndStart()
            return
        }
        if (startFlowStep === "upload") {
            if (!accessManager.canPerform("mission_upload")) {
                failPrepareAndStart("Mission upload blocked by local permissions.")
                return
            }
            waitingForUploadBeforePilotStart = true
            setStartFlowMessage("Uploading mission to aircraft.")
            missionUploadManager.uploadActiveMission()
            return
        }
        if (startFlowStep === "pilotMode") {
            if (!accessManager.canPerform("mission_start")) {
                failPrepareAndStart("Mission start blocked by local permissions.")
                return
            }
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
        if (!accessManager.canPerform("manual_flight")) {
            manualControlManager.neutral()
            showWarning("Your role cannot fly manually.")
            return
        }
        var forward = (keyForward ? 1 : 0) + (keyBack ? -1 : 0)
        var lateral = (keyRight ? 1 : 0) + (keyLeft ? -1 : 0)
        var vertical = (keyUp ? 1 : 0) + (keyDown ? -1 : 0)
        var yaw = (keyYawRight ? 1 : 0) + (keyYawLeft ? -1 : 0)
        manualControlManager.setInput(forward, lateral, vertical, yaw)
    }

    function importMissionFile(fileUrl) {
        if (!accessManager.canPerform("mission_planning")) {
            missionStore.plan.importStatus = "Mission import blocked by local permissions."
            return
        }
        if (missionStore.plan.importMissionFile(String(fileUrl))) {
            appState.selectedTool = "select"
            missionStore.plan.validateMission()
        }
    }

    function captureMissionPreview(stage) {
        if (typeof missionPreviewManager === "undefined"
                || missionStore.plan.createdLocally
                || String(missionStore.plan.missionId).length === 0
                || map.width < 80
                || map.height < 80) {
            return
        }
        var path = missionPreviewManager.previewPath(String(missionStore.plan.missionId), stage)
        var targetWidth = Math.min(1280, Math.max(320, Math.round(map.width)))
        var targetHeight = Math.min(720, Math.max(180, Math.round(map.height)))
        map.grabToImage(function(result) {
            if (result && result.saveToFile(path)) {
                missionPreviewManager.syncPreview(String(missionStore.plan.missionId),
                                                  stage,
                                                  path,
                                                  targetWidth,
                                                  targetHeight)
            }
        }, Qt.size(targetWidth, targetHeight))
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
            root.captureMissionPreview("uploaded")
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
            root.captureMissionPreview("saved")
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
            root.captureMissionPreview("validated")
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
            if (!permissionManager.canFlyManual) {
                showWarning("Your role cannot command manual hold.")
                event.accepted = true
                return
            }
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
        visible: accessManager.can("can_plan_mission")
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

    Rectangle {
        id: advancedMissionButton
        objectName: "advancedMissionEditorButton"
        anchors.left: root.crampedPlanningControls ? parent.left : importButton.right
        anchors.top: root.crampedPlanningControls ? importButton.bottom : importButton.top
        anchors.leftMargin: root.crampedPlanningControls ? root.sideInset : 12
        anchors.topMargin: root.crampedPlanningControls ? 8 : 0
        width: Math.max(198, advancedMissionButtonText.implicitWidth + 42)
        height: importButton.height
        radius: 8
        color: root.advancedMissionPanelOpen ? Theme.purple2 : (advancedMissionButtonMouse.containsMouse ? "#100226" : "#432a6b")
        border.color: root.rawMissionUploadMode ? Theme.amber : "#ffffff33"
        border.width: root.rawMissionUploadMode ? 2 : 1
        z: 9
        visible: accessManager.can("can_plan_mission")
        Row {
            anchors.centerIn: parent
            spacing: 8
            Text {
                text: "MAV"
                color: root.rawMissionUploadMode ? Theme.amber : Theme.white
                font.pixelSize: 12
                font.bold: true
            }
            Text {
                id: advancedMissionButtonText
                text: root.rawMissionUploadMode ? "Raw Mission Active" : "Mission Command Editor"
                color: Theme.white
                font.pixelSize: 12
                font.bold: true
            }
        }
        MouseArea {
            id: advancedMissionButtonMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: root.advancedMissionPanelOpen = !root.advancedMissionPanelOpen
        }
        ToolTip.visible: advancedMissionButtonMouse.containsMouse
        ToolTip.text: "Open MAVLink command table, geofence, rally, read/write, compare, and file tools"
    }

    GlassPanel {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: root.sideInset
        anchors.topMargin: root.crampedPlanningControls ? 100 : (root.compactLayout ? 58 : 70)
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
        anchors.topMargin: root.crampedPlanningControls ? 222 : (root.compactLayout ? 190 : 260)
        width: root.compactLayout ? 68 : 84
        height: Math.max(root.compactLayout ? 312 : 420,
                         Math.min(root.compactLayout ? 408 : 520,
                                  parent.height - anchors.topMargin - (root.compactLayout ? 92 : 120)))
        radius: 8
        color: "#ffffff"
        clip: true
        z: 8
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: root.compactLayout ? 8 : 12
            spacing: root.compactLayout ? 6 : 8
            MissionToolButton { Layout.fillWidth: true; Layout.preferredHeight: root.compactLayout ? 34 : 48; iconSource: AssetRegistry.icons.boxicons_cursor_pointer; tool: "select" }
            MissionToolButton { Layout.fillWidth: true; Layout.preferredHeight: root.compactLayout ? 34 : 48; visible: accessManager.can("can_plan_mission"); iconSource: AssetRegistry.icons.plane; tool: "takeoff"; label: "Takeoff" }
            MissionToolButton { Layout.fillWidth: true; Layout.preferredHeight: root.compactLayout ? 34 : 48; visible: accessManager.can("can_plan_mission"); iconSource: AssetRegistry.icons.pin_location; tool: "point" }
            MissionToolButton { Layout.fillWidth: true; Layout.preferredHeight: root.compactLayout ? 34 : 48; visible: accessManager.can("can_plan_mission"); iconSource: AssetRegistry.icons.lucide_waypoints; tool: "poi" }
            MissionToolButton { Layout.fillWidth: true; Layout.preferredHeight: root.compactLayout ? 34 : 48; visible: accessManager.can("can_plan_mission"); iconSource: AssetRegistry.icons.lucide_route; tool: "route" }
            MissionToolButton { Layout.fillWidth: true; Layout.preferredHeight: root.compactLayout ? 34 : 48; visible: accessManager.can("can_plan_mission"); iconSource: AssetRegistry.icons.lucide_grid_3x3; tool: "polygon" }
            IconButton {
                Layout.fillWidth: true
                Layout.preferredHeight: root.compactLayout ? 34 : 48
                visible: accessManager.can("can_plan_mission")
                iconSource: AssetRegistry.icons.boxicons_undo
                onClicked: missionStore.plan.undoLastGeometry()
                ToolTip.visible: hovered
                ToolTip.text: "Undo last geometry"
            }
            IconButton {
                Layout.fillWidth: true
                Layout.preferredHeight: root.compactLayout ? 34 : 48
                visible: accessManager.can("can_plan_mission")
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
                Layout.preferredHeight: root.compactLayout ? 34 : 48
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
        objectName: "missionElevationProfile"
        anchors.left: parent.left
        anchors.right: root.planningPanelExpanded ? panel.left : parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: root.sideInset
        anchors.rightMargin: root.compactLayout ? 88 : 128
        anchors.bottomMargin: 24
        height: root.elevationProfileOpen ? (root.compactLayout ? 108 : 132) : 0
        visible: root.elevationProfileOpen
        z: 7
    }

    AdvancedMissionEditorPanel {
        id: advancedMissionPanel
        objectName: "advancedMissionEditorPanel"
        anchors.left: parent.left
        anchors.right: root.planningPanelExpanded ? panel.left : parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: root.sideInset
        anchors.rightMargin: root.planningPanelExpanded ? 12 : root.sideInset
        anchors.bottomMargin: root.elevationProfileOpen ? (root.compactLayout ? 140 : 168) : 24
        height: Math.min(parent.height - 120, root.compactLayout ? 360 : 462)
        visible: root.advancedMissionPanelOpen
        enabled: visible
        z: 26
        onCloseRequested: root.advancedMissionPanelOpen = false
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
        width: root.panelWidth
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

    MouseArea {
        id: panelInputShield
        anchors.fill: panel
        visible: panel.visible
        enabled: visible
        z: panel.z - 1
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        preventStealing: true
        propagateComposedEvents: false
        onPressed: function(mouse) { mouse.accepted = true }
        onReleased: function(mouse) { mouse.accepted = true }
        onClicked: function(mouse) { mouse.accepted = true }
        onDoubleClicked: function(mouse) { mouse.accepted = true }
        onPositionChanged: function(mouse) { mouse.accepted = true }
        onWheel: function(wheel) { wheel.accepted = true }
    }

    Rectangle {
        id: pilotWarningToast
        objectName: "pilotModeWarningToast"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.elevationProfileOpen ? (root.compactLayout ? 150 : 190) : 56
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
