import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Item {
    id: item1
    width: 720
    height: 190

    function valueOrNa(value, decimals) {
        return telemetryStore.connected ? Number(value).toFixed(decimals) : "N/A"
    }

    GlassPanel {
        y: 63
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 170
        anchors.bottomMargin: 89
        width: providerText.implicitWidth + 34
        height: 38
        color: mapProvider.online ? "#11351fdc" : "#2f2038dc"
        border.color: mapProvider.online ? "#3fe27b70" : "#f7c94870"
        Text {
            id: providerText
            anchors.centerIn: parent
            text: mapProvider.providerName + " · " + mapProvider.providerStatus
            color: Theme.white
            font.pixelSize: 13
            font.bold: true
        }
    }

    GlassPanel {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 146
        width: 350
        height: 40
        color: "#160720d9"
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 8
            spacing: 8
            Text {
                text: tileCacheManager.offlineMode ? "OFFLINE MAP" : (tileCacheManager.downloading ? "CACHING " + tileCacheManager.downloadedTiles : "MAP CACHE")
                color: Theme.white
                font.pixelSize: 11
                font.bold: true
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
            Rectangle {
                width: 72
                height: 24
                radius: 4
                color: cacheDownloadArea.containsMouse ? Theme.purple2 : Theme.purple
                Text { anchors.centerIn: parent; text: "Download"; color: Theme.white; font.pixelSize: 11; font.bold: true }
                MouseArea {
                    id: cacheDownloadArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        var geometry = missionStore.plan.polygon.length > 0 ? missionStore.plan.polygon : missionStore.plan.waypoints
                        tileCacheManager.downloadMissionArea(geometry, mapProvider.layerMode, mapState.centerLatitude, mapState.centerLongitude, Math.max(3, Math.round(mapState.zoomLevel) - 1), Math.min(18, Math.round(mapState.zoomLevel) + 2))
                    }
                }
            }
            Rectangle {
                width: 54
                height: 24
                radius: 4
                color: tileCacheManager.offlineMode ? Theme.purple2 : "Theme.purple"
                Text { anchors.centerIn: parent; text: "Offline"; color: tileCacheManager.offlineMode ? Theme.ink : Theme.white; font.pixelSize: 11; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: tileCacheManager.offlineMode = !tileCacheManager.offlineMode }
            }
            Rectangle {
                width: 54
                height: 24
                radius: 4
                color: mapState.followAircraft ? Theme.purple2 : Theme.purple
                Text { anchors.centerIn: parent; text: "Follow"; color: Theme.white; font.pixelSize: 11; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: mapState.followAircraft = !mapState.followAircraft }
            }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: 154
        height: 154
        radius: 77
        color: "transparent"
        AssetIcon {
            anchors.centerIn: parent
            width: 154
            height: 154
            iconSize: 154
            source: AssetRegistry.icons.compass_border
            active: true
        }
        Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.topMargin: 13; text: "N"; color: Theme.white; font.pixelSize: 17; font.bold: true }
        AssetIcon {
            anchors.centerIn: parent
            width: 32
            height: 32
            source: AssetRegistry.icons.mingcute_compass_line
            active: true
        }
        Rectangle { anchors.centerIn: parent; width: 118; height: 1; color: "#ffffff80" }
        Rectangle { anchors.centerIn: parent; width: 1; height: 118; color: "#ffffff80" }
    }

    GlassPanel {
        anchors.left: parent.left
        anchors.leftMargin: 170
        anchors.bottom: parent.bottom
        width: 560
        height: 88
        color: Theme.purple
        RowLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 16
            Column {
                Layout.fillWidth: true
                Text { text: "Ground Speed"; color: "#9d7ec4"; font.pixelSize: 13 }
                Text {
                    text: telemetryStore.connected ? (Number(telemetryStore.speed * 2.23694).toFixed(1) + " MPH") : "N/A MPH"
                    color: Theme.white
                    font.pixelSize: 20
                    font.bold: true
                }
            }
            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#ffffff35" }
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                AssetIcon { Layout.preferredWidth: 22; Layout.preferredHeight: 22; source: AssetRegistry.icons.pin_location; active: true }
                Column {
                    Layout.fillWidth: true
                    Text { text: "LAT: " + valueOrNa(telemetryStore.latitude, 5); color: Theme.white; font.pixelSize: 18; font.bold: true }
                    Text { text: "LON: " + valueOrNa(telemetryStore.longitude, 5); color: Theme.white; font.pixelSize: 18; font.bold: true }
                }
            }
            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#ffffff35" }
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                AssetIcon { Layout.preferredWidth: 22; Layout.preferredHeight: 22; source: AssetRegistry.icons.lucide_mountain; active: true }
                Column {
                    Layout.fillWidth: true
                    Text { text: "Altitude"; color: "#9d7ec4"; font.pixelSize: 13 }
                    Text {
                        text: telemetryStore.connected ? (Number(telemetryStore.altitude * 3.28084).toFixed(1) + " FT") : "N/A FT"
                        color: Theme.white
                        font.pixelSize: 20
                        font.bold: true
                    }
                }
            }
        }
    }
}
