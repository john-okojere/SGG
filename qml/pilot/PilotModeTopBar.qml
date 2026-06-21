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

    readonly property bool compact: width < 1500
    readonly property bool narrow: width < 1220
    readonly property bool micro: width < 1080
    readonly property bool showDetailedTelemetry: !root.narrow
    readonly property bool showProfile: !root.compact

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: root.compact ? 14 : 26
        anchors.rightMargin: root.compact ? 14 : 26
        spacing: root.compact ? 8 : 12

        Image {
            Layout.preferredWidth: root.micro ? 96 : (root.narrow ? 132 : (root.compact ? 168 : 220))
            Layout.preferredHeight: root.narrow ? 36 : 50
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
            Layout.preferredWidth: root.micro ? 236 : (root.narrow ? 270 : (root.compact ? 306 : 350))
            Layout.preferredHeight: root.narrow ? 44 : 52
        }

        Item { Layout.fillWidth: true }

        TelemetryPill {
            visible: !root.micro
            Layout.preferredWidth: root.narrow ? 130 : (root.compact ? 166 : 230)
            iconSource: AssetRegistry.icons.plane
            text: telemetryStore.connected ? telemetryStore.aircraftId : "NO AIRCRAFT"
            trailingIcon: root.narrow ? "" : AssetRegistry.icons.plane
        }
        TelemetryPill {
            visible: !root.micro
            Layout.preferredWidth: root.narrow ? 76 : (root.compact ? 88 : 104)
            iconSource: AssetRegistry.icons.boxicons_battery_3
            text: telemetryStore.connected ? telemetryStore.battery + "%" : "--%"
            accent: telemetryStore.battery > 30 ? "#18b83f" : "#f5c542"
        }
        TelemetryPill {
            visible: !root.micro
            Layout.preferredWidth: root.narrow ? 90 : (root.compact ? 102 : 120)
            iconSource: AssetRegistry.icons.lucide_satellite
            text: telemetryStore.connected ? telemetryStore.satellites + " SAT" : "NO GPS"
        }
        TelemetryPill {
            visible: !root.micro
            Layout.preferredWidth: root.narrow ? 82 : (root.compact ? 96 : 112)
            iconSource: AssetRegistry.icons.boxicons_wifi
            text: telemetryStore.connected ? telemetryStore.transmission + "%" : "--%"
            accent: telemetryStore.transmission > 70 ? "#18b83f" : "#f5c542"
        }
        TelemetryPill {
            visible: root.showDetailedTelemetry
            Layout.preferredWidth: root.compact ? 96 : 112
            iconSource: AssetRegistry.icons.boxicons_wind_filled
            text: telemetryStore.connected ? Number(telemetryStore.speed).toFixed(0) + " m/s" : "-- m/s"
        }

        Button {
            Layout.preferredWidth: root.micro ? 40 : (root.compact ? 44 : 122)
            Layout.preferredHeight: root.narrow ? 34 : 38
            visible: accessManager.can("can_view_vehicle_audit")
            enabled: sessionManager.operationsAllowed
            onClicked: {
                if (accessManager.canPerform("security_audit")) {
                    root.eventLogRequested()
                } else {
                    accessManager.recordBlocked("security_audit", "Event log opening blocked by local permissions.", {})
                }
            }
            background: Rectangle {
                radius: 7
                color: parent.enabled && parent.hovered ? "#f4eff9" : "#ffffff"
                border.color: "#d7c8e6"
            }
            contentItem: Row {
                spacing: root.compact ? 0 : 8
                anchors.centerIn: parent
                AssetIcon {
                    width: 15
                    height: 15
                    source: AssetRegistry.icons.boxicons_save
                    active: true
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    visible: !root.compact
                    text: "Event Log"
                    color: "#2e005f"
                    font.pixelSize: 11
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        Button {
            id: overflowButton
            visible: root.narrow
            Layout.preferredWidth: 44
            Layout.preferredHeight: 34
            hoverEnabled: true
            contentItem: Text {
                text: "..."
                color: "#2e005f"
                font.pixelSize: 12
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 7
                color: overflowButton.hovered ? "#f4eff9" : "#ffffff"
                border.color: "#d7c8e6"
            }
            onClicked: overflowPopup.open()

            Popup {
                id: overflowPopup
                y: overflowButton.height + 8
                x: -Math.max(0, width - overflowButton.width)
                width: 260
                padding: 12
                modal: false
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
                background: Rectangle { radius: 8; color: "#ffffff"; border.color: "#d7c8e6" }

                ColumnLayout {
                    width: parent.width
                    spacing: 8

                    TelemetryPill {
                        visible: root.micro
                        Layout.fillWidth: true
                        iconSource: AssetRegistry.icons.plane
                        text: telemetryStore.connected ? telemetryStore.aircraftId : "NO AIRCRAFT"
                    }
                    TelemetryPill {
                        visible: root.micro
                        Layout.fillWidth: true
                        iconSource: AssetRegistry.icons.boxicons_battery_3
                        text: telemetryStore.connected ? telemetryStore.battery + "%" : "--%"
                        accent: telemetryStore.battery > 30 ? "#18b83f" : "#f5c542"
                    }
                    TelemetryPill {
                        visible: root.micro
                        Layout.fillWidth: true
                        iconSource: AssetRegistry.icons.lucide_satellite
                        text: telemetryStore.connected ? telemetryStore.satellites + " SAT" : "NO GPS"
                    }
                    TelemetryPill {
                        visible: root.micro
                        Layout.fillWidth: true
                        iconSource: AssetRegistry.icons.boxicons_wifi
                        text: telemetryStore.connected ? telemetryStore.transmission + "%" : "--%"
                        accent: telemetryStore.transmission > 70 ? "#18b83f" : "#f5c542"
                    }
                    TelemetryPill {
                        Layout.fillWidth: true
                        iconSource: AssetRegistry.icons.boxicons_wind_filled
                        text: telemetryStore.connected ? Number(telemetryStore.speed).toFixed(0) + " m/s" : "-- m/s"
                    }
                    Text {
                        Layout.fillWidth: true
                        text: profileManager.displayName.length > 0 ? profileManager.displayName : "Sarah Brickleback"
                        color: "#111111"
                        font.pixelSize: 12
                        font.bold: true
                        elide: Text.ElideRight
                    }
                }
            }
        }

        RowLayout {
            visible: root.showProfile
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
            Text { text: "v"; color: "#111111"; font.pixelSize: 14; font.bold: true }
        }
    }

    component TelemetryPill: Rectangle {
        id: pill
        property url iconSource: ""
        property url trailingIcon: ""
        property string text: ""
        property color accent: "#111111"
        Layout.preferredHeight: root.narrow ? 44 : 52
        radius: 7
        color: "#ffffff"
        border.color: "#e2d8ee"
        clip: true
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: root.narrow ? 9 : 14
            anchors.rightMargin: root.narrow ? 9 : 14
            spacing: root.narrow ? 6 : 8
            AssetIcon {
                Layout.preferredWidth: root.narrow ? 15 : 17
                Layout.preferredHeight: root.narrow ? 15 : 17
                source: iconSource
                active: true
            }
            Text {
                Layout.fillWidth: true
                text: pill.text
                color: "#111111"
                font.pixelSize: root.narrow ? 11 : 12
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
