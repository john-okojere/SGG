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
    readonly property bool compact: width < 1320
    readonly property bool narrow: width < 1040

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: root.compact ? 14 : 24
        anchors.rightMargin: root.compact ? 14 : 24
        spacing: root.compact ? 7 : 9

        Item {
            Layout.preferredWidth: root.narrow ? 142 : (root.compact ? 188 : 250)
            Layout.fillHeight: true
            Image {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                width: root.narrow ? 132 : (root.compact ? 178 : 226)
                height: root.narrow ? 34 : 44
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
            implicitWidth: root.compact ? 150 : 190
        }
        StatusPill { value: telemetryStore.connected ? telemetryStore.battery + "%" : "--%"; iconSource: AssetRegistry.icons.boxicons_battery_3; accent: telemetryStore.battery > 35 ? Theme.green : Theme.amber; implicitWidth: root.compact ? 88 : 104 }
        StatusPill { value: telemetryStore.connected ? telemetryStore.satellites + " SAT" : "NO GPS"; iconSource: AssetRegistry.icons.lucide_satellite; accent: telemetryStore.gpsQuality === "High" ? Theme.green : Theme.amber; implicitWidth: root.compact ? 104 : 124 }
        StatusPill { value: telemetryStore.connected ? telemetryStore.transmission + "%" : "OFFLINE"; iconSource: AssetRegistry.icons.boxicons_wifi; accent: telemetryStore.transmission > 90 ? Theme.green : Theme.amber; implicitWidth: root.compact ? 104 : 124 }
        StatusPill { value: telemetryStore.recordingState; iconSource: AssetRegistry.icons.boxicons_camera; accent: telemetryStore.recording ? Theme.red : Theme.ink; implicitWidth: root.compact ? 102 : 120 }
        StatusPill { value: telemetryStore.connected ? Number(telemetryStore.speed).toFixed(1) + " m/s" : "-- m/s"; iconSource: AssetRegistry.icons.boxicons_wind_filled; accent: Theme.ink; implicitWidth: root.compact ? 104 : 124 }
        StatusPill { value: telemetryStore.connected ? Number(telemetryStore.altitude).toFixed(0) + " m" : "-- m"; iconSource: AssetRegistry.icons.lucide_mountain; accent: Theme.ink; implicitWidth: root.compact ? 92 : 108 }
        StatusPill { value: telemetryStore.connected ? (telemetryStore.armed ? "ARMED" : "IDLE") : "NO AIR"; iconSource: AssetRegistry.icons.boxicons_cursor_pointer; accent: telemetryStore.armed ? Theme.purple : Theme.ink; implicitWidth: root.compact ? 92 : 108 }
        StatusPill {
            objectName: "backendLiveSyncStatus"
            visible: !root.compact
            value: webSocketClient.liveStreamSummary + (telemetrySyncManager.pendingQueueCount > 0 ? " / Q " + telemetrySyncManager.pendingQueueCount : "")
            iconSource: AssetRegistry.icons.boxicons_wifi
            accent: webSocketClient.liveStreamCount >= 4 ? Theme.green : (telemetrySyncManager.pendingQueueCount > 0 ? Theme.amber : Theme.ink)
            implicitWidth: 158
        }

        Rectangle {
            visible: !root.compact
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
            visible: !root.compact
            text: profileManager.displayName + (profileManager.organizationName.length > 0 ? "\n" + profileManager.organizationName : "")
            color: Theme.ink
            font.pixelSize: 12
            font.bold: true
            lineHeight: 0.88
            Layout.preferredWidth: 132
            elide: Text.ElideRight
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
            visible: root.compact
            Layout.preferredWidth: 42
            Layout.preferredHeight: 34
            hoverEnabled: true
            contentItem: Text {
                text: "More"
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
                        text: "Logout"
                        onClicked: {
                            overflowPopup.close()
                            authManager.logout()
                        }
                    }
                }
            }
        }

        WindowCloseButton {
            Layout.preferredWidth: root.compact ? 34 : 38
            Layout.preferredHeight: root.compact ? 34 : 38
        }
    }
}
