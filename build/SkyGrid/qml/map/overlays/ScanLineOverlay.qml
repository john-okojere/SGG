import QtQuick
import SkyGrid 1.0

Canvas {
    id: canvas
    property var mapItem
    property real phase: 0
    antialiasing: true
    opacity: 0.82

    Timer {
        interval: 70
        running: canvas.visible
        repeat: true
        onTriggered: {
            canvas.phase = (canvas.phase + 1.2) % 80
            canvas.requestPaint()
        }
    }

    Connections {
        target: missionStore.plan
        function onPlanChanged() { canvas.requestPaint() }
    }
    Connections {
        target: mapState
        function onOverlayChanged() { canvas.requestPaint() }
        function onZoomChanged() { canvas.requestPaint() }
        function onCenterChanged() { canvas.requestPaint() }
    }
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        var polygon = missionStore.plan.polygon
        if (!canvas.mapItem || polygon.length < 3) return
        var left = width, right = 0, top = height, bottom = 0
        for (var i = 0; i < polygon.length; ++i) {
            var p = canvas.mapItem.pointFor(polygon[i].latitude, polygon[i].longitude)
            left = Math.min(left, p.x)
            right = Math.max(right, p.x)
            top = Math.min(top, p.y)
            bottom = Math.max(bottom, p.y)
        }
        var overlap = Math.max(10, missionStore.plan.sideOverlap)
        var spacing = Math.max(14, 58 - overlap * 0.38)
        ctx.save()
        ctx.beginPath()
        ctx.moveTo(canvas.mapItem.pointFor(polygon[0].latitude, polygon[0].longitude).x, canvas.mapItem.pointFor(polygon[0].latitude, polygon[0].longitude).y)
        for (var j = 1; j < polygon.length; ++j) {
            var cp = canvas.mapItem.pointFor(polygon[j].latitude, polygon[j].longitude)
            ctx.lineTo(cp.x, cp.y)
        }
        ctx.closePath()
        ctx.clip()
        ctx.strokeStyle = "rgba(255,255,255,0.76)"
        ctx.lineWidth = 2.2
        for (var x = left - 260 - canvas.phase; x < right + 260; x += spacing) {
            ctx.beginPath()
            ctx.moveTo(x, bottom)
            ctx.lineTo(x + 260, top)
            ctx.stroke()
        }
        ctx.restore()
    }
}
