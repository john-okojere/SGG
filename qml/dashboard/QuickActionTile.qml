import QtQuick 2.15
import QtQuick.Layouts 1.15
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root

    property string iconText: "+"
    property url iconSource: ""
    property string title: "Action"
    property string subtitle: ""
    property bool primary: false

    signal clicked()

    radius: 8
    color: root.primary ? (mouse.containsMouse ? "#4f0aa6" : "#3b0787") : (mouse.containsMouse ? "#fbfaff" : "#ffffff")
    border.color: root.primary ? "#3b0787" : "#d9cfec"
    border.width: 1

    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width - 24
        spacing: 5

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 38
            Layout.preferredHeight: 38
            radius: 12
            color: root.primary ? "#ffffff22" : "#f2ebfb"
            border.color: root.primary ? "#ffffff35" : "#ded3f0"

            AssetIcon {
                anchors.centerIn: parent
                width: 22
                height: 22
                iconSize: 22
                source: root.iconSource
                active: true
                visible: root.iconSource.toString().length > 0
                inactiveOpacity: 1
            }

            Text {
                anchors.centerIn: parent
                visible: root.iconSource.toString().length === 0
                text: root.iconText
                color: root.primary ? "#ffffff" : "#4B3DA0"
                font.pixelSize: 16
                font.bold: true
            }
        }

        Text {
            Layout.fillWidth: true
            text: root.title
            color: root.primary ? "#ffffff" : "#3b0787"
            font.pixelSize: 12
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }

        Text {
            Layout.fillWidth: true
            text: root.subtitle
            color: root.primary ? "#ebe4ff" : "#6d647d"
            font.pixelSize: 9
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
