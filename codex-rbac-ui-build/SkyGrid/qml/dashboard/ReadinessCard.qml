import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property int readiness: 100
    property var checks: []

    signal detailsRequested()

    radius: 10
    color: "#ffffff"
    border.color: "#e2dceb"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 12

        Text {
            text: "Mission Readiness"
            color: "#14111d"
            font.pixelSize: 16
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18

            Item {
                Layout.preferredWidth: 145
                Layout.preferredHeight: 145

                Canvas {
                    id: ring
                    anchors.fill: parent
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)
                        var cx = width / 2
                        var cy = height / 2
                        var radius = Math.min(width, height) / 2 - 12
                        ctx.lineWidth = 13
                        ctx.strokeStyle = "#e8e3ef"
                        ctx.beginPath()
                        ctx.arc(cx, cy, radius, 0, Math.PI * 2)
                        ctx.stroke()
                        ctx.strokeStyle = root.readiness >= 90 ? "#36b94b" : (root.readiness >= 70 ? "#f4b000" : "#ef4444")
                        ctx.beginPath()
                        ctx.arc(cx, cy, radius, -Math.PI / 2, -Math.PI / 2 + Math.PI * 2 * root.readiness / 100)
                        ctx.stroke()
                    }
                    Connections {
                        target: root
                        function onReadinessChanged() { ring.requestPaint() }
                    }
                }

                Column {
                    anchors.centerIn: parent
                    spacing: 2
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: root.readiness + "%"
                        color: "#111111"
                        font.pixelSize: 25
                        font.bold: true
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: root.readiness >= 90 ? "READY" : (root.readiness >= 70 ? "CAUTION" : "BLOCKED")
                        color: "#211a30"
                        font.pixelSize: 10
                        font.bold: true
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0

                Repeater {
                    model: root.checks
                    delegate: RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 28
                        Text {
                            Layout.fillWidth: true
                            text: modelData.label
                            color: "#211a30"
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }
                        Text {
                            text: modelData.value
                            color: modelData.ok ? "#27a844" : "#b87900"
                            font.pixelSize: 10
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        Text {
                            text: modelData.ok ? "OK" : "!"
                            color: modelData.ok ? "#27a844" : "#b87900"
                            font.pixelSize: 10
                            font.bold: true
                        }
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            radius: 7
            color: detailMouse.containsMouse ? "#f6f2fb" : "#ffffff"
            border.color: "#cfc4eb"
            Text {
                anchors.centerIn: parent
                text: "View Readiness Details"
                color: "#3b0787"
                font.pixelSize: 12
                font.bold: true
            }
            MouseArea {
                id: detailMouse
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.detailsRequested()
            }
        }
    }
}
