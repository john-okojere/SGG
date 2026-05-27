import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    radius: 7
    color: "#ffffff"
    border.color: "#e2d8ee"
    clip: true

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: root.width
            spacing: 10

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 42
                Layout.leftMargin: 14
                Layout.rightMargin: 14
                Text {
                    text: "TELEMETRY"
                    color: "#3b294d"
                    font.pixelSize: 11
                    font.bold: true
                    Layout.fillWidth: true
                }
                Text { text: "⌃"; color: "#111111"; font.pixelSize: 13; font.bold: true }
            }

            TelemetryGrid {
                Layout.fillWidth: true
                Layout.leftMargin: 12
                Layout.rightMargin: 12
                Layout.bottomMargin: 12
            }
        }
    }
}
