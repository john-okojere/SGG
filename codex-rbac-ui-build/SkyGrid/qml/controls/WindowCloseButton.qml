import QtQuick
import QtQuick.Controls
import SkyGrid 1.0

Button {
    id: root
    objectName: "windowCloseButton"

    implicitWidth: 42
    implicitHeight: 42
    hoverEnabled: true
    focusPolicy: Qt.NoFocus
    onClicked: Qt.quit()

    background: Rectangle {
        radius: 21
        color: root.hovered ? "#eadff4" : "#ffffff"
        border.color: root.hovered ? Theme.purple : "#d9cce8"
        border.width: 1
        Behavior on color { ColorAnimation { duration: Theme.animFast } }
        Behavior on border.color { ColorAnimation { duration: Theme.animFast } }
    }

    contentItem: Text {
        text: "X"
        color: Theme.purple
        font.pixelSize: 14
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
