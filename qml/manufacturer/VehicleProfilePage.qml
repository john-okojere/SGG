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

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: Math.max(0, root.width - (root.width < 520 ? 32 : 48))
            x: root.width < 520 ? 16 : 24
            y: root.width < 520 ? 16 : 24
            spacing: 14

            Text { Layout.fillWidth: true; text: "Vehicle Profile"; color: "#14111d"; font.pixelSize: root.width < 520 ? 20 : 24; font.bold: true; elide: Text.ElideRight }
            Text { Layout.fillWidth: true; text: "Create or update scoped vehicle profiles through SGG_CC."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }

            GridLayout {
                Layout.fillWidth: true
                columns: width < 520 ? 1 : 2
                rowSpacing: 10
                columnSpacing: 12
                ProfileField { id: nameField; label: "Name"; text: root.selected("name", "") }
                ProfileField { id: modelField; label: "Model"; text: root.selected("model", "") }
                ProfileField { id: serialField; label: "Serial"; text: root.selected("serial_number", "") }
                ProfileField { id: firmwareField; label: "Firmware"; text: root.selected("firmware_version", "") }
                ProfileField { id: airframeField; label: "Airframe"; text: root.selected("airframe_type", "") }
                Text { Layout.fillWidth: true; text: "Status: " + root.selected("status", "draft"); color: "#706a7e"; font.pixelSize: 13 }
            }

            Flow {
                Layout.fillWidth: true
                Layout.preferredHeight: childrenRect.height
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
            Item { Layout.preferredHeight: root.width < 520 ? 16 : 24 }
        }
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
