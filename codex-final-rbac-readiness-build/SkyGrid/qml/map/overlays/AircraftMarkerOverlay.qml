import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../../controls"

Item {
    id: root
    property var mapItem
    property bool hasFix: telemetryStore.connected
                          && Math.abs(telemetryStore.latitude) > 0.000001
                          && Math.abs(telemetryStore.longitude) > 0.000001
    property point aircraftPoint: hasFix && mapItem ? mapItem.pointFor(telemetryStore.latitude, telemetryStore.longitude) : Qt.point(-200, -200)
    property var path: telemetryStore.livePath
    property var homePoint: path.length > 0 && mapItem ? mapItem.pointFor(path[0].latitude, path[0].longitude) : Qt.point(-200, -200)
    property color stateColor: !telemetryStore.connected ? "#71717a"
                               : telemetryStore.battery <= 25 ? Theme.amber
                               : telemetryStore.flightMode.indexOf("Mission") !== -1 || telemetryStore.flightMode.indexOf("MISSION") !== -1 ? Theme.green
                               : telemetryStore.armed ? Theme.purpleSoft
                               : Theme.white

    visible: hasFix

    Rectangle {
        visible: path.length > 0
        x: root.homePoint.x - width / 2
        y: root.homePoint.y - height / 2
        width: 22
        height: 22
        radius: 11
        color: "#12001fcc"
        border.color: Theme.white
        border.width: 2
        Text {
            anchors.centerIn: parent
            text: "H"
            color: Theme.white
            font.pixelSize: 11
            font.bold: true
        }
    }

    Item {
        id: marker
        x: root.aircraftPoint.x - width / 2
        y: root.aircraftPoint.y - height / 2
        width: 62
        height: 62
        scale: aircraftArea.containsMouse || aircraftArea.pressed ? 1.08 : 1

        Rectangle {
            anchors.centerIn: parent
            width: telemetryStore.armed ? 58 : 48
            height: width
            radius: width / 2
            color: "transparent"
            border.color: root.stateColor
            border.width: telemetryStore.armed ? 2 : 1
            opacity: telemetryStore.armed ? 0.42 : 0.22
            SequentialAnimation on opacity {
                running: telemetryStore.connected && telemetryStore.battery <= 25
                loops: Animation.Infinite
                NumberAnimation { to: 0.16; duration: 420 }
                NumberAnimation { to: 0.58; duration: 420 }
            }
        }

        Rectangle {
            anchors.centerIn: parent
            width: 38
            height: 38
            radius: 19
            color: "#160720ee"
            border.color: root.stateColor
            border.width: 2
        }

        AssetIcon {
            anchors.centerIn: parent
            width: 25
            height: 25
            source: AssetRegistry.icons.plane
            active: true
            rotation: telemetryStore.heading
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 3
            width: 3
            height: 15
            radius: 2
            color: root.stateColor
            rotation: telemetryStore.heading
            transformOrigin: Item.Bottom
        }

        MouseArea {
            id: aircraftArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: mapState.followAircraft = !mapState.followAircraft
        }

        Behavior on x { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
        Behavior on y { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
        Behavior on scale { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
    }

    Rectangle {
        visible: aircraftArea.containsMouse
        x: Math.min(root.aircraftPoint.x + 34, root.width - width - 12)
        y: Math.max(12, root.aircraftPoint.y - height - 26)
        width: 224
        height: 132
        radius: 8
        color: "#160720ee"
        border.color: "#ffffff24"
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 5
            RowLayout {
                Layout.fillWidth: true
                Text { text: telemetryStore.aircraftId; color: Theme.white; font.pixelSize: 13; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
                Text { text: telemetryStore.armed ? "ARMED" : "IDLE"; color: root.stateColor; font.pixelSize: 11; font.bold: true }
            }
            Text { text: telemetryStore.flightMode + " · " + telemetryStore.gpsMode; color: "#d9cae9"; font.pixelSize: 11; Layout.fillWidth: true; elide: Text.ElideRight }
            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: 4
                columnSpacing: 12
                Text { text: "Alt " + Number(telemetryStore.altitude).toFixed(1) + " m"; color: Theme.white; font.pixelSize: 11; font.bold: true }
                Text { text: "Spd " + Number(telemetryStore.speed).toFixed(1) + " m/s"; color: Theme.white; font.pixelSize: 11; font.bold: true }
                Text { text: "Hdg " + Number(telemetryStore.heading).toFixed(0) + " deg"; color: Theme.white; font.pixelSize: 11; font.bold: true }
                Text { text: "Bat " + telemetryStore.battery + "%"; color: Theme.white; font.pixelSize: 11; font.bold: true }
                Text { text: telemetryStore.satellites + " SAT"; color: Theme.white; font.pixelSize: 11; font.bold: true }
                Text { text: "Link " + telemetryStore.transmission + "%"; color: Theme.white; font.pixelSize: 11; font.bold: true }
            }
            Text {
                text: Number(telemetryStore.latitude).toFixed(6) + ", " + Number(telemetryStore.longitude).toFixed(6)
                color: "#bba7d4"
                font.pixelSize: 10
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }
    }
}
