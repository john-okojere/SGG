import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property string mode: "MAP"
    signal modeSelected(string mode)
    radius: 7
    color: "#ffffffee"
    border.color: "#e2d8ee"
    implicitWidth: 246
    implicitHeight: 38

    WheelHandler {
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        onWheel: function(event) { event.accepted = true }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 3
        Repeater {
            model: ["MAP", "VIDEO", "SPLIT", "HUD"]
            Button {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: modelData
                hoverEnabled: true
                WheelHandler {
                    acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
                    onWheel: function(event) { event.accepted = true }
                }
                onClicked: root.modeSelected(modelData)
                background: Rectangle {
                    radius: 4
                    color: root.mode === modelData ? "#5b22a8" : (parent.hovered ? "#f3eef8" : "transparent")
                }
                contentItem: Text {
                    text: parent.text
                    color: root.mode === modelData ? "#ffffff" : "#2e005f"
                    font.pixelSize: 10
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
