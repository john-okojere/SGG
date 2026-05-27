import QtQuick
import SkyGrid 1.0

Item {
    id: root
    readonly property bool usingScene: mapProvider.arcGisQmlAvailable && mapProvider.layerMode === "3D" && mapProvider.supports3D
    readonly property string surfaceSource: mapProvider.arcGisQmlAvailable
        ? (usingScene ? "ArcGisSceneSurface.qml" : "ArcGisRuntimeMapSurface.qml")
        : "OfflineMapSurface.qml"

    function pointFor(latitude, longitude) {
        return activeSurface.item && activeSurface.item.pointFor
            ? activeSurface.item.pointFor(latitude, longitude)
            : Qt.point(width / 2, height / 2)
    }

    function coordinateFor(point) {
        return activeSurface.item && activeSurface.item.coordinateFor
            ? activeSurface.item.coordinateFor(point)
            : { latitude: mapState.centerLatitude, longitude: mapState.centerLongitude }
    }

    Loader {
        id: activeSurface
        anchors.fill: parent
        source: root.surfaceSource
    }

    Connections {
        target: mapProvider
        function onLayerModeChanged() {
            activeSurface.active = false
            activeSurface.active = true
        }
    }

    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 18
        width: statusText.implicitWidth + 26
        height: 34
        radius: 6
        color: root.usingScene ? "#1d1636dd" : (mapProvider.arcGisQmlAvailable ? "#0b2b1add" : "#0b0712dd")
        border.color: root.usingScene ? "#8d6cff90" : (mapProvider.arcGisQmlAvailable ? "#3fe27b70" : "#ffffff35")
        Text {
            id: statusText
            anchors.centerIn: parent
            text: root.usingScene ? "ArcGIS 3D Scene" : (mapProvider.arcGisQmlAvailable ? "ArcGIS " + mapProvider.layerMode + " live" : "ArcGIS SDK detected · fallback")
            color: Theme.white
            font.pixelSize: 12
            font.bold: true
        }
    }
}
