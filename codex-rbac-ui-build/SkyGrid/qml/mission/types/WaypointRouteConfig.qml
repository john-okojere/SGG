import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../../controls"

ColumnLayout {
    id: root
    spacing: 12
    property int selectedIndex: appState.selectedWaypointIndex
    property var selectedWaypoint: missionStore.plan.waypointAt(selectedIndex)
    property bool hasSelection: selectedIndex >= 0 && selectedIndex < missionStore.plan.waypoints.length

    SegmentedControl { Layout.fillWidth: true; height: 42; options: ["All Points", "Selected Point"]; currentIndex: root.hasSelection ? 1 : 0 }
    ParameterSlider { Layout.fillWidth: true; label: "Speed"; from: 2; to: 140; value: missionStore.plan.speed; unit: "m/s"; onValueEdited: missionStore.plan.speed = value }
    ParameterSlider { Layout.fillWidth: true; label: "Mission Alt."; from: 20; to: 160; value: missionStore.plan.altitude; decimals: 1; unit: "m"; onValueEdited: missionStore.plan.altitude = value }
    SelectRow { Layout.fillWidth: true; label: "Heading Mode"; value: "Follow Course"; options: ["Follow Course", "Manual", "Toward POI"] }
    SelectRow { Layout.fillWidth: true; label: "Aircraft Rot."; value: "Auto"; options: ["Auto", "Manual", "Path Locked"] }
    SelectRow { Layout.fillWidth: true; label: "Gimbal Pitch"; value: "Defined Per Point"; options: ["Defined Per Point", "Fixed Down", "Follow POI"] }
    SelectRow { Layout.fillWidth: true; label: "Cornering Style"; value: "Curved"; options: ["Curved", "Stop and Turn", "Adaptive"] }

    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#d8cde5" }
    Text {
        text: root.hasSelection ? "Waypoint " + (root.selectedIndex + 1) + " Settings" : "Select a waypoint on the map"
        color: Theme.ink
        font.pixelSize: 13
        font.bold: true
    }
    ParameterSlider {
        Layout.fillWidth: true
        enabled: root.hasSelection
        opacity: enabled ? 1 : 0.45
        label: "WP Alt."
        from: 10
        to: 160
        value: root.hasSelection ? Number(root.selectedWaypoint.altitude || missionStore.plan.altitude) : missionStore.plan.altitude
        decimals: 1
        unit: "m"
        onValueEdited: missionStore.plan.setWaypointAltitude(root.selectedIndex, value)
    }
    ParameterSlider {
        Layout.fillWidth: true
        enabled: root.hasSelection
        opacity: enabled ? 1 : 0.45
        label: "WP Speed"
        from: 1
        to: 35
        value: root.hasSelection ? Number(root.selectedWaypoint.speed || missionStore.plan.speed) : missionStore.plan.speed
        decimals: 1
        unit: "m/s"
        onValueEdited: missionStore.plan.setWaypointSpeed(root.selectedIndex, value)
    }
    ParameterSlider {
        Layout.fillWidth: true
        enabled: root.hasSelection
        opacity: enabled ? 1 : 0.45
        label: "Heading"
        from: 0
        to: 359
        value: root.hasSelection ? Number(root.selectedWaypoint.heading || 0) : 0
        unit: "deg"
        onValueEdited: missionStore.plan.setWaypointHeading(root.selectedIndex, value)
    }
    ParameterSlider {
        Layout.fillWidth: true
        enabled: root.hasSelection
        opacity: enabled ? 1 : 0.45
        label: "Gimbal"
        from: -90
        to: 20
        value: root.hasSelection ? Number(root.selectedWaypoint.gimbal_pitch || -45) : -45
        unit: "deg"
        onValueEdited: missionStore.plan.setWaypointGimbalPitch(root.selectedIndex, value)
    }
    RowLayout {
        Layout.fillWidth: true
        spacing: 8
        PrimaryButton {
            Layout.fillWidth: true
            secondary: true
            enabled: root.hasSelection && root.selectedIndex > 0
            text: "Move Up"
            onClicked: {
                missionStore.plan.moveWaypointOrder(root.selectedIndex, root.selectedIndex - 1)
                appState.selectedWaypointIndex = root.selectedIndex - 1
            }
        }
        PrimaryButton {
            Layout.fillWidth: true
            secondary: true
            enabled: root.hasSelection && root.selectedIndex < missionStore.plan.waypoints.length - 1
            text: "Move Down"
            onClicked: {
                missionStore.plan.moveWaypointOrder(root.selectedIndex, root.selectedIndex + 1)
                appState.selectedWaypointIndex = root.selectedIndex + 1
            }
        }
        PrimaryButton {
            Layout.fillWidth: true
            secondary: true
            enabled: root.hasSelection
            text: "Delete"
            iconSource: AssetRegistry.icons.boxicons_trash
            onClicked: {
                missionStore.plan.deleteWaypoint(root.selectedIndex)
                appState.selectedWaypointIndex = Math.min(root.selectedIndex, missionStore.plan.waypoints.length - 1)
            }
        }
    }
    SelectRow { Layout.fillWidth: true; label: "Latitude"; value: root.hasSelection ? Number(root.selectedWaypoint.latitude).toFixed(7) : "--" }
    SelectRow { Layout.fillWidth: true; label: "Longitude"; value: root.hasSelection ? Number(root.selectedWaypoint.longitude).toFixed(7) : "--" }
    SelectRow { Layout.fillWidth: true; label: "Finish Action"; value: "Return to Home"; options: ["Return to Home", "Hover", "Land"] }
}
