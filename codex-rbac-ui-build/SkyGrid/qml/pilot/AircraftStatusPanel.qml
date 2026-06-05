import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

ColumnLayout {
    id: root
    spacing: 12
    signal moreSettingsRequested()

    PanelCard {
        Layout.fillWidth: true
        Layout.preferredHeight: 196
        title: "AIRCRAFT STATUS"
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 8
            Text { text: "AIRCRAFT STATUS"; color: "#3b294d"; font.pixelSize: 11; font.bold: true }
            RowLayout {
                spacing: 7
                Rectangle { Layout.preferredWidth: 14; Layout.preferredHeight: 14; radius: 7; color: vehicleManager.connected ? "#18b83f" : "#9b94a6" }
                Text {
                    text: vehicleManager.connected ? "Ready to Fly" : "Waiting for Aircraft"
                    color: vehicleManager.connected ? "#18b83f" : "#5f5a66"
                    font.pixelSize: 12
                    font.bold: true
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Rectangle {
                    anchors.centerIn: parent
                    width: 156
                    height: 54
                    radius: 27
                    color: "#f5f1fa"
                    border.color: "#e2d8ee"
                    AssetIcon {
                        anchors.centerIn: parent
                        width: 92
                        height: 38
                        source: AssetRegistry.icons.plane
                        active: true
                        rotation: -2
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: vehicleManager.connected ? telemetryStore.aircraftId : "Mavic 3E"
                    color: "#111111"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
                Text { text: "Details  ›"; color: "#2e005f"; font.pixelSize: 11; font.bold: true }
            }
        }
    }

    FlightStateCard {
        Layout.fillWidth: true
        Layout.preferredHeight: 138
    }

    QuickActionsPanel {
        Layout.fillWidth: true
        Layout.preferredHeight: 212
    }

    ManualSettingsPanel {
        Layout.fillWidth: true
        Layout.fillHeight: true
        onMoreSettingsRequested: root.moreSettingsRequested()
    }

    component PanelCard: Rectangle {
        property string title: ""
        radius: 7
        color: "#ffffff"
        border.color: "#e2d8ee"
        border.width: 1
        layer.enabled: false
    }
}
