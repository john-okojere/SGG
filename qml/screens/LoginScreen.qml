import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Item {
    id: root
    readonly property color loginSurface: "#11091b"
    readonly property color loginSurfaceRaised: "#180d25"
    readonly property color loginText: "#ffffff"
    readonly property color loginMuted: "#d8d0e8"
    readonly property color inputText: "#050505"
    readonly property color inputBorder: "#d9cce8"

    Image {
        anchors.fill: parent
        source: AssetRegistry.login.background
        fillMode: Image.PreserveAspectCrop
        smooth: true
        mipmap: true
    }

    Rectangle {
        anchors.fill: parent
        color: "#07030c"
        opacity: 0.54
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#12051fdd" }
            GradientStop { position: 0.46; color: "#09040dbb" }
            GradientStop { position: 1.0; color: "#160526ee" }
        }
    }

    Rectangle {
        anchors.centerIn: parent
        width: Math.min(520, parent.width - 48)
        height: authManager.devicePending ? 530 : 610
        radius: 10
        color: root.loginSurface
        border.color: "#332047"
        border.width: 1

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
                color: root.loginText
                font.pixelSize: 28
                font.bold: true
            }

            Text {
                Layout.fillWidth: true
                text: authManager.statusMessage
                color: authManager.devicePending ? Theme.amber : root.loginMuted
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
                    Layout.preferredHeight: 54
                    placeholderText: "Email"
                    inputMethodHints: Qt.ImhEmailCharactersOnly
                    font.pixelSize: 17
                    color: root.inputText
                    placeholderTextColor: "#6f687a"
                    background: Rectangle {
                        radius: 7
                        color: "#ffffff"
                        border.color: emailField.activeFocus ? "#7c45b8" : root.inputBorder
                        border.width: emailField.activeFocus ? 2 : 1
                    }
                }

                TextField {
                    id: passwordField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 54
                    placeholderText: "Password"
                    echoMode: TextInput.Password
                    font.pixelSize: 17
                    color: root.inputText
                    placeholderTextColor: "#6f687a"
                    background: Rectangle {
                        radius: 7
                        color: "#ffffff"
                        border.color: passwordField.activeFocus ? "#7c45b8" : root.inputBorder
                        border.width: passwordField.activeFocus ? 2 : 1
                    }
                    Keys.onReturnPressed: authManager.login(emailField.text, passwordField.text)
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 72
                radius: 8
                color: root.loginSurfaceRaised
                border.color: "#332047"
                border.width: 1
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 12
                    AssetIcon { Layout.preferredWidth: 26; Layout.preferredHeight: 26; source: AssetRegistry.icons.plane; active: true }
                    Column {
                        Layout.fillWidth: true
                        Text { text: deviceManager.hostname; color: root.loginText; font.pixelSize: 16; font.bold: true }
                        Text { text: deviceManager.osName + " - " + deviceManager.deviceUuid; color: root.loginMuted; font.pixelSize: 12; elide: Text.ElideRight; width: parent.width }
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                visible: authManager.securityWarning.length > 0
                text: authManager.securityWarning
                color: "#ff8a8a"
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
                color: cancelArea.containsMouse ? "#211334" : "#00000000"
                border.color: "#5a3a78"
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
                        color: root.loginText
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
                color: root.loginMuted
                font.pixelSize: 12
                wrapMode: Text.WordWrap
            }
        }
    }
}
