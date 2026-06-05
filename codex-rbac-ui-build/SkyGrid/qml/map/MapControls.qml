import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

ColumnLayout {
    id: columnLayout
    spacing: 14
    IconButton {
        width: 28
        height: 28
        anchors.right: parent.right
        iconOnAccent: true;
        iconSource: AssetRegistry.icons.pin_location
        active: mapState.followAircraft
        ToolTip.visible: hovered
        ToolTip.text: mapState.followAircraft ? "Following aircraft" : "Follow aircraft"
        onClicked: {
            mapState.followAircraft = !mapState.followAircraft
            if (mapState.followAircraft && telemetryStore.connected) {
                mapState.centerLatitude = telemetryStore.latitude
                mapState.centerLongitude = telemetryStore.longitude
            }
        }
    }
    RowLayout {
        spacing: 6
        IconButton {
            width: 24
            height: 24
            text: "2D"
            font.pointSize: 9
            flat: false
            iconOnAccent: true;
            implicitWidth: 54
            iconText: "2D"
            active: mapProvider.layerMode === "2D"
            onClicked: {
                mapProvider.layerMode = "2D"
                mapState.layerMode = "2D"
            }
        }
        IconButton {
            implicitWidth: 54
            iconText: "3D"
            iconOnAccent: true;
            active: mapProvider.layerMode === "3D"
            onClicked: {
                mapProvider.layerMode = "3D"
                mapState.layerMode = "3D"
            }
        }
        IconButton {
            implicitWidth: 62
            iconText: "SAT"
            iconOnAccent: true;
            active: mapProvider.layerMode === "SAT"
            onClicked: {
                mapProvider.layerMode = "SAT"
                mapState.layerMode = "SAT"
            }
        }
    }
    ColumnLayout {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        spacing: 0
        IconButton { iconText: "+"; active: true; iconOnAccent: true; onClicked: mapState.zoomIn() }
        IconButton { iconText: "−"; active: true; iconOnAccent: true; onClicked: mapState.zoomOut() }
    }
}
