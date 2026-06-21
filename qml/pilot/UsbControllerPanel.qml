import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Popup {
    id: root
    modal: false
    focus: true
    width: Math.min(460, parent ? parent.width - 40 : 460)
    height: Math.min(620, parent ? parent.height - 120 : 620)
    x: parent ? Math.max(16, parent.width - width - 24) : 24
    y: parent ? 92 : 24
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    readonly property bool readyToEnable: usbControllerManager.connected
                                          && usbControllerManager.centerCalibrated
                                          && permissionManager.canFlyManual
                                          && vehicleManager.connected
                                          && sessionManager.operationsAllowed

    background: Rectangle {
        radius: 10
        color: "#ffffff"
        border.color: "#d8cbe8"
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: root.width
            spacing: 14

            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                Layout.topMargin: 16
                spacing: 10
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 3
                    Text {
                        text: "USB RC CONTROLLER"
                        color: "#2e005f"
                        font.pixelSize: 14
                        font.bold: true
                    }
                    Text {
                        Layout.fillWidth: true
                        text: usbControllerManager.deviceName.length > 0 ? usbControllerManager.deviceName : "No controller selected"
                        color: "#5d5368"
                        font.pixelSize: 11
                        elide: Text.ElideRight
                    }
                }
                Button {
                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30
                    text: "X"
                    onClicked: root.close()
                    background: Rectangle { radius: 15; color: parent.hovered ? "#f3eef8" : "transparent" }
                }
            }

            StatusBox {
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                Layout.fillWidth: true
            }

            GridLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                columns: width < 390 ? 1 : 3
                columnSpacing: 8
                rowSpacing: 8

                PanelButton {
                    text: "Refresh"
                    enabled: usbControllerManager.supported
                    onClicked: usbControllerManager.refreshDevices()
                }
                PanelButton {
                    text: "Calibrate Center"
                    enabled: usbControllerManager.connected
                    onClicked: usbControllerManager.calibrateCenter()
                }
                PanelButton {
                    text: usbControllerManager.enabled ? "Disable USB" : "Enable USB"
                    enabled: usbControllerManager.enabled || root.readyToEnable
                    danger: usbControllerManager.enabled
                    onClicked: usbControllerManager.setEnabled(!usbControllerManager.enabled)
                }
            }

            SectionTitle { text: "LIVE AXES" }
            Repeater {
                model: usbControllerManager.axes
                delegate: AxisBar {
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    Layout.fillWidth: true
                    label: modelData.name
                    value: modelData.value
                    rawValue: modelData.raw
                }
            }

            SectionTitle { text: "CONTROL MAPPING" }
            MappingRow {
                label: "Forward / Back"
                controlKey: "forward"
                axisIndex: usbControllerManager.forwardAxis
                inverted: usbControllerManager.forwardInverted
            }
            MappingRow {
                label: "Left / Right"
                controlKey: "lateral"
                axisIndex: usbControllerManager.lateralAxis
                inverted: usbControllerManager.lateralInverted
            }
            MappingRow {
                label: "Up / Down"
                controlKey: "vertical"
                axisIndex: usbControllerManager.verticalAxis
                inverted: usbControllerManager.verticalInverted
            }
            MappingRow {
                label: "Yaw"
                controlKey: "yaw"
                axisIndex: usbControllerManager.yawAxis
                inverted: usbControllerManager.yawInverted
            }

            SectionTitle { text: "OUTPUT" }
            GridLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                Layout.bottomMargin: 18
                columns: 2
                columnSpacing: 8
                rowSpacing: 8
                OutputChip { label: "Forward"; value: usbControllerManager.forward }
                OutputChip { label: "Lateral"; value: usbControllerManager.lateral }
                OutputChip { label: "Vertical"; value: usbControllerManager.vertical }
                OutputChip { label: "Yaw"; value: usbControllerManager.yaw }
            }
        }
    }

    component StatusBox: Rectangle {
        radius: 8
        color: usbControllerManager.enabled ? "#eefbf1" : (usbControllerManager.connected ? "#fff9e8" : "#f8f5fb")
        border.color: usbControllerManager.enabled ? "#6fd087" : (usbControllerManager.connected ? "#f0c75e" : "#d8cbe8")
        implicitHeight: statusColumn.implicitHeight + 20
        ColumnLayout {
            id: statusColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            spacing: 5
            Text {
                text: usbControllerManager.enabled ? "ACTIVE" : (usbControllerManager.connected ? "STANDBY" : "DISCONNECTED")
                color: usbControllerManager.enabled ? "#11842d" : (usbControllerManager.connected ? "#8a6500" : "#5d5368")
                font.pixelSize: 11
                font.bold: true
            }
            Text {
                Layout.fillWidth: true
                text: usbControllerManager.status
                color: "#21152d"
                font.pixelSize: 11
                wrapMode: Text.WordWrap
            }
            Text {
                Layout.fillWidth: true
                visible: !root.readyToEnable && !usbControllerManager.enabled
                text: !permissionManager.canFlyManual ? "Your role cannot fly manually."
                      : (!vehicleManager.connected ? "Connect the aircraft before enabling USB control."
                      : (!sessionManager.operationsAllowed ? "Control Center approval is required."
                      : (!usbControllerManager.centerCalibrated ? "Center the sticks and calibrate before enabling." : "")))
                color: "#8a6500"
                font.pixelSize: 10
                wrapMode: Text.WordWrap
            }
        }
    }

    component SectionTitle: Text {
        Layout.fillWidth: true
        Layout.leftMargin: 16
        Layout.rightMargin: 16
        color: "#3b294d"
        font.pixelSize: 11
        font.bold: true
    }

    component PanelButton: Button {
        id: button
        property bool danger: false
        Layout.fillWidth: true
        Layout.preferredHeight: 34
        hoverEnabled: true
        opacity: enabled ? 1 : 0.45
        background: Rectangle {
            radius: 6
            color: button.hovered && button.enabled ? (button.danger ? "#fff1f3" : "#f7f1fd") : "#ffffff"
            border.color: button.danger ? "#ef233c" : "#cdb9e4"
        }
        contentItem: Text {
            text: button.text
            color: button.danger ? "#ef233c" : "#2e005f"
            font.pixelSize: 11
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    component AxisBar: ColumnLayout {
        id: axisBar
        property string label: ""
        property real value: 0
        property real rawValue: 0
        spacing: 5
        RowLayout {
            Layout.fillWidth: true
            Text {
                text: axisBar.label
                color: "#111111"
                font.pixelSize: 11
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
            Text {
                text: Number(axisBar.value).toFixed(2)
                color: "#2e005f"
                font.pixelSize: 11
                font.bold: true
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 9
            radius: 4
            color: "#eee8f4"
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                x: parent.width / 2
                width: 1
                height: parent.height + 5
                color: "#bda8d6"
            }
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                height: 5
                radius: 3
                color: "#5b22a8"
                x: axisBar.value >= 0 ? parent.width / 2 : parent.width / 2 + axisBar.value * parent.width / 2
                width: Math.abs(axisBar.value) * parent.width / 2
            }
        }
    }

    component MappingRow: RowLayout {
        id: mappingRow
        property string label: ""
        property string controlKey: ""
        property int axisIndex: 0
        property bool inverted: false
        Layout.fillWidth: true
        Layout.leftMargin: 16
        Layout.rightMargin: 16
        spacing: 8
        Text {
            text: mappingRow.label
            color: "#111111"
            font.pixelSize: 11
            Layout.preferredWidth: 100
            elide: Text.ElideRight
        }
        ComboBox {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            model: usbControllerManager.axes
            textRole: "name"
            valueRole: "index"
            currentIndex: Math.max(0, Math.min(mappingRow.axisIndex, count - 1))
            font.pixelSize: 11
            onActivated: usbControllerManager.setAxisMapping(mappingRow.controlKey, currentValue, mappingRow.inverted)
            background: Rectangle { radius: 5; color: "#fbf9fe"; border.color: "#d8cbe8" }
        }
        CheckBox {
            checked: mappingRow.inverted
            text: "Inv"
            font.pixelSize: 10
            onToggled: usbControllerManager.setAxisMapping(mappingRow.controlKey, mappingRow.axisIndex, checked)
        }
    }

    component OutputChip: Rectangle {
        property string label: ""
        property real value: 0
        Layout.fillWidth: true
        Layout.preferredHeight: 42
        radius: 7
        color: "#fbf9fe"
        border.color: "#e2d8ee"
        Column {
            anchors.centerIn: parent
            spacing: 2
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: label
                color: "#5d5368"
                font.pixelSize: 9
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: Number(value).toFixed(2)
                color: "#111111"
                font.pixelSize: 12
                font.bold: true
            }
        }
    }
}
