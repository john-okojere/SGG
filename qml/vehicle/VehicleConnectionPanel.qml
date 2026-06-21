import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    radius: 8
    color: "#ffffff"
    border.color: "#e2dceb"
    implicitHeight: connectionContent.implicitHeight + 32
    property int connectionElapsedSeconds: 0
    property bool showBindAction: true
    readonly property bool connecting: vehicleManager.discoveryActive && !vehicleManager.connected
    readonly property bool heartbeatLate: connecting && connectionElapsedSeconds >= 20

    function serialDisplayModel() {
        var items = [];
        for (var i = 0; i < vehicleManager.serialPortOptions.length; ++i)
            items.push(vehicleManager.serialPortOptions[i].display);
        if (items.length === 0)
            items.push(vehicleConfigManager.serialPort);
        return items;
    }

    function serialConnectionName(displayText) {
        for (var i = 0; i < vehicleManager.serialPortOptions.length; ++i) {
            var option = vehicleManager.serialPortOptions[i];
            if (option.display === displayText)
                return option.connectionName || option.portName || option.systemLocation;
        }
        return displayText;
    }

    ColumnLayout {
        id: connectionContent
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        PanelHeader {
            title: "Step 1 - Connect Flight Controller"
            subtitle: vehicleManager.connected ? "Heartbeat received from " + vehicleManager.autopilot : "Plug in USB, refresh ports, choose baud, then connect."
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 10
            columnSpacing: 10

            Label {
                text: "Mode"
                color: "#706a7e"
                font.pixelSize: 11
                font.bold: true
            }
            ComboBox {
                Layout.fillWidth: true
                model: ["UDP", "TCP", "Serial"]
                currentIndex: Math.max(0, model.indexOf(vehicleConfigManager.connectionMode === "SERIAL" ? "Serial" : vehicleConfigManager.connectionMode))
                onActivated: vehicleConfigManager.connectionMode = currentText
            }

            Label {
                visible: vehicleConfigManager.connectionMode === "SERIAL"
                text: "Serial port"
                color: "#706a7e"
                font.pixelSize: 11
                font.bold: true
            }
            ComboBox {
                visible: vehicleConfigManager.connectionMode === "SERIAL"
                Layout.fillWidth: true
                editable: true
                model: root.serialDisplayModel()
                currentIndex: Math.max(0, vehicleManager.availableSerialPorts.indexOf(vehicleConfigManager.serialPort))
                editText: vehicleConfigManager.serialPort
                onActivated: vehicleConfigManager.serialPort = root.serialConnectionName(currentText)
                onAccepted: vehicleConfigManager.serialPort = editText
                Component.onCompleted: vehicleManager.refreshSerialPorts()
            }

            Label {
                visible: vehicleConfigManager.connectionMode === "SERIAL"
                text: "Baud"
                color: "#706a7e"
                font.pixelSize: 11
                font.bold: true
            }
            ComboBox {
                visible: vehicleConfigManager.connectionMode === "SERIAL"
                Layout.fillWidth: true
                model: [57600, 115200, 921600]
                currentIndex: Math.max(0, model.indexOf(vehicleConfigManager.baudRate))
                onActivated: vehicleConfigManager.baudRate = Number(currentText)
            }

            Label {
                visible: vehicleConfigManager.connectionMode === "UDP"
                text: "UDP bind"
                color: "#706a7e"
                font.pixelSize: 11
                font.bold: true
            }
            RowLayout {
                visible: vehicleConfigManager.connectionMode === "UDP"
                Layout.fillWidth: true
                TextField {
                    Layout.fillWidth: true
                    text: vehicleConfigManager.udpHost
                    onEditingFinished: vehicleConfigManager.udpHost = text
                }
                TextField {
                    Layout.preferredWidth: 88
                    text: String(vehicleConfigManager.udpPort)
                    validator: IntValidator {
                        bottom: 1
                        top: 65535
                    }
                    onEditingFinished: vehicleConfigManager.udpPort = Number(text)
                }
            }

            Label {
                visible: vehicleConfigManager.connectionMode === "TCP"
                text: "TCP target"
                color: "#706a7e"
                font.pixelSize: 11
                font.bold: true
            }
            RowLayout {
                visible: vehicleConfigManager.connectionMode === "TCP"
                Layout.fillWidth: true
                TextField {
                    Layout.fillWidth: true
                    text: vehicleConfigManager.tcpHost
                    onEditingFinished: vehicleConfigManager.tcpHost = text
                }
                TextField {
                    Layout.preferredWidth: 88
                    text: String(vehicleConfigManager.tcpPort)
                    validator: IntValidator {
                        bottom: 1
                        top: 65535
                    }
                    onEditingFinished: vehicleConfigManager.tcpPort = Number(text)
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumHeight: connectionStatusContent.implicitHeight + 20
            implicitHeight: connectionStatusContent.implicitHeight + 20
            radius: 8
            color: root.connecting ? "#fffaf0" : "#f8f6fb"
            border.color: root.connecting ? "#e9c96d" : "#e7def0"
            ColumnLayout {
                id: connectionStatusContent
                anchors.fill: parent
                anchors.margins: 10
                spacing: 6
                Text {
                    Layout.fillWidth: true
                    text: vehicleConfigManager.connectionUrl
                    color: "#3b294d"
                    font.pixelSize: 12
                    font.bold: true
                    elide: Text.ElideRight
                }
                RowLayout {
                    Layout.fillWidth: true
                    visible: root.connecting
                    spacing: 10
                    BusyIndicator {
                        Layout.preferredWidth: 28
                        Layout.preferredHeight: 28
                        running: root.connecting
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text {
                            Layout.fillWidth: true
                            text: "Connecting... waiting for MAVLink heartbeat"
                            color: "#715400"
                            font.pixelSize: 12
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        Text {
                            Layout.fillWidth: true
                            text: "Elapsed " + root.connectionElapsedSeconds + "s - " + vehicleManager.status
                            color: "#706a7e"
                            font.pixelSize: 11
                            wrapMode: Text.WordWrap
                        }
                    }
                }
                Text {
                    Layout.fillWidth: true
                    text: vehicleManager.connected ? ("System " + vehicleManager.systemId + " - " + vehicleManager.flightMode + " - " + vehicleManager.health) : (root.connecting ? (root.heartbeatLate ? "No MAVLink heartbeat yet. Try 57600, 115200, or 921600 baud; close other GCS/configurator apps; confirm ArduPilot or PX4 is running." : "Serial device detected. Waiting for a MAVLink heartbeat; no arm, takeoff, or motor command is sent.") : (vehicleManager.availableSerialPorts.length + " serial port(s) detected. No arm, takeoff, or motor command is sent."))
                    color: root.heartbeatLate ? "#a15c00" : "#706a7e"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: root.width < 560 ? 2 : 4
            rowSpacing: 8
            columnSpacing: 10
            PanelButton {
                Layout.fillWidth: true
                text: "Refresh Ports"
                visible: vehicleConfigManager.connectionMode === "SERIAL"
                onClicked: vehicleManager.refreshSerialPorts()
            }
            PanelButton {
                Layout.fillWidth: true
                text: root.connecting ? "Connecting..." : (vehicleManager.discoveryActive ? "Retry" : "Connect")
                primary: true
                enabled: permissionManager.canBindFlightController && !root.connecting
                onClicked: {
                    root.connectionElapsedSeconds = 0;
                    vehicleConfigManager.connectVehicle();
                }
            }
            PanelButton {
                Layout.fillWidth: true
                text: "Stop"
                enabled: vehicleManager.discoveryActive
                onClicked: vehicleConfigManager.disconnectVehicle()
            }
            PanelButton {
                Layout.fillWidth: true
                text: "Bind FC"
                visible: root.showBindAction
                enabled: permissionManager.canBindFlightController && vehicleManager.connected
                onClicked: vehicleConfigManager.bindFlightController()
            }
        }

        Text {
            Layout.fillWidth: true
            text: root.connecting ? vehicleManager.status + " | " + vehicleConfigManager.status : vehicleConfigManager.status
            color: root.connecting ? "#715400" : "#706a7e"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }
    }

    Timer {
        interval: 1000
        repeat: true
        running: root.connecting
        onTriggered: root.connectionElapsedSeconds += 1
        onRunningChanged: {
            if (!running)
                root.connectionElapsedSeconds = 0;
        }
    }

    component PanelHeader: ColumnLayout {
        property string title: ""
        property string subtitle: ""
        Layout.fillWidth: true
        spacing: 2
        Text {
            Layout.fillWidth: true
            text: parent.title
            color: "#171222"
            font.pixelSize: 16
            font.bold: true
            elide: Text.ElideRight
        }
        Text {
            Layout.fillWidth: true
            text: parent.subtitle
            color: "#706a7e"
            font.pixelSize: 11
            elide: Text.ElideRight
        }
    }

    component PanelButton: Button {
        id: button
        property bool primary: false
        height: 36
        opacity: enabled ? 1 : 0.45
        background: Rectangle {
            radius: 8
            color: button.primary ? (button.hovered ? "#4f0aa6" : "#3b0787") : (button.hovered ? "#f5f0fb" : "#ffffff")
            border.color: button.primary ? "#3b0787" : "#d8ceeb"
        }
        contentItem: Text {
            text: button.text
            color: button.primary ? "#ffffff" : "#3b0787"
            font.pixelSize: 11
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
}
