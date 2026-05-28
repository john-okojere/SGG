import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Item {
    id: root
    objectName: "postMissionSummaryModal"
    visible: postMissionSummaryManager.visible
    z: 200

    readonly property var summary: postMissionSummaryManager.lastSummary
    readonly property bool compact: width < 980 || height < 760
    readonly property color purple: "#3b0787"
    readonly property color purple2: "#5b22a8"

    function value(key, fallback) {
        if (!summary || summary[key] === undefined || summary[key] === null || summary[key] === "")
            return fallback
        return summary[key]
    }

    function numberValue(key, decimals, suffix) {
        var v = Number(value(key, 0))
        if (!isFinite(v)) v = 0
        return v.toFixed(decimals) + (suffix || "")
    }

    function syncValue(key, fallback) {
        var sync = summary && summary.sync ? summary.sync : ({})
        if (sync[key] === undefined || sync[key] === null || sync[key] === "")
            return fallback
        return sync[key]
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        preventStealing: true
        onWheel: function(wheel) { wheel.accepted = true }
    }

    Rectangle {
        anchors.fill: parent
        color: "#00000099"
    }

    Rectangle {
        id: card
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, root.compact ? 820 : 1040)
        height: Math.min(parent.height - 44, root.compact ? 650 : 760)
        radius: 18
        color: "#f7f4fb"
        border.color: "#ded2ed"
        border.width: 1
        clip: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: root.compact ? 72 : 86
                color: root.purple
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: root.compact ? 22 : 30
                    anchors.rightMargin: root.compact ? 16 : 24
                    spacing: 14
                    Text {
                        Layout.fillWidth: true
                        text: root.value("title", "Flight Summary")
                        color: "white"
                        font.pixelSize: root.compact ? 24 : 30
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Rectangle {
                        radius: 16
                        color: root.value("result_status", "completed") === "completed" ? "#19a648" : "#d94a4a"
                        Layout.preferredWidth: 124
                        Layout.preferredHeight: 32
                        Text {
                            anchors.centerIn: parent
                            text: String(root.value("result_status", "completed")).toUpperCase()
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                    IconButton {
                        implicitWidth: 34
                        implicitHeight: 34
                        iconText: "×"
                        iconOnAccent: true
                        active: true
                        onClicked: postMissionSummaryManager.dismiss()
                        ToolTip.visible: hovered
                        ToolTip.text: "Close summary"
                    }
                }
            }

            Flickable {
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentWidth: width
                contentHeight: body.implicitHeight + 32
                clip: true

                ColumnLayout {
                    id: body
                    width: parent.width - (root.compact ? 28 : 44)
                    x: root.compact ? 14 : 22
                    y: root.compact ? 14 : 20
                    spacing: root.compact ? 12 : 16

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        SummaryCard { title: "Mission"; value: root.value("mission_name", "Manual Flight"); detail: root.value("mission_type", "Flight") }
                        SummaryCard { title: "Pilot"; value: root.value("pilot_name", "Operator"); detail: root.value("aircraft_name", "Aircraft") }
                        SummaryCard { title: "Flight Time"; value: root.value("duration_text", "00:00:00"); detail: root.value("start_time", "--") }
                        SummaryCard { title: "Distance"; value: root.numberValue("distance_km", 2, " km"); detail: root.numberValue("average_speed_mps", 1, " m/s avg") }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: root.compact ? 210 : 260
                            radius: 14
                            color: "#18121f"
                            border.color: "#d8c9ec"
                            border.width: 1
                            clip: true

                            Canvas {
                                id: previewCanvas
                                anchors.fill: parent
                                anchors.margins: 12
                                onPaint: {
                                    var ctx = getContext("2d")
                                    ctx.setTransform(1, 0, 0, 1, 0, 0)
                                    ctx.clearRect(0, 0, width, height)
                                    ctx.fillStyle = "#21192b"
                                    ctx.fillRect(0, 0, width, height)

                                    var route = root.value("route_preview", [])
                                    var path = root.value("flight_path", [])
                                    var pts = []
                                    for (var i = 0; i < route.length; ++i) pts.push(route[i])
                                    for (var j = 0; j < path.length; ++j) pts.push(path[j])
                                    if (pts.length === 0) {
                                        ctx.fillStyle = "#d9c9ef"
                                        ctx.font = "bold 14px sans-serif"
                                        ctx.fillText("No route preview available", 20, 34)
                                        return
                                    }

                                    var minLat = 90, maxLat = -90, minLon = 180, maxLon = -180
                                    for (var k = 0; k < pts.length; ++k) {
                                        var lat = Number(pts[k].latitude)
                                        var lon = Number(pts[k].longitude)
                                        if (!isFinite(lat) || !isFinite(lon)) continue
                                        minLat = Math.min(minLat, lat); maxLat = Math.max(maxLat, lat)
                                        minLon = Math.min(minLon, lon); maxLon = Math.max(maxLon, lon)
                                    }
                                    var pad = 24
                                    function px(p) {
                                        var lon = Number(p.longitude), lat = Number(p.latitude)
                                        var x = pad + ((lon - minLon) / Math.max(0.000001, maxLon - minLon)) * (width - pad * 2)
                                        var y = pad + ((maxLat - lat) / Math.max(0.000001, maxLat - minLat)) * (height - pad * 2)
                                        return { x: x, y: y }
                                    }
                                    function drawLine(list, color, widthPx) {
                                        if (list.length < 2) return
                                        ctx.strokeStyle = color
                                        ctx.lineWidth = widthPx
                                        ctx.beginPath()
                                        var first = px(list[0])
                                        ctx.moveTo(first.x, first.y)
                                        for (var i = 1; i < list.length; ++i) {
                                            var p = px(list[i])
                                            ctx.lineTo(p.x, p.y)
                                        }
                                        ctx.stroke()
                                    }
                                    drawLine(route, "#9d6df0", 3)
                                    drawLine(path, "#24d66f", 2)
                                    var start = route.length > 0 ? px(route[0]) : (path.length > 0 ? px(path[0]) : null)
                                    var end = path.length > 0 ? px(path[path.length - 1]) : (route.length > 0 ? px(route[route.length - 1]) : null)
                                    if (start) {
                                        ctx.fillStyle = "#f7c948"
                                        ctx.beginPath(); ctx.arc(start.x, start.y, 6, 0, Math.PI * 2); ctx.fill()
                                    }
                                    if (end) {
                                        ctx.fillStyle = "#ffffff"
                                        ctx.beginPath(); ctx.arc(end.x, end.y, 5, 0, Math.PI * 2); ctx.fill()
                                    }
                                }
                                Connections {
                                    target: postMissionSummaryManager
                                    function onSummaryChanged() { previewCanvas.requestPaint() }
                                }
                                Component.onCompleted: requestPaint()
                            }

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 18
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: 14
                                text: "Yellow: takeoff/start  ·  Purple: planned route  ·  Green: flown path"
                                color: "#efe7f7"
                                font.pixelSize: 11
                            }
                        }

                        ColumnLayout {
                            Layout.preferredWidth: root.compact ? 250 : 310
                            Layout.fillHeight: true
                            spacing: 10
                            SummaryCard { Layout.fillWidth: true; title: "Max Altitude"; value: root.numberValue("max_altitude_m", 1, " m"); detail: "highest recorded" }
                            SummaryCard { Layout.fillWidth: true; title: "Battery Used"; value: root.numberValue("battery_used_percent", 1, "%"); detail: root.numberValue("battery_end_percent", 0, "% remaining") }
                            SummaryCard { Layout.fillWidth: true; title: "Waypoints"; value: root.value("waypoints_completed", 0) + " / " + root.value("waypoint_count", 0); detail: root.value("photos_captured", 0) + " photos" }
                            SummaryCard { Layout.fillWidth: true; title: "Events"; value: root.value("warnings_count", 0) + " warnings"; detail: root.value("events_count", 0) + " events" }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: postMissionSummaryManager.fullLogVisible ? 190 : 116
                        radius: 14
                        color: "#ffffff"
                        border.color: "#ded2ed"
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 14
                            spacing: 8
                            Text { text: "Status"; color: root.purple; font.pixelSize: 16; font.bold: true }
                            GridLayout {
                                Layout.fillWidth: true
                                columns: root.compact ? 2 : 4
                                rowSpacing: 8
                                columnSpacing: 12
                                StatusLine { label: "Control Center"; value: postMissionSummaryManager.syncStatus }
                                StatusLine { label: "Telemetry"; value: root.syncValue("telemetry", "Unknown") }
                                StatusLine { label: "Flight Log"; value: root.syncValue("flight_log", "Saved locally") }
                                StatusLine { label: "Preflight"; value: root.syncValue("preflight", "Not recorded") }
                            }
                            Text {
                                Layout.fillWidth: true
                                visible: postMissionSummaryManager.fullLogVisible
                                text: "Reason: " + root.value("reason", "No failure reason recorded.") + "\nReports: " + root.value("pdf_report_path", "No PDF exported") + "\nTelemetry queue: " + root.syncValue("telemetry_queue", 0)
                                color: "#5b5364"
                                font.pixelSize: 12
                                wrapMode: Text.WordWrap
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        ActionButton { text: "View Full Log"; onClicked: postMissionSummaryManager.viewFullLog() }
                        ActionButton { text: "Export Report"; onClicked: postMissionSummaryManager.exportReport() }
                        ActionButton { text: "Sync Now"; onClicked: postMissionSummaryManager.syncNow() }
                        Item { Layout.fillWidth: true }
                        ActionButton { text: "Return to Dashboard"; filled: true; onClicked: postMissionSummaryManager.returnToDashboard() }
                        ActionButton { text: "Start New Mission"; filled: true; onClicked: postMissionSummaryManager.startNewMission() }
                    }
                }
            }
        }
    }

    component SummaryCard: Rectangle {
        property string title: ""
        property string value: ""
        property string detail: ""
        Layout.fillWidth: true
        Layout.preferredHeight: root.compact ? 78 : 88
        radius: 12
        color: "#ffffff"
        border.color: "#ded2ed"
        Column {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 5
            Text { text: title; color: "#6f667a"; font.pixelSize: 11; font.bold: true; elide: Text.ElideRight; width: parent.width }
            Text { text: value; color: "#111018"; font.pixelSize: root.compact ? 18 : 22; font.bold: true; elide: Text.ElideRight; width: parent.width }
            Text { text: detail; color: "#81758f"; font.pixelSize: 11; elide: Text.ElideRight; width: parent.width }
        }
    }

    component StatusLine: ColumnLayout {
        property string label: ""
        property string value: ""
        Layout.fillWidth: true
        spacing: 2
        Text { text: label; color: "#786c86"; font.pixelSize: 11; font.bold: true }
        Text { text: value; color: "#17111f"; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true }
    }

    component ActionButton: Button {
        property bool filled: false
        Layout.preferredHeight: 38
        Layout.preferredWidth: filled ? 164 : 132
        background: Rectangle {
            radius: 10
            color: parent.filled ? (parent.hovered ? root.purple2 : root.purple) : (parent.hovered ? "#f0e8fa" : "#ffffff")
            border.color: root.purple
            border.width: 1
        }
        contentItem: Text {
            text: parent.text
            color: parent.filled ? "white" : root.purple
            font.pixelSize: 12
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
}
