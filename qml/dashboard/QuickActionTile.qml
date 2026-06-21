import QtQuick 2.15
import QtQuick.Layouts 1.15
import SkyGrid 1.0
import "../app"
import "../controls"

Rectangle {
    id: root

    property string iconText: "+"
    property url iconSource: ""
    property string title: "Action"
    property string subtitle: ""
    property bool primary: false
    property string disabledReason: ""

    signal clicked()

    radius: 8
    opacity: root.enabled ? 1 : 0.7
    color: !root.enabled ? "#ebe7f2" : (root.primary ? (mouse.containsMouse ? "#4b128b" : "#2c0057") : (mouse.containsMouse ? "#f4effb" : "#ffffff"))
    border.color: root.primary ? "#2c0057" : "#e7dfef"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 5

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 34
            Layout.preferredHeight: 34
            radius: 10
            color: root.primary ? "#ffffff22" : "#f4effb"
            border.color: root.primary ? "#ffffff35" : "#e7dfef"

            AssetIcon {
                anchors.centerIn: parent
                width: 22
                height: 22
                iconSize: 22
                source: root.iconSource
                active: true
                visible: root.iconSource.toString().length > 0
                inactiveOpacity: 1
            }

            Text {
                anchors.centerIn: parent
                visible: root.iconSource.toString().length === 0
                text: root.iconText
                color: root.primary ? "#ffffff" : "#4B3DA0"
                font.pixelSize: 16
                font.bold: true
            }
        }

        Text {
            Layout.fillWidth: true
            text: root.title
            color: !root.enabled ? "#766f82" : (root.primary ? "#ffffff" : "#2c0057")
            font.pixelSize: 12
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            maximumLineCount: 1
            elide: Text.ElideRight
        }

        Text {
            Layout.fillWidth: true
            text: !root.enabled && root.disabledReason.length > 0 ? root.disabledReason : root.subtitle
            color: root.primary ? "#ebe4ff" : "#766f82"
            font.pixelSize: 9
            horizontalAlignment: Text.AlignHCenter
            maximumLineCount: 2
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        enabled: root.enabled
        hoverEnabled: true
        onClicked: if (root.enabled) root.clicked()
    }
}
