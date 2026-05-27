import QtQuick
import Esri.ArcGISRuntime

MapView {
    id: root
    readonly property bool satelliteMode: mapProvider.layerMode === "SAT"
    readonly property real scaleForZoom: Math.max(1200, 2600000 / Math.pow(2, mapState.zoomLevel - 6))

    function pointFor(latitude, longitude) {
        screenPointBuilder.setXY(longitude, latitude)
        var result = locationToScreen(screenPointBuilder.geometry)
        if (result && result.screenPoint) {
            return result.screenPoint
        }
        return Qt.point(width / 2, height / 2)
    }

    function coordinateFor(point) {
        var location = screenToLocation(point.x, point.y)
        return {
            latitude: location.y,
            longitude: location.x
        }
    }

    rotationByPinchingEnabled: true
    zoomByPinchingEnabled: true
    wrapAroundMode: Enums.WrapAroundModeEnabledWhenSupported
    attributionTextVisible: true

    Component.onCompleted: {
        forceActiveFocus()
        ArcGISRuntimeEnvironment.apiKey = mapProvider.apiKey
    }

    Connections {
        target: mapState
        function onCenterChanged() {
            centerPointBuilder.setXY(mapState.centerLongitude, mapState.centerLatitude)
            root.setViewpointCenterAndScale(centerPointBuilder.geometry, root.scaleForZoom)
        }
        function onZoomChanged() {
            root.setViewpointScale(root.scaleForZoom)
        }
    }

    PointBuilder {
        id: centerPointBuilder
        spatialReference: SpatialReference { wkid: 4326 }
        Component.onCompleted: setXY(mapState.centerLongitude, mapState.centerLatitude)
    }

    PointBuilder {
        id: screenPointBuilder
        spatialReference: SpatialReference { wkid: 4326 }
    }

    Map {
        initBasemapStyle: root.satelliteMode
            ? Enums.BasemapStyleArcGISImageryStandard
            : Enums.BasemapStyleArcGISTopographic
        initialViewpoint: ViewpointCenter {
            center: Point {
                x: mapState.centerLongitude
                y: mapState.centerLatitude
                spatialReference: SpatialReference { wkid: 4326 }
            }
            targetScale: root.scaleForZoom
        }
    }
}
