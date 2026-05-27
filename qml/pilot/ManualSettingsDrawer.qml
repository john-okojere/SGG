import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root
    modal: false
    focus: true
    width: 390
    height: parent ? parent.height - 120 : 760
    x: 24
    y: 92
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property real controlSensitivity: 0.62
    property real yawSensitivity: 0.55
    property real throttleSensitivity: 0.58
    property real rthAltitude: 60
    property real lowBatteryThreshold: 25
    property int uploadInterval: 1000

    background: Rectangle { radius: 10; color: "#ffffff"; border.color: "#e2d8ee" }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true
        ColumnLayout {
            width: root.width
            spacing: 14
            anchors.margins: 16

            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                Layout.topMargin: 16
                Text { text: "PILOT MODE SETTINGS"; color: "#2e005f"; font.pixelSize: 14; font.bold: true; Layout.fillWidth: true }
                Button {
                    Layout.preferredWidth: 28; Layout.preferredHeight: 28; text: "×"; onClicked: root.close()
                    background: Rectangle { radius: 14; color: parent.hovered ? "#f3eef8" : "transparent" }
                }
            }

            SectionTitle { text: "MANUAL FLIGHT" }
            SettingsSlider { label: "Control sensitivity"; value: root.controlSensitivity; from: 0.1; to: 1.0; decimals: 2; onMovedValue: root.controlSensitivity = value }
            SettingsSlider { label: "Yaw sensitivity"; value: root.yawSensitivity; from: 0.1; to: 1.0; decimals: 2; onMovedValue: root.yawSensitivity = value }
            SettingsSlider { label: "Throttle sensitivity"; value: root.throttleSensitivity; from: 0.1; to: 1.0; decimals: 2; onMovedValue: root.throttleSensitivity = value }
            SettingsSlider { label: "RTH altitude"; value: root.rthAltitude; from: 20; to: 160; unit: "m"; onMovedValue: root.rthAltitude = value }
            ToggleRow { label: "Geofence enforcement"; checked: true }
            ToggleRow { label: "Obstacle avoidance"; checked: true }
            SettingsSlider { label: "Low battery warning"; value: root.lowBatteryThreshold; from: 10; to: 50; unit: "%"; onMovedValue: root.lowBatteryThreshold = value }
            SettingsSlider { label: "Telemetry upload"; value: root.uploadInterval; from: 500; to: 5000; unit: "ms"; onMovedValue: root.uploadInterval = value }

            SectionTitle { text: "AIRCRAFT" }
            InfoRow { label: "Connection"; value: vehicleManager.connectionUrl.length > 0 ? vehicleManager.connectionUrl : "Waiting for UDP 14540/14550" }
            InfoRow { label: "System ID"; value: vehicleManager.systemId.length > 0 ? vehicleManager.systemId : "--" }
            InfoRow { label: "Autopilot"; value: vehicleManager.autopilot.length > 0 ? vehicleManager.autopilot : "--" }
            InfoRow { label: "Firmware"; value: "Read from vehicle later" }

            SectionTitle { text: "SAFETY" }
            ToggleRow { label: "Emergency confirmation"; checked: true }
            InfoRow { label: "Loss of signal"; value: "Return to Home" }
            InfoRow { label: "Geofence breach"; value: "Hold and alert" }
            Item { height: 14 }
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

    component InfoRow: RowLayout {
        property string label: ""
        property string value: ""
        Layout.fillWidth: true
        Layout.leftMargin: 16
        Layout.rightMargin: 16
        Text { text: label; color: "#5f5a66"; font.pixelSize: 11; Layout.preferredWidth: 128 }
        Text { text: value; color: "#111111"; font.pixelSize: 11; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
    }

    component ToggleRow: RowLayout {
        property string label: ""
        property bool checked: false
        Layout.fillWidth: true
        Layout.leftMargin: 16
        Layout.rightMargin: 16
        Text { text: label; color: "#111111"; font.pixelSize: 11; Layout.fillWidth: true }
        Switch { checked: parent.checked; onToggled: eventLogManager.logEvent("pilot_setting_changed", "info", parent.label, { enabled: checked }) }
    }

    component SettingsSlider: ColumnLayout {
        id: sliderRow
        property string label: ""
        property real value: 0
        property real from: 0
        property real to: 1
        property int decimals: 0
        property string unit: ""
        signal movedValue(real value)
        Layout.fillWidth: true
        Layout.leftMargin: 16
        Layout.rightMargin: 16
        RowLayout {
            Layout.fillWidth: true
            Text { text: sliderRow.label; color: "#111111"; font.pixelSize: 11; Layout.fillWidth: true }
            Text { text: Number(sliderRow.value).toFixed(sliderRow.decimals) + " " + sliderRow.unit; color: "#2e005f"; font.pixelSize: 11; font.bold: true }
        }
        Slider {
            Layout.fillWidth: true
            from: sliderRow.from
            to: sliderRow.to
            value: sliderRow.value
            onMoved: {
                sliderRow.movedValue(value)
                eventLogManager.logEvent("pilot_setting_changed", "info", sliderRow.label, { value: value })
            }
        }
    }
}
