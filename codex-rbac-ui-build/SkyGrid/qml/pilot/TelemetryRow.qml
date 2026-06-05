import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    property url iconSource: ""
    property string iconText: ""
    property string label: ""
    property string value: ""
    property string subValue: ""
    property color valueColor: "#111111"
    Layout.fillWidth: true
    Layout.preferredHeight: subValue.length > 0 ? 50 : 42
    color: "transparent"

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 12
        Item {
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            AssetIcon {
                anchors.centerIn: parent
                width: 16
                height: 16
                source: root.iconSource
                active: true
                visible: root.iconSource.toString().length > 0
            }
            Text {
                anchors.centerIn: parent
                visible: root.iconSource.toString().length === 0
                text: root.iconText
                color: "#5b22a8"
                font.pixelSize: 14
                font.bold: true
            }
        }
        Text {
            Layout.fillWidth: true
            text: root.label
            color: "#111111"
            font.pixelSize: 11
            elide: Text.ElideRight
        }
        ColumnLayout {
            spacing: 1
            Text {
                Layout.alignment: Qt.AlignRight
                text: root.value
                color: root.valueColor
                font.pixelSize: 11
                font.bold: true
            }
            Text {
                visible: root.subValue.length > 0
                Layout.alignment: Qt.AlignRight
                text: root.subValue
                color: "#9b94a6"
                font.pixelSize: 10
            }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        height: 1
        color: "#f0eaf5"
    }
}
