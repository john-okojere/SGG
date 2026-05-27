import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    radius: 8
    color: "#f5f5f7ee"
    border.color: "#d8cbe9"
    border.width: 1
    clip: true
    readonly property bool compact: height < 120
    width: 1030

    function routeItems() {
        return missionStore.plan.serializeForMavsdkMission()
    }

    function distanceMeters(a, b) {
        var earth = 6371000.0
        var dLat = (b.latitude - a.latitude) * Math.PI / 180.0
        var dLon = (b.longitude - a.longitude) * Math.PI / 180.0
        var lat1 = a.latitude * Math.PI / 180.0
        var lat2 = b.latitude * Math.PI / 180.0
        var h = Math.sin(dLat / 2) * Math.sin(dLat / 2)
                + Math.cos(lat1) * Math.cos(lat2) * Math.sin(dLon / 2) * Math.sin(dLon / 2)
        return earth * 2.0 * Math.atan2(Math.sqrt(h), Math.sqrt(1.0 - h))
    }

    function cumulativeDistances(route) {
        var distances = [0]
        var total = 0
        for (var i = 1; i < route.length; ++i) {
            total += distanceMeters(route[i - 1], route[i])
            distances.push(total)
        }
        return { values: distances, total: Math.max(total, 1) }
    }

    Connections { target: missionStore.plan; function onPlanChanged() { profileCanvas.requestPaint() } }
    Connections { target: mapState; function onCenterChanged() { profileCanvas.requestPaint() } }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: root.compact ? 8 : 12
        spacing: root.compact ? 4 : 6

        RowLayout {
            Layout.fillWidth: true
            Text {
                Layout.fillWidth: true
                text: "Elevation Profile"
                color: Theme.purple
                font.pixelSize: root.compact ? 12 : 14
                font.bold: true
            }
            Text {
                text: "Planned altitude profile"
                color: Theme.muted
                font.pixelSize: root.compact ? 10 : 11
            }
        }

        Canvas {
            id: profileCanvas
            Layout.fillWidth: true
            Layout.fillHeight: true
            antialiasing: true

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                var route = root.routeItems()
                ctx.fillStyle = "#efe9f6"
                ctx.fillRect(0, 0, width, height)
                ctx.strokeStyle = "#d8cbe9"
                ctx.lineWidth = 1
                for (var g = 1; g < 4; ++g) {
                    var gy = height * g / 4
                    ctx.beginPath()
                    ctx.moveTo(0, gy)
                    ctx.lineTo(width, gy)
                    ctx.stroke()
                }
                if (route.length < 2) {
                    ctx.fillStyle = Theme.muted
                    ctx.font = (root.compact ? "10px" : "12px") + " sans-serif"
                    ctx.fillText("Add a takeoff point and route items to preview planned altitude.", 12, height / 2)
                    return
                }

                var distances = root.cumulativeDistances(route)
                var minAlt = 999999
                var maxAlt = -999999
                for (var i = 0; i < route.length; ++i) {
                    var alt = Number(route[i].altitude || missionStore.plan.altitude)
                    minAlt = Math.min(minAlt, alt)
                    maxAlt = Math.max(maxAlt, alt)
                }
                if (Math.abs(maxAlt - minAlt) < 1) {
                    maxAlt += 1
                    minAlt -= 1
                }

                ctx.beginPath()
                for (var p = 0; p < route.length; ++p) {
                    var x = (distances.values[p] / distances.total) * (width - 24) + 12
                    var altitude = Number(route[p].altitude || missionStore.plan.altitude)
                    var y = height - 14 - ((altitude - minAlt) / (maxAlt - minAlt)) * (height - 28)
                    if (p === 0) ctx.moveTo(x, y)
                    else ctx.lineTo(x, y)
                }
                ctx.strokeStyle = Theme.purple
                ctx.lineWidth = 3
                ctx.stroke()

                for (var q = 0; q < route.length; ++q) {
                    var px = (distances.values[q] / distances.total) * (width - 24) + 12
                    var py = height - 14 - ((Number(route[q].altitude || missionStore.plan.altitude) - minAlt) / (maxAlt - minAlt)) * (height - 28)
                    ctx.beginPath()
                    ctx.arc(px, py, q === 0 ? 5 : 4, 0, Math.PI * 2)
                    ctx.fillStyle = q === 0 ? Theme.green : Theme.purple2
                    ctx.fill()
                    ctx.strokeStyle = "#ffffff"
                    ctx.lineWidth = 1.5
                    ctx.stroke()
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var route = root.routeItems()
                    if (route.length < 2 || missionStore.plan.waypoints.length === 0) return
                    var ratio = Math.max(0, Math.min(1, mouse.x / Math.max(1, width)))
                    var routeIndex = Math.round(ratio * (route.length - 1))
                    var waypointIndex = missionStore.plan.hasTakeoffPoint ? routeIndex - 1 : routeIndex
                    appState.selectedWaypointIndex = Math.max(0, Math.min(missionStore.plan.waypoints.length - 1, waypointIndex))
                }
            }
        }
    }
}
