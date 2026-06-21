import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import SkyGrid 1.0

Rectangle {
    id: root

    property string message: "Starting SkyGrid GCS"
    property int progress: 8
    property bool offline: false

    color: "#f8f6fc"

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 420)
        spacing: 22

        Image {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 220
            Layout.preferredHeight: 64
            source: AssetRegistry.logos.full_logo
            fillMode: Image.PreserveAspectFit
            smooth: true
        }

        BusyIndicator {
            Layout.alignment: Qt.AlignHCenter
            running: true
            implicitWidth: 42
            implicitHeight: 42
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                Layout.fillWidth: true
                text: root.offline ? "Control Center unavailable. Loading cached workspace." : root.message
                color: root.offline ? "#715400" : "#211735"
                font.pixelSize: 15
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 6
                radius: 3
                color: "#e6def1"

                Rectangle {
                    width: parent.width * Math.max(0.05, Math.min(1, root.progress / 100))
                    height: parent.height
                    radius: parent.radius
                    color: root.offline ? "#f0b429" : "#4b0aa2"

                    Behavior on width {
                        NumberAnimation {
                            duration: 180
                            easing.type: Easing.OutCubic
                        }
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                text: appStartupManager.backgroundSyncing ? "Heavy data will continue syncing after the workspace opens." : "Preparing a lightweight role workspace."
                color: "#746984"
                font.pixelSize: 11
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }
    }
}
