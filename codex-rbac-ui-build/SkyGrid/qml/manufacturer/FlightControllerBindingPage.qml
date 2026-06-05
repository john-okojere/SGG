import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: "#f6f4fa"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 14

        Text { text: "Flight Controller Binding"; color: "#14111d"; font.pixelSize: 24; font.bold: true }
        Text { Layout.fillWidth: true; text: "Bind the selected scoped profile to a flight controller UID."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }

        Text { text: "Selected: " + (manufacturerVehicleManager.selectedProfile.name || "None"); color: "#14111d"; font.pixelSize: 13; font.bold: true }
        TextField { id: uidField; Layout.fillWidth: true; placeholderText: "Flight controller UID"; text: manufacturerVehicleManager.selectedProfile.flight_controller_uid || "" }
        TextField { id: autopilotField; Layout.fillWidth: true; placeholderText: "Autopilot"; text: manufacturerVehicleManager.selectedProfile.autopilot || "" }

        Button {
            text: "Bind Flight Controller"
            visible: accessManager.can("can_bind_flight_controller")
            enabled: !manufacturerVehicleManager.loading && manufacturerVehicleManager.selectedProfile.id !== undefined && uidField.text.length > 0
            onClicked: manufacturerVehicleManager.bindFlightController(manufacturerVehicleManager.selectedProfile.id, uidField.text, autopilotField.text)
        }

        StatusFooter {}
    }
}
