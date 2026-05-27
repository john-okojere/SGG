import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"
import "../map"

Item {
    id: root
    property real targetLatitude: 0
    property real targetLongitude: 0
    property bool targetVisible: false
    property bool hudVisible: false

    function pointFor(latitude, longitude) {
        return surface.item && surface.item.pointFor ? surface.item.pointFor(latitude, longitude) : Qt.point(width / 2, height / 2)
    }

    function coordinateFor(point) {
        return surface.item && surface.item.coordinateFor ? surface.item.coordinateFor(point) : { latitude: mapState.centerLatitude, longitude: mapState.centerLongitude }
    }

    Component.onCompleted: {
        mapProvider.layerMode = "SAT"
        mapState.layerMode = "SAT"
        mapState.followAircraft = true
    }

    Connections {
        target: telemetryStore
        function onTelemetryChanged() {
            if (mapState.followAircraft
                    && telemetryStore.connected
                    && Math.abs(telemetryStore.latitude) > 0.000001
                    && Math.abs(telemetryStore.longitude) > 0.000001) {
                mapState.centerLatitude = telemetryStore.latitude
                mapState.centerLongitude = telemetryStore.longitude
            }
        }
    }

    Loader {
        id: surface
        anchors.fill: parent
        source: "../map/TileMapSurface.qml"
    }

    FlightTrailOverlay {
        anchors.fill: parent
        mapItem: root
        z: 2
    }

    HomeMarker {
        mapItem: root
        z: 4
    }

    AircraftMarker {
        mapItem: root
        z: 5
    }

    Item {
        id: targetMarker
        visible: root.targetVisible
        z: 6
        property point p: root.pointFor(root.targetLatitude, root.targetLongitude)
        x: p.x - 18
        y: p.y - 18
        width: 36
        height: 36
        Rectangle {
            anchors.fill: parent
            radius: 18
            color: "#ffffffdd"
            border.color: "#f5c542"
            border.width: 2
        }
        Text {
            anchors.centerIn: parent
            text: "◎"
            color: "#2e005f"
            font.pixelSize: 20
            font.bold: true
        }
    }

    MouseArea {
        anchors.fill: parent
        z: 1
        acceptedButtons: Qt.LeftButton
        property real startX: 0
        property real startY: 0
        onPressed: {
            startX = mouse.x
            startY = mouse.y
        }
        onPositionChanged: {
            if (pressed) {
                var a = root.coordinateFor(Qt.point(startX, startY))
                var b = root.coordinateFor(Qt.point(mouse.x, mouse.y))
                mapState.centerLatitude += a.latitude - b.latitude
                mapState.centerLongitude += a.longitude - b.longitude
                mapState.followAircraft = false
                startX = mouse.x
                startY = mouse.y
            }
        }
        onWheel: function(wheel) {
            if (wheel.angleDelta.y > 0) mapState.zoomIn()
            else mapState.zoomOut()
        }
    }

    RowLayout {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 22
        anchors.rightMargin: 22
        spacing: 8
        z: 8
        IconButton {
            implicitWidth: 44
            implicitHeight: 44
            iconSource: AssetRegistry.icons.pin_location
            active: mapState.followAircraft
            onClicked: mapState.followAircraft = !mapState.followAircraft
        }
        RowLayout {
            spacing: 4
            Rectangle {
                Layout.preferredHeight: 44
                Layout.preferredWidth: 148
                radius: 6
                color: "#ffffff"
                border.color: "#e2d8ee"
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 4
                    spacing: 2
                    LayerButton { label: "2D"; active: mapProvider.layerMode === "2D"; onClicked: { mapProvider.layerMode = "2D"; mapState.layerMode = "2D" } }
                    LayerButton { label: "3D"; active: mapProvider.layerMode === "3D"; onClicked: { mapProvider.layerMode = "3D"; mapState.layerMode = "3D" } }
                    LayerButton { label: "SAT"; active: mapProvider.layerMode === "SAT"; onClicked: { mapProvider.layerMode = "SAT"; mapState.layerMode = "SAT" } }
                }
            }
        }
        CompassHudStack {
            Layout.preferredWidth: 78
            Layout.preferredHeight: root.hudVisible ? 164 : 78
            hudVisible: root.hudVisible
        }
    }

    component LayerButton: Button {
        id: button
        property string label: ""
        property bool active: false
        Layout.fillWidth: true
        Layout.fillHeight: true
        background: Rectangle {
            radius: 4
            color: button.active ? "#8b74a3" : (button.hovered ? "#f3eef8" : "#ffffff")
        }
        contentItem: Text {
            text: button.label
            color: button.active ? "#ffffff" : "#111111"
            font.pixelSize: 11
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
