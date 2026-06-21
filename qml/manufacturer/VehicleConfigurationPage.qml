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

            Text { Layout.fillWidth: true; text: "Vehicle Configuration"; color: "#14111d"; font.pixelSize: root.width < 520 ? 20 : 24; font.bold: true; elide: Text.ElideRight }
            Text { Layout.fillWidth: true; text: "Edit configuration fields on the selected scoped vehicle profile."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }

            GridLayout {
                Layout.fillWidth: true
                columns: width < 520 ? 1 : 2
                rowSpacing: 10
                columnSpacing: 12
                ConfigField { id: modelField; label: "Model"; text: root.selected("model", "") }
                ConfigField { id: airframeField; label: "Airframe"; text: root.selected("airframe_type", "") }
                ConfigField { id: firmwareField; label: "Firmware"; text: root.selected("firmware_version", "") }
                ConfigField { id: serialField; label: "Serial"; text: root.selected("serial_number", "") }
            }

            Button {
                text: "Save Configuration"
                visible: accessManager.can("can_edit_vehicle_profile")
                enabled: !manufacturerVehicleManager.loading && manufacturerVehicleManager.selectedProfile.id !== undefined
                onClicked: manufacturerVehicleManager.updateVehicleProfile(manufacturerVehicleManager.selectedProfile.id, {
                    model: modelField.text,
                    airframe_type: airframeField.text,
                    firmware_version: firmwareField.text,
                    serial_number: serialField.text,
                    name: root.selected("name", modelField.text)
                })
            }

            StatusFooter {}
            Item { Layout.preferredHeight: root.width < 520 ? 16 : 24 }
        }
    }

    component ConfigField: ColumnLayout {
        property string label: ""
        property alias text: input.text
        Layout.fillWidth: true
        spacing: 4
        Text { text: label; color: "#706a7e"; font.pixelSize: 11; font.bold: true }
        TextField { id: input; Layout.fillWidth: true }
    }
}
