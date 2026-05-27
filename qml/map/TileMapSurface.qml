import QtQuick
import SkyGrid 1.0

Item {
    id: root

    readonly property int tileSize: 256
    readonly property int zoom: Math.max(3, Math.min(19, Math.round(mapState.zoomLevel)))
    readonly property real worldSize: tileSize * Math.pow(2, zoom)
    readonly property real centerPixelX: longitudeToPixelX(mapState.centerLongitude)
    readonly property real centerPixelY: latitudeToPixelY(mapState.centerLatitude)
    readonly property real topLeftPixelX: centerPixelX - width / 2
    readonly property real topLeftPixelY: centerPixelY - height / 2
    readonly property int firstTileX: Math.floor(topLeftPixelX / tileSize) - 1
    readonly property int firstTileY: Math.floor(topLeftPixelY / tileSize) - 1
    readonly property int columns: Math.ceil(width / tileSize) + 3
    readonly property int rows: Math.ceil(height / tileSize) + 3
    readonly property string imageryUrl: "https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/"
    readonly property string topoUrl: "https://server.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer/tile/"
    readonly property string terrainUrl: "https://server.arcgisonline.com/ArcGIS/rest/services/World_Terrain_Base/MapServer/tile/"

    function clampLatitude(latitude) {
        return Math.max(-85.05112878, Math.min(85.05112878, latitude))
    }

    function longitudeToPixelX(longitude) {
        return ((longitude + 180.0) / 360.0) * worldSize
    }

    function latitudeToPixelY(latitude) {
        var sinLat = Math.sin(clampLatitude(latitude) * Math.PI / 180.0)
        return (0.5 - Math.log((1 + sinLat) / (1 - sinLat)) / (4 * Math.PI)) * worldSize
    }

    function pixelXToLongitude(pixelX) {
        return (pixelX / worldSize) * 360.0 - 180.0
    }

    function pixelYToLatitude(pixelY) {
        var mercator = Math.PI - (2.0 * Math.PI * pixelY) / worldSize
        return (180.0 / Math.PI) * Math.atan(0.5 * (Math.exp(mercator) - Math.exp(-mercator)))
    }

    function wrappedTileX(tileX) {
        var count = Math.pow(2, zoom)
        return ((tileX % count) + count) % count
    }

    function tileUrl(tileX, tileY) {
        var count = Math.pow(2, zoom)
        if (tileY < 0 || tileY >= count) {
            return ""
        }
        var base = mapProvider.layerMode === "2D" ? topoUrl : (mapProvider.layerMode === "3D" ? terrainUrl : imageryUrl)
        return base + zoom + "/" + tileY + "/" + wrappedTileX(tileX)
    }

    function tileSource(tileX, tileY) {
        var remote = tileUrl(tileX, tileY)
        if (remote.length === 0) {
            return ""
        }
        return tileCacheManager.tileSource(mapProvider.layerMode, zoom, tileX, tileY, remote, tileCacheManager.cacheRevision)
    }

    function pointFor(latitude, longitude) {
        return Qt.point(longitudeToPixelX(longitude) - topLeftPixelX,
                        latitudeToPixelY(latitude) - topLeftPixelY)
    }

    function coordinateFor(point) {
        return {
            latitude: pixelYToLatitude(topLeftPixelY + point.y),
            longitude: pixelXToLongitude(topLeftPixelX + point.x)
        }
    }

    Rectangle {
        anchors.fill: parent
        color: mapProvider.layerMode === "2D" ? "#dde3df" : "#1b241e"
    }

    Repeater {
        model: root.columns * root.rows
        delegate: Image {
            required property int index
            readonly property int col: index % root.columns
            readonly property int row: Math.floor(index / root.columns)
            readonly property int tileX: root.firstTileX + col
            readonly property int tileY: root.firstTileY + row

            x: tileX * root.tileSize - root.topLeftPixelX
            y: tileY * root.tileSize - root.topLeftPixelY
            width: root.tileSize
            height: root.tileSize
            source: root.tileSource(tileX, tileY)
            fillMode: Image.Stretch
            asynchronous: true
            cache: true
            smooth: true
            opacity: status === Image.Ready ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: 140; easing.type: Easing.OutCubic } }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: mapProvider.layerMode === "3D" ? "#13051f" : "#000000"
        opacity: mapProvider.layerMode === "3D" ? 0.16 : 0.04
    }

    Canvas {
        anchors.fill: parent
        visible: mapProvider.layerMode === "3D"
        opacity: 0.28
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.strokeStyle = "rgba(255,255,255,0.22)"
            ctx.lineWidth = 1
            for (var x = -width; x < width * 1.5; x += 56) {
                ctx.beginPath()
                ctx.moveTo(x, 0)
                ctx.lineTo(x + width * 0.24, height)
                ctx.stroke()
            }
        }
    }
}
