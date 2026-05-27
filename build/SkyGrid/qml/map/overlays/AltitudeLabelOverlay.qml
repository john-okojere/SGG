import QtQuick
import SkyGrid 1.0

Item {
    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 170
        width: label.implicitWidth + 24
        height: 34
        radius: 6
        color: "#160720d9"
        border.color: "#ffffff30"
        Text {
            id: label
            anchors.centerIn: parent
            text: "ALT " + Number(missionStore.plan.altitude).toFixed(1) + " ft · " + missionStore.plan.estimatedTime + " · " + Number(missionStore.plan.estimatedBattery).toFixed(0) + "% batt"
            color: Theme.white
            font.pixelSize: 14
            font.bold: true
        }
    }
}
