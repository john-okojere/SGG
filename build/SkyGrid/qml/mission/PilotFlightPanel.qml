import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

ColumnLayout {
    id: root
    spacing: 10

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 78
        radius: 8
        color: "#ffffff"
        border.color: Theme.line
        RowLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 10
            Rectangle {
                Layout.preferredWidth: 46
                Layout.preferredHeight: 46
                radius: 23
                color: telemetryStore.armed ? Theme.purple : "#eee8f7"
                border.color: telemetryStore.connected ? Theme.green : Theme.red
                border.width: 2
                AssetIcon {
                    anchors.centerIn: parent
                    width: 26
                    height: 26
                    source: AssetRegistry.icons.plane
                    active: telemetryStore.armed
                    rotation: telemetryStore.heading
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text {
                    text: telemetryStore.connected ? telemetryStore.aircraftId : "No Connected Aircraft"
                    color: Theme.ink
                    font.pixelSize: 15
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                Text {
                    text: telemetryStore.flightMode + " · " + (telemetryStore.armed ? "ARMED" : "DISARMED") + " · " + telemetryStore.gpsMode
                    color: Theme.muted
                    font.pixelSize: 11
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
            Rectangle {
                Layout.preferredWidth: 82
                Layout.preferredHeight: 28
                radius: 5
                color: mapState.followAircraft ? Theme.purple : "#f3eff7"
                border.color: Theme.line
                Text {
                    anchors.centerIn: parent
                    text: "Follow"
                    color: mapState.followAircraft ? Theme.white : Theme.purple
                    font.pixelSize: 11
                    font.bold: true
                }
                MouseArea { anchors.fill: parent; onClicked: mapState.followAircraft = !mapState.followAircraft }
            }
        }
    }

    GridLayout {
        Layout.fillWidth: true
        columns: 3
        rowSpacing: 8
        columnSpacing: 8
        PilotMetric { label: "Altitude"; value: Number(telemetryStore.altitude).toFixed(1); unit: "m" }
        PilotMetric { label: "Speed"; value: Number(telemetryStore.speed).toFixed(1); unit: "m/s" }
        PilotMetric { label: "Heading"; value: Number(telemetryStore.heading).toFixed(0); unit: "deg" }
        PilotMetric { label: "Battery"; value: telemetryStore.battery; unit: "%" }
        PilotMetric { label: "GPS"; value: telemetryStore.satellites; unit: "sat" }
        PilotMetric { label: "Link"; value: telemetryStore.transmission; unit: "%" }
    }

    MissionCommandPanel {
        Layout.fillWidth: true
        Layout.preferredHeight: 166
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 292
        radius: 8
        color: "#ffffff"
        border.color: Theme.line
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8
            RowLayout {
                Layout.fillWidth: true
                Text { text: "Direct Control"; color: Theme.purple; font.pixelSize: 13; font.bold: true; Layout.fillWidth: true }
                Text { text: manualControlManager.active ? "ACTIVE" : "STANDBY"; color: manualControlManager.active ? Theme.green : Theme.muted; font.pixelSize: 10; font.bold: true }
            }
            Text {
                Layout.fillWidth: true
                text: "Use the buttons or keyboard: W/S forward/back · arrows lateral · A/D yaw · Q/E altitude"
                color: Theme.muted
                font.pixelSize: 11
                wrapMode: Text.WordWrap
            }

            GridLayout {
                Layout.alignment: Qt.AlignHCenter
                columns: 3
                rowSpacing: 6
                columnSpacing: 6

                Item { Layout.preferredWidth: 74; Layout.preferredHeight: 34 }
                PadButton {
                    label: "Forward"
                    onControlPressed: manualControlManager.setInput(1, 0, 0, 0)
                    onControlReleased: manualControlManager.neutral()
                }
                PadButton {
                    label: "Up"
                    onControlPressed: manualControlManager.setInput(0, 0, 1, 0)
                    onControlReleased: manualControlManager.neutral()
                }

                PadButton {
                    label: "Left"
                    onControlPressed: manualControlManager.setInput(0, -1, 0, 0)
                    onControlReleased: manualControlManager.neutral()
                }
                PadButton {
                    label: "Hold"
                    active: true
                    onControlPressed: {
                        manualControlManager.neutral()
                        vehicleActionManager.holdPosition()
                    }
                }
                PadButton {
                    label: "Right"
                    onControlPressed: manualControlManager.setInput(0, 1, 0, 0)
                    onControlReleased: manualControlManager.neutral()
                }

                PadButton {
                    label: "Yaw L"
                    onControlPressed: manualControlManager.setInput(0, 0, 0, -1)
                    onControlReleased: manualControlManager.neutral()
                }
                PadButton {
                    label: "Back"
                    onControlPressed: manualControlManager.setInput(-1, 0, 0, 0)
                    onControlReleased: manualControlManager.neutral()
                }
                PadButton {
                    label: "Yaw R"
                    onControlPressed: manualControlManager.setInput(0, 0, 0, 1)
                    onControlReleased: manualControlManager.neutral()
                }

                Item { Layout.preferredWidth: 74; Layout.preferredHeight: 34 }
                PadButton {
                    label: "Down"
                    onControlPressed: manualControlManager.setInput(0, 0, -1, 0)
                    onControlReleased: manualControlManager.neutral()
                }
                Item { Layout.preferredWidth: 74; Layout.preferredHeight: 34 }
            }

            RowLayout {
                Layout.fillWidth: true
                PrimaryButton {
                    Layout.fillWidth: true
                    secondary: true
                    text: "Hold"
                    iconSource: AssetRegistry.icons.boxicons_cursor_pointer
                    enabled: vehicleManager.connected
                    onClicked: {
                        manualControlManager.neutral()
                        vehicleActionManager.holdPosition()
                    }
                }
                PrimaryButton {
                    Layout.fillWidth: true
                    text: "Emergency Stop"
                    iconSource: AssetRegistry.icons.iconoir_cancel
                    enabled: vehicleManager.connected
                    onClicked: vehicleActionManager.emergencyStop()
                }
            }
            Text {
                Layout.fillWidth: true
                text: manualControlManager.status
                color: Theme.muted
                font.pixelSize: 10
                elide: Text.ElideRight
            }
        }
    }

    component PilotMetric: Rectangle {
        property string label: ""
        property string value: ""
        property string unit: ""
        Layout.fillWidth: true
        Layout.preferredHeight: 56
        radius: 7
        color: "#ffffff"
        border.color: Theme.line
        Column {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 3
            Text { text: label; color: Theme.muted; font.pixelSize: 10; font.bold: true }
            Text { text: value + " " + unit; color: Theme.ink; font.pixelSize: 14; font.bold: true; elide: Text.ElideRight; width: parent.width }
        }
    }

    component PadButton: Button {
        id: pad
        property string label: ""
        property bool active: false
        signal controlPressed()
        signal controlReleased()

        Layout.preferredWidth: 74
        Layout.preferredHeight: 34
        enabled: vehicleManager.connected && sessionManager.operationsAllowed
        hoverEnabled: true
        opacity: enabled ? 1 : 0.46
        onPressedChanged: {
            if (pressed) {
                controlPressed()
            } else {
                controlReleased()
            }
        }

        contentItem: Text {
            text: pad.label
            color: pad.active || pad.pressed ? Theme.white : Theme.ink
            font.pixelSize: 11
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 6
            color: pad.active || pad.pressed ? Theme.purple : (pad.hovered ? "#eee8f7" : "#f8f5fb")
            border.color: pad.active || pad.pressed ? Theme.purple : Theme.line
            Behavior on color { ColorAnimation { duration: Theme.animFast } }
        }
    }
}
