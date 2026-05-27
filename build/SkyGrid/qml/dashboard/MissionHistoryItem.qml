import QtQuick 2.15
import QtQuick.Layouts 1.15
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root

    property string missionName: "Untitled Mission"
    property string category: "Surveillance"
    property string timestamp: "May 20, 2026 - 08:32"
    property string status: "Completed"
    property url iconSource: ""
    property color badgeColor: "#efe2ff"
    property color badgeTextColor: "#5b22a8"

    signal clicked()

    height: 58
    radius: 8
    color: mouse.containsMouse ? "#fbfaff" : "#ffffff"
    border.color: mouse.containsMouse ? "#cfc4eb" : "#e8e3ef"
    border.width: 1

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 9

        Rectangle {
            Layout.preferredWidth: 34
            Layout.preferredHeight: 34
            radius: 17
            color: root.badgeColor

            AssetIcon {
                anchors.centerIn: parent
                width: 19
                height: 19
                iconSize: 19
                source: root.iconSource
                active: true
                inactiveOpacity: 1
                visible: root.iconSource.toString().length > 0
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 3

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                Layout.fillWidth: true
                text: root.missionName
                color: "#171222"
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
            }

            Rectangle {
                Layout.preferredHeight: 18
                Layout.preferredWidth: Math.max(48, badgeText.implicitWidth + 12)
                radius: 9
                color: root.badgeColor
                Text {
                    id: badgeText
                    anchors.centerIn: parent
                    text: root.category
                    color: root.badgeTextColor
                    font.pixelSize: 8
                    font.bold: true
                    elide: Text.ElideRight
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Text {
                Layout.fillWidth: true
                text: root.timestamp
                color: "#746d82"
                font.pixelSize: 10
                elide: Text.ElideRight
            }
            Text {
                text: root.status
                color: "#211a30"
                font.pixelSize: 9
                font.bold: true
            }
            Rectangle {
                width: 6
                height: 6
                radius: 3
                color: root.status.toLowerCase().indexOf("complete") >= 0 ? "#28b947" : "#f4b000"
            }
        }
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
