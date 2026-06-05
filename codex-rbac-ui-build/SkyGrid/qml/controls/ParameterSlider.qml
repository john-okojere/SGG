import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

RowLayout {
    id: root
    property string label: ""
    property string unit: ""
    property real from: 0
    property real to: 100
    property real value: 0
    property int decimals: 0
    signal valueEdited(real value)

    spacing: 8
    height: 42

    Text {
        Layout.preferredWidth: 116
        text: root.label
        color: Theme.ink
        font.pixelSize: 14
    }

    Slider {
        id: slider
        Layout.fillWidth: true
        from: root.from
        to: root.to
        value: root.value
        live: true
        onMoved: root.valueEdited(value)
        background: Rectangle {
            x: slider.leftPadding
            y: slider.topPadding + slider.availableHeight / 2 - 2
            width: slider.availableWidth
            height: 4
            radius: 2
            color: "#77727d"
            Rectangle {
                width: slider.visualPosition * parent.width
                height: parent.height
                radius: 2
                color: Theme.purple
            }
        }
        handle: Rectangle {
            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            width: 14
            height: 14
            radius: 7
            color: "#f8f6fb"
            border.color: Theme.ink
            border.width: 1
        }
    }

    Rectangle {
        Layout.preferredWidth: 78
        Layout.preferredHeight: 32
        radius: Theme.controlRadius
        color: "#f5f1fa"
        border.color: Theme.line
        Row {
            anchors.centerIn: parent
            spacing: 8
            Text { text: Number(root.value).toFixed(root.decimals); color: Theme.ink; font.pixelSize: 13 }
            Text { text: root.unit; color: Theme.ink; font.pixelSize: 13 }
        }
    }
}
