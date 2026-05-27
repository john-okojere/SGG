import QtQuick
import SkyGrid 1.0

Item {
    id: root

    function pointFor(latitude, longitude) {
        var scale = Math.max(0.00008, 0.028 / mapState.zoomLevel)
        return Qt.point(
            width / 2 + ((longitude - mapState.centerLongitude) / scale) * width,
            height / 2 - ((latitude - mapState.centerLatitude) / scale) * height
        )
    }

    function coordinateFor(point) {
        var scale = Math.max(0.00008, 0.028 / mapState.zoomLevel)
        return {
            latitude: mapState.centerLatitude - ((point.y - height / 2) / height) * scale,
            longitude: mapState.centerLongitude + ((point.x - width / 2) / width) * scale
        }
    }

    Rectangle {
        anchors.fill: parent
        color: mapProvider.layerMode === "SAT" ? "#172319" : "#202531"
    }

    Canvas {
        id: baseCanvas
        anchors.fill: parent
        antialiasing: true

        Connections { target: mapState; function onZoomChanged() { baseCanvas.requestPaint() } }
        Connections { target: mapState; function onCenterChanged() { baseCanvas.requestPaint() } }
        Connections { target: mapProvider; function onLayerModeChanged() { baseCanvas.requestPaint() } }
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            var sat = mapProvider.layerMode === "SAT"

            var g = ctx.createLinearGradient(0, 0, width, height)
            g.addColorStop(0, sat ? "#334a30" : "#243044")
            g.addColorStop(0.42, sat ? "#76663f" : "#31455f")
            g.addColorStop(1, sat ? "#1f3d32" : "#1d2a38")
            ctx.fillStyle = g
            ctx.fillRect(0, 0, width, height)

            ctx.globalAlpha = sat ? 0.24 : 0.42
            ctx.strokeStyle = sat ? "#e8dfb0" : "#d3e1f5"
            ctx.lineWidth = 5
            var roadY = height * 0.78
            ctx.beginPath()
            ctx.moveTo(width * 0.1, roadY)
            ctx.bezierCurveTo(width * 0.35, roadY - 50, width * 0.58, roadY + 34, width * 0.92, roadY - 12)
            ctx.stroke()

            ctx.strokeStyle = "#f1f5f9"
            ctx.lineWidth = 4
            ctx.beginPath()
            ctx.moveTo(width * 0.42, 0)
            ctx.bezierCurveTo(width * 0.35, height * 0.25, width * 0.52, height * 0.42, width * 0.46, height)
            ctx.stroke()

            ctx.strokeStyle = sat ? "#89a17f" : "#7990ab"
            ctx.lineWidth = 1
            for (var x = -40; x < width + 80; x += 56) {
                ctx.beginPath()
                ctx.moveTo(x, 0)
                ctx.lineTo(x + width * 0.18, height)
                ctx.stroke()
            }
            for (var y = -20; y < height + 80; y += 48) {
                ctx.beginPath()
                ctx.moveTo(0, y)
                ctx.lineTo(width, y + 34)
                ctx.stroke()
            }

            ctx.globalAlpha = 0.75
            ctx.fillStyle = "#ffffff"
            ctx.font = "bold 24px sans-serif"
            ctx.fillText("Kaduna", width * 0.42, height * 0.28)
            ctx.font = "16px sans-serif"
            ctx.fillText("Kinkinau", width * 0.22, height * 0.45)
            ctx.fillText("Barnawa", width * 0.43, height * 0.52)
            ctx.fillText("Kachia Rd", width * 0.62, height * 0.76)
            ctx.globalAlpha = 1
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#16051d"
        opacity: 0.12
    }
}
