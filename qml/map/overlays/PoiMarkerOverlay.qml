import QtQuick
import SkyGrid 1.0
import "../../controls"

Rectangle {
    id: root
    property bool hovered: false
    width: 44
    height: 44
    radius: 22
    color: hovered ? Theme.purple2 : Theme.purple
    border.color: Theme.white
    border.width: 3
    scale: hovered ? 1.12 : 1
    AssetIcon {
        anchors.centerIn: parent
        width: 24
        height: 24
        source: AssetRegistry.icons.pin_location
        active: true
    }
    Behavior on scale { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
    Behavior on color { ColorAnimation { duration: Theme.animFast } }
}
