import QtQuick
import QtQuick.Controls

Button {
    id: root
    property bool controlsVisible: true
    signal toggled(bool visible)
    implicitWidth: 132
    implicitHeight: 34
    text: controlsVisible ? "Hide Controls" : "Show Controls"
    onClicked: {
        controlsVisible = !controlsVisible
        toggled(controlsVisible)
    }
    background: Rectangle {
        radius: 17
        color: root.hovered ? "#ffffff" : "#ffffffdd"
        border.color: "#5b22a8"
    }
    contentItem: Text {
        text: root.text
        color: "#5b22a8"
        font.pixelSize: 11
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
