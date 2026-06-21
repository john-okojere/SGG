import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtMultimedia
import "../vehicle"
import "../manufacturer"
import "../mission"

Rectangle {
    id: root
    color: "#f6f4fa"

    property string selectedTool: appState.currentGcsTool.length > 0 ? appState.currentGcsTool : "connect"
    readonly property bool compact: width < 1120
    readonly property bool narrow: width < 860
    property bool videoExpanded: false
    property string streamUrlDraft: videoStreamManager.streamUrl
    property string streamUserDraft: videoStreamManager.username
    property string streamPasswordDraft: videoStreamManager.password
    property string rtcmDraft: ""
    property string terminalDraft: ""
    property string sikPortDraft: optionalHardwareManager.sikPort
    property string sikBaudDraft: String(optionalHardwareManager.sikBaud)
    property string sikNetIdDraft: ""
    property string sikAirSpeedDraft: ""
    property string trackerLatDraft: "0"
    property string trackerLonDraft: "0"
    property string trackerAltDraft: "0"
    readonly property bool diagnosticsEnabled: typeof rbacDiagnosticsEnabled !== "undefined" && rbacDiagnosticsEnabled

    function toolTitle(key) {
        var tool = gcsToolCatalog.toolForKey(key)
        return tool.title || "GCS Tools"
    }

    function toolDescription(key) {
        var tool = gcsToolCatalog.toolForKey(key)
        return tool.description || "Standalone setup, flight, payload, and diagnostics workspace."
    }

    function selectTool(key, available) {
        if (!available)
            return
        appState.openGcsTool(key)
    }

    function ensureAllowedSelectedTool() {
        if (gcsToolCatalog.availableCount > 0 && !gcsToolCatalog.canOpenTool(root.selectedTool))
            appState.openGcsTools()
    }

    function setupGroupForTool(key) {
        if (key === "flightModes") return "Flight Modes"
        if (key === "rcRadio") return "RC"
        if (key === "escSetup") return "Motor/ESC"
        if (key === "servoSetup") return "Servo"
        if (key === "batterySetup") return "Battery"
        if (key === "failsafeSetup") return "Failsafe"
        if (key === "airframeSetup") return "Airframe"
        return ""
    }

    function synchronizeToolSelection() {
        var group = setupGroupForTool(selectedTool)
        if (group.length > 0)
            parameterManager.selectedGroup = group
        if (selectedTool === "parametersTuning" || selectedTool === "configurationTuning")
            parameterManager.selectedGroup = "All"
        if (selectedTool === "optionalHardware" && optionalHardwareManager.activeTool.length === 0)
            optionalHardwareManager.selectTool("rtkGpsInject")
        if (isOptionalTool(selectedTool))
            optionalHardwareManager.selectTool(selectedTool)
    }

    function isOptionalTool(key) {
        return key === "optionalHardware" || key === "rtkGpsInject" || key === "sikRadio" || key === "droneCan"
            || key === "joystick" || key === "px4flow" || key === "bluetoothSetup"
            || key === "antennaTracker"
    }

    function optionIndex(options, value) {
        for (var i = 0; i < options.length; ++i) {
            if (String(options[i].value) === String(value))
                return i
        }
        return -1
    }

    function listIndex(values, value) {
        for (var i = 0; i < values.length; ++i) {
            if (String(values[i]) === String(value))
                return i
        }
        return 0
    }

    function componentForTool(key) {
        if (key === "dashboard") return dashboardPanel
        if (key === "flightData") return flightDataPanel
        if (key === "missionPlanner") return missionPanel
        if (key === "multiVehicle") return multiVehiclePanel
        if (key === "installFirmware" || key === "installFirmwareLegacy" || key === "firmwareManager") return firmwarePanel
        if (key === "connect") return connectPanel
        if (key === "initialSetup") return setupBenchPanel
        if (key === "parametersTuning" || key === "configurationTuning" || setupGroupForTool(key).length > 0) return parameterPanel
        if (isOptionalTool(key) || key === "terminal") return optionalHardwarePanel
        if (key === "payloadCamera" || key === "payloadGimbal" || key === "payloadVideo"
                || key === "geotagging" || key === "mappingOverlap" || key === "payloadTools") return payloadPanel
        if (key === "mavlinkInspector" || key === "messageSender" || key === "mavftp"
                || key === "signing" || key === "serialPassthrough" || key === "boardInfo"
                || key === "advancedTools") return advancedPanel
        if (key === "logsAnalysis") return logsPanel
        if (key === "simulation") return simulationPanel
        if (key === "commandCenterSync") return syncPanel
        return setupBenchPanel
    }

    function joined(value) {
        return value && value.length > 0 ? value.join(", ") : "none"
    }

    function rbacDiagnosticRows() {
        var d = accessManager.diagnosticSnapshot()
        return [
            { "name": "Current Role", "value": d.role || "none" },
            { "name": "Raw Roles", "value": joined(d.raw_roles) },
            { "name": "Normalized Roles", "value": joined(d.roles) },
            { "name": "Permissions", "value": joined(d.permissions) },
            { "name": "Allowed Modules", "value": joined(d.allowed_modules) },
            { "name": "Trusted Device", "value": d.trusted_device ? "trusted" : "blocked" },
            { "name": "Session", "value": sessionManager.operationsAllowed ? "operations allowed" : sessionManager.blockReason },
            { "name": "Current Screen", "value": appState.currentScreen + " / " + appState.operationalMode },
            { "name": "Organization", "value": String(d.organization_id) },
            { "name": "Assigned Aircraft", "value": joined(d.assigned_aircraft_ids) },
            { "name": "Assigned Missions", "value": joined(d.assigned_mission_ids) },
            { "name": "Offline Authorization", "value": d.offline_authorization_valid ? "valid" : "not available" }
        ]
    }

    function rbacWorkspaceRows(visible) {
        var d = accessManager.diagnosticSnapshot()
        var rows = []
        var workspaces = d.workspaces || []
        for (var i = 0; i < workspaces.length; ++i) {
            var item = workspaces[i]
            if (!!item.visible === visible)
                rows.push({ "name": item.name, "value": visible ? "visible" : item.reason })
        }
        if (rows.length === 0)
            rows.push({ "name": "None", "value": "none" })
        return rows
    }

    function hiddenToolRows() {
        var rows = []
        var tools = gcsToolCatalog.hiddenTools()
        for (var i = 0; i < tools.length; ++i)
            rows.push({ "name": tools[i].title, "value": tools[i].reason || tools[i].statusText })
        if (rows.length === 0)
            rows.push({ "name": "None", "value": "none" })
        return rows
    }

    FileDialog {
        id: missionSaveDialog
        title: "Save Mission File"
        fileMode: FileDialog.SaveFile
        nameFilters: ["SkyGrid raw mission (*.json)", "QGC waypoint file (*.waypoints *.txt)"]
        onAccepted: advancedMissionManager.saveMissionFile(selectedFile)
    }

    FileDialog {
        id: missionLoadDialog
        title: "Load Mission File"
        fileMode: FileDialog.OpenFile
        nameFilters: ["Mission files (*.json *.waypoints *.txt)", "All files (*)"]
        onAccepted: advancedMissionManager.loadMissionFile(selectedFile)
    }

    FileDialog {
        id: kmlExportDialog
        title: "Export KML"
        fileMode: FileDialog.SaveFile
        nameFilters: ["KML files (*.kml)"]
        onAccepted: logAnalysisManager.exportKml(selectedFile)
    }

    FileDialog {
        id: gpxExportDialog
        title: "Export GPX"
        fileMode: FileDialog.SaveFile
        nameFilters: ["GPX files (*.gpx)"]
        onAccepted: logAnalysisManager.exportGpx(selectedFile)
    }

    FileDialog {
        id: tlogPlaybackDialog
        title: "Open TLog"
        fileMode: FileDialog.OpenFile
        nameFilters: ["TLog files (*.tlog)", "All files (*)"]
        onAccepted: logAnalysisManager.playbackTlogFile(selectedFile)
    }

    onSelectedToolChanged: synchronizeToolSelection()
    Component.onCompleted: {
        ensureAllowedSelectedTool()
        synchronizeToolSelection()
    }

    Connections {
        target: gcsToolCatalog
        function onToolsChanged() {
            root.ensureAllowedSelectedTool()
        }
    }

    GridLayout {
        anchors.fill: parent
        columns: root.narrow ? 1 : 2
        rowSpacing: 0
        columnSpacing: 0

        Rectangle {
            Layout.fillWidth: root.narrow
            Layout.preferredWidth: root.narrow ? root.width : (root.compact ? 300 : 330)
            Layout.preferredHeight: root.narrow ? Math.min(220, Math.max(150, root.height * 0.32)) : root.height
            Layout.minimumWidth: 0
            Layout.minimumHeight: 0
            Layout.fillHeight: !root.narrow
            color: "#ffffff"
            border.color: "#e2dceb"
            clip: true

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    Button {
                        text: "Back"
                        onClicked: appState.goHome()
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text {
                            Layout.fillWidth: true
                            text: "Setup Bench"
                            color: "#14111d"
                            font.pixelSize: 21
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        Text {
                            Layout.fillWidth: true
                            text: gcsToolCatalog.availableCount + " available tools"
                            color: "#706a7e"
                            font.pixelSize: 12
                            elide: Text.ElideRight
                        }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: availableWidth

                    ColumnLayout {
                        width: parent.width
                        spacing: 12
                        Repeater {
                            model: gcsToolCatalog.sections
                            delegate: ColumnLayout {
                                width: parent.width
                                spacing: 6
                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.name
                                    color: "#4a4259"
                                    font.pixelSize: 12
                                    font.bold: true
                                    elide: Text.ElideRight
                                }
                                Repeater {
                                    model: gcsToolCatalog.toolsForSection(modelData.name)
                                    delegate: NavItem {
                                        Layout.fillWidth: true
                                        tool: modelData
                                        selected: modelData.key === root.selectedTool
                                        onClicked: root.selectTool(modelData.key, modelData.available)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 0
            Layout.minimumHeight: 0
            color: "#f6f4fa"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: root.narrow ? 14 : 24
                spacing: root.narrow ? 12 : 16

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        Text {
                            Layout.fillWidth: true
                            text: root.toolTitle(root.selectedTool)
                            color: "#08070d"
                            font.pixelSize: 28
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        Text {
                            Layout.fillWidth: true
                            text: root.toolDescription(root.selectedTool)
                            color: "#706a7e"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                        }
                    }
                    Button {
                        text: "Refresh"
                        onClicked: {
                            vehicleManager.refreshSerialPorts()
                            flightDataManager.refreshSnapshot()
                            autopilotToolsFacade.detectBoard()
                            optionalHardwareManager.refresh()
                            missionSyncManager.bootstrap(true)
                        }
                    }
                }

                Loader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    sourceComponent: root.componentForTool(root.selectedTool)
                }
            }
        }
    }

    Component {
        id: connectPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                VehicleConnectionPanel {
                    Layout.fillWidth: true
                    showBindAction: false
                }
                SectionPanel {
                    title: "Connection Status"
                    rows: [
                        { "name": "MAVSDK", "value": vehicleManager.status },
                        { "name": "Connection URL", "value": vehicleConfigManager.connectionUrl },
                        { "name": "Serial Ports", "value": String(vehicleManager.availableSerialPorts.length) },
                        { "name": "Autopilot", "value": vehicleManager.autopilot },
                        { "name": "Health", "value": vehicleManager.health }
                    ]
                }
            }
        }
    }

    Component {
        id: dashboardPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                SummaryGrid {
                    rows: [
                        { "name": "Aircraft", "value": vehicleManager.connected ? "1 connected" : "0 connected" },
                        { "name": "Telemetry", "value": telemetryStore.connected ? "Online" : "Offline" },
                        { "name": "Readiness", "value": telemetryStore.aircraftReadiness },
                        { "name": "Production", "value": productionReadinessManager.readinessPercent + "%" },
                        { "name": "Session", "value": sessionManager.operationsAllowed ? "Trusted" : "Blocked" }
                    ]
                }
                SectionPanel {
                    title: "Production Readiness"
                    rows: productionReadinessManager.readinessRows
                }
                SectionPanel {
                    title: "Production Blockers"
                    rows: productionReadinessManager.blockers
                }
            }
        }
    }

    Component {
        id: setupBenchPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                GridLayout {
                    Layout.fillWidth: true
                    columns: width < 560 ? 1 : (width < 1280 ? 2 : 4)
                    rowSpacing: 12
                    columnSpacing: 12
                    SetupTile { title: "Connect FC"; detail: "COM/USB/UDP/TCP"; status: vehicleManager.connected ? "Connected" : "Required"; onClicked: root.selectTool("connect", true) }
                    SetupTile { title: "Detect Board"; detail: autopilotToolsFacade.boardInfo.stack || "Autopilot metadata"; status: vehicleManager.connected ? "Live" : "Offline"; onClicked: autopilotToolsFacade.detectBoard() }
                    SetupTile { title: "Install Firmware"; detail: firmwareUpdateManager.packageType + " " + firmwareUpdateManager.version; status: firmwareUpdateManager.state; onClicked: root.selectTool("installFirmware", true) }
                    SetupTile { title: "Airframe"; detail: "Frame class/type"; status: "Selectable"; onClicked: root.selectTool("airframeSetup", true) }
                    SetupTile { title: "RC / Radio"; detail: "Mapping and calibration"; status: rcCalibrationManager.rcAvailable ? "Input detected" : "Ready"; onClicked: root.selectTool("rcRadio", true) }
                    SetupTile { title: "Battery / Failsafe"; detail: "Power and safety"; status: "Selectable"; onClicked: root.selectTool("batterySetup", true) }
                    SetupTile { title: "Payload Video"; detail: videoStreamManager.status; status: videoStreamManager.connected ? "Live" : "Ready"; onClicked: root.selectTool("payloadVideo", true) }
                    SetupTile { title: "Optional Hardware"; detail: optionalHardwareManager.status; status: "Ready"; onClicked: root.selectTool("rtkGpsInject", true) }
                    SetupTile { title: "Health Check"; detail: vehicleManager.health; status: telemetryStore.aircraftReady ? "Ready" : "Check"; onClicked: flightDataManager.refreshSnapshot() }
                }
                SectionPanel {
                    title: "Initial Setup Safety State"
                    rows: initialSetupManager.setupRows
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: calibrationContent.implicitHeight + 28
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e2dceb"
                    ColumnLayout {
                        id: calibrationContent
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 10
                        RowLayout {
                            Layout.fillWidth: true
                            Text {
                                Layout.fillWidth: true
                                text: "Calibration Tools"
                                color: "#14111d"
                                font.pixelSize: 16
                                font.bold: true
                                elide: Text.ElideRight
                            }
                            Button {
                                text: "Cancel"
                                enabled: initialSetupManager.running
                                onClicked: initialSetupManager.cancelCalibration()
                            }
                        }
                        ProgressBar {
                            Layout.fillWidth: true
                            from: 0
                            to: 100
                            value: initialSetupManager.progress
                            visible: initialSetupManager.running || initialSetupManager.progress > 0
                        }
                        Text {
                            Layout.fillWidth: true
                            text: initialSetupManager.status
                            color: "#706a7e"
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }
                        GridLayout {
                            Layout.fillWidth: true
                            columns: width < 560 ? 1 : (width < 1280 ? 2 : 3)
                            rowSpacing: 10
                            columnSpacing: 10
                            Repeater {
                                model: initialSetupManager.calibrationSteps
                                delegate: SetupTile {
                                    title: modelData.name
                                    detail: modelData.description
                                    status: modelData.supported ? "MAVSDK" : "Adapter gated"
                                    onClicked: {
                                        if (modelData.supported)
                                            initialSetupManager.startCalibration(modelData.key)
                                        else
                                            initialSetupManager.markUnsupported(modelData.name)
                                    }
                                }
                            }
                        }
                    }
                }
                ControllerMappingPanel {
                    Layout.fillWidth: true
                }
            }
        }
    }

    Component {
        id: parameterPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: parameterFilterFlow.childrenRect.height + 28
                    implicitHeight: parameterFilterFlow.childrenRect.height + 28
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e2dceb"
                    Flow {
                        id: parameterFilterFlow
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 14
                        spacing: 10
                        StyledComboBox {
                            width: Math.min(190, parameterFilterFlow.width)
                            model: parameterManager.setupGroups
                            currentIndex: root.listIndex(parameterManager.setupGroups, parameterManager.selectedGroup)
                            onActivated: parameterManager.selectedGroup = currentText
                        }
                        StyledTextField {
                            width: Math.min(parameterFilterFlow.width, Math.max(160, parameterFilterFlow.width - 420))
                            placeholderText: "Search parameters"
                            text: parameterManager.searchText
                            onEditingFinished: parameterManager.searchText = text
                        }
                        CheckBox {
                            text: "Modified"
                            checked: parameterManager.modifiedOnly
                            onToggled: parameterManager.modifiedOnly = checked
                        }
                        Button { text: "Read All"; onClicked: parameterManager.readAllParameters() }
                        Button { text: "Write Changed"; enabled: parameterManager.changedCount > 0 && !parameterManager.busy; onClicked: parameterManager.writeChangedParameters() }
                        Button { text: "Write Group"; enabled: parameterManager.selectedGroup !== "All" && !parameterManager.busy; onClicked: parameterManager.writeParameterGroup(parameterManager.selectedGroup) }
                    }
                }
                Text {
                    Layout.fillWidth: true
                    text: parameterManager.status + "  Changed: " + parameterManager.changedCount
                          + "  Metadata: " + parameterMetadataManager.metadataCount
                          + "  Override: " + (parameterManager.rawEditorAllowed ? "enabled" : "off")
                    color: "#706a7e"
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.max(320, Math.min(590, root.height - 180))
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e2dceb"
                    ListView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true
                        model: parameterManager.filteredParameters
                        delegate: ParameterRow {
                            width: ListView.view.width
                            row: modelData
                        }
                    }
                }
            }
        }
    }

    Component {
        id: firmwarePanel
        FirmwareManagerPage {
            anchors.fill: parent
        }
    }

    Component {
        id: flightDataPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                SummaryGrid { rows: flightDataManager.telemetryRows }
                TwoColumnPanels { leftTitle: "GPS / Position"; leftRows: flightDataManager.gpsRows; rightTitle: "Battery / Power"; rightRows: flightDataManager.powerRows }
                TwoColumnPanels { leftTitle: "RC / Link"; leftRows: flightDataManager.linkRows; rightTitle: "Attitude / HUD"; rightRows: flightDataManager.attitudeRows }
                SectionPanel { title: "Vehicle Messages"; rows: flightDataManager.vehicleMessages }
                SectionPanel {
                    title: "Commands"
                    rows: [
                        { "name": "Mode Selector", "value": flightDataManager.modeOptions.join(", ") },
                        { "name": "Action Dropdown", "value": flightDataManager.actionOptions.join(", ") },
                        { "name": "Servo / Relay", "value": "Adapter gated" },
                        { "name": "Tuning Graph", "value": "Graph shell ready" }
                    ]
                }
            }
        }
    }

    Component {
        id: missionPanel
        AdvancedMissionEditorPanel {
            anchors.fill: parent
        }
    }

    Component {
        id: optionalHardwarePanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                GridLayout {
                    Layout.fillWidth: true
                    columns: width < 560 ? 1 : (width < 1280 ? 2 : 4)
                    rowSpacing: 12
                    columnSpacing: 12
                    Repeater {
                        model: optionalHardwareManager.tools
                        delegate: SetupTile {
                            title: modelData.name
                            detail: modelData.description
                            status: modelData.supported ? modelData.status : "Unsupported"
                            selected: modelData.key === optionalHardwareManager.activeTool
                            onClicked: optionalHardwareManager.configureTool(modelData.key)
                        }
                    }
                }
                ActionRow {
                    buttons: [
                        { "label": "Refresh Ports", "action": "refresh" },
                        { "label": "Configure SiK", "action": "sik" },
                        { "label": "Radio Status", "action": "radio" },
                        { "label": "Probe CAN", "action": "can" },
                        { "label": "Probe Flow", "action": "flow" },
                        { "label": "Bluetooth", "action": "bluetooth" },
                        { "label": "Tracker Target", "action": "tracker" },
                        { "label": "Inject RTCM", "action": "rtcm" },
                        { "label": "Send Terminal", "action": "terminal" }
                    ]
                    onTriggered: function(action) {
                        if (action === "refresh") optionalHardwareManager.refresh()
                        else if (action === "sik") optionalHardwareManager.configureSikRadio(root.sikPortDraft, Number(root.sikBaudDraft), root.sikNetIdDraft.length > 0 ? Number(root.sikNetIdDraft) : -1, root.sikAirSpeedDraft.length > 0 ? Number(root.sikAirSpeedDraft) : -1)
                        else if (action === "radio") optionalHardwareManager.probeRadioStatus()
                        else if (action === "can") optionalHardwareManager.probeDroneCan()
                        else if (action === "flow") optionalHardwareManager.probePx4Flow()
                        else if (action === "bluetooth") optionalHardwareManager.scanBluetooth()
                        else if (action === "tracker") optionalHardwareManager.updateAntennaTrackerTarget(Number(root.trackerLatDraft), Number(root.trackerLonDraft), Number(root.trackerAltDraft))
                        else if (action === "rtcm") {
                            if (root.rtcmDraft.length > 0)
                                optionalHardwareManager.injectRtcmBase64(root.rtcmDraft)
                            else
                                optionalHardwareManager.injectRtcmSample()
                        }
                        else optionalHardwareManager.sendTerminalCommand(root.terminalDraft)
                    }
                }
                GridLayout {
                    Layout.fillWidth: true
                    columns: width < 560 ? 1 : (width < 1280 ? 2 : 4)
                    rowSpacing: 8
                    columnSpacing: 10
                    StyledTextField { Layout.fillWidth: true; placeholderText: "SiK COM port"; text: root.sikPortDraft; onTextChanged: root.sikPortDraft = text }
                    StyledTextField { Layout.fillWidth: true; placeholderText: "SiK baud, e.g. 57600"; text: root.sikBaudDraft; onTextChanged: root.sikBaudDraft = text }
                    StyledTextField { Layout.fillWidth: true; placeholderText: "SiK NETID optional"; text: root.sikNetIdDraft; onTextChanged: root.sikNetIdDraft = text }
                    StyledTextField { Layout.fillWidth: true; placeholderText: "SiK air speed optional"; text: root.sikAirSpeedDraft; onTextChanged: root.sikAirSpeedDraft = text }
                    StyledTextField { Layout.fillWidth: true; placeholderText: "Tracker lat"; text: root.trackerLatDraft; onTextChanged: root.trackerLatDraft = text }
                    StyledTextField { Layout.fillWidth: true; placeholderText: "Tracker lon"; text: root.trackerLonDraft; onTextChanged: root.trackerLonDraft = text }
                    StyledTextField { Layout.fillWidth: true; placeholderText: "Tracker alt m"; text: root.trackerAltDraft; onTextChanged: root.trackerAltDraft = text }
                }
                StyledTextField {
                    Layout.fillWidth: true
                    placeholderText: "Base64 RTCM correction data for RTK/GPS injection"
                    text: root.rtcmDraft
                    onTextChanged: root.rtcmDraft = text
                }
                StyledTextField {
                    Layout.fillWidth: true
                    placeholderText: "Terminal command for SERIAL_CONTROL-capable autopilots"
                    text: root.terminalDraft
                    onTextChanged: root.terminalDraft = text
                }
                TwoColumnPanels {
                    leftTitle: "Serial / USB Devices"
                    leftRows: optionalHardwareManager.serialRows
                    rightTitle: "Hardware Status"
                    rightRows: [
                        { "name": "Active Tool", "value": optionalHardwareManager.activeTool },
                        { "name": "Status", "value": optionalHardwareManager.status },
                        { "name": "Policy", "value": "Unsupported vendor-specific cases are shown as unsupported, not faked." }
                    ]
                }
                SectionPanel { title: "Hardware Diagnostics"; rows: optionalHardwareManager.hardwareRows }
            }
        }
    }

    Component {
        id: payloadPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                GridLayout {
                    Layout.fillWidth: true
                    columns: width < 900 ? 1 : 2
                    rowSpacing: 12
                    columnSpacing: 12

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.videoExpanded ? 640 : 430
                        radius: 8
                        color: "#ffffff"
                        border.color: "#e2dceb"
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 14
                            spacing: 10
                            Text { Layout.fillWidth: true; text: "RTSP/H264 Video"; color: "#14111d"; font.pixelSize: 16; font.bold: true }
                            GridLayout {
                                Layout.fillWidth: true
                                columns: width < 560 ? 1 : 2
                                rowSpacing: 8
                                columnSpacing: 10
                                StyledTextField { Layout.fillWidth: true; placeholderText: "rtsp://camera.local:8554/live"; text: root.streamUrlDraft; onTextChanged: root.streamUrlDraft = text }
                                StyledComboBox { Layout.fillWidth: true; model: ["Low latency", "Balanced", "Stable"]; currentIndex: root.listIndex(["Low latency", "Balanced", "Stable"], videoStreamManager.latencyMode); onActivated: videoStreamManager.latencyMode = currentText }
                                StyledTextField { Layout.fillWidth: true; placeholderText: "Username"; text: root.streamUserDraft; onTextChanged: root.streamUserDraft = text }
                                StyledTextField { Layout.fillWidth: true; placeholderText: "Password"; echoMode: TextInput.Password; text: root.streamPasswordDraft; onTextChanged: root.streamPasswordDraft = text }
                            }
                            ActionRow {
                                buttons: [
                                    { "label": "Save", "action": "save" },
                                    { "label": "Connect", "action": "connect" },
                                    { "label": "Disconnect", "action": "disconnect" },
                                    { "label": videoStreamManager.recording ? "Stop Rec" : "Record", "action": "record" },
                                    { "label": root.videoExpanded ? "Fit" : "Fullscreen", "action": "full" }
                                ]
                                onTriggered: function(action) {
                                    if (action === "save") videoStreamManager.configure(root.streamUrlDraft, root.streamUserDraft, root.streamPasswordDraft, videoStreamManager.latencyMode)
                                    else if (action === "connect") {
                                        videoStreamManager.configure(root.streamUrlDraft, root.streamUserDraft, root.streamPasswordDraft, videoStreamManager.latencyMode)
                                        videoStreamManager.connectStream()
                                    } else if (action === "disconnect") videoStreamManager.disconnectStream()
                                    else if (action === "record") {
                                        if (videoStreamManager.recording) videoStreamManager.stopRecording()
                                        else videoStreamManager.startRecording()
                                    } else root.videoExpanded = !root.videoExpanded
                                }
                            }
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                radius: 6
                                color: "#08070d"
                                border.color: "#2a2433"
                                clip: true
                                VideoOutput {
                                    id: videoOutput
                                    anchors.fill: parent
                                    fillMode: VideoOutput.PreserveAspectFit
                                }
                                Text {
                                    anchors.centerIn: parent
                                    visible: !videoStreamManager.connected
                                    text: videoStreamManager.status
                                    color: "#ffffff"
                                    font.pixelSize: 13
                                    wrapMode: Text.WordWrap
                                    width: parent.width - 32
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                            MediaPlayer {
                                id: rtspPlayer
                                source: videoStreamManager.playbackUrl
                                videoOutput: videoOutput
                                audioOutput: AudioOutput { muted: true }
                                onPlaybackStateChanged: {
                                    if (playbackState === MediaPlayer.PlayingState)
                                        videoStreamManager.reportPlaybackReady()
                                }
                                onErrorOccurred: function(error, errorString) {
                                    if (errorString && errorString.length > 0)
                                        videoStreamManager.reportPlaybackError(errorString)
                                }
                                onSourceChanged: {
                                    if (videoStreamManager.connected && String(source).length > 0)
                                        play()
                                    else
                                        stop()
                                }
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        ActionRow {
                            buttons: [
                                { "label": "Validate Camera", "action": "validateCamera" },
                                { "label": "Validate Gimbal", "action": "validateGimbal" },
                                { "label": "Trigger Camera", "action": "trigger" },
                                { "label": "Gimbal -45", "action": "gimbal" },
                                { "label": "Overlap", "action": "overlap" },
                                { "label": "Geotag", "action": "geotag" }
                            ]
                            onTriggered: function(action) {
                                if (action === "validateCamera") payloadManager.validateCamera()
                                else if (action === "validateGimbal") payloadManager.validateGimbal()
                                else if (action === "trigger") payloadManager.triggerCamera()
                                else if (action === "gimbal") payloadManager.setGimbalPitch(-45)
                                else if (action === "overlap") payloadManager.verifyOverlap()
                                else payloadManager.startGeotagWorkflow()
                            }
                        }
                        SectionPanel { title: "Video State"; rows: videoStreamManager.videoRows }
                        SectionPanel { title: "Payload Status"; rows: payloadManager.payloadRows }
                        SectionPanel { title: "Camera"; rows: payloadManager.cameraRows }
                        SectionPanel { title: "Gimbal"; rows: payloadManager.gimbalRows }
                    }
                }
            }
        }
    }

    Component {
        id: advancedPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                ActionRow {
                    buttons: [
                        { "label": "Detect Board", "action": "detect" },
                        { "label": "Refresh Caps", "action": "caps" },
                        { "label": "MAVLink Sender", "action": "sender" }
                    ]
                    onTriggered: function(action) {
                        if (action === "detect") autopilotToolsFacade.detectBoard()
                        else if (action === "caps") autopilotToolsFacade.refreshCapabilities()
                        else autopilotToolsFacade.markUnsupported("MAVLink message sender")
                    }
                }
                SectionPanel {
                    title: "Board Info"
                    rows: [
                        { "name": "Connected", "value": String(autopilotToolsFacade.boardInfo.connected) },
                        { "name": "Stack", "value": autopilotToolsFacade.boardInfo.stack || "--" },
                        { "name": "Autopilot", "value": autopilotToolsFacade.boardInfo.autopilot || "--" },
                        { "name": "System", "value": autopilotToolsFacade.boardInfo.systemId || "--" },
                        { "name": "Connection", "value": autopilotToolsFacade.boardInfo.connectionUrl || "--" }
                    ]
                }
                SectionPanel { title: "Capabilities"; rows: autopilotToolsFacade.capabilities }
                SectionPanel {
                    title: "Advanced Tool States"
                    rows: [
                        { "name": "MAVLink inspector", "value": "Adapter shell" },
                        { "name": "Message sender", "value": "Adapter shell" },
                        { "name": "Terminal", "value": optionalHardwareManager.activeTool === "terminal" ? optionalHardwareManager.status : "SERIAL_CONTROL adapter gated" },
                        { "name": "MAVFTP", "value": "Adapter shell" },
                        { "name": "Signing", "value": "Adapter shell" },
                        { "name": "Serial passthrough", "value": "Adapter shell" }
                    ]
                }
            }
        }
    }

    Component {
        id: logsPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                ActionRow {
                    buttons: [
                        { "label": "List Logs", "action": "list" },
                        { "label": "Local Replay", "action": "replay" },
                        { "label": "Warnings", "action": "warnings" },
                        { "label": logAnalysisManager.tlogRecording ? "TLog Stop" : "TLog Start", "action": "tlog" },
                        { "label": "Open TLog", "action": "openTlog" },
                        { "label": "Export KML", "action": "kml" },
                        { "label": "Export GPX", "action": "gpx" }
                    ]
                    onTriggered: function(action) {
                        if (action === "list") logAnalysisManager.listOnboardLogs()
                        else if (action === "replay") logAnalysisManager.playbackLocalEvents()
                        else if (action === "warnings") logAnalysisManager.extractWarnings()
                        else if (action === "tlog") {
                            if (logAnalysisManager.tlogRecording) logAnalysisManager.stopTlogRecording()
                            else logAnalysisManager.startTlogRecording()
                        }
                        else if (action === "openTlog") tlogPlaybackDialog.open()
                        else if (action === "kml") kmlExportDialog.open()
                        else if (action === "gpx") gpxExportDialog.open()
                    }
                }
                Text { Layout.fillWidth: true; text: logAnalysisManager.status + "  Progress: " + logAnalysisManager.downloadProgress + "%  TLog: " + (logAnalysisManager.tlogPath || "--"); color: "#706a7e"; font.pixelSize: 12; wrapMode: Text.WordWrap }
                TwoColumnPanels { leftTitle: "Onboard Logs"; leftRows: logAnalysisManager.logEntries; rightTitle: "Replay / Warnings"; rightRows: logAnalysisManager.replayEvents }
                SectionPanel {
                    title: "Graph / Export"
                    rows: [
                        { "name": "Graph Fields", "value": logAnalysisManager.graphFields.join(", ") },
                        { "name": "KML Export", "value": "Exports loaded replay events when coordinates are present" },
                        { "name": "GPX Export", "value": "Exports loaded replay events when coordinates are present" }
                    ]
                }
                SectionPanel { title: "Graph Summary"; rows: logAnalysisManager.graphRows }
            }
        }
    }

    Component {
        id: simulationPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                GridLayout {
                    Layout.fillWidth: true
                    columns: width < 620 ? 1 : (width < 1300 ? 2 : 3)
                    rowSpacing: 12
                    columnSpacing: 12
                    Repeater {
                        model: simulationManager.profiles
                        delegate: SetupTile {
                            title: modelData.name
                            detail: modelData.mode + " " + modelData.host + ":" + modelData.port
                            status: modelData.key === simulationManager.activeProfile ? "Selected" : modelData.stack
                            onClicked: simulationManager.selectProfile(modelData.key)
                        }
                    }
                }
                ActionRow {
                    buttons: [
                        { "label": "Connect", "action": "connect" },
                        { "label": "Launch", "action": "launch" },
                        { "label": "Stop", "action": "stop" },
                        { "label": "Restart", "action": "restart" }
                    ]
                    onTriggered: function(action) {
                        if (action === "connect") simulationManager.connectSelected()
                        else if (action === "launch") simulationManager.launchSelected()
                        else if (action === "stop") simulationManager.stopSimulation()
                        else if (action === "restart") simulationManager.restartSimulation()
                    }
                }
                SectionPanel {
                    title: "Simulation State"
                    rows: [
                        { "name": "Active Profile", "value": simulationManager.activeProfile },
                        { "name": "Running", "value": simulationManager.running ? "Yes" : "No" },
                        { "name": "Status", "value": simulationManager.status }
                    ]
                }
                ActionRow {
                    buttons: [
                        { "label": "Run Protocol Tests", "action": "all" },
                        { "label": "Firmware Samples", "action": "firmware" },
                        { "label": "Sample TLog", "action": "tlog" },
                        { "label": "Bootloader Self-Test", "action": "bootloader" },
                        { "label": "Optional HW Self-Test", "action": "optional" },
                        { "label": "RTSP Profile", "action": "rtsp" },
                        { "label": "Clear", "action": "clear" }
                    ]
                    onTriggered: function(action) {
                        if (action === "all") protocolTestManager.runAll()
                        else if (action === "firmware") protocolTestManager.createFirmwareSamples()
                        else if (action === "tlog") protocolTestManager.createSampleTlog()
                        else if (action === "bootloader") protocolTestManager.runBootloaderSelfTest()
                        else if (action === "optional") protocolTestManager.runOptionalHardwareSelfTest()
                        else if (action === "rtsp") protocolTestManager.prepareRtspTestProfile()
                        else protocolTestManager.clear()
                    }
                }
                SectionPanel {
                    title: "Pre-Hardware Protocol Tests"
                    rows: [
                        { "name": "Status", "value": protocolTestManager.status },
                        { "name": "Running", "value": protocolTestManager.running ? "Yes" : "No" },
                        { "name": "Artifacts", "value": protocolTestManager.artifactDirectory }
                    ]
                }
                TwoColumnPanels {
                    leftTitle: "Test Results"
                    leftRows: protocolTestManager.testRows
                    rightTitle: "Generated Artifacts"
                    rightRows: protocolTestManager.artifactRows
                }
            }
        }
    }

    Component {
        id: multiVehiclePanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                ActionRow {
                    buttons: [
                        { "label": "Refresh", "action": "refresh" },
                        { "label": "Select Primary", "action": "primary" }
                    ]
                    onTriggered: function(action) {
                        if (action === "refresh") multiVehicleManager.refreshVehicles()
                        else multiVehicleManager.selectVehicle("primary")
                    }
                }
                TwoColumnPanels { leftTitle: "Connected Vehicles"; leftRows: multiVehicleManager.connectedVehicles; rightTitle: "Connection Profiles"; rightRows: multiVehicleManager.connectionProfiles }
                SectionPanel {
                    title: "Multi-Vehicle State"
                    rows: [
                        { "name": "Active Vehicle", "value": multiVehicleManager.activeVehicleId },
                        { "name": "Status", "value": multiVehicleManager.status },
                        { "name": "Control Model", "value": "One active controllable vehicle in this foundation build" }
                    ]
                }
            }
        }
    }

    Component {
        id: syncPanel
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 14
                TwoColumnPanels {
                    leftTitle: "Session"
                    leftRows: [
                        { "name": "Operations", "value": sessionManager.operationsAllowed ? "Allowed" : "Blocked" },
                        { "name": "Access", "value": accessManager.status },
                        { "name": "Role", "value": accessManager.role },
                        { "name": "Organization", "value": String(accessManager.organizationId) }
                    ]
                    rightTitle: "Command Center"
                    rightRows: [
                        { "name": "Event Sync", "value": gcsEventSyncManager.status },
                        { "name": "Mission Sync", "value": missionSyncManager.status },
                        { "name": "Backend URL", "value": backendTrustManager.baseUrl },
                        { "name": "Cached Permissions", "value": accessManager.permissions.length + " scopes" },
                        { "name": "Production", "value": productionReadinessManager.status }
                    ]
                }
                SectionPanel {
                    title: "Standalone Rules"
                    rows: [
                        { "name": "Local setup", "value": "Allowed from trusted cached session when Command Center is unreachable" },
                        { "name": "Sync", "value": "Targeted Command Center updates and queued audit events" },
                        { "name": "Mission safety", "value": "Telemetry, MAVSDK, missions, and manual control are not restarted by UI refreshes" }
                    ]
                }
                SectionPanel {
                    visible: root.diagnosticsEnabled
                    title: "RBAC Diagnostics"
                    rows: root.rbacDiagnosticRows()
                }
                TwoColumnPanels {
                    visible: root.diagnosticsEnabled
                    leftTitle: "Visible Workspaces"
                    leftRows: root.rbacWorkspaceRows(true)
                    rightTitle: "Hidden Workspaces"
                    rightRows: root.rbacWorkspaceRows(false)
                }
                SectionPanel {
                    visible: root.diagnosticsEnabled
                    title: "Hidden Tool Cards"
                    rows: root.hiddenToolRows()
                }
            }
        }
    }

    component NavItem: Rectangle {
        id: card
        property var tool
        property bool selected: false
        signal clicked()
        height: 44
        radius: 4
        color: selected ? "#6d9700" : (navMouse.containsMouse ? "#f4f1f8" : "#ffffff")
        border.color: selected ? "#5b8000" : "#e2dceb"
        opacity: tool.available ? 1.0 : 0.48

        MouseArea {
            id: navMouse
            anchors.fill: parent
            enabled: tool.available
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: card.clicked()
        }
        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 8
            Rectangle {
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26
                radius: 4
                color: card.selected ? "#ffffff" : "#f2ecfb"
                Text {
                    anchors.centerIn: parent
                    text: tool.iconText
                    color: card.selected ? "#5b8000" : "#5f16c5"
                    font.pixelSize: 9
                    font.bold: true
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0
                Text {
                    Layout.fillWidth: true
                    text: tool.title
                    color: card.selected ? "#ffffff" : "#14111d"
                    font.pixelSize: 12
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    Layout.fillWidth: true
                    text: tool.available ? tool.statusText : "Restricted"
                    color: card.selected ? "#eef9cf" : "#706a7e"
                    font.pixelSize: 9
                    elide: Text.ElideRight
                }
            }
        }
    }

    component ParameterRow: Rectangle {
        id: parameterRow
        property var row
        height: 72
        color: row.modified ? "#fffaf0" : "#ffffff"
        border.color: "#eee8f5"

        Flickable {
            id: parameterScroller
            anchors.fill: parent
            contentWidth: Math.max(width, parameterContent.implicitWidth + 16)
            contentHeight: height
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            ScrollBar.horizontal: ScrollBar { policy: parameterScroller.contentWidth > parameterScroller.width ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff }

            RowLayout {
                id: parameterContent
                width: Math.max(parameterScroller.width, implicitWidth + 16)
                height: parameterScroller.height
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                spacing: 10
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.minimumWidth: 180
                    Layout.maximumWidth: 420
                    spacing: 3
                    Text { Layout.fillWidth: true; text: row.name; color: "#14111d"; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
                    Text { Layout.fillWidth: true; text: row.group + (row.known ? "" : " / metadata missing"); color: row.known ? "#706a7e" : "#a15c00"; font.pixelSize: 11; elide: Text.ElideRight }
                    Text { Layout.fillWidth: true; text: row.description || ""; color: "#8d839a"; font.pixelSize: 10; elide: Text.ElideRight }
                }

                Text {
                    Layout.preferredWidth: 96
                    text: row.value
                    color: "#706a7e"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }

                StyledComboBox {
                    Layout.preferredWidth: 190
                    visible: row.controlType === "enum" || row.controlType === "switch"
                    enabled: !row.readOnly
                    model: row.options
                    textRole: "label"
                    currentIndex: root.optionIndex(row.options, row.draftValue)
                    onActivated: {
                        if (index >= 0)
                            parameterManager.setDraftValue(row.name, row.options[index].value)
                    }
                }

                RowLayout {
                    Layout.preferredWidth: 210
                    visible: row.controlType === "number"
                    spacing: 4
                    Button {
                        text: "-"
                        enabled: !row.readOnly
                        onClicked: parameterManager.setDraftValue(row.name, String(Number(row.draftValue) - Number(row.step || 1)))
                    }
                    StyledTextField {
                        Layout.fillWidth: true
                        text: row.draftValue
                        enabled: !row.readOnly
                        onEditingFinished: parameterManager.setDraftValue(row.name, text)
                    }
                    Button {
                        text: "+"
                        enabled: !row.readOnly
                        onClicked: parameterManager.setDraftValue(row.name, String(Number(row.draftValue) + Number(row.step || 1)))
                    }
                }

                StyledTextField {
                    Layout.preferredWidth: 190
                    visible: row.controlType === "raw" || row.controlType === "text"
                    text: row.draftValue
                    enabled: !row.readOnly
                    onEditingFinished: parameterManager.setDraftValue(row.name, text)
                }

                Text {
                    Layout.preferredWidth: 190
                    visible: row.controlType === "readonly"
                    text: row.draftValue
                    color: "#706a7e"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                Text { Layout.preferredWidth: 60; text: row.unit || row.type; color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
                Button { text: "Reset"; enabled: row.modified; onClicked: parameterManager.resetDraft(row.name) }
            }
        }
    }

    component SummaryGrid: GridLayout {
        property var rows: []
        Layout.fillWidth: true
        columns: width < 560 ? 1 : (width < 1200 ? 2 : 4)
        rowSpacing: 12
        columnSpacing: 12
        Repeater {
            model: rows
            delegate: Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 92
                radius: 8
                color: "#ffffff"
                border.color: "#e2dceb"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 4
                    Text { Layout.fillWidth: true; text: modelData.name; color: "#706a7e"; font.pixelSize: 11; font.bold: true; elide: Text.ElideRight }
                    Text { Layout.fillWidth: true; text: modelData.value; color: "#14111d"; font.pixelSize: 20; font.bold: true; elide: Text.ElideRight }
                    Text { Layout.fillWidth: true; text: modelData.unit || ""; color: "#8d839a"; font.pixelSize: 10; elide: Text.ElideRight }
                }
            }
        }
    }

    component SectionPanel: Rectangle {
        id: panel
        property string title: ""
        property var rows: []
        property string footerButtonText: ""
        signal footerClicked()
        Layout.fillWidth: true
        Layout.minimumHeight: content.implicitHeight + 28
        radius: 8
        color: "#ffffff"
        border.color: "#e2dceb"
        ColumnLayout {
            id: content
            anchors.fill: parent
            anchors.margins: 14
            spacing: 10
            Text { Layout.fillWidth: true; text: panel.title; color: "#14111d"; font.pixelSize: 16; font.bold: true; elide: Text.ElideRight }
            Repeater {
                model: panel.rows
                delegate: RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    Text {
                        Layout.preferredWidth: panel.width < 460 ? Math.max(92, panel.width * 0.34) : 170
                        text: modelData.name || modelData.label || modelData.severity || "--"
                        color: "#706a7e"
                        font.pixelSize: 11
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Text {
                        Layout.fillWidth: true
                        text: modelData.value || modelData.detail || modelData.message || modelData.state || JSON.stringify(modelData)
                        color: "#14111d"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }
                }
            }
            Button { visible: panel.footerButtonText.length > 0; text: panel.footerButtonText; onClicked: panel.footerClicked() }
        }
    }

    component TwoColumnPanels: GridLayout {
        property string leftTitle: ""
        property var leftRows: []
        property string rightTitle: ""
        property var rightRows: []
        Layout.fillWidth: true
        columns: width < 760 ? 1 : 2
        rowSpacing: 12
        columnSpacing: 12
        SectionPanel { title: leftTitle; rows: leftRows }
        SectionPanel { title: rightTitle; rows: rightRows }
    }

    component RawMissionTable: Rectangle {
        property string title: ""
        property var rows: []
        Layout.fillWidth: true
        Layout.preferredHeight: 360
        radius: 8
        color: "#ffffff"
        border.color: "#e2dceb"
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 8
            Text { Layout.fillWidth: true; text: title; color: "#14111d"; font.pixelSize: 16; font.bold: true; elide: Text.ElideRight }
            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: rows
                delegate: Rectangle {
                    width: ListView.view.width
                    height: 42
                    color: index % 2 === 0 ? "#ffffff" : "#fbfaff"
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8
                        Text { Layout.preferredWidth: 44; text: String(modelData.seq); color: "#706a7e"; font.pixelSize: 11 }
                        Text { Layout.preferredWidth: 76; text: "CMD " + modelData.command; color: "#14111d"; font.pixelSize: 12; font.bold: true }
                        Text { Layout.fillWidth: true; text: "P " + modelData.param1 + ", " + modelData.param2 + ", " + modelData.param3 + ", " + modelData.param4; color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
                        Text { Layout.preferredWidth: 150; text: modelData.x + ", " + modelData.y + ", " + modelData.z; color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
                    }
                }
            }
        }
    }

    component SetupTile: Rectangle {
        id: setupTile
        property string title: ""
        property string detail: ""
        property string status: ""
        property bool selected: false
        signal clicked()
        Layout.fillWidth: true
        Layout.preferredHeight: 116
        radius: 8
        color: selected ? "#f0e7fb" : "#ffffff"
        border.color: selected ? "#8b62c4" : "#e2dceb"
        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: setupTile.clicked() }
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 5
            Text { Layout.fillWidth: true; text: title; color: "#14111d"; font.pixelSize: 14; font.bold: true; elide: Text.ElideRight }
            Text { Layout.fillWidth: true; text: detail; color: "#706a7e"; font.pixelSize: 12; wrapMode: Text.WordWrap; maximumLineCount: 2 }
            Text { Layout.fillWidth: true; text: status; color: status === "Unsupported" || status === "Required" ? "#a15c00" : "#167a3a"; font.pixelSize: 11; font.bold: true; elide: Text.ElideRight }
        }
    }

    component ActionRow: Flow {
        id: actionRow
        property var buttons: []
        signal triggered(string action)
        Layout.fillWidth: true
        Layout.preferredHeight: childrenRect.height
        spacing: 10
        Repeater {
            model: buttons
            delegate: Button {
                width: Math.min(actionRow.width, Math.max(96, implicitWidth))
                height: 32
                text: modelData.label
                onClicked: actionRow.triggered(modelData.action)
            }
        }
    }

    component StyledTextField: TextField {
        color: "#050505"
        placeholderTextColor: "#6f687a"
        selectByMouse: true
        background: Rectangle {
            radius: 6
            color: "#ffffff"
            border.color: parent.activeFocus ? "#5f16c5" : "#cfc5da"
            border.width: parent.activeFocus ? 2 : 1
        }
    }

    component StyledComboBox: ComboBox {
        id: combo
        font.pixelSize: 12
        contentItem: Text {
            text: combo.displayText
            color: "#050505"
            verticalAlignment: Text.AlignVCenter
            leftPadding: 8
            rightPadding: 22
            elide: Text.ElideRight
        }
        background: Rectangle {
            radius: 6
            color: "#ffffff"
            border.color: combo.activeFocus ? "#5f16c5" : "#cfc5da"
            border.width: combo.activeFocus ? 2 : 1
        }
    }
}
