import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../vehicle"

Rectangle {
    id: root
    color: "#f6f4fa"

    function selectedValue(key, fallback) {
        var value = manufacturerVehicleManager.selectedProfile[key]
        return value === undefined || value === null ? fallback : String(value)
    }

    function connectedUid() {
        return vehicleManager.connected && vehicleManager.systemId.length > 0 ? vehicleManager.systemId : root.selectedValue("flight_controller_uid", "")
    }

    function connectedAutopilot() {
        return vehicleManager.connected && vehicleManager.autopilot.length > 0 ? vehicleManager.autopilot : root.selectedValue("autopilot", "")
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            x: 24
            width: Math.max(0, root.width - 48)
            spacing: 14

            Item { Layout.preferredHeight: 10 }

            Text { text: "Flight Controller Binding"; color: "#14111d"; font.pixelSize: 24; font.bold: true }
            Text {
                Layout.fillWidth: true
                text: "Choose the flight controller interface, connect to MAVLink, then bind the detected controller to the selected aircraft profile."
                color: "#706a7e"
                font.pixelSize: 13
                wrapMode: Text.WordWrap
            }

            VehicleConnectionPanel {
                Layout.fillWidth: true
                showBindAction: false
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: bindingContent.implicitHeight + 32
                radius: 8
                color: "#ffffff"
                border.color: "#e2dceb"

                ColumnLayout {
                    id: bindingContent
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Text { Layout.fillWidth: true; text: "Selected Profile"; color: "#171222"; font.pixelSize: 16; font.bold: true; elide: Text.ElideRight }
                            Text {
                                Layout.fillWidth: true
                                text: root.selectedValue("name", root.selectedValue("model", "No profile selected"))
                                color: "#706a7e"
                                font.pixelSize: 12
                                elide: Text.ElideRight
                            }
                        }
                        Rectangle {
                            Layout.preferredWidth: 128
                            Layout.preferredHeight: 32
                            radius: 16
                            color: vehicleManager.connected ? "#e8f8ed" : "#f8f6fb"
                            border.color: vehicleManager.connected ? "#84d69a" : "#e2dceb"
                            Text {
                                anchors.centerIn: parent
                                text: vehicleManager.connected ? "Connected" : "Not Connected"
                                color: vehicleManager.connected ? "#14692a" : "#706a7e"
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: root.width < 840 ? 1 : 2
                        rowSpacing: 10
                        columnSpacing: 12

                        BindingField {
                            id: uidField
                            label: "Flight Controller UID"
                            text: root.connectedUid()
                            placeholder: "Connect controller or enter UID"
                        }
                        BindingField {
                            id: autopilotField
                            label: "Autopilot"
                            text: root.connectedAutopilot()
                            placeholder: "PX4, ArduPilot, or SkyGrid"
                        }
                    }

                    Flow {
                        Layout.fillWidth: true
                        Layout.preferredHeight: childrenRect.height
                        spacing: 10
                        Button {
                            text: "Use Connected Controller"
                            enabled: vehicleManager.connected && vehicleManager.systemId.length > 0
                            onClicked: {
                                uidField.text = vehicleManager.systemId
                                autopilotField.text = vehicleManager.autopilot
                            }
                        }
                        Button {
                            text: "Bind Selected Profile"
                            visible: accessManager.can("can_bind_flight_controller")
                            enabled: !manufacturerVehicleManager.loading
                                     && manufacturerVehicleManager.selectedProfile.id !== undefined
                                     && uidField.text.length > 0
                            onClicked: manufacturerVehicleManager.bindFlightController(
                                manufacturerVehicleManager.selectedProfile.id,
                                uidField.text,
                                autopilotField.text
                            )
                        }
                    }

                    Text {
                        Layout.fillWidth: true
                        text: vehicleManager.connected
                              ? ("Detected system " + vehicleManager.systemId + " on " + vehicleConfigManager.connectionUrl + ". Binding does not arm, upload missions, or move actuators.")
                              : "Refresh ports, select the correct serial/USB interface and baud rate, then connect before binding."
                        color: vehicleManager.connected ? "#14692a" : "#706a7e"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }
                }
            }

            StatusFooter {}
            Item { Layout.preferredHeight: 18 }
        }
    }

    component BindingField: ColumnLayout {
        property string label: ""
        property string placeholder: ""
        property alias text: input.text
        Layout.fillWidth: true
        spacing: 4
        Text { text: label; color: "#706a7e"; font.pixelSize: 11; font.bold: true }
        TextField {
            id: input
            Layout.fillWidth: true
            placeholderText: placeholder
        }
    }
}
