import QtQuick
import Esri.ArcGISRuntime

SceneView {
    id: root

    readonly property real cameraDistance: Math.max(850, 155000 / Math.pow(2, mapState.zoomLevel - 8))
    readonly property real cameraAltitude: Math.max(700, 210000 / Math.pow(2, mapState.zoomLevel - 8))

    function pointFor(latitude, longitude) {
        screenPointBuilder.setXY(longitude, latitude)
        var result = locationToScreen(screenPointBuilder.geometry)
        if (result && result.screenPoint) {
            return result.screenPoint
        }
        return Qt.point(width / 2, height / 2)
    }

    function coordinateFor(point) {
        var location = screenToBaseSurface(point.x, point.y)
        if (location) {
            return {
                latitude: location.y,
                longitude: location.x
            }
        }
        return {
            latitude: mapState.centerLatitude,
            longitude: mapState.centerLongitude
        }
    }

    function refreshCamera(seconds) {
        var location = ArcGISRuntimeEnvironment.createObject("Point", {
            x: mapState.centerLongitude,
            y: mapState.centerLatitude - Math.min(0.09, cameraDistance / 1300000),
            z: cameraAltitude,
            spatialReference: wgs84
        }, root)
        var camera = ArcGISRuntimeEnvironment.createObject("Camera", {
            location: location,
            heading: 0,
            pitch: 58,
            roll: 0
        }, root)
        root.setViewpointCameraAndSeconds(camera, seconds)
    }

    Component.onCompleted: {
        forceActiveFocus()
        ArcGISRuntimeEnvironment.apiKey = mapProvider.apiKey
        refreshCamera(0)
    }

    Scene {
        Basemap {
            initStyle: Enums.BasemapStyleArcGISImageryStandard
        }
        Surface {
            ArcGISTiledElevationSource {
                url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
            }
        }
    }

    SpatialReference {
        id: wgs84
        wkid: 4326
    }

    PointBuilder {
        id: screenPointBuilder
        spatialReference: SpatialReference { wkid: 4326 }
    }

    Connections {
        target: mapState
        function onCenterChanged() { root.refreshCamera(0.18) }
        function onZoomChanged() { root.refreshCamera(0.18) }
    }
}
