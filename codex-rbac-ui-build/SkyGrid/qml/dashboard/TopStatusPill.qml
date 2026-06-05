import QtQuick 2.15
import QtQuick.Layouts 1.15
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root

    property string iconText: ""
    property url iconSource: ""
    property string value: "--"
    property string label: ""
    property color accent: "#4B3DA0"

    signal clicked()

    width: 146
    height: 48
    radius: 8
    color: mouse.containsMouse ? "#fbfaff" : "#ffffff"
    border.color: mouse.containsMouse ? "#cfc4eb" : "#e7e2ef"
    border.width: 1

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 11
        anchors.rightMargin: 10
        spacing: 8

        Rectangle {
            Layout.preferredWidth: 28
            Layout.preferredHeight: 28
            radius: 8
            color: Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.10)
            border.color: Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.20)

            AssetIcon {
                anchors.centerIn: parent
                width: 17
                height: 17
                iconSize: 17
                source: root.iconSource
                active: true
                visible: root.iconSource.toString().length > 0
            }

            Text {
                anchors.centerIn: parent
                visible: root.iconSource.toString().length === 0
                text: root.iconText
                color: root.accent
                font.pixelSize: 9
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }
        }

        /*Text {
            text: root.iconText
            color: root.accent
            font.pixelSize: 18
            font.bold: true
            Layout.preferredWidth: 22
            horizontalAlignment: Text.AlignHCenter
        }*/

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1

            Text {
                Layout.fillWidth: true
                text: root.value
                color: "#14111d"
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
            }

            Text {
                Layout.fillWidth: true
                text: root.label
                color: "#706a7e"
                font.pixelSize: 9
                elide: Text.ElideRight
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
