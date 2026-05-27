import QtQuick
import QtQuick.Controls
import SkyGrid 1.0

Button {
    id: root

    property string iconText: ""
    property url iconSource: ""
    property bool active: false
    property bool iconOnAccent: false
    property color accent: Theme.purple
    readonly property bool hasAssetIcon: iconSource.toString().length > 0

    implicitWidth: 48
    implicitHeight: 48
    hoverEnabled: true
    text: iconText
    font.pixelSize: 23
    font.bold: true

    contentItem: Item {
        AssetIcon {
            anchors.centerIn: parent
            width: 25
            height: 25
            source: root.iconSource
            active: root.active
            hovered: root.hovered
            visible: root.hasAssetIcon
        }
        Text {
            anchors.fill: parent
            visible: !root.hasAssetIcon
            text: root.text
            color: root.active && root.iconOnAccent ? Theme.white : (root.active ? Theme.purple : Theme.ink)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: root.font.pixelSize
            font.bold: root.font.bold
        }
    }

    background: Rectangle {
        radius: 5
        color: root.active ? (root.iconOnAccent ? root.accent : "#320d6b") : (root.hovered ? "#100226" : "#432a6b")
        border.color: root.active && !root.iconOnAccent ? root.accent : "#320d6b"
        border.width: root.active ? 2 : 1
        scale: root.hovered ? 1.03 : 1
        Behavior on scale { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
        Behavior on color { ColorAnimation { duration: Theme.animFast } }
        Behavior on border.color { ColorAnimation { duration: Theme.animFast } }
    }
}
