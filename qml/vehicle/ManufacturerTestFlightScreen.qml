import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Item {
    id: root

    readonly property bool authorized: permissionManager.canRunManufacturerTestFlight && moduleAccessManager.manufacturerTestFlightWorkspaceAllowed
    readonly property bool compact: width < 1100

    Rectangle {
        anchors.fill: parent
        color: "#f6f4fa"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 74
            color: "#ffffff"
            border.color: "#e2dceb"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: root.compact ? 16 : 26
                anchors.rightMargin: root.compact ? 16 : 26
                spacing: 14

                Button {
                    id: backButton
                    Layout.preferredWidth: 42
                    Layout.preferredHeight: 38
                    text: "<"
                    onClicked: appState.openDefaultWorkspace("vehicleConfiguration")
                    background: Rectangle {
                        radius: 8
                        color: backButton.hovered ? "#f1eafa" : "#ffffff"
                        border.color: "#d8ceeb"
                    }
                    contentItem: Text {
                        text: backButton.text
                        color: "#3b0787"
                        font.pixelSize: 16
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.fillWidth: true
                        text: "Manufacturer Test Flight"
                        color: "#171222"
                        font.pixelSize: root.compact ? 18 : 22
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Text {
                        Layout.fillWidth: true
                        text: "Restricted verification workspace. Customer pilot controls and mission start stay locked."
                        color: "#706a7e"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 170
                    Layout.preferredHeight: 38
                    radius: 8
                    color: root.authorized ? "#ecf8ef" : "#fff6d8"
                    border.color: root.authorized ? "#bfe8c8" : "#efd06c"
                    Text {
                        anchors.centerIn: parent
                        width: parent.width - 18
                        text: root.authorized ? "TEST ACCESS" : "LOCKED"
                        color: root.authorized ? "#14692a" : "#715400"
                        font.pixelSize: 11
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }
                }
            }
        }

        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width
            contentHeight: content.implicitHeight + 36
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            ColumnLayout {
                id: content
                width: parent.width - (root.compact ? 32 : 48)
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: root.compact ? 16 : 24
                spacing: 16

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 74
                    radius: 8
                    color: "#fff6d8"
                    border.color: "#efd06c"
                    Text {
                        anchors.fill: parent
                        anchors.margins: 16
                        text: "Remove propellers before bench testing."
                        color: "#715400"
                        font.pixelSize: 18
                        font.bold: true
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: root.compact ? 1 : 2
                    rowSpacing: 16
                    columnSpacing: 16

                    VehicleConnectionPanel {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 360
                        enabled: root.authorized
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 360
                        radius: 8
                        color: "#ffffff"
                        border.color: "#e2dceb"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            spacing: 12
                            Text {
                                Layout.fillWidth: true
                                text: "Test Flight Gates"
                                color: "#171222"
                                font.pixelSize: 16
                                font.bold: true
                                elide: Text.ElideRight
                            }
                            GateRow {
                                label: "Vehicle connection"
                                value: vehicleManager.connected ? "Connected" : "Required"
                                ok: vehicleManager.connected
                            }
                            GateRow {
                                label: "Autopilot health"
                                value: vehicleManager.health
                                ok: vehicleManager.health === "Ready"
                            }
                            GateRow {
                                label: "Armed state"
                                value: vehicleManager.armed ? "Armed" : "Disarmed"
                                ok: !vehicleManager.armed
                            }
                            GateRow {
                                label: "Flight mode"
                                value: vehicleManager.flightMode
                                ok: vehicleManager.connected
                            }
                            GateRow {
                                label: "Permission"
                                value: root.authorized ? "Manufacturer test flight allowed" : "Permission required"
                                ok: root.authorized
                            }
                            GateRow {
                                label: "Safety mode"
                                value: "Bench test blocks arm, takeoff, motor test, and mission start"
                                ok: true
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 250
                        radius: 8
                        color: "#ffffff"
                        border.color: "#e2dceb"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            spacing: 12
                            Text {
                                Layout.fillWidth: true
                                text: "Telemetry"
                                color: "#171222"
                                font.pixelSize: 16
                                font.bold: true
                                elide: Text.ElideRight
                            }
                            GateRow {
                                label: "Latitude"
                                value: telemetryStore.latitude.toFixed(6)
                                ok: vehicleManager.connected
                            }
                            GateRow {
                                label: "Longitude"
                                value: telemetryStore.longitude.toFixed(6)
                                ok: vehicleManager.connected
                            }
                            GateRow {
                                label: "Altitude"
                                value: telemetryStore.altitude.toFixed(1) + " m"
                                ok: vehicleManager.connected
                            }
                            GateRow {
                                label: "Battery"
                                value: telemetryStore.battery.toFixed(0) + "%"
                                ok: telemetryStore.battery > 25
                            }
                            GateRow {
                                label: "Satellites"
                                value: String(telemetryStore.satellites)
                                ok: telemetryStore.satellites >= 6
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 250
                        radius: 8
                        color: "#ffffff"
                        border.color: "#e2dceb"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            spacing: 12
                            Text {
                                Layout.fillWidth: true
                                text: "Safe Test Actions"
                                color: "#171222"
                                font.pixelSize: 16
                                font.bold: true
                                elide: Text.ElideRight
                            }
                            ButtonRow {
                                label: "Read parameters"
                                enabledState: permissionManager.canReadVehicleParameters
                                onClicked: vehicleConfigManager.readParameterSnapshot()
                            }
                            ButtonRow {
                                label: "Save snapshot"
                                enabledState: permissionManager.canWriteVehicleParameters
                                onClicked: vehicleConfigManager.saveParameterSnapshot({})
                            }
                            ButtonRow {
                                label: "Stop discovery"
                                enabledState: vehicleManager.discoveryActive
                                onClicked: vehicleConfigManager.disconnectVehicle()
                            }
                            Text {
                                Layout.fillWidth: true
                                text: "Arm, takeoff, mission start, and motor test commands are intentionally unavailable in this workspace."
                                color: "#706a7e"
                                font.pixelSize: 12
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        visible: !root.authorized
        color: "#f6f4facc"
        z: 20

        Rectangle {
            width: Math.min(parent.width - 48, 520)
            height: 170
            anchors.centerIn: parent
            radius: 8
            color: "#ffffff"
            border.color: "#e2dceb"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 22
                spacing: 12
                Text {
                    Layout.fillWidth: true
                    text: "Manufacturer test flight is locked"
                    color: "#171222"
                    font.pixelSize: 20
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    Layout.fillWidth: true
                    text: "Your Control Center permissions do not include manufacturer test flight authority."
                    color: "#706a7e"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
                Button {
                    id: vehicleConfigButton
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 180
                    Layout.preferredHeight: 36
                    text: "Vehicle Config"
                    onClicked: appState.openDefaultWorkspace("vehicleConfiguration")
                    background: Rectangle {
                        radius: 8
                        color: vehicleConfigButton.hovered ? "#4f0aa6" : "#3b0787"
                    }
                    contentItem: Text {
                        text: vehicleConfigButton.text
                        color: "#ffffff"
                        font.pixelSize: 12
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }

    component GateRow: RowLayout {
        id: gateRow
        property string label: ""
        property string value: ""
        property bool ok: false
        Layout.fillWidth: true
        Rectangle {
            Layout.preferredWidth: 8
            Layout.preferredHeight: 8
            radius: 4
            color: gateRow.ok ? "#2fb344" : "#d99000"
        }
        Text {
            Layout.preferredWidth: 150
            text: gateRow.label
            color: "#706a7e"
            font.pixelSize: 11
            font.bold: true
            elide: Text.ElideRight
        }
        Text {
            Layout.fillWidth: true
            text: gateRow.value
            color: "#171222"
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }

    component ButtonRow: RowLayout {
        id: buttonRow
        property string label: ""
        property bool enabledState: false
        signal clicked
        Layout.fillWidth: true
        Text {
            Layout.fillWidth: true
            text: buttonRow.label
            color: "#171222"
            font.pixelSize: 12
            elide: Text.ElideRight
        }
        Button {
            id: runButton
            Layout.preferredWidth: 110
            Layout.preferredHeight: 34
            enabled: buttonRow.enabledState
            text: "Run"
            onClicked: buttonRow.clicked()
            background: Rectangle {
                radius: 8
                color: runButton.enabled ? (runButton.hovered ? "#4f0aa6" : "#3b0787") : "#e7e1ee"
            }
            contentItem: Text {
                text: runButton.text
                color: runButton.enabled ? "#ffffff" : "#8f8799"
                font.pixelSize: 11
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
