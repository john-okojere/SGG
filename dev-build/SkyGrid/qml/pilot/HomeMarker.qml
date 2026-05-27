import QtQuick
import SkyGrid 1.0
import "../controls"

Item {
    id: root
    property var mapItem
    property var path: telemetryStore.livePath
    property bool hasHome: path.length > 0 && mapItem
    property point homePoint: hasHome ? mapItem.pointFor(path[0].latitude, path[0].longitude) : Qt.point(-300, -300)
    visible: hasHome
    x: homePoint.x - width / 2
    y: homePoint.y - height / 2
    width: 54
    height: 58

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        width: 36
        height: 36
        radius: 18
        color: "#5b22a8"
        border.color: "#ffffff"
        border.width: 2
        Text { anchors.centerIn: parent; text: "⌂"; color: "#ffffff"; font.pixelSize: 18; font.bold: true }
    }
    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 32
        width: 42
        height: 18
        radius: 3
        color: "#2e005f"
        Text { anchors.centerIn: parent; text: "HOME"; color: "#ffffff"; font.pixelSize: 10; font.bold: true }
    }

    Behavior on x { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
    Behavior on y { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
}
