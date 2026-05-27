import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

RowLayout {
    id: root
    spacing: 0
    readonly property string activeMissionType: missionStore.plan.missionType.length > 0 ? missionStore.plan.missionType : appState.currentMissionType

    function fourthLabel() {
        if (root.activeMissionType === "photomap" || root.activeMissionType === "virtualFence") return "Area"
        if (root.activeMissionType === "towerInspection") return "Route Items"
        return "Route Items"
    }

    function fourthValue() {
        if (root.activeMissionType === "photomap" || root.activeMissionType === "virtualFence") return Number(missionStore.plan.missionAreaHa).toFixed(1)
        return String(missionStore.plan.serializeForMavsdkMission().length)
    }

    function fourthUnit() {
        if (root.activeMissionType === "photomap" || root.activeMissionType === "virtualFence") return "ha"
        return "pts"
    }

    MetricTile { Layout.fillWidth: true; label: "Waypoint Qty"; value: String(missionStore.plan.waypoints.length); unit: "pts" }
    Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.line }
    MetricTile { Layout.fillWidth: true; label: "Distance"; value: Number(missionStore.plan.routeDistanceKm).toFixed(2); unit: "km" }
    Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.line }
    MetricTile { Layout.fillWidth: true; label: fourthLabel(); value: fourthValue(); unit: fourthUnit() }
    Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.line }
    MetricTile { Layout.fillWidth: true; label: "Est. Flight Time"; value: missionStore.plan.estimatedTime; unit: "" }
    Rectangle { visible: root.activeMissionType !== "photomap" && root.activeMissionType !== "virtualFence"; Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.line }
    MetricTile { visible: root.activeMissionType !== "photomap" && root.activeMissionType !== "virtualFence"; Layout.fillWidth: true; label: "Battery Est."; value: Number(missionStore.plan.estimatedBattery).toFixed(0); unit: "%" }
}
