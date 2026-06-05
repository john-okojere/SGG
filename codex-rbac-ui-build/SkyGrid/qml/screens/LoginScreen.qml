import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Item {
    id: root

    Rectangle {
        anchors.fill: parent
        color: "#08040d"
    }

    Canvas {
        anchors.fill: parent
        opacity: 0.24
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            var g = ctx.createLinearGradient(0, 0, width, height)
            g.addColorStop(0, "#19042e")
            g.addColorStop(0.48, "#06070d")
            g.addColorStop(1, "#10241f")
            ctx.fillStyle = g
            ctx.fillRect(0, 0, width, height)
            ctx.strokeStyle = "rgba(255,255,255,0.10)"
            ctx.lineWidth = 1
            for (var x = -120; x < width + 120; x += 64) {
                ctx.beginPath()
                ctx.moveTo(x, 0)
                ctx.lineTo(x + width * 0.18, height)
                ctx.stroke()
            }
        }
    }

    Rectangle {
        anchors.centerIn: parent
        width: Math.min(520, parent.width - 48)
        height: authManager.devicePending ? 530 : 610
        radius: 10
        color: "#f7f3fb"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 34
            spacing: 18

            Image {
                Layout.preferredWidth: 270
                Layout.preferredHeight: 52
                source: AssetRegistry.logos.full_logo
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
            }

            Text {
                text: "Control Center Login"
                color: Theme.ink
                font.pixelSize: 28
                font.bold: true
            }

            Text {
                Layout.fillWidth: true
                text: authManager.statusMessage
                color: authManager.devicePending ? Theme.amber : Theme.muted
                font.pixelSize: 15
                wrapMode: Text.WordWrap
            }

            ColumnLayout {
                Layout.fillWidth: true
                visible: !authManager.devicePending
                spacing: 12

                TextField {
                    id: emailField
                    Layout.fillWidth: true
                    height: 54
                    placeholderText: "Email"
                    inputMethodHints: Qt.ImhEmailCharactersOnly
                    font.pixelSize: 17
                    color: Theme.ink
                }

                TextField {
                    id: passwordField
                    Layout.fillWidth: true
                    height: 54
                    placeholderText: "Password"
                    echoMode: TextInput.Password
                    font.pixelSize: 17
                    color: Theme.ink
                    Keys.onReturnPressed: authManager.login(emailField.text, passwordField.text)
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 72
                radius: 8
                color: "#ffffff"
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 12
                    AssetIcon { Layout.preferredWidth: 26; Layout.preferredHeight: 26; source: AssetRegistry.icons.plane; active: true }
                    Column {
                        Layout.fillWidth: true
                        Text { text: deviceManager.hostname; color: Theme.ink; font.pixelSize: 16; font.bold: true }
                        Text { text: deviceManager.osName + " · " + deviceManager.deviceUuid; color: Theme.muted; font.pixelSize: 12; elide: Text.ElideRight; width: parent.width }
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                visible: authManager.securityWarning.length > 0
                text: authManager.securityWarning
                color: Theme.red
                font.pixelSize: 14
                wrapMode: Text.WordWrap
            }

            Item { Layout.fillHeight: true }

            PrimaryButton {
                Layout.fillWidth: true
                text: authManager.devicePending ? "Check Approval" : (authManager.busy ? "Connecting..." : "Sign In")
                enabled: !authManager.busy
                onClicked: {
                    if (authManager.devicePending) {
                        authManager.checkDeviceApproval()
                    } else {
                        authManager.login(emailField.text, passwordField.text)
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 46
                radius: Theme.controlRadius
                color: cancelArea.containsMouse ? "#ede6f5" : "#00000000"
                border.color: "#d9cce8"
                border.width: 1
                RowLayout {
                    anchors.centerIn: parent
                    spacing: 8
                    AssetIcon {
                        Layout.preferredWidth: 18
                        Layout.preferredHeight: 18
                        source: AssetRegistry.icons.iconoir_cancel
                        active: true
                    }
                    Text {
                        text: authManager.devicePending ? "Cancel Sign In" : "Clear Session"
                        color: Theme.purple
                        font.pixelSize: 15
                        font.bold: true
                    }
                }
                MouseArea {
                    id: cancelArea
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: !authManager.busy
                    onClicked: {
                        passwordField.text = ""
                        authManager.logout()
                    }
                }
                Behavior on color { ColorAnimation { duration: Theme.animFast } }
            }

            Text {
                Layout.fillWidth: true
                text: "Accounts are created in SkyGrid Control Center. This GCS only signs in approved operators and trusted devices."
                color: Theme.muted
                font.pixelSize: 12
                wrapMode: Text.WordWrap
            }
        }
    }
}
