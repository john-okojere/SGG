import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    radius: 8
    color: "#ffffff"
    border.color: "#e2dceb"
    implicitHeight: snapshotContent.implicitHeight + 32

    ColumnLayout {
        id: snapshotContent
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text {
                    Layout.fillWidth: true
                    text: "Step 6 - Parameter Snapshot"
                    color: "#171222"
                    font.pixelSize: 16
                    font.bold: true
                }
                Text {
                    Layout.fillWidth: true
                    text: "Capture read-only flight-controller state for SGG_CC audit. Writing parameters requires explicit permission and is not automatic."
                    color: "#706a7e"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }
            }
            Button {
                Layout.preferredWidth: 94
                Layout.preferredHeight: 32
                text: "Read"
                enabled: permissionManager.canReadVehicleParameters
                onClicked: vehicleConfigManager.readParameterSnapshot()
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? "#f5f0fb" : "#ffffff"
                    border.color: "#d8ceeb"
                }
                contentItem: Text {
                    text: parent.text
                    color: "#3b0787"
                    font.pixelSize: 11
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        TextArea {
            Layout.fillWidth: true
            Layout.minimumHeight: 150
            Layout.preferredHeight: 190
            readOnly: true
            wrapMode: TextEdit.Wrap
            text: JSON.stringify(vehicleConfigManager.parameterSnapshot).length > 2 ? JSON.stringify(vehicleConfigManager.parameterSnapshot, null, 2) : "No snapshot captured yet.\n\nConnect a flight controller, then click Read. The snapshot records connection, autopilot, health, armed state, and flight mode without changing vehicle settings."
            color: "#2c2538"
            font.family: "Consolas"
            font.pixelSize: 11
            background: Rectangle {
                radius: 8
                color: "#f8f6fb"
                border.color: "#e7def0"
            }
        }

        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 38
            text: "Save Snapshot"
            enabled: permissionManager.canWriteVehicleParameters && JSON.stringify(vehicleConfigManager.parameterSnapshot).length > 2
            onClicked: vehicleConfigManager.saveParameterSnapshot(vehicleConfigManager.parameterSnapshot)
            background: Rectangle {
                radius: 8
                color: parent.hovered ? "#4f0aa6" : "#3b0787"
            }
            contentItem: Text {
                text: parent.text
                color: "#ffffff"
                font.pixelSize: 11
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
