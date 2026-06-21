import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    objectName: "telemetryBar"
    color: "#f5f1fa"
    border.color: "#e2d8ed"
    border.width: 0
    readonly property bool compact: width < 1440
    readonly property bool narrow: width < 1180
    readonly property bool micro: width < 1060
    readonly property bool showDetailedTelemetry: !root.narrow
    readonly property bool showProfile: !root.compact

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: root.compact ? 14 : 24
        anchors.rightMargin: root.compact ? 14 : 24
        spacing: root.compact ? 7 : 9

        Item {
            Layout.preferredWidth: root.micro ? 96 : (root.narrow ? 128 : (root.compact ? 176 : 238))
            Layout.fillHeight: true
            Image {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                width: root.micro ? 88 : (root.narrow ? 120 : (root.compact ? 168 : 226))
                height: root.narrow ? 30 : 44
                source: AssetRegistry.logos.full_logo
                sourceSize.width: 552
                sourceSize.height: 104
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
            }
            MouseArea { anchors.fill: parent; onClicked: appState.goHome() }
        }

        Item { Layout.fillWidth: true }

        StatusPill {
            value: (operatorStateManager.activeAircraftName && operatorStateManager.activeAircraftName !== "No aircraft selected"
                    ? operatorStateManager.activeAircraftName
                    : telemetryStore.aircraftId) + "  AIR"
            iconSource: AssetRegistry.icons.plane
            implicitWidth: root.narrow ? 128 : (root.compact ? 150 : 190)
        }
        StatusPill { value: telemetryStore.connected ? telemetryStore.battery + "%" : "--%"; iconSource: AssetRegistry.icons.boxicons_battery_3; accent: telemetryStore.battery > 35 ? Theme.green : Theme.amber; implicitWidth: root.narrow ? 76 : (root.compact ? 88 : 104) }
        StatusPill { value: telemetryStore.connected ? telemetryStore.satellites + " SAT" : "NO GPS"; iconSource: AssetRegistry.icons.lucide_satellite; accent: telemetryStore.gpsQuality === "High" ? Theme.green : Theme.amber; implicitWidth: root.narrow ? 92 : (root.compact ? 104 : 124) }
        StatusPill { value: telemetryStore.connected ? telemetryStore.transmission + "%" : "OFFLINE"; iconSource: AssetRegistry.icons.boxicons_wifi; accent: telemetryStore.transmission > 90 ? Theme.green : Theme.amber; implicitWidth: root.narrow ? 92 : (root.compact ? 104 : 124) }
        StatusPill { visible: root.showDetailedTelemetry; value: telemetryStore.recordingState; iconSource: AssetRegistry.icons.boxicons_camera; accent: telemetryStore.recording ? Theme.red : Theme.ink; implicitWidth: root.compact ? 96 : 120 }
        StatusPill { visible: root.showDetailedTelemetry; value: telemetryStore.connected ? Number(telemetryStore.speed).toFixed(1) + " m/s" : "-- m/s"; iconSource: AssetRegistry.icons.boxicons_wind_filled; accent: Theme.ink; implicitWidth: root.compact ? 98 : 124 }
        StatusPill { visible: root.showDetailedTelemetry; value: telemetryStore.connected ? Number(telemetryStore.altitude).toFixed(0) + " m" : "-- m"; iconSource: AssetRegistry.icons.lucide_mountain; accent: Theme.ink; implicitWidth: root.compact ? 86 : 108 }
        StatusPill { visible: root.showDetailedTelemetry; value: telemetryStore.connected ? (telemetryStore.armed ? "ARMED" : "IDLE") : "NO AIR"; iconSource: AssetRegistry.icons.boxicons_cursor_pointer; accent: telemetryStore.armed ? Theme.purple : Theme.ink; implicitWidth: root.compact ? 86 : 108 }
        StatusPill {
            objectName: "backendLiveSyncStatus"
            visible: root.showProfile
            value: webSocketClient.liveStreamSummary + (telemetrySyncManager.pendingQueueCount > 0 ? " / Q " + telemetrySyncManager.pendingQueueCount : "")
            iconSource: AssetRegistry.icons.boxicons_wifi
            accent: webSocketClient.liveStreamCount >= 4 ? Theme.green : (telemetrySyncManager.pendingQueueCount > 0 ? Theme.amber : Theme.ink)
            implicitWidth: 158
        }

        Rectangle {
            visible: root.showProfile
            Layout.preferredWidth: 42
            Layout.preferredHeight: 42
            radius: 21
            color: Theme.purple
            Image {
                anchors.fill: parent
                anchors.margins: 2
                visible: profileManager.avatarUrl.length > 0
                source: profileManager.avatarUrl
                fillMode: Image.PreserveAspectCrop
                smooth: true
                mipmap: true
            }
            AssetIcon {
                anchors.centerIn: parent
                width: 24
                height: 24
                source: AssetRegistry.icons.mdi_person_outline
                visible: profileManager.avatarUrl.length === 0
                active: true
            }
        }
        Text {
            visible: root.showProfile
            text: profileManager.displayName + (profileManager.organizationName.length > 0 ? "\n" + profileManager.organizationName : "")
            color: Theme.ink
            font.pixelSize: 12
            font.bold: true
            lineHeight: 0.88
            Layout.preferredWidth: 132
            elide: Text.ElideRight
        }

        Button {
            id: helpButton
            Layout.preferredWidth: 40
            Layout.preferredHeight: 36
            text: "?"
            hoverEnabled: true
            ToolTip.text: "Open Help Center"
            ToolTip.visible: hovered
            onClicked: appState.openHelpCenter()
            contentItem: Text {
                text: "?"
                color: Theme.purple
                font.pixelSize: 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 18
                color: helpButton.hovered ? "#eadff4" : "#ffffff"
                border.color: "#d9cce8"
            }
        }

        Rectangle {
            visible: !root.compact
            Layout.preferredWidth: 72
            Layout.preferredHeight: 36
            radius: 6
            color: logoutArea.containsMouse ? "#eadff4" : "#ffffff"
            border.color: "#d9cce8"
            border.width: 1
            Text {
                anchors.centerIn: parent
                text: "Logout"
                color: Theme.purple
                font.pixelSize: 12
                font.bold: true
            }
            MouseArea {
                id: logoutArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: authManager.logout()
            }
            Behavior on color { ColorAnimation { duration: Theme.animFast } }
        }

        Button {
            id: moreButton
            visible: root.compact || root.narrow
            Layout.preferredWidth: root.micro ? 46 : 54
            Layout.preferredHeight: 34
            hoverEnabled: true
            contentItem: Text {
                text: root.micro ? "..." : "More"
                color: Theme.purple
                font.pixelSize: 12
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 6
                color: moreButton.hovered ? "#eadff4" : "#ffffff"
                border.color: "#d9cce8"
            }
            onClicked: overflowPopup.open()

            Popup {
                id: overflowPopup
                y: moreButton.height + 8
                x: -Math.max(0, width - moreButton.width)
                width: 280
                padding: 12
                modal: false
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
                background: Rectangle { radius: 8; color: "#ffffff"; border.color: "#d9cce8" }
                ColumnLayout {
                    width: parent.width
                    spacing: 8
                    StatusPill {
                        visible: !root.showDetailedTelemetry
                        Layout.fillWidth: true
                        value: telemetryStore.recordingState
                        iconSource: AssetRegistry.icons.boxicons_camera
                        accent: telemetryStore.recording ? Theme.red : Theme.ink
                    }
                    StatusPill {
                        visible: !root.showDetailedTelemetry
                        Layout.fillWidth: true
                        value: telemetryStore.connected ? Number(telemetryStore.speed).toFixed(1) + " m/s" : "-- m/s"
                        iconSource: AssetRegistry.icons.boxicons_wind_filled
                        accent: Theme.ink
                    }
                    StatusPill {
                        visible: !root.showDetailedTelemetry
                        Layout.fillWidth: true
                        value: telemetryStore.connected ? Number(telemetryStore.altitude).toFixed(0) + " m" : "-- m"
                        iconSource: AssetRegistry.icons.lucide_mountain
                        accent: Theme.ink
                    }
                    StatusPill {
                        visible: !root.showDetailedTelemetry
                        Layout.fillWidth: true
                        value: telemetryStore.connected ? (telemetryStore.armed ? "ARMED" : "IDLE") : "NO AIR"
                        iconSource: AssetRegistry.icons.boxicons_cursor_pointer
                        accent: telemetryStore.armed ? Theme.purple : Theme.ink
                    }
                    StatusPill {
                        Layout.fillWidth: true
                        value: webSocketClient.liveStreamSummary + (telemetrySyncManager.pendingQueueCount > 0 ? " / Q " + telemetrySyncManager.pendingQueueCount : "")
                        iconSource: AssetRegistry.icons.boxicons_wifi
                        accent: webSocketClient.liveStreamCount >= 4 ? Theme.green : (telemetrySyncManager.pendingQueueCount > 0 ? Theme.amber : Theme.ink)
                    }
                    Text {
                        Layout.fillWidth: true
                        text: profileManager.displayName + (profileManager.organizationName.length > 0 ? "\n" + profileManager.organizationName : "")
                        color: Theme.ink
                        font.pixelSize: 12
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }
                    Button {
                        Layout.fillWidth: true
                        text: "Help Center"
                        onClicked: {
                            overflowPopup.close()
                            appState.openHelpCenter()
                        }
                    }
                    Button {
                        Layout.fillWidth: true
                        text: "Logout"
                        onClicked: {
                            overflowPopup.close()
                            authManager.logout()
                        }
                    }
                }
            }
        }
    }
}
