import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property string status: "GOOD_TO_FLY"
    property string temperature: "28 C"
    property string condition: "Partly Cloudy"
    property string location: "Lagos, Nigeria"
    property string wind: "8 km/h NE"
    property string visibility: "10 km"
    property string precipitation: "10%"
    property string gusts: "15 km/h"
    property string forecastWindow: "Good for next 6 hours"
    property bool syncing: false

    signal refreshRequested()

    readonly property color statusColor: status === "DO_NOT_FLY" ? "#ef4444" : (status === "CAUTION" ? "#f4b000" : "#28b947")
    readonly property string statusText: status === "DO_NOT_FLY" ? "Do Not Fly" : (status === "CAUTION" ? "Caution" : (status === "SOURCE_UNAVAILABLE" ? "Unavailable" : "Good to Fly"))

    radius: 10
    color: "#ffffff"
    border.color: "#e2dceb"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Text {
                Layout.fillWidth: true
                text: "Weather & UAV Forecast"
                color: "#14111d"
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
            }
            Rectangle {
                Layout.preferredWidth: Math.max(86, badge.implicitWidth + 20)
                Layout.preferredHeight: 24
                radius: 12
                color: Qt.rgba(root.statusColor.r, root.statusColor.g, root.statusColor.b, 0.12)
                border.color: root.statusColor
                Text {
                    id: badge
                    anchors.centerIn: parent
                    text: root.statusText
                    color: root.statusColor
                    font.pixelSize: 11
                    font.bold: true
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 14

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text { text: root.temperature; color: "#0e0b14"; font.pixelSize: 31; font.bold: true }
                Text { Layout.fillWidth: true; text: root.condition; color: "#5f586d"; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
                Text { Layout.fillWidth: true; text: root.location; color: "#7e778a"; font.pixelSize: 11; elide: Text.ElideRight }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 7
                MetricRow { label: "Wind"; value: root.wind }
                MetricRow { label: "Visibility"; value: root.visibility }
                MetricRow { label: "Precipitation"; value: root.precipitation }
                MetricRow { label: "Gusts"; value: root.gusts }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#e9e4ef"
        }

        RowLayout {
            Layout.fillWidth: true
            Text { text: "UAV Forecast Window"; color: "#6f687c"; font.pixelSize: 11; Layout.fillWidth: true }
            Text { text: root.syncing ? "Refreshing..." : root.forecastWindow; color: root.statusColor; font.pixelSize: 11; font.bold: true; elide: Text.ElideRight }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            radius: 7
            color: refreshMouse.containsMouse ? "#f6f2fb" : "#ffffff"
            border.color: "#cfc4eb"
            Text { anchors.centerIn: parent; text: "Refresh Forecast"; color: "#3b0787"; font.pixelSize: 11; font.bold: true }
            MouseArea {
                id: refreshMouse
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.refreshRequested()
            }
        }
    }

    component MetricRow: RowLayout {
        id: metric
        property string label: ""
        property string value: ""
        Layout.fillWidth: true
        Text { Layout.fillWidth: true; text: metric.label; color: "#6f687c"; font.pixelSize: 11 }
        Text { text: metric.value; color: "#332c42"; font.pixelSize: 11; font.bold: true; elide: Text.ElideRight }
    }
}
