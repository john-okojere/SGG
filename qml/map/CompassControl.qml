import QtQuick
import SkyGrid 1.0
import "../controls"

Rectangle {
    width: 112
    height: 112
    radius: 8
    color: "#ffffff"
    AssetIcon {
        anchors.centerIn: parent
        width: 78
        height: 78
        source: AssetRegistry.icons.mingcute_compass_line
        active: true
        opacity: 0.28
    }
    Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.topMargin: 12; text: "N"; font.pixelSize: 18; font.bold: true }
    Text { anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; anchors.leftMargin: 12; text: "W"; font.pixelSize: 18; font.bold: true }
    Text { anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; anchors.rightMargin: 12; text: "E"; font.pixelSize: 18; font.bold: true }
    Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 12; text: "S"; font.pixelSize: 18; font.bold: true }
    Rectangle { anchors.centerIn: parent; width: 1; height: 42; color: Theme.purpleSoft }
}
