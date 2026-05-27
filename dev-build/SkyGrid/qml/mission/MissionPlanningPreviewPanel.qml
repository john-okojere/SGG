import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    radius: 8
    color: "#ffffff"
    border.color: canReachFirstWaypoint() ? "#b9e8c6" : "#efd06c"
    border.width: 1
    implicitHeight: 132

    function routeItems() {
        return missionStore.plan.serializeForMavsdkMission()
    }

    function firstPoint() {
        var route = routeItems()
        if (route.length > 0) return route[0]
        if (missionStore.plan.polygon.length > 0) return missionStore.plan.polygon[0]
        if (missionStore.plan.hasPoi) return missionStore.plan.poi
        return null
    }

    function distanceMeters(aLat, aLon, bLat, bLon) {
        var r = 6371000
        var p1 = aLat * Math.PI / 180
        var p2 = bLat * Math.PI / 180
        var dp = (bLat - aLat) * Math.PI / 180
        var dl = (bLon - aLon) * Math.PI / 180
        var h = Math.sin(dp / 2) * Math.sin(dp / 2)
                + Math.cos(p1) * Math.cos(p2) * Math.sin(dl / 2) * Math.sin(dl / 2)
        return r * 2 * Math.atan2(Math.sqrt(h), Math.sqrt(1 - h))
    }

    function distanceToFirstMeters() {
        var p = firstPoint()
        if (!p || !telemetryStore.connected) return 0
        return distanceMeters(telemetryStore.latitude, telemetryStore.longitude, p.latitude, p.longitude)
    }

    function reserveAfterMission() {
        return telemetryStore.connected ? telemetryStore.battery - missionStore.plan.estimatedBattery : 0
    }

    function canReachFirstWaypoint() {
        if (missionStore.plan.boundaryOnly) return missionStore.plan.backendSyncReady
        if (!telemetryStore.connected || routeItems().length < 2) return false
        return reserveAfterMission() >= 20 && distanceToFirstMeters() < 5000 && windCheckManager.result !== "Do Not Fly"
    }

    function backendLabel() {
        if (missionStore.plan.backendUploadEligible || missionStore.plan.boundaryOnly) return "Ready"
        if (missionStore.plan.backendValidationState === "blocked") return "Needs review"
        return "Validate"
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 10
        columns: 4
        rowSpacing: 8
        columnSpacing: 8

        RowLayout {
            Layout.columnSpan: 4
            Layout.fillWidth: true
            spacing: 8
            AssetIcon {
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                source: AssetRegistry.icons.lucide_route
                active: true
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0
                Text {
                    text: "Live Planning Preview"
                    color: Theme.purple
                    font.pixelSize: 12
                    font.bold: true
                }
                Text {
                    text: missionStore.plan.boundaryOnly
                          ? "Boundary-only safety area synced through Control Center"
                          : (canReachFirstWaypoint() ? "Aircraft can reach the planned start with reserve." : "Resolve route, aircraft, battery, or weather before upload.")
                    color: canReachFirstWaypoint() ? "#148a34" : "#9b6b00"
                    font.pixelSize: 10
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        Metric { label: "Aircraft"; value: telemetryStore.connected ? "Online" : "Offline"; accent: telemetryStore.connected ? "#148a34" : Theme.red }
        Metric { label: "To Start"; value: telemetryStore.connected ? (distanceToFirstMeters() / 1000).toFixed(2) : "--"; unit: "km" }
        Metric { label: "Battery"; value: telemetryStore.connected ? telemetryStore.battery : "--"; unit: "%" }
        Metric { label: "Reserve"; value: telemetryStore.connected ? reserveAfterMission().toFixed(0) : "--"; unit: "%" }
        Metric { label: "Wind"; value: weatherManager.available ? windCheckManager.result : "Waiting"; accent: windCheckManager.severity === "critical" ? Theme.red : (windCheckManager.severity === "warning" ? "#b77900" : Theme.ink) }
        Metric { label: "Route"; value: missionStore.plan.boundaryOnly ? "Boundary" : routeItems().length; unit: missionStore.plan.boundaryOnly ? "" : "pts" }
        Metric { label: "Backend"; value: root.backendLabel(); accent: missionStore.plan.backendUploadEligible || missionStore.plan.boundaryOnly ? "#148a34" : "#9b6b00" }
        Metric { label: "Reach"; value: canReachFirstWaypoint() ? "Yes" : "No"; accent: canReachFirstWaypoint() ? "#148a34" : Theme.red }
    }

    component Metric: ColumnLayout {
        property string label: ""
        property var value: "--"
        property string unit: ""
        property color accent: Theme.ink
        spacing: 0
        Text { text: label; color: Theme.muted; font.pixelSize: 9; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
        RowLayout {
            spacing: 2
            Text { text: String(value); color: accent; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
            Text { text: unit; visible: unit.length > 0; color: Theme.muted; font.pixelSize: 9; font.bold: true }
        }
    }
}
