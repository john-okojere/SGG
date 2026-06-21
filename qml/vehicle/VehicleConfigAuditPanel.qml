import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    radius: 8
    color: "#ffffff"
    border.color: "#e2dceb"
    implicitHeight: auditContent.implicitHeight + 32

    ColumnLayout {
        id: auditContent
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Text { Layout.fillWidth: true; text: "Configuration Audit"; color: "#171222"; font.pixelSize: 16; font.bold: true }
            Rectangle {
                Layout.preferredWidth: 122
                Layout.preferredHeight: 28
                radius: 8
                color: "#f8f6fb"
                border.color: "#e2dceb"
                Text { anchors.centerIn: parent; text: vehicleConfigManager.auditTrail.length + " local events"; color: "#706a7e"; font.pixelSize: 10; font.bold: true }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.minimumHeight: 170
            Layout.preferredHeight: 220
            clip: true
            spacing: 8
            model: vehicleConfigManager.auditTrail
            delegate: Rectangle {
                width: ListView.view.width
                height: Math.max(48, messageText.implicitHeight + 24)
                radius: 8
                color: "#fbf9fe"
                border.color: "#e7def0"
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10
                    Rectangle {
                        Layout.preferredWidth: 8
                        Layout.fillHeight: true
                        radius: 4
                        color: String(modelData.action).indexOf("failed") >= 0 || modelData.action === "permission_denied" ? "#e05252" : "#28b947"
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text { Layout.fillWidth: true; text: modelData.action || "event"; color: "#171222"; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
                        Text { id: messageText; Layout.fillWidth: true; text: modelData.message || ""; color: "#706a7e"; font.pixelSize: 11; wrapMode: Text.WordWrap }
                    }
                    Text { Layout.preferredWidth: 170; text: modelData.created_at || ""; color: "#9b94a6"; font.pixelSize: 10; horizontalAlignment: Text.AlignRight; elide: Text.ElideRight }
                }
            }
        }
    }
}
