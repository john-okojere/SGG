import QtQuick
import QtQuick.Layouts

Item {
    id: root
    property bool hudVisible: false
    property real targetLatitude: 0
    property real targetLongitude: 0
    property bool targetVisible: false

    RowLayout {
        anchors.fill: parent
        spacing: 8
        ManualFlightMap {
            Layout.fillWidth: true
            Layout.fillHeight: true
            targetLatitude: root.targetLatitude
            targetLongitude: root.targetLongitude
            targetVisible: root.targetVisible
            hudVisible: root.hudVisible
        }
        CameraFpvPanel {
            Layout.preferredWidth: Math.max(320, root.width * 0.34)
            Layout.fillHeight: true
            Layout.margins: 10
        }
    }

}
