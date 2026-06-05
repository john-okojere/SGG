import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: "#f6f4fa"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 14

        Text { text: "Vehicle Parameters"; color: "#14111d"; font.pixelSize: 24; font.bold: true }
        Text { Layout.fillWidth: true; text: "Save a parameter snapshot to SGG_CC. Live parameter reading still depends on the vehicle connection layer."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 10
            columnSpacing: 12
            ParamField { id: maxAlt; label: "Max Altitude"; text: "120" }
            ParamField { id: rtlAlt; label: "RTL Altitude"; text: "50" }
            ParamField { id: geofence; label: "Geofence Radius"; text: "500" }
            ParamField { id: firmware; label: "Firmware"; text: manufacturerVehicleManager.selectedProfile.firmware_version || "" }
        }

        Button {
            text: "Save Parameter Snapshot"
            visible: accessManager.can("can_write_vehicle_parameters")
            enabled: !manufacturerVehicleManager.loading && manufacturerVehicleManager.selectedProfile.id !== undefined
            onClicked: manufacturerVehicleManager.saveParameterSnapshot(manufacturerVehicleManager.selectedProfile.id, {
                max_altitude_m: maxAlt.text,
                rtl_altitude_m: rtlAlt.text,
                geofence_radius_m: geofence.text,
                firmware_version: firmware.text,
                source: vehicleManager.connected ? "connected_vehicle" : "manual_gcs_entry"
            })
        }

        StatusFooter {}
    }

    component ParamField: ColumnLayout {
        property string label: ""
        property alias text: input.text
        Layout.fillWidth: true
        spacing: 4
        Text { text: label; color: "#706a7e"; font.pixelSize: 11; font.bold: true }
        TextField { id: input; Layout.fillWidth: true }
    }
}
