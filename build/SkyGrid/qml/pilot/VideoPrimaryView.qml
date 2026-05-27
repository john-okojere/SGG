import QtQuick
import QtQuick.Layouts

Item {
    id: root
    property bool hudVisible: false
    property real targetLatitude: 0
    property real targetLongitude: 0
    property bool targetVisible: false

    CameraFpvPanel {
        anchors.fill: parent
        anchors.margins: 14
    }

    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 24
        width: Math.min(360, parent.width * 0.30)
        height: Math.min(240, parent.height * 0.28)
        radius: 8
        color: "#ffffff"
        border.color: "#e2d8ee"
        clip: true
        ManualFlightMap {
            anchors.fill: parent
            targetLatitude: root.targetLatitude
            targetLongitude: root.targetLongitude
            targetVisible: root.targetVisible
            hudVisible: root.hudVisible
        }
    }
}
