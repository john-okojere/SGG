import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    property url iconSource: ""
    property string iconText: ""
    property string label: ""
    property string value: "--"
    property string unit: ""
    property string subValue: ""
    property color statusColor: "#5b22a8"

    radius: 6
    color: "#fbf9fe"
    border.color: "#ece3f3"
    border.width: 1
    implicitHeight: 62

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 7

        Rectangle {
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24
            radius: 12
            color: Qt.rgba(root.statusColor.r, root.statusColor.g, root.statusColor.b, 0.10)
            AssetIcon {
                anchors.centerIn: parent
                width: 14
                height: 14
                source: root.iconSource
                active: true
                visible: root.iconSource.toString().length > 0
            }
            Text {
                anchors.centerIn: parent
                text: root.iconText
                color: root.statusColor
                font.pixelSize: 13
                font.bold: true
                visible: root.iconSource.toString().length === 0
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1
            Text {
                Layout.fillWidth: true
                text: root.label
                color: "#6d6377"
                font.pixelSize: 9
                font.bold: true
                elide: Text.ElideRight
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 3
                Text {
                    text: root.value
                    color: "#111111"
                    font.pixelSize: 13
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                Text {
                    text: root.unit
                    color: "#5f5a66"
                    font.pixelSize: 9
                    visible: root.unit.length > 0
                }
            }
            Text {
                Layout.fillWidth: true
                text: root.subValue
                color: root.statusColor
                font.pixelSize: 9
                elide: Text.ElideRight
                visible: root.subValue.length > 0
            }
        }
    }
}
