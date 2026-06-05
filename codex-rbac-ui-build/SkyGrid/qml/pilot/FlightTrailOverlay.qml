import QtQuick
import SkyGrid 1.0

Canvas {
    id: root
    property var mapItem
    property real dashOffset: 0

    Timer {
        interval: 45
        repeat: true
        running: root.visible
        onTriggered: {
            root.dashOffset = (root.dashOffset + 0.65) % 22
            root.requestPaint()
        }
    }

    Connections { target: telemetryStore; function onTelemetryChanged() { root.requestPaint() } }
    Connections { target: mapState; function onCenterChanged() { root.requestPaint() } }
    Connections { target: mapState; function onZoomChanged() { root.requestPaint() } }
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        var path = telemetryStore.livePath
        if (!mapItem || path.length < 2) {
            return
        }
        ctx.lineJoin = "round"
        ctx.lineCap = "round"
        ctx.beginPath()
        for (var i = 0; i < path.length; ++i) {
            var p = mapItem.pointFor(path[i].latitude, path[i].longitude)
            if (i === 0) ctx.moveTo(p.x, p.y)
            else ctx.lineTo(p.x, p.y)
        }
        ctx.strokeStyle = "rgba(46,0,95,0.35)"
        ctx.lineWidth = 8
        ctx.stroke()
        ctx.strokeStyle = "rgba(91,34,168,0.92)"
        ctx.lineWidth = 3
        ctx.setLineDash([11, 8])
        ctx.lineDashOffset = -root.dashOffset
        ctx.stroke()
        ctx.setLineDash([])
    }
}
