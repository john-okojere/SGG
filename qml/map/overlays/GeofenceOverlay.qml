import QtQuick
import SkyGrid 1.0

Canvas {
    id: canvas
    property var mapItem
    antialiasing: true

    Connections { target: missionStore.plan; function onPlanChanged() { canvas.requestPaint() } }
    Connections { target: mapState; function onZoomChanged() { canvas.requestPaint() } function onCenterChanged() { canvas.requestPaint() } }
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        if (!canvas.mapItem) return
        var polygon = missionStore.plan.polygon
        if (polygon.length < 3) return
        var left = width, right = 0, top = height, bottom = 0
        for (var i = 0; i < polygon.length; ++i) {
            var p = canvas.mapItem.pointFor(polygon[i].latitude, polygon[i].longitude)
            left = Math.min(left, p.x)
            right = Math.max(right, p.x)
            top = Math.min(top, p.y)
            bottom = Math.max(bottom, p.y)
        }
        var pad = missionStore.plan.safeMargin * 1.4
        var unsafe = missionStore.plan.safeMargin < 15
        ctx.strokeStyle = unsafe ? "rgba(239,68,68,0.94)" : "rgba(247,201,72,0.92)"
        ctx.lineWidth = unsafe ? 5 : 4
        ctx.setLineDash([18, 12])
        ctx.strokeRect(left - pad, top - pad, right - left + pad * 2, bottom - top + pad * 2)
        ctx.setLineDash([])
        ctx.fillStyle = unsafe ? "rgba(239,68,68,0.10)" : "rgba(247,201,72,0.08)"
        ctx.fillRect(left - pad, top - pad, right - left + pad * 2, bottom - top + pad * 2)
    }
}
