import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    property var options: ["Basic", "Advanced"]
    property int currentIndex: 0
    signal selected(int index)

    height: 40
    radius: 6
    color: "#eeeaf2"

    WheelHandler {
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        onWheel: function(event) { event.accepted = true }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 4
        Repeater {
            model: root.options
            delegate: Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 4
                color: index === root.currentIndex ? Theme.purple : "transparent"
                Text {
                    anchors.centerIn: parent
                    text: modelData
                    color: index === root.currentIndex ? Theme.white : Theme.ink
                    font.pixelSize: 13
                    font.bold: true
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onWheel: function(wheel) { wheel.accepted = true }
                    onClicked: {
                        root.currentIndex = index
                        root.selected(index)
                    }
                }
                Behavior on color { ColorAnimation { duration: Theme.animFast } }
            }
        }
    }
}
