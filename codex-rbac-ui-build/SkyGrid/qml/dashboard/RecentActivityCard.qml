import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property var activities: []

    signal viewAllRequested()

    radius: 10
    color: "#ffffff"
    border.color: "#e2dceb"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 12

        Text {
            text: "Recent Activity"
            color: "#14111d"
            font.pixelSize: 16
            font.bold: true
        }

        Repeater {
            model: root.activities.slice(0, 5)
            delegate: RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 26
                spacing: 10

                Rectangle {
                    Layout.preferredWidth: 7
                    Layout.preferredHeight: 7
                    radius: 4
                    color: modelData.color || "#4B3DA0"
                }

                Text {
                    Layout.fillWidth: true
                    text: modelData.message
                    color: "#5f586d"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }

                Text {
                    text: modelData.time
                    color: "#8a8494"
                    font.pixelSize: 10
                    elide: Text.ElideRight
                }
            }
        }

        Item { Layout.fillHeight: true }

        Text {
            text: "View All Activity"
            color: activityMouse.containsMouse ? "#4f0aa6" : "#3b0787"
            font.pixelSize: 12
            font.bold: true
            MouseArea {
                id: activityMouse
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.viewAllRequested()
            }
        }
    }
}
