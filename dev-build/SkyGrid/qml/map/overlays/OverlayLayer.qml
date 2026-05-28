import QtQuick
import SkyGrid 1.0

Item {
    id: root
    property var mapItem
    readonly property string activeMissionType: missionStore.plan.missionType.length > 0 ? missionStore.plan.missionType : appState.currentMissionType

    ScanLineOverlay {
        anchors.fill: parent
        mapItem: root.mapItem
        visible: root.activeMissionType === "map3dArea" || root.activeMissionType === "towerInspection"
    }
    OrbitOverlay {
        anchors.fill: parent
        mapItem: root.mapItem
        visible: missionStore.plan.hasPoi && root.activeMissionType === "map3dPoi"
    }
    GeofenceOverlay {
        anchors.fill: parent
        mapItem: root.mapItem
        visible: root.activeMissionType === "virtualFence"
    }
    AltitudeLabelOverlay {
        anchors.fill: parent
    }
    AircraftMarkerOverlay {
        anchors.fill: parent
        mapItem: root.mapItem
    }
}
