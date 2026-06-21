import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Item {
    id: root
    focus: true

    property bool keyForward: false
    property bool keyBack: false
    property bool keyYawLeft: false
    property bool keyYawRight: false
    property bool keyLeft: false
    property bool keyRight: false
    property bool keyUp: false
    property bool keyDown: false
    property string viewMode: preferencesManager.pilotViewMode
    property bool joystickVisible: preferencesManager.joystickVisible
    property bool targetVisible: false
    property real targetLatitude: 0
    property real targetLongitude: 0
    property double lastManualActionAt: 0
    readonly property bool compactLayout: width <= 1280 || height <= 720
    readonly property real sidePanelWidth: Math.max(292, Math.min(318, width * 0.18))

    function updateManualInput() {
        if (!accessManager.canPerform("manual_flight")) {
            manualControlManager.neutral()
            return
        }
        var forward = (keyForward ? 1 : 0) + (keyBack ? -1 : 0)
        var lateral = (keyRight ? 1 : 0) + (keyLeft ? -1 : 0)
        var vertical = (keyUp ? 1 : 0) + (keyDown ? -1 : 0)
        var yaw = (keyYawRight ? 1 : 0) + (keyYawLeft ? -1 : 0)
        manualControlManager.setInput(forward, lateral, vertical, yaw)
        recordManualInput(forward, lateral, vertical, yaw)
    }

    function recordManualInput(forward, lateral, vertical, yaw) {
        var now = Date.now()
        if (now - lastManualActionAt < 250) return
        lastManualActionAt = now
        if (typeof pilotActionSyncManager !== "undefined") {
            pilotActionSyncManager.recordAction("manual_input_active", "Pilot manual input active", {
                forward: forward,
                lateral: lateral,
                vertical: vertical,
                yaw: yaw
            })
        }
    }

    function setViewMode(mode) {
        viewMode = mode
        preferencesManager.setPilotViewMode(mode)
        if (typeof pilotActionSyncManager !== "undefined") {
            pilotActionSyncManager.recordAction("pilot_view_mode_changed", "Pilot switched view mode", { view_mode: mode })
        } else {
            eventLogManager.logEvent("pilot_view_mode_changed", "info", "Pilot switched view mode", { view_mode: mode })
        }
    }

    Component.onCompleted: {
        forceActiveFocus()
        if (!accessManager.canPerform("manual_flight")) {
            eventLogManager.logEvent("pilot_mode_blocked", "warning", "Manual Flight mode blocked by local permissions")
            appState.goHome()
            return
        }
        if (typeof flightSessionSyncManager !== "undefined") {
            flightSessionSyncManager.beginPilotSession("")
        }
        if (typeof pilotActionSyncManager !== "undefined") {
            pilotActionSyncManager.recordAction("pilot_mode_started", "Manual Flight mode opened")
        } else {
            eventLogManager.logEvent("pilot_mode_started", "info", "Manual Flight mode opened")
        }
    }

    Component.onDestruction: {
        usbControllerManager.setEnabled(false)
        if (typeof flightSessionSyncManager !== "undefined" && flightSessionSyncManager.active) {
            flightSessionSyncManager.endActiveSession("completed", "Pilot mode closed")
        }
    }

    Keys.onPressed: function(event) {
        if (event.isAutoRepeat) return
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
                event.accepted = true
                return
            }
            manualControlManager.neutral()
            vehicleActionManager.holdPosition()
            if (typeof pilotActionSyncManager !== "undefined") {
                pilotActionSyncManager.recordAction("vehicle_hold", "Pilot requested hold from keyboard")
            } else {
                eventLogManager.logEvent("vehicle_hold_requested", "info", "Pilot requested hold from keyboard")
            }
            event.accepted = true
            return
        } else {
            return
        }
        updateManualInput()
        event.accepted = true
    }

    Keys.onReleased: function(event) {
        if (event.isAutoRepeat) return
        if (event.key === Qt.Key_W) keyForward = false
        else if (event.key === Qt.Key_S) keyBack = false
        else if (event.key === Qt.Key_A) keyYawLeft = false
        else if (event.key === Qt.Key_D) keyYawRight = false
        else if (event.key === Qt.Key_Left) keyLeft = false
        else if (event.key === Qt.Key_Right) keyRight = false
        else if (event.key === Qt.Key_Q) keyDown = false
        else if (event.key === Qt.Key_E) keyUp = false
        else return
        updateManualInput()
        event.accepted = true
    }

    Rectangle { anchors.fill: parent; color: "#f7f4fb" }

    PilotModeTopBar {
        id: topBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: Math.max(74, Math.min(84, parent.height * 0.08))
        z: 10
        onEventLogRequested: {
            if (eventLogManager.authorizeLogView()) {
                eventLogManager.logEvent("event_log_opened", "info", "Pilot opened event log")
                eventLogPanel.open()
            }
        }
    }

    RowLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: topBar.bottom
        anchors.bottom: parent.bottom
        spacing: 0

        Rectangle {
            visible: !root.compactLayout
            Layout.preferredWidth: visible ? root.sidePanelWidth : 0
            Layout.minimumWidth: 0
            Layout.fillHeight: true
            color: "#fbf9fe"
            border.color: "#e2d8ee"
            AircraftStatusPanel {
                anchors.fill: parent
                anchors.margins: 14
                onMoreSettingsRequested: settingsDrawer.open()
            }
        }

        Item {
            id: centerStage
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Loader {
                id: viewLoader
                anchors.fill: parent
                sourceComponent: root.viewMode === "VIDEO" ? videoPrimary
                                 : (root.viewMode === "SPLIT" ? splitPrimary : mapPrimary)
            }

            Component {
                id: mapPrimary
                MapPrimaryView {
                    hudVisible: root.viewMode === "HUD"
                    targetLatitude: root.targetLatitude
                    targetLongitude: root.targetLongitude
                    targetVisible: root.targetVisible
                }
            }

            Component {
                id: videoPrimary
                VideoPrimaryView {
                    hudVisible: root.viewMode === "HUD"
                    targetLatitude: root.targetLatitude
                    targetLongitude: root.targetLongitude
                    targetVisible: root.targetVisible
                }
            }

            Component {
                id: splitPrimary
                SplitFlightView {
                    hudVisible: root.viewMode === "HUD"
                    targetLatitude: root.targetLatitude
                    targetLongitude: root.targetLongitude
                    targetVisible: root.targetVisible
                }
            }

            PilotViewSwitcher {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: root.compactLayout ? 12 : 18
                mode: root.viewMode
                z: 30
                onModeSelected: root.setViewMode(mode)
            }

            Row {
                visible: root.compactLayout
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 14
                spacing: 8
                z: 31

                CompactPanelButton {
                    iconSource: AssetRegistry.icons.plane
                    onClicked: aircraftDrawer.open()
                    ToolTip.visible: hovered
                    ToolTip.text: "Aircraft status"
                }
                CompactPanelButton {
                    iconSource: AssetRegistry.icons.boxicons_wifi
                    onClicked: telemetryDrawer.open()
                    ToolTip.visible: hovered
                    ToolTip.text: "Telemetry"
                }
            }

            Row {
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.margins: root.compactLayout ? 12 : 18
                spacing: root.compactLayout ? 8 : 10
                z: 30
                JoystickVisibilityToggle {
                    controlsVisible: root.joystickVisible
                    onVisibilityToggled: function(visible) {
                        root.joystickVisible = visible
                        preferencesManager.setJoystickVisible(visible)
                        if (typeof pilotActionSyncManager !== "undefined") {
                            pilotActionSyncManager.recordAction("joystick_visibility_changed", visible ? "Pilot controls shown" : "Pilot controls hidden", { visible: visible })
                        } else {
                            eventLogManager.logEvent("joystick_visibility_changed", "info", visible ? "Pilot controls shown" : "Pilot controls hidden", { visible: visible })
                        }
                    }
                }
                Button {
                    width: root.compactLayout ? 96 : 132
                    height: 34
                    text: "Go To"
                    visible: accessManager.can("can_fly_manual")
                    enabled: sessionManager.operationsAllowed && vehicleManager.connected
                    onClicked: goToDialog.open()
                    background: Rectangle { radius: 17; color: parent.hovered ? "#ffffff" : "#ffffffdd"; border.color: "#5b22a8" }
                    contentItem: Text { text: parent.text; color: "#5b22a8"; font.pixelSize: 11; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                }
                Button {
                    width: root.compactLayout ? 104 : 136
                    height: 34
                    text: usbControllerManager.enabled ? "USB RC On" : "USB RC"
                    onClicked: usbControllerPanel.open()
                    background: Rectangle {
                        radius: 17
                        color: usbControllerManager.enabled ? "#eefbf1" : (parent.hovered ? "#ffffff" : "#ffffffdd")
                        border.color: usbControllerManager.enabled ? "#18b83f" : "#5b22a8"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: usbControllerManager.enabled ? "#11842d" : "#5b22a8"
                        font.pixelSize: 11
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            VirtualJoystickOverlay {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: root.compactLayout ? 18 : 28
                width: Math.min(root.compactLayout ? 430 : 560, parent.width * (root.compactLayout ? 0.84 : 0.62))
                height: root.compactLayout ? 144 : (root.width < 1300 ? 164 : 184)
                z: 25
                visible: root.joystickVisible && accessManager.can("can_fly_manual")
                enabled: permissionManager.canFlyManual && telemetryStore.inAir && vehicleManager.connected && sessionManager.operationsAllowed
            }
        }

        Rectangle {
            visible: !root.compactLayout
            Layout.preferredWidth: visible ? root.sidePanelWidth : 0
            Layout.minimumWidth: 0
            Layout.fillHeight: true
            color: "#fbf9fe"
            border.color: "#e2d8ee"
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10
                TelemetryPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumHeight: 300
                }
                WindCompassWidget {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 108
                }
                CameraFpvPanel {
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.viewMode === "VIDEO" ? 150 : 220
                    visible: root.viewMode !== "VIDEO"
                }
            }
        }
    }

    ManualSettingsDrawer { id: settingsDrawer }
    UsbControllerPanel { id: usbControllerPanel }
    EventLogPanel { id: eventLogPanel }
    Drawer {
        id: aircraftDrawer
        edge: Qt.LeftEdge
        width: Math.min(340, root.width * 0.86)
        height: root.height - topBar.height
        y: topBar.height
        modal: true
        interactive: root.compactLayout
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle { color: "#fbf9fe"; border.color: "#e2d8ee" }
        AircraftStatusPanel {
            anchors.fill: parent
            anchors.margins: 14
            onMoreSettingsRequested: settingsDrawer.open()
        }
    }
    Drawer {
        id: telemetryDrawer
        edge: Qt.RightEdge
        width: Math.min(340, root.width * 0.86)
        height: root.height - topBar.height
        y: topBar.height
        modal: true
        interactive: root.compactLayout
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle { color: "#fbf9fe"; border.color: "#e2d8ee" }
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 10
            TelemetryPanel {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 300
            }
            WindCompassWidget {
                Layout.fillWidth: true
                Layout.preferredHeight: 108
            }
            CameraFpvPanel {
                Layout.fillWidth: true
                Layout.preferredHeight: root.viewMode === "VIDEO" ? 150 : 220
                visible: root.viewMode !== "VIDEO"
            }
        }
    }
    GoToCoordinateDialog {
        id: goToDialog
        onAcceptedTarget: function(latitude, longitude, altitude, speed) {
            root.targetLatitude = latitude
            root.targetLongitude = longitude
            root.targetVisible = true
        }
    }

    component CompactPanelButton: Button {
        id: panelButton
        property url iconSource: ""
        width: 38
        height: 38
        hoverEnabled: true
        contentItem: AssetIcon {
            anchors.centerIn: parent
            width: 18
            height: 18
            source: panelButton.iconSource
            active: true
        }
        background: Rectangle {
            radius: 19
            color: panelButton.hovered ? "#ffffff" : "#ffffffdd"
            border.color: "#5b22a8"
        }
    }
}
