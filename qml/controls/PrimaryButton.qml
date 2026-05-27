import QtQuick
import QtQuick.Controls
import SkyGrid 1.0

Button {
    id: root
    property bool secondary: false
    property string iconText: ""
    property url iconSource: ""
    property color foreground: Theme.white
    readonly property string iconSourceString: iconSource.toString()
    readonly property string resolvedIconText: iconText.length > 0
        ? iconText
        : (iconSourceString.indexOf("plus.") !== -1
            ? "+"
            : (iconSourceString.indexOf("boxicons_save.") !== -1
                ? "▣"
                : (iconSourceString.indexOf("boxicons_play.") !== -1 ? "▶" : "")))
    readonly property bool hasAssetIcon: iconSource.toString().length > 0

    implicitHeight: 38
    hoverEnabled: true
    font.pixelSize: 14
    font.bold: true

    contentItem: Row {
        spacing: 8
        anchors.centerIn: parent
        Rectangle {
            visible: root.hasAssetIcon || root.resolvedIconText.length > 0
            width: 20
            height: 20
            radius: 5
            color: "#00000000"
            AssetIcon {
                anchors.centerIn: parent
                width: 16
                height: 16
                source: root.iconSource
                active: true
                hovered: root.hovered
                visible: root.hasAssetIcon && root.resolvedIconText.length === 0
            }
            Text {
                anchors.centerIn: parent
                visible: root.resolvedIconText.length > 0
                text: root.resolvedIconText
                color: Theme.white
                font.pixelSize: root.resolvedIconText === "+" ? 22 : 17
                font.bold: true
            }
        }
        Text {
            visible: false
            text: ""
            color: root.foreground
            font.pixelSize: 14
            font.bold: true
        }
        Text {
            text: root.text
            color: root.foreground
            font.pixelSize: root.font.pixelSize
            font.bold: root.font.bold
        }
    }

    background: Rectangle {
        radius: Theme.controlRadius
        color: root.hovered ? Theme.purple2 : Theme.purple
        border.color: "#00000000"
        border.width: 0
        scale: root.hovered ? 1.01 : 1
        Behavior on color { ColorAnimation { duration: Theme.animFast } }
        Behavior on scale { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
        Behavior on border.color { ColorAnimation { duration: Theme.animFast } }
    }
}
