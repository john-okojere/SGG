import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0

ColumnLayout {
    id: root
    property string title: ""
    property bool expanded: true
    default property alias content: contentColumn.data

    spacing: 10

    Rectangle {
        Layout.fillWidth: true
        height: 42
        color: "transparent"
        Text {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: root.title
            color: Theme.ink
            font.pixelSize: 19
            font.bold: true
        }
        Text {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: root.expanded ? "⌃" : "⌄"
            color: Theme.ink
            font.pixelSize: 22
        }
        MouseArea { anchors.fill: parent; onClicked: root.expanded = !root.expanded }
    }

    ColumnLayout {
        id: contentColumn
        Layout.fillWidth: true
        visible: root.expanded
        opacity: root.expanded ? 1 : 0
        spacing: 8
        Behavior on opacity { NumberAnimation { duration: Theme.animFast } }
    }
}
