import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Item {
    id: root
    property var mapItem
    property bool hasFix: telemetryStore.connected
                          && Math.abs(telemetryStore.latitude) > 0.000001
                          && Math.abs(telemetryStore.longitude) > 0.000001
                          && mapItem
    property point aircraftPoint: hasFix ? mapItem.pointFor(telemetryStore.latitude, telemetryStore.longitude) : Qt.point(-400, -400)
    property color stateColor: !telemetryStore.connected ? "#9b94a6"
                               : telemetryStore.battery <= 25 ? "#f5c542"
                               : telemetryStore.transmission <= 30 ? "#ef233c"
                               : telemetryStore.armed ? "#5b22a8"
                               : "#ffffff"
    visible: hasFix
    x: aircraftPoint.x - width / 2
    y: aircraftPoint.y - height / 2
    width: 74
    height: 74
    scale: area.containsMouse ? 1.08 : 1

    Rectangle {
        anchors.centerIn: parent
        width: telemetryStore.armed ? 60 : 50
        height: width
        radius: width / 2
        color: "transparent"
        border.color: root.stateColor
        border.width: telemetryStore.armed ? 3 : 2
        opacity: telemetryStore.transmission <= 30 ? 0.72 : 0.45
        SequentialAnimation on opacity {
            running: telemetryStore.connected && telemetryStore.transmission <= 30
            loops: Animation.Infinite
            NumberAnimation { to: 0.18; duration: 420 }
            NumberAnimation { to: 0.78; duration: 420 }
        }
    }

    Rectangle {
        anchors.centerIn: parent
        width: 38
        height: 38
        radius: 19
        color: "#ffffffee"
        border.color: "#5b22a8"
        border.width: 2
    }

    Item {
        anchors.centerIn: parent
        width: 42
        height: 42
        rotation: telemetryStore.heading
        Text {
            anchors.centerIn: parent
            text: "▲"
            color: "#5b22a8"
            font.pixelSize: 36
            font.bold: true
        }
    }

    Rectangle {
        visible: area.containsMouse
        x: Math.min(50, root.parent ? root.parent.width - root.x - width - 12 : 50)
        y: -132
        width: 218
        height: 118
        radius: 7
        color: "#ffffffee"
        border.color: "#e2d8ee"
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 4
            RowLayout {
                Layout.fillWidth: true
                Text { text: telemetryStore.aircraftId; color: "#111111"; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
                Text { text: telemetryStore.armed ? "ARMED" : "IDLE"; color: telemetryStore.armed ? "#18b83f" : "#9b94a6"; font.pixelSize: 10; font.bold: true }
            }
            Text { text: telemetryStore.flightMode + " · " + telemetryStore.gpsMode; color: "#5f5a66"; font.pixelSize: 10; Layout.fillWidth: true; elide: Text.ElideRight }
            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: 3
                columnSpacing: 10
                SmallValue { text: "Alt " + Number(telemetryStore.altitude).toFixed(1) + " m" }
                SmallValue { text: "Spd " + Number(telemetryStore.speed).toFixed(1) + " m/s" }
                SmallValue { text: "Hdg " + Number(telemetryStore.heading).toFixed(0) + " deg" }
                SmallValue { text: "Bat " + telemetryStore.battery + "%" }
                SmallValue { text: telemetryStore.satellites + " SAT" }
                SmallValue { text: "Link " + telemetryStore.transmission + "%" }
            }
        }
    }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        onClicked: mapState.followAircraft = !mapState.followAircraft
    }

    Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on scale { NumberAnimation { duration: 140; easing.type: Easing.OutCubic } }

    component SmallValue: Text {
        color: "#111111"
        font.pixelSize: 10
        font.bold: true
    }
}
