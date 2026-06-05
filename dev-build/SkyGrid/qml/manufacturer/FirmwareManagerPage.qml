import QtQuick
import QtQuick.Layouts

Rectangle {
    color: "#f6f4fa"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 14

        Text { text: "Firmware Manager"; color: "#14111d"; font.pixelSize: 24; font.bold: true }
        Text { Layout.fillWidth: true; text: "Firmware management remains intentionally disabled until a backend firmware service, upload validation, and release signing flow exist."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }
        Text { text: "Permission: " + (accessManager.can("can_configure_vehicle") ? "Allowed" : "Blocked"); color: "#14111d"; font.pixelSize: 13 }
        Text { text: "Status: Not implemented"; color: "#b00020"; font.pixelSize: 13; font.bold: true }
    }
}
