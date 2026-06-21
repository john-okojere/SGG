import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#f6f4fa"
    readonly property bool firmwareAllowed: accessManager.can("can_flash_firmware") || accessManager.can("can_configure_vehicle")

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            x: 24
            width: Math.max(0, root.width - 48)
            spacing: 14

            Item { Layout.preferredHeight: 10 }

            Text { text: "Firmware Manager"; color: "#14111d"; font.pixelSize: 24; font.bold: true }
            Text {
                Layout.fillWidth: true
                text: "Select a firmware package, review target information, then stage an upload to the connected controller."
                color: "#706a7e"
                font.pixelSize: 13
                wrapMode: Text.WordWrap
            }

            GridLayout {
                Layout.fillWidth: true
                columns: root.width < 980 ? 1 : 2
                columnSpacing: 14
                rowSpacing: 14

                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: firmwareContent.implicitHeight + 32
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e2dceb"

                    ColumnLayout {
                        id: firmwareContent
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            Text { Layout.fillWidth: true; text: "Firmware Package"; color: "#171222"; font.pixelSize: 16; font.bold: true; elide: Text.ElideRight }
                            Button {
                                text: "Select File"
                                enabled: root.firmwareAllowed && !firmwareUpdateManager.uploading
                                onClicked: firmwareFileDialog.open()
                            }
                        }

                        InfoRow { label: "File"; value: firmwareUpdateManager.fileName }
                        InfoRow { label: "Version"; value: firmwareUpdateManager.version }
                        InfoRow { label: "Size"; value: firmwareUpdateManager.fileSize }
                        InfoRow { label: "SHA-256"; value: firmwareUpdateManager.checksum }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            Text { text: "Package Type"; color: "#706a7e"; font.pixelSize: 11; font.bold: true }
                            ComboBox {
                                id: packageType
                                Layout.fillWidth: true
                                model: ["Auto", "PX4", "ArduPilot", "SkyGrid", "Custom"]
                                currentIndex: Math.max(0, model.indexOf(firmwareUpdateManager.packageType))
                                enabled: !firmwareUpdateManager.uploading
                                onActivated: firmwareUpdateManager.packageType = currentText
                            }
                        }

                        InfoRow { label: "Target"; value: firmwareUpdateManager.target }
                        InfoRow { label: "Aircraft Link"; value: vehicleManager.connected ? ("Connected: " + vehicleManager.autopilot + " SYS-" + vehicleManager.systemId) : "No connected flight controller" }
                        InfoRow { label: "Board"; value: firmwareUpdateManager.boardSummary }
                        InfoRow { label: "Adapter"; value: firmwareUpdateManager.adapterName }
                        InfoRow { label: "Permission"; value: root.firmwareAllowed ? "Allowed" : "Blocked" }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: uploadContent.implicitHeight + 32
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e2dceb"

                    ColumnLayout {
                        id: uploadContent
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        Text { Layout.fillWidth: true; text: "Upload Status"; color: "#171222"; font.pixelSize: 16; font.bold: true; elide: Text.ElideRight }
                        InfoRow { label: "State"; value: firmwareUpdateManager.state }
                        InfoRow { label: "Validation"; value: firmwareUpdateManager.validationStatus }
                        InfoRow { label: "Bootloader"; value: firmwareUpdateManager.bootloaderStatus }
                        InfoRow { label: "Flashing"; value: firmwareUpdateManager.supported ? "Serial bootloader adapter ready" : "Select a supported package and bootloader port" }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.minimumHeight: bootloaderBox.implicitHeight + 20
                            radius: 6
                            color: "#fbfaff"
                            border.color: "#e7def0"

                            ColumnLayout {
                                id: bootloaderBox
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 8

                                RowLayout {
                                    Layout.fillWidth: true
                                    Text {
                                        Layout.fillWidth: true
                                        text: "Bootloader Serial Transport"
                                        color: "#171222"
                                        font.pixelSize: 13
                                        font.bold: true
                                    }
                                    Button {
                                        text: "Refresh Ports"
                                        enabled: !firmwareUpdateManager.uploading
                                        onClicked: firmwareUpdateManager.refreshBootloaderPorts()
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 10
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4
                                        Text { text: "Bootloader COM Port"; color: "#706a7e"; font.pixelSize: 11; font.bold: true }
                                        ComboBox {
                                            Layout.fillWidth: true
                                            model: firmwareUpdateManager.bootloaderPorts
                                            enabled: !firmwareUpdateManager.uploading
                                            currentIndex: Math.max(0, firmwareUpdateManager.bootloaderPorts.indexOf(firmwareUpdateManager.bootloaderPort))
                                            onActivated: firmwareUpdateManager.bootloaderPort = currentText
                                            displayText: firmwareUpdateManager.bootloaderPort.length > 0 ? firmwareUpdateManager.bootloaderPort : "Select port"
                                        }
                                    }
                                    ColumnLayout {
                                        Layout.preferredWidth: 180
                                        spacing: 4
                                        Text { text: "Baud"; color: "#706a7e"; font.pixelSize: 11; font.bold: true }
                                        TextField {
                                            Layout.fillWidth: true
                                            text: String(firmwareUpdateManager.bootloaderBaud)
                                            enabled: !firmwareUpdateManager.uploading
                                            color: "#000000"
                                            selectedTextColor: "#ffffff"
                                            selectionColor: "#5c2d91"
                                            validator: IntValidator { bottom: 57600; top: 1500000 }
                                            background: Rectangle {
                                                color: "#ffffff"
                                                border.color: parent.activeFocus ? "#5c2d91" : "#cfc6dd"
                                                radius: 4
                                            }
                                            onEditingFinished: firmwareUpdateManager.bootloaderBaud = Number(text)
                                        }
                                    }
                                }
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: firmwareUpdateManager.status
                            color: firmwareUpdateManager.status.indexOf("failed") >= 0 || firmwareUpdateManager.status.indexOf("blocked") >= 0 ? "#b00020" : "#3b294d"
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }

                        ProgressBar {
                            Layout.fillWidth: true
                            from: 0
                            to: 100
                            value: firmwareUpdateManager.progress
                        }

                        Flow {
                            Layout.fillWidth: true
                            Layout.preferredHeight: childrenRect.height
                            spacing: 10
                            Button {
                                width: Math.max(92, implicitWidth)
                                height: 32
                                text: "Detect Board"
                                enabled: root.firmwareAllowed && !firmwareUpdateManager.uploading
                                onClicked: {
                                    autopilotToolsFacade.detectBoard();
                                    firmwareUpdateManager.detectBoard();
                                }
                            }
                            Button {
                                width: Math.max(92, implicitWidth)
                                height: 32
                                text: "Validate"
                                enabled: root.firmwareAllowed && firmwareUpdateManager.fileLoaded && !firmwareUpdateManager.uploading
                                onClicked: firmwareUpdateManager.validatePackage()
                            }
                            Button {
                                width: Math.max(92, implicitWidth)
                                height: 32
                                text: "Bootloader"
                                enabled: root.firmwareAllowed && vehicleManager.connected && !firmwareUpdateManager.uploading
                                onClicked: firmwareUpdateManager.requestBootloaderMode()
                            }
                            Button {
                                width: Math.max(92, implicitWidth)
                                height: 32
                                text: firmwareUpdateManager.uploading ? "Running..." : "Flash"
                                enabled: root.firmwareAllowed
                                         && firmwareUpdateManager.fileLoaded
                                         && !firmwareUpdateManager.uploading
                                onClicked: firmwareUpdateManager.startUpload()
                            }
                            Button {
                                width: Math.max(92, implicitWidth)
                                height: 32
                                text: "Retry"
                                enabled: !firmwareUpdateManager.uploading
                                onClicked: firmwareUpdateManager.retryRecovery()
                            }
                            Button {
                                width: Math.max(92, implicitWidth)
                                height: 32
                                text: "Clear"
                                enabled: !firmwareUpdateManager.uploading
                                onClicked: firmwareUpdateManager.clear()
                            }
                            Button {
                                width: Math.max(92, implicitWidth)
                                height: 32
                                text: "Clear Logs"
                                enabled: !firmwareUpdateManager.uploading
                                onClicked: firmwareUpdateManager.clearLogs()
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 248
                            radius: 8
                            color: "#fbfaff"
                            border.color: "#e7def0"

                            ListView {
                                anchors.fill: parent
                                anchors.margins: 10
                                clip: true
                                model: firmwareUpdateManager.logs
                                delegate: Text {
                                    width: ListView.view.width
                                    text: modelData
                                    color: "#2b2435"
                                    font.pixelSize: 11
                                    wrapMode: Text.WordWrap
                                }
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: "OTA deployment will use this same package metadata and log pipeline when the signed backend firmware service is connected."
                            color: "#706a7e"
                            font.pixelSize: 11
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }

            Item { Layout.preferredHeight: 18 }
        }
    }

    FileDialog {
        id: firmwareFileDialog
        title: "Select firmware package"
        nameFilters: ["Firmware packages (*.px4 *.apj *.hex *.sgfw *.bin)", "All files (*)"]
        onAccepted: firmwareUpdateManager.loadFirmwareFile(String(selectedFile))
    }

    component InfoRow: RowLayout {
        property string label: ""
        property string value: ""
        Layout.fillWidth: true
        spacing: 12
        Text {
            Layout.preferredWidth: 118
            text: label
            color: "#706a7e"
            font.pixelSize: 11
            font.bold: true
            elide: Text.ElideRight
        }
        Text {
            Layout.fillWidth: true
            text: value.length > 0 ? value : "--"
            color: "#14111d"
            font.pixelSize: 12
            elide: Text.ElideMiddle
        }
    }
}
