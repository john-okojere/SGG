import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root
    property bool hudVisible: false
    spacing: 8

    CompassMini {
        Layout.preferredWidth: 78
        Layout.preferredHeight: 78
    }

    SmallHudWidget {
        Layout.preferredWidth: 78
        Layout.preferredHeight: 78
        visible: root.hudVisible
    }

    component CompassMini: Rectangle {
        radius: width / 2
        color: "#ffffffee"
        border.color: "#e2d8ee"
        Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.topMargin: 8; text: "N"; color: "#111111"; font.pixelSize: 12; font.bold: true }
        Text { anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; anchors.leftMargin: 9; text: "W"; color: "#111111"; font.pixelSize: 12; font.bold: true }
        Text { anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; anchors.rightMargin: 9; text: "E"; color: "#111111"; font.pixelSize: 12; font.bold: true }
        Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 8; text: "S"; color: "#111111"; font.pixelSize: 12; font.bold: true }
        Text { anchors.centerIn: parent; text: "▲"; color: "#ef233c"; font.pixelSize: 26; font.bold: true; rotation: telemetryStore.heading }
    }
}
