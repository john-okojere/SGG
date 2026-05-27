import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property var rows: []

    radius: 10
    color: "#ffffff"
    border.color: "#e2dceb"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 10

        Text {
            text: "System Status"
            color: "#14111d"
            font.pixelSize: 16
            font.bold: true
        }

        Repeater {
            model: root.rows
            delegate: RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 24
                Text {
                    Layout.fillWidth: true
                    text: modelData.label
                    color: "#332c42"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
                Text {
                    text: modelData.value
                    color: "#211a30"
                    font.pixelSize: 10
                    font.bold: true
                    elide: Text.ElideRight
                }
                Rectangle {
                    width: 7
                    height: 7
                    radius: 4
                    color: modelData.ok ? "#28b947" : "#f4b000"
                }
            }
        }

        Item { Layout.fillHeight: true }

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: "OK"
                color: "#28b947"
                font.pixelSize: 12
                font.bold: true
            }
            Text {
                Layout.fillWidth: true
                text: "All Systems Operational"
                color: "#28a545"
                font.pixelSize: 12
                font.bold: true
            }
        }
    }
}
