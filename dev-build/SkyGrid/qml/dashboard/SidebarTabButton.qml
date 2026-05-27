import QtQuick 2.15
import QtQuick.Layouts 1.15
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root

    property string text: ""
    property string iconText: ""
    property url iconSource: ""
    property bool active: false

    signal clicked()

    height: 46
    radius: 8
    color: root.active ? "#3c0787" : (mouse.containsMouse ? "#f5f1fb" : "#ffffff")
    border.color: root.active ? "#3c0787" : "#e1dceb"
    border.width: 1

    RowLayout {
        anchors.centerIn: parent
        spacing: 7

        AssetIcon {
            width: 17
            height: 17
            iconSize: 17
            source: root.iconSource
            active: root.active
            visible: root.iconSource.toString().length > 0
            inactiveOpacity: 0.9
        }

        Text {
            visible: root.iconSource.toString().length === 0
            text: root.iconText
            color: root.active ? "#ffffff" : "#1b1726"
            font.pixelSize: 12
            font.bold: true
        }

        Text {
            text: root.text
            color: root.active ? "#ffffff" : "#1b1726"
            font.pixelSize: 11
            font.bold: true
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
