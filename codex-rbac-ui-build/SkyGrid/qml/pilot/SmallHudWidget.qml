import QtQuick
import SkyGrid 1.0

Rectangle {
    id: root
    width: 78
    height: 78
    radius: width / 2
    color: "#ffffffee"
    border.color: telemetryStore.armed ? "#5b22a8" : "#e2d8ee"
    border.width: 1

    Canvas {
        id: canvas
        anchors.fill: parent
        anchors.margins: 7
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            var cx = width / 2
            var cy = height / 2
            ctx.save()
            ctx.beginPath()
            ctx.arc(cx, cy, Math.min(width, height) * 0.48, 0, Math.PI * 2)
            ctx.clip()
            ctx.translate(cx, cy)
            ctx.rotate(telemetryStore.roll * Math.PI / 180)
            ctx.translate(0, telemetryStore.pitch * 0.7)
            ctx.fillStyle = "rgba(91, 34, 168, 0.20)"
            ctx.fillRect(-width, -height, width * 2, height)
            ctx.fillStyle = "rgba(24, 184, 63, 0.16)"
            ctx.fillRect(-width, 0, width * 2, height)
            ctx.strokeStyle = "rgba(46,0,95,0.72)"
            ctx.lineWidth = 1.5
            ctx.beginPath()
            ctx.moveTo(-width, 0)
            ctx.lineTo(width, 0)
            ctx.stroke()
            ctx.restore()

            ctx.strokeStyle = "#ef233c"
            ctx.lineWidth = 1.5
            ctx.beginPath()
            ctx.moveTo(cx - 22, cy)
            ctx.lineTo(cx - 7, cy)
            ctx.moveTo(cx + 7, cy)
            ctx.lineTo(cx + 22, cy)
            ctx.moveTo(cx, cy - 8)
            ctx.lineTo(cx, cy + 8)
            ctx.stroke()
        }

        Connections {
            target: telemetryStore
            function onTelemetryChanged() { canvas.requestPaint() }
        }
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 7
        text: ("000" + Math.round(telemetryStore.heading)).slice(-3)
        color: "#2e005f"
        font.pixelSize: 9
        font.bold: true
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        text: telemetryStore.armed ? "ARM" : "DIS"
        color: telemetryStore.armed ? "#18b83f" : "#9b94a6"
        font.pixelSize: 9
        font.bold: true
    }

    Component.onCompleted: canvas.requestPaint()
}
