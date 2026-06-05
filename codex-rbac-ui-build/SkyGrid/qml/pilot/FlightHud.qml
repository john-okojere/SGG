import QtQuick
import QtQuick.Layouts

Item {
    id: root
    property bool compact: false
    readonly property bool fastMode: String(performanceMode).toLowerCase() === "fast"
    property bool repaintPending: false
    property real roll: telemetryStore.roll
    property real pitch: telemetryStore.pitch
    property real heading: telemetryStore.heading
    property real altitude: telemetryStore.altitude
    property real speed: telemetryStore.speed

    function schedulePaint() {
        if (!visible || repaintPending) {
            return
        }
        repaintPending = true
        hudRepaintTimer.start()
    }

    Timer {
        id: hudRepaintTimer
        interval: root.fastMode ? 140 : 90
        repeat: false
        onTriggered: {
            root.repaintPending = false
            hudCanvas.requestPaint()
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: 10
        color: "#11111144"
        border.color: "#ffffff55"
        border.width: 1
    }

    Canvas {
        id: hudCanvas
        anchors.fill: parent
        antialiasing: !root.fastMode
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            var cx = width / 2
            var cy = height / 2
            ctx.save()
            ctx.translate(cx, cy)
            ctx.rotate(root.roll * Math.PI / 180)
            ctx.translate(0, root.pitch * 2.0)
            ctx.fillStyle = "rgba(84, 148, 220, 0.42)"
            ctx.fillRect(-width, -height * 2, width * 2, height * 2)
            ctx.fillStyle = "rgba(72, 126, 38, 0.48)"
            ctx.fillRect(-width, 0, width * 2, height * 2)
            ctx.strokeStyle = "rgba(255,255,255,0.92)"
            ctx.lineWidth = 2
            ctx.beginPath()
            ctx.moveTo(-width, 0)
            ctx.lineTo(width, 0)
            ctx.stroke()
            ctx.font = "10px sans-serif"
            ctx.fillStyle = "white"
            ctx.strokeStyle = "rgba(255,255,255,0.72)"
            for (var p = -30; p <= 30; p += 10) {
                var y = -p * 2
                ctx.beginPath()
                ctx.moveTo(-34, y)
                ctx.lineTo(34, y)
                ctx.stroke()
                if (p !== 0) {
                    ctx.fillText(String(Math.abs(p)), -52, y + 3)
                    ctx.fillText(String(Math.abs(p)), 40, y + 3)
                }
            }
            ctx.restore()

            ctx.strokeStyle = "#ff334a"
            ctx.lineWidth = 2
            ctx.beginPath()
            ctx.moveTo(cx - 45, cy)
            ctx.lineTo(cx - 12, cy)
            ctx.moveTo(cx + 12, cy)
            ctx.lineTo(cx + 45, cy)
            ctx.moveTo(cx, cy - 14)
            ctx.lineTo(cx, cy + 14)
            ctx.stroke()

            ctx.strokeStyle = "rgba(255,255,255,0.85)"
            ctx.lineWidth = 1
            ctx.beginPath()
            ctx.arc(cx, cy, Math.min(width, height) * 0.34, Math.PI * 1.12, Math.PI * 1.88)
            ctx.stroke()
        }
        Connections {
            target: telemetryStore
            function onTelemetryChanged() { root.schedulePaint() }
        }
    }

    RowLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        spacing: 8
        Text { text: ("000" + Math.round(root.heading)).slice(-3) + "°"; color: "#ffffff"; font.pixelSize: 12; font.bold: true; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
    }

    Text {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -34
        text: telemetryStore.armed ? telemetryStore.flightMode : "DISARMED"
        color: telemetryStore.armed ? "#18b83f" : "#ef233c"
        font.pixelSize: root.compact ? 16 : 22
        font.bold: true
    }

    RowLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        Text { text: "AS " + Number(root.speed).toFixed(1); color: "#ffffff"; font.pixelSize: 11; font.bold: true; Layout.fillWidth: true }
        Text { text: "GPS " + telemetryStore.gpsMode; color: "#ffffff"; font.pixelSize: 11; font.bold: true; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
        Text { text: "ALT " + Number(root.altitude).toFixed(1); color: "#ffffff"; font.pixelSize: 11; font.bold: true; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight }
    }

    onRollChanged: schedulePaint()
    onPitchChanged: schedulePaint()
    Component.onCompleted: hudCanvas.requestPaint()
}
