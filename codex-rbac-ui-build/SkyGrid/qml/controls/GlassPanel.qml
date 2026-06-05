import QtQuick
import SkyGrid 1.0

Rectangle {
    id: root
    radius: Theme.panelRadius
    color: Theme.glass
    border.color: "#00000000"
    border.width: 0
    // Avoid forcing every glass panel through an offscreen FBO. On some
    // OpenGL/VM stacks that can black out the whole Qt Quick scene.
    layer.enabled: false

    Behavior on opacity { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
    Behavior on color { ColorAnimation { duration: Theme.animFast } }
}
