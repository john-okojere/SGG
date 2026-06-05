import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#f6f4fa"

    function selected(key, fallback) {
        var value = manufacturerVehicleManager.selectedProfile[key]
        return value === undefined || value === null ? fallback : String(value)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 14

        Text { text: "Vehicle Profile"; color: "#14111d"; font.pixelSize: 24; font.bold: true }
        Text { Layout.fillWidth: true; text: "Create or update scoped vehicle profiles through SGG_CC."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 10
            columnSpacing: 12
            ProfileField { id: nameField; label: "Name"; text: root.selected("name", "") }
            ProfileField { id: modelField; label: "Model"; text: root.selected("model", "") }
            ProfileField { id: serialField; label: "Serial"; text: root.selected("serial_number", "") }
            ProfileField { id: firmwareField; label: "Firmware"; text: root.selected("firmware_version", "") }
            ProfileField { id: airframeField; label: "Airframe"; text: root.selected("airframe_type", "") }
            Text { Layout.fillWidth: true; text: "Status: " + root.selected("status", "draft"); color: "#706a7e"; font.pixelSize: 13 }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            Button {
                text: "Create Profile"
                visible: accessManager.can("can_register_vehicle")
                enabled: !manufacturerVehicleManager.loading && nameField.text.length > 0
                onClicked: manufacturerVehicleManager.createVehicleProfile({
                    name: nameField.text,
                    model: modelField.text,
                    serial_number: serialField.text,
                    firmware_version: firmwareField.text,
                    airframe_type: airframeField.text
                })
            }
            Button {
                text: "Update Selected"
                visible: accessManager.can("can_edit_vehicle_profile")
                enabled: !manufacturerVehicleManager.loading && manufacturerVehicleManager.selectedProfile.id !== undefined
                onClicked: manufacturerVehicleManager.updateVehicleProfile(manufacturerVehicleManager.selectedProfile.id, {
                    name: nameField.text,
                    model: modelField.text,
                    serial_number: serialField.text,
                    firmware_version: firmwareField.text,
                    airframe_type: airframeField.text
                })
            }
        }

        StatusFooter {}
    }

    component ProfileField: ColumnLayout {
        property string label: ""
        property alias text: input.text
        Layout.fillWidth: true
        spacing: 4
        Text { text: label; color: "#706a7e"; font.pixelSize: 11; font.bold: true }
        TextField { id: input; Layout.fillWidth: true }
    }
}
