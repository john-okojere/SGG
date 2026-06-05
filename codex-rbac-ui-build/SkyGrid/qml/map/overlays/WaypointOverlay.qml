import QtQuick
import SkyGrid 1.0

Rectangle {
    id: root
    property string indexText: "1"
    property bool selected: false
    property bool hovered: false
    width: 30
    height: 30
    radius: 15
    color: selected ? Theme.amber : (hovered ? Theme.purple2 : Theme.purple)
    border.color: hovered || selected ? "#ffffff" : "#ffffffcc"
    border.width: selected ? 4 : 3
    Text { anchors.centerIn: parent; text: root.indexText; color: Theme.white; font.pixelSize: 12; font.bold: true }
    scale: selected ? 1.24 : (hovered ? 1.12 : 1)
    Behavior on scale { NumberAnimation { duration: Theme.animFast } }
    Behavior on color { ColorAnimation { duration: Theme.animFast } }

    Rectangle {
        anchors.centerIn: parent
        width: parent.width + 16
        height: width
        radius: width / 2
        color: "transparent"
        border.color: selected ? "#f7c94888" : "transparent"
        border.width: 2
        opacity: selected ? 1 : 0
        SequentialAnimation on scale {
            running: root.selected
            loops: Animation.Infinite
            NumberAnimation { from: 1; to: 1.35; duration: 900; easing.type: Easing.OutCubic }
            NumberAnimation { from: 1.35; to: 1; duration: 900; easing.type: Easing.InCubic }
        }
    }
}
