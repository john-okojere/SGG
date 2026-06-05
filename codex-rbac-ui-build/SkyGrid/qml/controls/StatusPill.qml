import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    property string label: ""
    property string value: ""
    property string iconText: ""
    property url iconSource: ""
    property color accent: Theme.green
    readonly property bool hasAssetIcon: iconSource.toString().length > 0
    readonly property bool compact: width > 0 && width < 112

    radius: 6
    color: "#f5f1fa"
    border.color: Theme.line
    implicitHeight: 40
    implicitWidth: 180

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: root.compact ? 8 : 10
        anchors.rightMargin: root.compact ? 8 : 10
        spacing: root.compact ? 5 : 7
        Rectangle {
            Layout.preferredWidth: root.compact ? 22 : 24
            Layout.preferredHeight: root.compact ? 22 : 24
            radius: 6
            visible: root.hasAssetIcon || root.iconText.length > 0
            color: root.hasAssetIcon ? "#ffffff" : "#00000000"
            border.color: root.hasAssetIcon ? "#00000008" : "#00000000"
            AssetIcon {
                anchors.centerIn: parent
                width: root.compact ? 16 : 18
                height: root.compact ? 16 : 18
                source: root.iconSource
                visible: root.hasAssetIcon
                active: true
            }
            Text {
                anchors.centerIn: parent
                visible: !root.hasAssetIcon
                text: root.iconText
                color: root.accent
                font.pixelSize: 16
                font.bold: true
            }
        }
        Text { text: root.value; color: Theme.ink; font.pixelSize: root.compact ? 11 : 12; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
    }
}
