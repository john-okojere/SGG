import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

RowLayout {
    id: root
    property string label: ""
    property bool checked: true
    signal toggled(bool checked)
    height: 36

    Text {
        Layout.fillWidth: true
        text: root.label
        color: Theme.ink
        font.pixelSize: 14
    }
    Switch {
        checked: root.checked
        onToggled: root.toggled(checked)
    }
}
