import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0

RowLayout {
    id: root
    property string label: ""
    property string value: ""
    property var options: []
    property bool expanded: false
    signal clicked()
    signal selected(string value)
    height: 38
    spacing: 8

    Text {
        Layout.preferredWidth: 116
        text: root.label
        color: Theme.ink
        font.pixelSize: 14
    }
    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 34
        radius: Theme.controlRadius
        color: "#f5f1fa"
        border.color: Theme.line
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.verticalCenter: parent.verticalCenter
            text: root.value
            color: Theme.ink
            font.pixelSize: 14
            elide: Text.ElideRight
            width: parent.width - 46
        }
        Text {
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.verticalCenter: parent.verticalCenter
            text: "›"
            color: Theme.ink
            font.pixelSize: 22
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                root.clicked()
                if (root.options.length > 0) root.expanded = !root.expanded
            }
        }
        Rectangle {
            visible: root.expanded && root.options.length > 0
            z: 20
            anchors.top: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: Math.min(160, root.options.length * 32 + 8)
            radius: Theme.controlRadius
            color: "#ffffff"
            border.color: Theme.line
            Column {
                anchors.fill: parent
                anchors.margins: 4
                Repeater {
                    model: root.options
                    delegate: Rectangle {
                        width: parent.width
                        height: 32
                        radius: 4
                        color: optionArea.containsMouse ? "#eee7f8" : "#00000000"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData
                            color: Theme.ink
                            font.pixelSize: 13
                        }
                        MouseArea {
                            id: optionArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                root.value = modelData
                                root.expanded = false
                                root.selected(modelData)
                            }
                        }
                    }
                }
            }
        }
    }
}
