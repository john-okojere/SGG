import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    radius: 8
    color: "#ffffff"
    border.color: "#e2dceb"
    implicitHeight: mappingContent.implicitHeight + 32

    ColumnLayout {
        id: mappingContent
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            Layout.fillWidth: true
            text: "Step 4 - RC / Controller Mapping"
            color: "#171222"
            font.pixelSize: 16
            font.bold: true
        }
        Text {
            Layout.fillWidth: true
            text: "Read receiver channels from the connected flight controller, capture min/max/trim, then save ArduPilot RC calibration parameters. This workspace never starts motors."
            color: Theme.muted
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }

        GridLayout {
            Layout.fillWidth: true
            columns: root.width < 620 ? 2 : 4
            rowSpacing: 8
            columnSpacing: 8
            StepChip { label: "1 Start monitor"; active: rcCalibrationManager.monitoring; done: rcCalibrationManager.rcAvailable }
            StepChip { label: "2 Move sticks"; active: rcCalibrationManager.rcAvailable; done: rcCalibrationManager.sampleCount > 20 }
            StepChip { label: "3 Capture center"; active: rcCalibrationManager.rcAvailable; done: false }
            StepChip { label: "4 Save to FC"; active: permissionManager.canWriteVehicleParameters; done: false }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 62
            radius: 8
            color: rcCalibrationManager.rcAvailable ? "#f0fff5" : "#f8f6fb"
            border.color: rcCalibrationManager.rcAvailable ? "#bfe8c8" : "#e7def0"
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 12
                StatusDot {
                    ok: rcCalibrationManager.rcAvailable
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.fillWidth: true
                        text: rcCalibrationManager.rcAvailable ? "RC input detected - samples " + rcCalibrationManager.sampleCount : (vehicleManager.connected ? "Flight controller connected. Start monitor, then move the transmitter sticks." : "Connect a flight controller before RC calibration.")
                        color: "#3b294d"
                        font.pixelSize: 11
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }
                    Text {
                        Layout.fillWidth: true
                        text: rcCalibrationManager.status
                        color: "#706a7e"
                        font.pixelSize: 10
                        elide: Text.ElideRight
                    }
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: root.width < 620 ? 2 : 4
            rowSpacing: 8
            columnSpacing: 8
            CalibButton {
                Layout.fillWidth: true
                text: rcCalibrationManager.monitoring ? "Stop Monitor" : "Start Monitor"
                primary: true
                enabled: permissionManager.canConfigureRc && vehicleManager.connected
                onClicked: rcCalibrationManager.monitoring ? rcCalibrationManager.stopMonitoring() : rcCalibrationManager.startMonitoring()
            }
            CalibButton {
                Layout.fillWidth: true
                text: "Reset"
                enabled: permissionManager.canConfigureRc
                onClicked: rcCalibrationManager.resetCalibration()
            }
            CalibButton {
                Layout.fillWidth: true
                text: "Capture Center"
                enabled: permissionManager.canConfigureRc && rcCalibrationManager.rcAvailable
                onClicked: rcCalibrationManager.captureTrim()
            }
            CalibButton {
                Layout.fillWidth: true
                text: "Save to FC"
                danger: true
                enabled: permissionManager.canConfigureRc && permissionManager.canWriteVehicleParameters && rcCalibrationManager.rcAvailable
                onClicked: rcCalibrationManager.saveCalibration()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 172
            radius: 8
            color: Theme.purpleWash
            border.color: Theme.lineSoft
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5
                Repeater {
                    model: rcCalibrationManager.channels
                    delegate: RcChannelRow {
                        Layout.fillWidth: true
                        channelName: modelData.name
                        pwm: modelData.value
                        minimum: modelData.minimum
                        maximum: modelData.maximum
                        trim: modelData.trim
                        seen: modelData.seen
                    }
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: root.width < 560 ? 1 : 2
            rowSpacing: 10
            columnSpacing: 12

            MappingRow {
                id: rollRow
                label: "Roll"
                defaultChannel: "CH1"
            }
            MappingRow {
                id: pitchRow
                label: "Pitch"
                defaultChannel: "CH2"
            }
            MappingRow {
                id: throttleRow
                label: "Throttle"
                defaultChannel: "CH3"
            }
            MappingRow {
                id: yawRow
                label: "Yaw"
                defaultChannel: "CH4"
            }
            MappingRow {
                id: modeRow
                label: "Mode"
                defaultChannel: "CH5"
            }
            MappingRow {
                id: failsafeRow
                label: "Failsafe"
                defaultChannel: "CH6"
            }
        }

        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 38
            text: "Save RC Mapping"
            enabled: permissionManager.canConfigureRc
            onClicked: vehicleConfigManager.saveRcMapping({
                roll: rollRow.channel,
                pitch: pitchRow.channel,
                throttle: throttleRow.channel,
                yaw: yawRow.channel,
                mode: modeRow.channel,
                failsafe: failsafeRow.channel,
                safety: "no_motor_test"
            })
            background: Rectangle {
                radius: 8
                color: parent.hovered ? "#4f0aa6" : "#3b0787"
            }
            contentItem: Text {
                text: parent.text
                color: "#ffffff"
                font.pixelSize: 11
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    component StatusDot: Rectangle {
        property bool ok: false
        Layout.preferredWidth: 10
        Layout.preferredHeight: 10
        radius: 5
        color: ok ? "#2fb344" : "#d99000"
    }

    component StepChip: Rectangle {
        property string label: ""
        property bool active: false
        property bool done: false
        Layout.fillWidth: true
        Layout.preferredHeight: 28
        radius: 7
        color: done ? Theme.greenSoft : (active ? Theme.amberSoft : Theme.surfaceAlt)
        border.color: done ? "#bfe8c8" : (active ? "#efd06c" : Theme.lineSoft)
        Text {
            anchors.centerIn: parent
            width: parent.width - 10
            text: label
            color: done ? "#14692a" : (active ? "#715400" : Theme.muted)
            font.pixelSize: 10
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }
    }

    component CalibButton: Button {
        id: calibButton
        property bool primary: false
        property bool danger: false
        Layout.preferredHeight: 34
        background: Rectangle {
            radius: 8
            color: !calibButton.enabled ? Theme.surfaceAlt : (calibButton.danger ? (calibButton.hovered ? "#7f1d1d" : "#991b1b") : (calibButton.primary ? (calibButton.hovered ? Theme.purple2 : Theme.purple) : (calibButton.hovered ? Theme.purpleWash : Theme.surfaceRaised)))
            border.color: calibButton.primary || calibButton.danger ? "transparent" : Theme.lineSoft
        }
        contentItem: Text {
            text: calibButton.text
            color: !calibButton.enabled ? Theme.muted : (calibButton.primary || calibButton.danger ? Theme.white : Theme.purple)
            font.pixelSize: 11
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    component MappingRow: RowLayout {
        id: row
        property string label: ""
        property string defaultChannel: "CH1"
        property string channel: channelBox.currentText
        Layout.fillWidth: true
        Label {
            Layout.preferredWidth: 74
            text: row.label
            color: "#706a7e"
            font.pixelSize: 11
            font.bold: true
        }
        ComboBox {
            id: channelBox
            Layout.fillWidth: true
            model: ["CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8", "Disabled"]
            Component.onCompleted: currentIndex = Math.max(0, model.indexOf(row.defaultChannel))
        }
    }

    component RcChannelRow: RowLayout {
        property string channelName: "CH1"
        property int pwm: 0
        property int minimum: 0
        property int maximum: 0
        property int trim: 0
        property bool seen: false
        Layout.preferredHeight: 14
        spacing: 8
        Text {
            Layout.preferredWidth: 34
            text: channelName
            color: seen ? "#171222" : "#9b94a6"
            font.pixelSize: 10
            font.bold: true
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 8
            radius: 4
            color: "#eee8f4"
            Rectangle {
                height: parent.height
                radius: 4
                color: seen ? "#3b0787" : "#d8ceeb"
                width: parent.width * Math.max(0, Math.min(1, (pwm - 900) / 1300))
            }
        }
        Text {
            Layout.preferredWidth: 170
            text: seen ? (pwm + "  min " + minimum + "  max " + maximum + "  trim " + trim) : "--"
            color: "#706a7e"
            font.pixelSize: 10
            horizontalAlignment: Text.AlignRight
        }
    }
}
