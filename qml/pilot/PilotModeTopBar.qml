import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    color: "#fbf9fe"
    border.color: "#e2d8ee"
    border.width: 1
    signal eventLogRequested()

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 26
        anchors.rightMargin: 26
        spacing: 12

        Image {
            Layout.preferredWidth: 220
            Layout.preferredHeight: 50
            source: AssetRegistry.logos.full_logo
            sourceSize.width: 440
            sourceSize.height: 100
            fillMode: Image.PreserveAspectFit
            smooth: true
            mipmap: true
            MouseArea { anchors.fill: parent; onClicked: appState.goHome() }
        }

        Item { Layout.fillWidth: true }

        ModeSwitchTabs {
            Layout.preferredWidth: 350
            Layout.preferredHeight: 52
        }

        Item { Layout.fillWidth: true }

        TelemetryPill {
            Layout.preferredWidth: 230
            iconSource: AssetRegistry.icons.plane
            text: telemetryStore.connected ? telemetryStore.aircraftId : "NO AIRCRAFT"
            trailingIcon: AssetRegistry.icons.plane
        }
        TelemetryPill {
            Layout.preferredWidth: 104
            iconSource: AssetRegistry.icons.boxicons_battery_3
            text: telemetryStore.connected ? telemetryStore.battery + "%" : "--%"
            accent: telemetryStore.battery > 30 ? "#18b83f" : "#f5c542"
        }
        TelemetryPill {
            Layout.preferredWidth: 120
            iconSource: AssetRegistry.icons.lucide_satellite
            text: telemetryStore.connected ? telemetryStore.satellites + " SAT" : "NO GPS"
        }
        TelemetryPill {
            Layout.preferredWidth: 112
            iconSource: AssetRegistry.icons.boxicons_wifi
            text: telemetryStore.connected ? telemetryStore.transmission + "%" : "--%"
            accent: telemetryStore.transmission > 70 ? "#18b83f" : "#f5c542"
        }
        TelemetryPill {
            Layout.preferredWidth: 112
            iconSource: AssetRegistry.icons.boxicons_wind_filled
            text: telemetryStore.connected ? Number(telemetryStore.speed).toFixed(0) + " m/s" : "-- m/s"
        }

        Button {
            Layout.preferredWidth: 122
            Layout.preferredHeight: 38
            onClicked: {
                eventLogManager.logEvent("event_log_opened", "info", "Pilot opened event log")
                root.eventLogRequested()
            }
            background: Rectangle {
                radius: 7
                color: parent.hovered ? "#f4eff9" : "#ffffff"
                border.color: "#d7c8e6"
            }
            contentItem: Row {
                spacing: 8
                anchors.centerIn: parent
                AssetIcon {
                    width: 15
                    height: 15
                    source: AssetRegistry.icons.boxicons_save
                    active: true
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    text: "Event Log"
                    color: "#2e005f"
                    font.pixelSize: 11
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

	        RowLayout {
	            Layout.preferredWidth: 178
            spacing: 9
            Rectangle {
                Layout.preferredWidth: 42
                Layout.preferredHeight: 42
                radius: 21
                color: "#2e005f"
                AssetIcon {
                    anchors.centerIn: parent
                    width: 24
                    height: 24
                    source: AssetRegistry.icons.mdi_person_outline
                    active: true
                }
            }
            Text {
                Layout.fillWidth: true
                text: profileManager.displayName.length > 0 ? profileManager.displayName : "Sarah Brickleback"
                color: "#111111"
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
	            }
	            Text { text: "⌄"; color: "#111111"; font.pixelSize: 14; font.bold: true }
	        }

	        WindowCloseButton {
	            Layout.preferredWidth: 42
	            Layout.preferredHeight: 42
	        }
	    }

    component TelemetryPill: Rectangle {
        id: pill
        property url iconSource: ""
        property url trailingIcon: ""
        property string text: ""
        property color accent: "#111111"
        Layout.preferredHeight: 52
        radius: 7
        color: "#ffffff"
        border.color: "#e2d8ee"
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 14
            anchors.rightMargin: 14
            spacing: 8
            AssetIcon { Layout.preferredWidth: 17; Layout.preferredHeight: 17; source: iconSource; active: true }
            Text {
                Layout.fillWidth: true
                text: pill.text
                color: "#111111"
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
            }
            AssetIcon {
                Layout.preferredWidth: 15
                Layout.preferredHeight: 15
                source: trailingIcon
                visible: trailingIcon.toString().length > 0
                active: true
            }
        }
    }
}
