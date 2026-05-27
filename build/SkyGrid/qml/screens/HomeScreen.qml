import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SkyGrid 1.0
import "../dashboard"
import "../controls"

Item {
    id: root

    property string activeTab: "mission"
    property string searchText: ""
    property string toast: ""
    property string modalTitle: ""
    property string modalBody: ""
    property bool modalOpen: false

    readonly property bool compact: width < 1040
    readonly property bool tablet: width < 1380
    readonly property int leftSidebarWidth: 330
    readonly property int rightSidebarWidth: 300
    readonly property int dashboardOuterMargin: compact ? 14 : 18
    readonly property int dashboardGap: 16
    readonly property int centerContentWidth: Math.max(900, width - (compact ? 0 : leftSidebarWidth) - rightSidebarWidth - (dashboardOuterMargin * 2) - dashboardGap)
    readonly property color purple: "#4B3DA0"
    readonly property color purpleDark: "#30006f"
    readonly property color surface: "#f6f4fa"
    readonly property color ink: "#14111d"
    readonly property color muted: "#706a7e"
    readonly property color line: "#e2dceb"

    function hasObject(name) {
        if (name === "telemetryStore")
            return typeof telemetryStore !== "undefined"
        if (name === "missionSyncManager")
            return typeof missionSyncManager !== "undefined"
        if (name === "missionStore")
            return typeof missionStore !== "undefined"
        if (name === "profileManager")
            return typeof profileManager !== "undefined"
        if (name === "weatherManager")
            return typeof weatherManager !== "undefined"
        if (name === "weatherSyncManager")
            return typeof weatherSyncManager !== "undefined"
        if (name === "eventLogManager")
            return typeof eventLogManager !== "undefined"
        if (name === "appState")
            return typeof appState !== "undefined"
        if (name === "authManager")
            return typeof authManager !== "undefined"
        if (name === "localSyncCache")
            return typeof localSyncCache !== "undefined"
        if (name === "sessionManager")
            return typeof sessionManager !== "undefined"
        if (name === "preferencesManager")
            return typeof preferencesManager !== "undefined"
        if (name === "flightStatsManager")
            return typeof flightStatsManager !== "undefined"
        if (name === "preflightChecklistManager")
            return typeof preflightChecklistManager !== "undefined"
        return false
    }

    function textValue(item, keys, fallback) {
        if (!item)
            return fallback
        for (var i = 0; i < keys.length; ++i) {
            var value = item[keys[i]]
            if (value !== undefined && value !== null && String(value).length > 0)
                return String(value)
        }
        return fallback
    }

    function numberValue(item, keys, fallback) {
        var text = textValue(item, keys, "")
        return text.length > 0 && !isNaN(Number(text)) ? Number(text) : fallback
    }

    function safeList(value) {
        return value && value.length !== undefined ? value : []
    }

    function profileName() {
        return hasObject("profileManager") ? profileManager.displayName : "John Okojere"
    }

    function organizationName() {
        return hasObject("profileManager") ? profileManager.organizationName : "Nigerian Air Force"
    }

    function telemetryConnected() {
        return hasObject("telemetryStore") && telemetryStore.connected
    }

    function missionItems() {
        var active = hasObject("missionSyncManager") ? safeList(missionSyncManager.activeMissions) : []
        var history = hasObject("missionSyncManager") ? safeList(missionSyncManager.missionHistory) : []
        var live = active.length > 0 ? active : (hasObject("missionSyncManager") ? safeList(missionSyncManager.approvedMissions) : [])
        var cached = hasObject("missionStore") ? safeList(missionStore.missionHistory) : []
        var source = history.length > 0 ? history : (live.length > 0 ? live : cached)
        if (source.length > 0)
            return source
        return [
            { name: "Border Patrol Sweep", mission_type: "Surveillance", date: "May 20, 2026 - 08:32", status: "Completed" },
            { name: "Supply Drop Zone A", mission_type: "Logistics", date: "May 19, 2026 - 16:45", status: "Completed" },
            { name: "Site Recon Alpha", mission_type: "Reconnaissance", date: "May 19, 2026 - 10:12", status: "Completed" },
            { name: "Training Mission 04", mission_type: "Training", date: "May 18, 2026 - 14:33", status: "Completed" },
            { name: "Perimeter Scan", mission_type: "Surveillance", date: "May 18, 2026 - 09:05", status: "Completed" }
        ]
    }

    function activeMissionItems() {
        var active = hasObject("missionSyncManager") ? safeList(missionSyncManager.activeMissions) : []
        if (active.length > 0)
            return active
        return hasObject("missionSyncManager") ? safeList(missionSyncManager.approvedMissions) : []
    }

    function filteredMissions() {
        var all = missionItems()
        var query = root.searchText.toLowerCase()
        if (query.length === 0)
            return all
        var out = []
        for (var i = 0; i < all.length; ++i) {
            var name = textValue(all[i], ["name", "title", "mission_name"], "").toLowerCase()
            var type = textValue(all[i], ["mission_type", "type", "kind"], "").toLowerCase()
            if (name.indexOf(query) >= 0 || type.indexOf(query) >= 0)
                out.push(all[i])
        }
        return out
    }

    function aircraftItems() {
        var assigned = hasObject("missionSyncManager") ? safeList(missionSyncManager.assignedAircraft) : []
        if (assigned.length > 0)
            return assigned
        var currentName = telemetryConnected() ? telemetryStore.aircraftId : "SkyGrid X8-01"
        return [
            { name: currentName, tail_number: "SGX8-01", battery_percent: telemetryConnected() ? telemetryStore.battery : 78, status: "Ready", link_quality: telemetryConnected() ? telemetryStore.transmission + "%" : "Strong" },
            { name: "SkyGrid VTOL-03", tail_number: "SGVT-03", battery_percent: 89, status: "Ready", link_quality: "Strong" }
        ]
    }

    function onlineAircraftCount() {
        var items = aircraftItems()
        if (telemetryConnected())
            return Math.max(1, Math.min(items.length, 2))
        return Math.min(items.length, 2)
    }

    function weatherStatus() {
        if (!hasObject("weatherManager") || !weatherManager.available)
            return "SOURCE_UNAVAILABLE"
        var status = String(weatherManager.status).toUpperCase()
        if (status.indexOf("DO_NOT") >= 0 || status.indexOf("D/NF") >= 0)
            return "DO_NOT_FLY"
        if (status.indexOf("CAUTION") >= 0)
            return "CAUTION"
        if (status.indexOf("GOOD") >= 0)
            return "GOOD_TO_FLY"
        return "GOOD_TO_FLY"
    }

    function weatherLabel() {
        var status = weatherStatus()
        if (status === "SOURCE_UNAVAILABLE")
            return "Pending"
        if (status === "DO_NOT_FLY")
            return "Blocked"
        if (status === "CAUTION")
            return "Caution"
        return "Good"
    }

    function weatherDirectionLabel(deg) {
        var labels = ["N", "NE", "E", "SE", "S", "SW", "W", "NW"]
        var index = Math.round((((deg % 360) + 360) % 360) / 45) % 8
        return labels[index]
    }

    function missionIcon(type) {
        var normalized = String(type || "").toLowerCase()
        if (normalized.indexOf("photo") >= 0 || normalized.indexOf("grid") >= 0)
            return AssetRegistry.icons.lucide_grid_3x3
        if (normalized.indexOf("fence") >= 0 || normalized.indexOf("geo") >= 0)
            return AssetRegistry.icons.cube
        if (normalized.indexOf("poi") >= 0 || normalized.indexOf("building") >= 0)
            return AssetRegistry.icons.lucide_building_2
        if (normalized.indexOf("3d") >= 0 || normalized.indexOf("terrain") >= 0 || normalized.indexOf("map") >= 0)
            return AssetRegistry.icons.lucide_mountain
        if (normalized.indexOf("route") >= 0 || normalized.indexOf("waypoint") >= 0)
            return AssetRegistry.icons.lucide_route
        if (normalized.indexOf("tower") >= 0 || normalized.indexOf("inspection") >= 0)
            return AssetRegistry.icons.waypoint
        return AssetRegistry.icons.lucide_waypoints
    }

    property int readinessRevision: 0

    function readinessPercent() {
        root.readinessRevision
        if (root.hasObject("preflightChecklistManager"))
            return preflightChecklistManager.readinessPercent
        var base = 100
        if (weatherStatus() === "CAUTION")
            base -= 12
        if (weatherStatus() === "DO_NOT_FLY")
            base -= 35
        if (telemetryConnected() && telemetryStore.battery < 35)
            base -= 20
        if (telemetryConnected() && telemetryStore.satellites < 8)
            base -= 15
        return Math.max(0, base)
    }

    function readinessCheckItems() {
        root.readinessRevision
        if (root.hasObject("preflightChecklistManager")) {
            var items = []
            var checks = preflightChecklistManager.checks || []
            for (var i = 0; i < checks.length; ++i) {
                var check = checks[i]
                items.push({
                    label: check.category || check.label || "Check",
                    value: check.message || check.label || "",
                    ok: !!check.passed
                })
            }
            if (items.length > 0)
                return items
        }
        return [
            { label: "Aircraft Systems", value: "All Systems Nominal", ok: true },
            { label: "Payload Systems", value: "Operational", ok: true },
            { label: "Communications", value: root.telemetryConnected() ? telemetryStore.rcQuality : "Strong Signal", ok: true },
            { label: "Weather Conditions", value: root.weatherLabel() === "Good" ? "Good to Fly" : root.weatherLabel(), ok: root.weatherStatus() === "GOOD_TO_FLY" },
            { label: "Mission Data", value: root.hasObject("missionSyncManager") ? missionSyncManager.status : "Synchronized", ok: true }
        ]
    }

    function activityItems() {
        var events = hasObject("eventLogManager") ? safeList(eventLogManager.events) : []
        var recent = hasObject("profileManager") ? safeList(profileManager.recentActivity) : []
        var source = events.length > 0 ? events : recent
        if (source.length > 0) {
            var out = []
            for (var i = 0; i < Math.min(source.length, 8); ++i) {
                out.push({
                    message: textValue(source[i], ["message", "title", "event_type"], "Operational event received"),
                    time: textValue(source[i], ["time", "recorded_at", "created_at"], "just now"),
                    color: textValue(source[i], ["severity"], "info") === "warning" ? "#f4b000" : "#4B3DA0"
                })
            }
            return out
        }
        return [
            { message: "Aircraft SGX8-01 preflight check completed", time: "2 min ago", color: "#28b947" },
            { message: "Mission Border Patrol Sweep completed", time: "28 min ago", color: "#4B3DA0" },
            { message: "Weather update: Conditions remain optimal", time: "45 min ago", color: "#276be8" },
            { message: "New mission plan Site Recon Bravo created", time: "1 hr ago", color: "#f4b000" },
            { message: "System backup completed successfully", time: "2 hr ago", color: "#8a8494" }
        ]
    }

    function showPanel(title, body) {
        root.modalTitle = title
        root.modalBody = body
        root.modalOpen = true
        root.toast = title
    }

    function logAction(type, message) {
        if (hasObject("eventLogManager"))
            eventLogManager.logEvent(type, "info", message)
        root.toast = message
    }

    function startPilotMode() {
        logAction("pilot_mode_opened", "Opening Pilot Mode")
        if (hasObject("appState"))
            appState.startPilotMode()
    }

    function openNewMission() {
        logAction("new_mission_opened", "Opening mission planner")
        if (hasObject("appState"))
            appState.openMissionSelector()
    }

    function missionRecordId(mission) {
        return textValue(mission, ["id", "local_id"], "")
    }

    function openMissionInPlanner(mission) {
        if (!mission)
            return
        var missionId = missionRecordId(mission)
        if (missionId.length === 0) {
            root.toast = "This sample mission cannot be opened. Sync missions from Control Center or create a new mission."
            return
        }
        if (!hasObject("missionSyncManager")) {
            root.toast = "Mission sync is unavailable."
            return
        }
        logAction("mission_opened", "Opening mission in planner: " + textValue(mission, ["name", "title", "mission_name"], missionId))
        missionSyncManager.openMission(mission)
    }

    function refreshWeather() {
        if (hasObject("weatherSyncManager") && hasObject("telemetryStore") && Math.abs(telemetryStore.latitude) > 0.000001)
            weatherSyncManager.refreshForCoordinate(telemetryStore.latitude, telemetryStore.longitude)
        else
            root.toast = "Weather will sync when a trusted session and location are available"
    }

    function refreshControlCenterDashboard(manual) {
        if (!hasObject("sessionManager")) {
            return
        }
        if (!sessionManager.operationsAllowed) {
            sessionManager.validateSession()
            if (manual)
                root.toast = "Checking Control Center session..."
            return
        }
        if (hasObject("missionSyncManager") && !missionSyncManager.syncing) {
            missionSyncManager.bootstrap()
        }
        if (hasObject("weatherSyncManager") && hasObject("telemetryStore") && Math.abs(telemetryStore.latitude) > 0.000001) {
            weatherSyncManager.refreshForCoordinate(telemetryStore.latitude, telemetryStore.longitude)
        }
        if (manual)
            root.toast = sessionManager.controlCenterReachable ? "Control Center sync refreshed" : "Control Center unreachable; using local cache"
    }

    Timer {
        id: toastTimer
        interval: 2600
        repeat: false
        onTriggered: root.toast = ""
    }

    Timer {
        id: controlCenterDashboardTimer
        interval: 60000
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: root.refreshControlCenterDashboard(false)
    }

    onToastChanged: {
        if (toast.length > 0)
            toastTimer.restart()
    }

    Rectangle {
        visible: root.compact
        anchors.left: parent.left
        anchors.leftMargin: 14
        anchors.top: parent.top
        anchors.topMargin: 96
        width: 118
        height: 38
        radius: 19
        color: missionDrawerMouse.containsMouse ? "#4f0aa6" : "#3b0787"
        border.color: "#ffffff"
        border.width: 1
        z: 35

        Text {
            anchors.centerIn: parent
            text: "MISSIONS"
            color: "#ffffff"
            font.pixelSize: 11
            font.bold: true
        }

        MouseArea {
            id: missionDrawerMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                if (root.filteredMissions().length > 0)
                    root.openMissionInPlanner(root.filteredMissions()[0])
                else
                    root.openNewMission()
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: root.surface
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 74
            color: "#ffffff"
            border.color: root.line
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: root.compact ? 14 : 26
                anchors.rightMargin: root.compact ? 14 : 22
                spacing: 14

                Image {
                    Layout.preferredWidth: root.compact ? 156 : 220
                    Layout.preferredHeight: 44
                    source: AssetRegistry.logos.full_logo
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    mipmap: true
                }

                Flickable {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 64
                    contentWidth: pillRow.implicitWidth
                    contentHeight: height
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    Row {
                        id: pillRow
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 10

                        TopStatusPill { iconSource: AssetRegistry.icons.plane; value: root.onlineAircraftCount() + "/" + root.aircraftItems().length; label: "Aircraft"; accent: root.purple }
                        TopStatusPill { iconSource: AssetRegistry.icons.boxicons_battery_3; value: root.telemetryConnected() ? telemetryStore.battery + "%" : "78%"; label: "Battery"; accent: "#28b947" }
                        TopStatusPill { iconSource: AssetRegistry.icons.lucide_satellite; value: root.telemetryConnected() ? telemetryStore.satellites : "18"; label: "GPS"; accent: root.purple }
                        TopStatusPill { iconSource: AssetRegistry.icons.boxicons_wifi; value: root.telemetryConnected() ? telemetryStore.transmission + "%" : "Strong"; label: "Link"; accent: "#28b947" }
                        TopStatusPill { iconSource: AssetRegistry.icons.boxicons_camera; value: root.telemetryConnected() ? telemetryStore.recordingState : "Online"; label: "Camera"; accent: "#171222" }
                        TopStatusPill { iconSource: AssetRegistry.icons.boxicons_wind_filled; value: root.telemetryConnected() ? Number(telemetryStore.speed).toFixed(1) + " m/s" : "0.0"; label: "Speed"; accent: "#171222" }
                        TopStatusPill { iconSource: AssetRegistry.icons.lucide_mountain; value: root.telemetryConnected() ? Number(telemetryStore.altitude).toFixed(0) + " m" : "122 m"; label: "Alt"; accent: "#171222" }
                        TopStatusPill { iconSource: AssetRegistry.icons.mdi_check_circle; value: root.telemetryConnected() && telemetryStore.armed ? "ARMED" : "OK"; label: "System"; accent: "#28b947" }
                    }
                }

                Rectangle {
                    Layout.preferredWidth: root.compact ? 54 : 230
                    Layout.preferredHeight: 50
                    radius: 28
                    color: profileMouse.containsMouse ? "#f7f3fb" : "#ffffff"
                    border.color: "#e1dceb"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 10
                        spacing: 10

                        Rectangle {
                            Layout.preferredWidth: 36
                            Layout.preferredHeight: 36
                            radius: 18
                            gradient: Gradient {
                                GradientStop { position: 0; color: "#5f16c5" }
                                GradientStop { position: 1; color: "#2f006b" }
                            }
                            AssetIcon {
                                anchors.centerIn: parent
                                width: 19
                                height: 19
                                iconSize: 19
                                source: AssetRegistry.icons.mdi_person_outline
                                active: true
                                inactiveOpacity: 1
                            }
                        }

                        ColumnLayout {
                            visible: !root.compact
                            Layout.fillWidth: true
                            spacing: 1
                            Text { Layout.fillWidth: true; text: root.profileName(); color: root.ink; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
                            Text { Layout.fillWidth: true; text: root.organizationName(); color: root.muted; font.pixelSize: 9; elide: Text.ElideRight }
                        }

                        Text {
                            visible: !root.compact
                            text: "v"
                            color: root.purple
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }

                    MouseArea {
                        id: profileMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: profileMenu.open()
                    }

	                    Popup {
	                        id: profileMenu
	                        x: parent.width - width
	                        y: parent.height + 8
	                        width: 190
                        height: 104
                        modal: false
                        focus: true
                        background: Rectangle { radius: 8; color: "#ffffff"; border.color: root.line }
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            Text { text: "Profile Settings"; color: root.ink; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true }
                            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: root.line }
                            Text {
                                text: "Logout"
                                color: root.purple
                                font.pixelSize: 12
                                font.bold: true
                                Layout.fillWidth: true
                                MouseArea {
                                    anchors.fill: parent
	                                    onClicked: {
	                                        profileMenu.close()
	                                        if (root.hasObject("authManager"))
	                                            authManager.logout()
	                                        else
	                                            root.toast = "Logout requested"
	                                    }
	                                }
	                            }
	                        }
	                    }
	                }

	                WindowCloseButton {
	                    Layout.preferredWidth: 42
	                    Layout.preferredHeight: 42
	                }
	            }
	        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                visible: !root.compact
                Layout.preferredWidth: root.leftSidebarWidth
                Layout.fillHeight: true
                color: "#fbfaff"
                border.color: root.line
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 14

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        SidebarTabButton { Layout.fillWidth: true; text: "Mission"; iconSource: AssetRegistry.icons.lucide_route; active: root.activeTab === "mission"; onClicked: root.activeTab = "mission" }
                        SidebarTabButton { Layout.fillWidth: true; text: "Material"; iconSource: AssetRegistry.icons.cube; active: root.activeTab === "material"; onClicked: root.activeTab = "material" }
                        SidebarTabButton { Layout.fillWidth: true; text: "Map"; iconSource: AssetRegistry.icons.lucide_satellite; active: root.activeTab === "map"; onClicked: root.activeTab = "map" }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            radius: 8
                            color: "#ffffff"
                            border.color: root.line
                            TextInput {
                                anchors.fill: parent
                                anchors.leftMargin: 38
                                anchors.rightMargin: 12
                                verticalAlignment: TextInput.AlignVCenter
                                text: root.searchText
                                color: root.ink
                                font.pixelSize: 12
                                clip: true
                                onTextChanged: root.searchText = text
                            }
                            Text {
                                visible: root.searchText.length === 0
                                anchors.left: parent.left
                                anchors.leftMargin: 38
                                anchors.verticalCenter: parent.verticalCenter
                                text: "Search missions..."
                                color: "#9b94a6"
                                font.pixelSize: 12
                            }
                            AssetIcon {
                                anchors.left: parent.left
                                anchors.leftMargin: 13
                                anchors.verticalCenter: parent.verticalCenter
                                width: 16
                                height: 16
                                iconSize: 16
                                source: AssetRegistry.icons.boxicons_search_big
                                active: true
                            }
                        }
                        Rectangle {
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 44
                            radius: 8
                            color: filterMouse.containsMouse ? "#f2ecfb" : "#ffffff"
                            border.color: root.line
                            AssetIcon {
                                anchors.centerIn: parent
                                width: 18
                                height: 18
                                iconSize: 18
                                source: AssetRegistry.icons.boxicons_filter
                                active: true
                            }
                            MouseArea {
                                id: filterMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: root.showPanel("Mission Filters", "Filter panel ready for mission type, status, date range, and sync state.")
                            }
                        }
                    }

                    Text {
                        text: "MISSION HISTORY"
                        color: root.ink
                            font.pixelSize: 11
                        font.bold: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        spacing: 10
                        model: root.filteredMissions()
                        delegate: MissionHistoryItem {
                            width: ListView.view.width
                            missionName: root.textValue(modelData, ["name", "title", "mission_name"], "Untitled Mission")
                            category: root.textValue(modelData, ["mission_type", "type", "kind"], "Surveillance")
                            timestamp: root.textValue(modelData, ["date", "updated_at", "created_at"], "Cached locally")
                            status: root.textValue(modelData, ["status", "execution_state", "sync_state"], "Completed")
                            badgeColor: category === "Logistics" ? "#e3efff" : (category === "Reconnaissance" ? "#ffe8d5" : (category === "Training" ? "#e4efff" : "#efe2ff"))
                            badgeTextColor: category === "Logistics" ? "#1f64c8" : (category === "Reconnaissance" ? "#d46c00" : (category === "Training" ? "#276be8" : "#5b22a8"))
                            iconSource: root.missionIcon(category)
                            onClicked: root.openMissionInPlanner(modelData)
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            radius: 8
                            color: newMissionMouse.containsMouse ? "#4f0aa6" : "#3b0787"
                            Row {
                                anchors.centerIn: parent
                                spacing: 8
                                AssetIcon { width: 17; height: 17; iconSize: 17; source: AssetRegistry.icons.plus; active: true; inactiveOpacity: 1 }
                                Text { text: "New Mission"; color: "#ffffff"; font.pixelSize: 12; font.bold: true }
                            }
                            MouseArea { id: newMissionMouse; anchors.fill: parent; hoverEnabled: true; onClicked: root.openNewMission() }
                        }
                        Rectangle {
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 44
                            radius: 8
                            color: settingsMouse.containsMouse ? "#f2ecfb" : "#ffffff"
                            border.color: root.line
                            AssetIcon { anchors.centerIn: parent; width: 18; height: 18; iconSize: 18; source: AssetRegistry.icons.edit; active: true }
                            MouseArea { id: settingsMouse; anchors.fill: parent; hoverEnabled: true; onClicked: root.showPanel("Dashboard Settings", "Mission sidebar preferences, filters, and cached mission controls are ready.") }
                        }
                    }
                }
            }

            Flickable {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                contentWidth: width
                contentHeight: dashboardContent.implicitHeight + 36
                boundsBehavior: Flickable.StopAtBounds

                ColumnLayout {
                    id: dashboardContent
                    width: parent.width - (root.dashboardOuterMargin * 2)
                    spacing: 16
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.margins: root.dashboardOuterMargin

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 16

                        ColumnLayout {
                            Layout.preferredWidth: root.centerContentWidth
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignTop
                            spacing: 16

                            HeroOperationsCard {
                                Layout.fillWidth: true
                                Layout.preferredHeight: root.compact ? 250 : 206
                                backgroundSource: AssetRegistry.dashboard.mission_operations_center_bg
                                metrics: [
                                    { label: "READY MISSIONS", value: String(root.activeMissionItems().length), iconSource: AssetRegistry.icons.lucide_grid_3x3, stateColor: "#ffffff" },
                                    { label: "AIRCRAFT ONLINE", value: root.onlineAircraftCount() + " / " + root.aircraftItems().length, iconSource: AssetRegistry.icons.plane, stateColor: "#ffffff" },
                                    { label: "ACTIVE FLIGHTS", value: root.hasObject("flightStatsManager") && flightStatsManager.active ? "1" : "0", iconSource: AssetRegistry.icons.lucide_route, stateColor: "#ffffff" },
                                    { label: "NOTIFICATIONS", value: root.hasObject("profileManager") ? String(profileManager.notifications.length) : "2", iconSource: AssetRegistry.icons.cloud, stateColor: "#ffffff" },
                                    { label: "WEATHER READY", value: root.weatherLabel(), iconSource: AssetRegistry.icons.boxicons_wind_filled, stateColor: root.weatherStatus() === "GOOD_TO_FLY" ? "#32e86a" : "#f4b000" },
                                    { label: "SYNC STATUS", value: root.hasObject("missionSyncManager") ? missionSyncManager.status : "Up to date", iconSource: AssetRegistry.icons.boxicons_wifi, stateColor: "#32e86a" }
                                ]
                                onOpenRequested: root.showPanel("Operations Center", "Expanded command-center overview is ready for live telemetry, mission state, and Control Center sync details.")
                            }

                            GridLayout {
                                Layout.fillWidth: true
                                columns: root.tablet ? 1 : 2
                                rowSpacing: 16
                                columnSpacing: 16

                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 286
                                    radius: 10
                                    color: "#ffffff"
                                    border.color: root.line

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 18
                                        spacing: 12
                                        RowLayout {
                                            Layout.fillWidth: true
                                            Text { Layout.fillWidth: true; text: "Assigned Aircraft"; color: root.ink; font.pixelSize: 16; font.bold: true }
                                            Rectangle {
                                                Layout.preferredWidth: 82
                                                Layout.preferredHeight: 30
                                                radius: 7
                                                color: fleetMouse.containsMouse ? "#f5f0fb" : "#ffffff"
                                                border.color: "#d8ceeb"
                                                Text { anchors.centerIn: parent; text: "View Fleet"; color: root.purpleDark; font.pixelSize: 11; font.bold: true }
                                                MouseArea { id: fleetMouse; anchors.fill: parent; hoverEnabled: true; onClicked: root.showPanel("Fleet Center", "Aircraft telemetry, health, assignment, maintenance, and live status are ready for expansion.") }
                                            }
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            spacing: 14
                                            Repeater {
                                                model: Math.min(2, root.aircraftItems().length)
                                                delegate: AircraftSummaryCard {
                                                    Layout.fillWidth: true
                                                    Layout.fillHeight: true
                                                    imageSource: index === 0 ? AssetRegistry.aircraft.x8_01 : AssetRegistry.aircraft.vtol_03
                                                    aircraftName: root.textValue(root.aircraftItems()[index], ["name", "tail_number", "serial_number", "aircraft_id"], index === 0 ? "SkyGrid X8-01" : "SkyGrid VTOL-03")
                                                    tailNumber: root.textValue(root.aircraftItems()[index], ["tail_number", "serial_number", "aircraft_id"], index === 0 ? "SGX8-01" : "SGVT-03")
                                                    battery: root.numberValue(root.aircraftItems()[index], ["battery_percent", "battery"], index === 0 ? 78 : 89)
                                                    linkQuality: root.textValue(root.aircraftItems()[index], ["link_quality", "rc_quality", "status"], "Strong")
                                                    lastSync: root.textValue(root.aircraftItems()[index], ["last_sync", "last_seen", "updated_at"], index === 0 ? "2 min ago" : "1 min ago")
                                                    ready: root.textValue(root.aircraftItems()[index], ["status", "readiness"], "Ready").toLowerCase().indexOf("ready") >= 0
                                                    onClicked: root.showPanel("Aircraft Details", aircraftName + " is selected. Live telemetry, battery history, link health, and assignment details are ready.")
                                                }
                                            }
                                        }
                                    }
                                }

                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 286
                                    radius: 10
                                    color: "#ffffff"
                                    border.color: root.line

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 18
                                        spacing: 14
                                        Text { text: "Quick Actions"; color: root.ink; font.pixelSize: 14; font.bold: true }
                                        GridLayout {
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            columns: 2
                                            rowSpacing: 14
                                            columnSpacing: 14
                                            QuickActionTile { Layout.fillWidth: true; Layout.fillHeight: true; iconSource: AssetRegistry.icons.plane; title: "Pilot Mode"; subtitle: "Live control"; primary: true; onClicked: root.startPilotMode() }
                                            QuickActionTile { Layout.fillWidth: true; Layout.fillHeight: true; iconSource: AssetRegistry.icons.plus; title: "New Mission"; subtitle: "Plan route"; onClicked: root.openNewMission() }
                                            QuickActionTile { Layout.fillWidth: true; Layout.fillHeight: true; iconSource: AssetRegistry.icons.boxicons_wifi; title: "Control Center"; subtitle: "Sync hub"; onClicked: { root.refreshControlCenterDashboard(true); root.showPanel("Control Center", "SGG_CC sync is " + (root.hasObject("sessionManager") && sessionManager.controlCenterReachable ? "online" : "using local cache") + ". Dashboard data refresh runs in the background so missions are not interrupted.") } }
                                            QuickActionTile { Layout.fillWidth: true; Layout.fillHeight: true; iconSource: AssetRegistry.icons.boxicons_save; title: "Logs"; subtitle: "Flight events"; onClicked: root.showPanel("Flight Logs", "Recent GCS events, telemetry sync events, and mission logs are available in the activity stream.") }
                                        }
                                    }
                                }
                            }

                            GridLayout {
                                Layout.fillWidth: true
                                columns: root.tablet ? 1 : 3
                                rowSpacing: 16
                                columnSpacing: 16
                                ReadinessCard {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 250
                                    readiness: root.readinessPercent()
                                    checks: root.readinessCheckItems()
                                    onDetailsRequested: {
                                        if (root.hasObject("preflightChecklistManager")) {
                                            preflightChecklistManager.runChecklist(true)
                                            var reason = preflightChecklistManager.blockReason()
                                            root.showPanel("Readiness Details",
                                                preflightChecklistManager.status
                                                + (reason.length > 0 ? ("\n\n" + reason) : ""))
                                        } else {
                                            root.showPanel("Readiness Details", "Readiness engine is using aircraft, communications, weather, mission data, and sync status.")
                                        }
                                    }
                                }
                                RecentActivityCard {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 250
                                    activities: root.activityItems()
                                    onViewAllRequested: root.showPanel("Activity Feed", "The full GCS and Control Center event log is ready for review.")
                                }
                                SystemStatusCard {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 250
                                    rows: [
                                        { label: "Command & Control", value: root.hasObject("sessionManager") && sessionManager.operationsAllowed ? "Operational" : "Local Ready", ok: true },
                                        { label: "Data Link", value: root.telemetryConnected() ? telemetryStore.rcQuality : "Strong", ok: true },
                                        { label: "GNSS", value: root.telemetryConnected() ? telemetryStore.satellites + " Satellites" : "18 Satellites", ok: true },
                                        { label: "Camera Systems", value: root.telemetryConnected() ? telemetryStore.recordingState : "Online", ok: true },
                                        { label: "Storage", value: "72% Available", ok: true },
                                        { label: "Software Version", value: "v2.4.1", ok: true }
                                    ]
                                }
                            }
                        }

                        ColumnLayout {
                            visible: !root.tablet
                            Layout.preferredWidth: root.rightSidebarWidth
                            Layout.minimumWidth: root.rightSidebarWidth
                            Layout.maximumWidth: root.rightSidebarWidth
                            Layout.fillHeight: false
                            Layout.alignment: Qt.AlignTop
                            spacing: 16

                            PilotAssignmentCard {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 300
                                pilotName: root.profileName()
                                organization: root.organizationName()
                                unitSystem: root.hasObject("preferencesManager") ? preferencesManager.units.toUpperCase() : "METRIC"
                                assignment: root.aircraftItems().length > 0 ? root.textValue(root.aircraftItems()[0], ["name", "tail_number"], "Unassigned") : "Unassigned"
                                clearance: "Level 3"
                                lastLogin: "May 20, 2026 - 08:21"
                                onPilotModeRequested: root.startPilotMode()
                                onChangePilotRequested: root.showPanel("Change Pilot", "Pilot assignment modal is ready for organization-approved operator switching.")
                            }

                            WeatherForecastCard {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 255
                                status: root.weatherStatus()
                                temperature: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.temperatureC).toFixed(0) + " C" : "28 C"
                                condition: root.hasObject("weatherManager") && weatherManager.available ? weatherManager.status : "Partly Cloudy"
                                wind: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.windSpeedMps * 3.6).toFixed(0) + " km/h " + root.weatherDirectionLabel(weatherManager.windDirectionDeg) : "8 km/h NE"
                                visibility: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.visibilityKm).toFixed(1) + " km" : "10 km"
                                precipitation: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.precipitationProbability).toFixed(0) + "%" : "10%"
                                gusts: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.windGustMps * 3.6).toFixed(0) + " km/h" : "15 km/h"
                                forecastWindow: root.weatherStatus() === "GOOD_TO_FLY" ? "Good for next 6 hours" : "Review before flight"
                                syncing: root.hasObject("weatherSyncManager") && weatherSyncManager.syncing
                                onRefreshRequested: root.refreshWeather()
                            }
                        }
                    }

                    ColumnLayout {
                        visible: root.tablet
                        Layout.fillWidth: true
                        spacing: 16
                        PilotAssignmentCard {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 250
                            pilotName: root.profileName()
                            organization: root.organizationName()
                            unitSystem: root.hasObject("preferencesManager") ? preferencesManager.units.toUpperCase() : "METRIC"
                            assignment: root.aircraftItems().length > 0 ? root.textValue(root.aircraftItems()[0], ["name", "tail_number"], "Unassigned") : "Unassigned"
                            clearance: "Level 3"
                            lastLogin: "May 20, 2026 - 08:21"
                            onPilotModeRequested: root.startPilotMode()
                            onChangePilotRequested: root.showPanel("Change Pilot", "Pilot assignment modal is ready for organization-approved operator switching.")
                        }
                        WeatherForecastCard {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 245
                            status: root.weatherStatus()
                            temperature: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.temperatureC).toFixed(0) + " C" : "28 C"
                            condition: root.hasObject("weatherManager") && weatherManager.available ? weatherManager.status : "Partly Cloudy"
                            wind: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.windSpeedMps * 3.6).toFixed(0) + " km/h " + root.weatherDirectionLabel(weatherManager.windDirectionDeg) : "8 km/h NE"
                            visibility: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.visibilityKm).toFixed(1) + " km" : "10 km"
                            precipitation: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.precipitationProbability).toFixed(0) + "%" : "10%"
                            gusts: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.windGustMps * 3.6).toFixed(0) + " km/h" : "15 km/h"
                            forecastWindow: root.weatherStatus() === "GOOD_TO_FLY" ? "Good for next 6 hours" : "Review before flight"
                            syncing: root.hasObject("weatherSyncManager") && weatherSyncManager.syncing
                            onRefreshRequested: root.refreshWeather()
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        visible: root.toast.length > 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 22
        width: Math.min(parent.width - 40, toastText.implicitWidth + 42)
        height: 42
        radius: 21
        color: "#211033ee"
        Text {
            id: toastText
            anchors.centerIn: parent
            text: root.toast
            color: "#ffffff"
            font.pixelSize: 12
            font.bold: true
            elide: Text.ElideRight
            width: parent.width - 30
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Rectangle {
        id: detailPanel
        visible: root.modalOpen
        z: 40
        width: root.compact ? Math.min(parent.width - 28, 560) : 410
        height: root.compact ? Math.min(250, parent.height - 130) : Math.min(310, parent.height - 120)
        anchors.right: parent.right
        anchors.rightMargin: root.compact ? 14 : 24
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.compact ? 18 : 24
        radius: 12
        color: "#ffffff"
        border.color: "#d8ceeb"
        border.width: 1
        opacity: root.modalOpen ? 1 : 0

        Rectangle {
            anchors.fill: parent
            anchors.margins: -1
            radius: parent.radius
            color: "transparent"
            border.color: "#ffffff"
            border.width: 1
            opacity: 0.7
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 12

            RowLayout {
                Layout.fillWidth: true

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.fillWidth: true
                        text: root.modalTitle
                        color: root.ink
                        font.pixelSize: 17
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Text {
                        Layout.fillWidth: true
                        text: "Dashboard detail panel"
                        color: root.muted
                        font.pixelSize: 10
                        elide: Text.ElideRight
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    radius: 16
                    color: closeMouse.containsMouse ? "#f2ecfb" : "#ffffff"
                    border.color: root.line
                    Text {
                        anchors.centerIn: parent
                        text: "X"
                        color: root.purple
                        font.pixelSize: 11
                        font.bold: true
                    }
                    MouseArea {
                        id: closeMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: root.modalOpen = false
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: root.line
            }

            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: root.modalBody
                color: root.muted
                font.pixelSize: 13
                wrapMode: Text.WordWrap
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    radius: 8
                    color: "#f7f3fb"
                    border.color: "#d8ceeb"
                    Text {
                        anchors.centerIn: parent
                        text: "Keep Dashboard Visible"
                        color: root.purple
                        font.pixelSize: 11
                        font.bold: true
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 84
                    Layout.preferredHeight: 36
                    radius: 8
                    color: doneMouse.containsMouse ? "#4f0aa6" : "#3b0787"
                    Text {
                        anchors.centerIn: parent
                        text: "Done"
                        color: "#ffffff"
                        font.pixelSize: 11
                        font.bold: true
                    }
                    MouseArea {
                        id: doneMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: root.modalOpen = false
                    }
                }
            }
        }

        Behavior on opacity { NumberAnimation { duration: 140 } }
    }

    Connections {
        target: root.hasObject("preflightChecklistManager") ? preflightChecklistManager : null
        function onChecklistChanged() { root.readinessRevision += 1 }
    }
}
