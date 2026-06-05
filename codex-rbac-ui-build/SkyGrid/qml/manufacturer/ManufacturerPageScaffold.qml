import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property string title: ""
    property string subtitle: ""
    property var rows: []
    color: "#f6f4fa"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Text { Layout.fillWidth: true; text: root.title; color: "#14111d"; font.pixelSize: 24; font.bold: true; elide: Text.ElideRight }
                Text { Layout.fillWidth: true; text: root.subtitle; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 8
            color: "#ffffff"
            border.color: "#e2dceb"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 12
                Repeater {
                    model: root.rows
                    delegate: Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 54
                        radius: 7
                        color: "#fbfaff"
                        border.color: "#eee8f5"
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 12
                            Text { Layout.fillWidth: true; text: modelData.label; color: "#14111d"; font.pixelSize: 13; font.bold: true; elide: Text.ElideRight }
                            Text { text: modelData.value; color: "#706a7e"; font.pixelSize: 12; elide: Text.ElideRight }
                        }
                    }
                }
                Item { Layout.fillHeight: true }
            }
        }
    }
}
