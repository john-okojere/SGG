import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property int readiness: 0
    property string statusText: "Readiness pending"

    signal detailsRequested()

    radius: 10
    color: mouse.containsMouse ? "#fbfaff" : "#ffffff"
    border.color: mouse.containsMouse ? "#cfc4eb" : "#e2dceb"
    border.width: 1

    readonly property color readinessColor: root.readiness >= 90 ? "#36b94b" : (root.readiness >= 70 ? "#f4b000" : "#ef4444")
    readonly property string readinessLabel: root.readiness >= 90 ? "Ready" : (root.readiness >= 70 ? "Caution" : "Blocked")

    RowLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 18

        Item {
            Layout.preferredWidth: 128
            Layout.preferredHeight: 128
            Layout.alignment: Qt.AlignVCenter

            Canvas {
                id: ring
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    var cx = width / 2
                    var cy = height / 2
                    var radius = Math.min(width, height) / 2 - 12
                    ctx.lineWidth = 12
                    ctx.strokeStyle = "#e8e3ef"
                    ctx.beginPath()
                    ctx.arc(cx, cy, radius, 0, Math.PI * 2)
                    ctx.stroke()
                    ctx.strokeStyle = root.readinessColor
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
                    font.pixelSize: 26
                    font.bold: true
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: root.readinessLabel
                    color: root.readinessColor
                    font.pixelSize: 10
                    font.bold: true
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            Text {
                Layout.fillWidth: true
                text: "Mission Readiness"
                color: "#14111d"
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
            }

            Text {
                Layout.fillWidth: true
                text: root.statusText
                color: "#706a7e"
                font.pixelSize: 12
                wrapMode: Text.WordWrap
                maximumLineCount: 3
                elide: Text.ElideRight
            }

            Item { Layout.fillHeight: true }

            Text {
                Layout.fillWidth: true
                text: "Click for readiness breakdown"
                color: "#3b0787"
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
            }
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.detailsRequested()
    }
}
