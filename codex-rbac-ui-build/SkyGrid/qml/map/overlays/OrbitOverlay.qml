import QtQuick
import SkyGrid 1.0

Canvas {
    id: canvas
    property var mapItem
    property real phase: 0
    antialiasing: true

    Timer {
        interval: 45
        running: canvas.visible
        repeat: true
        onTriggered: {
            canvas.phase = (canvas.phase + 0.018) % (Math.PI * 2)
            canvas.requestPaint()
        }
    }

    Connections { target: missionStore.plan; function onPlanChanged() { canvas.requestPaint() } }
    Connections { target: mapState; function onZoomChanged() { canvas.requestPaint() } function onCenterChanged() { canvas.requestPaint() } }
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        if (!canvas.mapItem) return
        var poi = missionStore.plan.poi
        var center = canvas.mapItem.pointFor(poi.latitude, poi.longitude)
        var cx = center.x
        var cy = center.y
        var r = Math.max(58, Math.min(220, missionStore.plan.radius / 4))
        ctx.strokeStyle = "rgba(255,255,255,0.92)"
        ctx.lineWidth = 5
        ctx.beginPath()
        ctx.arc(cx, cy, r, 0, Math.PI * 2)
        ctx.stroke()
        ctx.strokeStyle = "rgba(44,0,87,0.9)"
        ctx.lineWidth = 2
        ctx.setLineDash([12, 10])
        ctx.lineDashOffset = -canvas.phase * 26
        ctx.beginPath()
        ctx.arc(cx, cy, r * 0.58, 0, Math.PI * 2)
        ctx.stroke()
        ctx.setLineDash([])
        ctx.fillStyle = "rgba(247,201,72,0.24)"
        ctx.beginPath()
        ctx.moveTo(cx, cy)
        ctx.arc(cx, cy, r, canvas.phase - 0.65, canvas.phase - 0.08)
        ctx.closePath()
        ctx.fill()
    }
}
