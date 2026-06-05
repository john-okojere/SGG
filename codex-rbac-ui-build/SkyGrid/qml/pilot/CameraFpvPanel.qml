import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

ColumnLayout {
    id: root
    spacing: 8

    RowLayout {
        Layout.fillWidth: true
        Layout.leftMargin: 4
        Layout.rightMargin: 4
        Text { text: "CAMERA / FPV"; color: "#3b294d"; font.pixelSize: 11; font.bold: true; Layout.fillWidth: true }
        Text { text: "⌃"; color: "#111111"; font.pixelSize: 13; font.bold: true }
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        radius: 7
        color: "#dde3df"
        border.color: "#e2d8ee"
        clip: true

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#aeb8c4" }
            GradientStop { position: 0.55; color: "#d9d2c5" }
            GradientStop { position: 1.0; color: "#416b43" }
        }
        Canvas {
            anchors.fill: parent
            opacity: 0.72
            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                ctx.fillStyle = "rgba(255,255,255,0.75)"
                for (var i = 0; i < 7; ++i) {
                    ctx.fillRect(18 + i * 38, height * 0.48 - (i % 3) * 10, 20, 52 + (i % 4) * 12)
                }
                ctx.strokeStyle = "rgba(255,255,255,0.7)"
                ctx.lineWidth = 5
                ctx.beginPath()
                ctx.moveTo(0, height * 0.72)
                ctx.lineTo(width, height * 0.57)
                ctx.stroke()
            }
        }

        Row {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 10
            spacing: 8
            Badge { text: "LIVE"; bg: "#07130a"; fg: "#18b83f" }
            Badge { text: "1080p 30fps" }
            Badge { text: "1.0x" }
        }

        RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            AssetIcon { Layout.preferredWidth: 18; Layout.preferredHeight: 18; source: AssetRegistry.icons.boxicons_camera; active: true }
            AssetIcon { Layout.preferredWidth: 18; Layout.preferredHeight: 18; source: AssetRegistry.icons.boxicons_play; active: true }
            Item { Layout.fillWidth: true }
            Text { text: "⛶"; color: "#ffffff"; font.pixelSize: 18; font.bold: true }
        }
    }

    component Badge: Rectangle {
        property string text: ""
        property color bg: "#5f6673aa"
        property color fg: "#ffffff"
        width: label.implicitWidth + 12
        height: 22
        radius: 4
        color: bg
        Text {
            id: label
            anchors.centerIn: parent
            text: parent.text
            color: parent.fg
            font.pixelSize: 9
            font.bold: true
        }
    }
}
