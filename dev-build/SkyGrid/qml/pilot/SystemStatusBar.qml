import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    objectName: "pilotSystemStatusBar"
    color: "#ffffff"
    border.color: "#e2d8ee"
    border.width: 1
    signal eventLogRequested()

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 32
        anchors.rightMargin: 32
        spacing: 34

        RowLayout {
            Layout.preferredWidth: 240
            spacing: 10
            Text { text: "SYSTEM"; color: "#3b294d"; font.pixelSize: 11; font.bold: true }
            Rectangle { Layout.preferredWidth: 8; Layout.preferredHeight: 8; radius: 4; color: "#18b83f" }
            Text { text: vehicleManager.connected ? "All Systems Normal" : "Aircraft Link Standby"; color: vehicleManager.connected ? "#18b83f" : "#9b94a6"; font.pixelSize: 11 }
        }
        StatusBlock { label: "Time"; value: Qt.formatTime(new Date(), "hh:mm:ss") }
        StatusBlock { label: "Flight Time"; value: flightStatsManager.flightTimeText }
        StatusBlock { label: "Distance"; value: Number(flightStatsManager.distanceKm).toFixed(2) + " km" }
        LinkBlock { label: "Voice Link"; value: vehicleManager.connected ? "Good" : "Offline" }
        LinkBlock {
            objectName: "pilotBackendSyncStatus"
            label: "Data Link"
            value: telemetrySyncManager.syncState === "online"
                   ? "Online"
                   : (telemetrySyncManager.syncState === "syncing" ? "Syncing" : (telemetrySyncManager.pendingQueueCount > 0 ? "Pending" : "Offline"))
        }
        StatusBlock { label: "Queued"; value: telemetrySyncManager.pendingQueueCount + "" }
        Item { Layout.fillWidth: true }
        Button {
            Layout.preferredWidth: 150
            Layout.preferredHeight: 42
            onClicked: {
                eventLogManager.logEvent("event_log_opened", "info", "Pilot opened event log")
                root.eventLogRequested()
            }
            background: Rectangle {
                radius: 6
                color: parent.hovered ? "#f7f1fd" : "#ffffff"
                border.color: "#5b22a8"
            }
            contentItem: Row {
                spacing: 9
                anchors.centerIn: parent
                AssetIcon { width: 16; height: 16; source: AssetRegistry.icons.boxicons_save; active: true; anchors.verticalCenter: parent.verticalCenter }
                Text { text: "Event Log"; color: "#5b22a8"; font.pixelSize: 11; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
            }
        }
    }

    component StatusBlock: ColumnLayout {
        property string label: ""
        property string value: ""
        Layout.preferredWidth: 126
        spacing: 2
        Text { text: label; color: "#5f5a66"; font.pixelSize: 10 }
        Text { text: value; color: "#111111"; font.pixelSize: 12; font.bold: true }
    }

    component LinkBlock: RowLayout {
        property string label: ""
        property string value: ""
        Layout.preferredWidth: 156
        spacing: 10
        ColumnLayout {
            spacing: 2
            Text { text: label; color: "#5f5a66"; font.pixelSize: 10 }
            Text { text: value; color: value === "Offline" ? "#9b94a6" : "#18b83f"; font.pixelSize: 12; font.bold: true }
        }
        Row {
            spacing: 2
            Repeater {
                model: 10
                Rectangle {
                    width: 2
                    height: 5 + index * 1.8
                    radius: 1
                    color: parent.parent.value === "Offline" ? "#cfc7d9" : "#18b83f"
                    anchors.bottom: parent.bottom
                }
            }
        }
    }
}
