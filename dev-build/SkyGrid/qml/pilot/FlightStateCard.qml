import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    radius: 7
    color: "#ffffff"
    border.color: "#e2d8ee"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 0

        Text { text: "FLIGHT STATE"; color: "#3b294d"; font.pixelSize: 11; font.bold: true }
        StateRow { label: "Mode"; value: telemetryStore.connected ? "Manual (Stabilized)" : "Disconnected"; healthy: telemetryStore.connected; showDot: false }
        Divider {}
        StateRow { label: "Armed"; value: telemetryStore.armed ? "Armed" : "Disarmed"; healthy: telemetryStore.armed; showDot: true }
        Divider {}
        StateRow { label: "GPS"; value: telemetryStore.connected ? telemetryStore.gpsMode + " (" + telemetryStore.satellites + ")" : "No Fix"; healthy: telemetryStore.satellites >= 8; showDot: true }
    }

    component Divider: Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: "#f0eaf5"
    }

    component StateRow: RowLayout {
        property string label: ""
        property string value: ""
        property bool healthy: false
        property bool showDot: false
        Layout.fillWidth: true
        Layout.preferredHeight: 34
        Text { text: label; color: "#111111"; font.pixelSize: 11; Layout.fillWidth: true }
        Rectangle {
            visible: showDot
            Layout.preferredWidth: 7
            Layout.preferredHeight: 7
            radius: 4
            color: healthy ? "#18b83f" : "#b8b2c2"
        }
        Text { text: value; color: "#111111"; font.pixelSize: 11; font.bold: true }
    }
}
