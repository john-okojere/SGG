import QtQuick

Item {
    id: root
    property bool hudVisible: false
    property real targetLatitude: 0
    property real targetLongitude: 0
    property bool targetVisible: false

    ManualFlightMap {
        id: map
        anchors.fill: parent
        targetLatitude: root.targetLatitude
        targetLongitude: root.targetLongitude
        targetVisible: root.targetVisible
        hudVisible: root.hudVisible
    }
}
