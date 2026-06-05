import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    radius: 7
    color: "#ffffff"
    border.color: "#e2d8ee"
    implicitHeight: 118

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        Rectangle {
            Layout.preferredWidth: 82
            Layout.preferredHeight: 82
            radius: 41
            color: "#fbf9fe"
            border.color: windTelemetryManager.available ? "#5b22a8" : "#d8cce5"
            Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.topMargin: 6; text: "N"; color: "#3b294d"; font.pixelSize: 10; font.bold: true }
            Text { anchors.verticalCenter: parent.verticalCenter; anchors.left: parent.left; anchors.leftMargin: 8; text: "W"; color: "#3b294d"; font.pixelSize: 10; font.bold: true }
            Text { anchors.verticalCenter: parent.verticalCenter; anchors.right: parent.right; anchors.rightMargin: 8; text: "E"; color: "#3b294d"; font.pixelSize: 10; font.bold: true }
            Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 6; text: "S"; color: "#3b294d"; font.pixelSize: 10; font.bold: true }
            Text {
                anchors.centerIn: parent
                text: "➤"
                color: windTelemetryManager.available ? "#5b22a8" : "#b6adbf"
                font.pixelSize: 28
                font.bold: true
                rotation: windTelemetryManager.available ? windTelemetryManager.windDirection - 90 : -45
                Behavior on rotation { NumberAnimation { duration: 220; easing.type: Easing.OutCubic } }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            RowLayout {
                Layout.fillWidth: true
                AssetIcon { Layout.preferredWidth: 15; Layout.preferredHeight: 15; source: AssetRegistry.icons.boxicons_wind_filled; active: true }
                Text { text: "WIND"; color: "#3b294d"; font.pixelSize: 11; font.bold: true; Layout.fillWidth: true }
                Text { text: windTelemetryManager.available ? windTelemetryManager.directionLabel : "--"; color: windTelemetryManager.available ? "#5b22a8" : "#9b94a6"; font.pixelSize: 12; font.bold: true }
            }
            Text {
                text: windTelemetryManager.available
                      ? Number(windTelemetryManager.windSpeed).toFixed(1) + " m/s"
                      : "No live wind source"
                color: windTelemetryManager.available ? "#111111" : "#9b94a6"
                font.pixelSize: 20
                font.bold: true
            }
            Text {
                text: windTelemetryManager.available
                      ? "Gust " + Number(windTelemetryManager.gust).toFixed(1) + " m/s · " + Math.round(windTelemetryManager.windDirection) + "° · " + windCheckManager.result
                      : windCheckManager.status
                color: "#6d6377"
                font.pixelSize: 10
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }
    }
}
