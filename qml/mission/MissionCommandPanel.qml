import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    radius: 8
    color: "#ffffff"
    border.color: "#d8cde7"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 7

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Text {
                text: "Flight Actions"
                color: Theme.purple
                font.pixelSize: 12
                font.bold: true
                Layout.fillWidth: true
            }
            Text {
                text: vehicleManager.connected ? "AIRCRAFT ONLINE" : "NO AIRCRAFT"
                color: vehicleManager.connected ? Theme.green : Theme.red
                font.pixelSize: 10
                font.bold: true
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            radius: 6
            color: vehicleManager.connected ? "#effaf2" : "#fff8df"
            border.color: vehicleManager.connected ? "#b7dfbf" : "#e8cf7c"
            RowLayout {
                anchors.fill: parent
                anchors.margins: 7
                spacing: 8
                Text {
                    text: telemetryStore.armed ? "ARMED" : "DISARMED"
                    color: telemetryStore.armed ? Theme.red : Theme.green
                    font.pixelSize: 10
                    font.bold: true
                }
                Text {
                    Layout.fillWidth: true
                    text: vehicleManager.connected
                          ? (preflightChecklistManager.canArm ? "Preflight permits flight actions" : "Preflight blocks arm/takeoff")
                          : "Connect aircraft before sending direct commands"
                    color: "#4f465b"
                    font.pixelSize: 10
                    elide: Text.ElideRight
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: 6
            columnSpacing: 6

            CommandButton {
                Layout.fillWidth: true
                label: "Arm"
                iconSource: AssetRegistry.icons.plane
                visible: accessManager.can("can_fly_manual")
                enabled: sessionManager.operationsAllowed && vehicleManager.connected && preflightChecklistManager.canArm && !vehicleActionManager.busy && !telemetryStore.armed
                onTriggered: vehicleActionManager.arm()
            }
            CommandButton {
                Layout.fillWidth: true
                label: "Takeoff"
                iconSource: AssetRegistry.icons.iconamoon_arrow_up_2
                visible: accessManager.can("can_fly_manual")
                enabled: sessionManager.operationsAllowed && vehicleManager.connected && preflightChecklistManager.canArm && !vehicleActionManager.busy
                onTriggered: telemetryStore.armed ? vehicleActionManager.takeoff() : vehicleActionManager.armAndTakeoff()
            }
            CommandButton {
                Layout.fillWidth: true
                label: "Disarm"
                iconSource: AssetRegistry.icons.iconoir_cancel
                visible: accessManager.can("can_fly_manual")
                enabled: sessionManager.operationsAllowed && vehicleManager.connected && !vehicleActionManager.busy && telemetryStore.armed
                onTriggered: vehicleActionManager.disarm()
            }
            CommandButton {
                Layout.fillWidth: true
                label: "Land"
                iconSource: AssetRegistry.icons.iconamoon_arrow_up_2
                iconRotation: 180
                visible: accessManager.can("can_fly_manual")
                enabled: sessionManager.operationsAllowed && vehicleManager.connected && !vehicleActionManager.busy
                onTriggered: vehicleActionManager.land()
            }
            CommandButton {
                Layout.fillWidth: true
                label: "RTH"
                iconSource: AssetRegistry.icons.lucide_route
                visible: accessManager.can("can_fly_manual")
                enabled: sessionManager.operationsAllowed && vehicleManager.connected && !vehicleActionManager.busy
                onTriggered: vehicleActionManager.returnToLaunch()
            }
            CommandButton {
                Layout.fillWidth: true
                label: "Set POI"
                iconSource: AssetRegistry.icons.pin_location
                active: appState.selectedTool === "poi"
                enabled: true
                onTriggered: appState.selectedTool = "poi"
            }
        }

        Text {
            Layout.fillWidth: true
            text: vehicleActionManager.status
            color: Theme.muted
            font.pixelSize: 10
            elide: Text.ElideRight
        }
    }

    component CommandButton: Button {
        id: command
        property string label: ""
        property url iconSource: ""
        property bool active: false
        property real iconRotation: 0
        signal triggered()

        implicitHeight: 40
        hoverEnabled: true
        opacity: enabled ? 1 : 0.48
        onClicked: triggered()

        contentItem: Column {
            spacing: 2
            anchors.centerIn: parent
            AssetIcon {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 16
                height: 16
                source: command.iconSource
                active: command.active || command.hovered
                rotation: command.iconRotation
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: command.label
                color: command.active ? Theme.white : Theme.ink
                font.pixelSize: 10
                font.bold: true
            }
        }

        background: Rectangle {
            radius: 6
            color: command.active ? Theme.purple : (command.hovered ? "#eee8f7" : "#f8f5fb")
            border.color: command.active ? Theme.purple : "#ddd2e9"
            border.width: 1
            Behavior on color { ColorAnimation { duration: Theme.animFast } }
            Behavior on border.color { ColorAnimation { duration: Theme.animFast } }
        }
    }
}
