import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0

ColumnLayout {
    id: root
    property string label: ""
    property string value: ""
    property string unit: ""
    spacing: 6

    Text {
        Layout.alignment: Qt.AlignHCenter
        text: root.label
        color: Theme.ink
        opacity: 0.88
        font.pixelSize: 11
        horizontalAlignment: Text.AlignHCenter
    }
    Row {
        Layout.alignment: Qt.AlignHCenter
        spacing: 2
        Text { id: valueText; text: root.value; color: "#07040b"; font.pixelSize: 18; font.bold: true }
        Text { text: root.unit; color: "#07040b"; font.pixelSize: 11; font.bold: true; anchors.verticalCenter: valueText.verticalCenter }
    }
}
