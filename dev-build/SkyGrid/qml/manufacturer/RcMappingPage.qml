import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: "#f6f4fa"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 14

        Text { text: "RC Mapping"; color: "#14111d"; font.pixelSize: 24; font.bold: true }
        Text { Layout.fillWidth: true; text: "Save RC/manual channel mapping to the selected vehicle profile."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 10
            columnSpacing: 12
            MapField { id: rollField; label: "Roll"; text: "1" }
            MapField { id: pitchField; label: "Pitch"; text: "2" }
            MapField { id: throttleField; label: "Throttle"; text: "3" }
            MapField { id: yawField; label: "Yaw"; text: "4" }
            MapField { id: modeField; label: "Mode"; text: "5" }
            MapField { id: killField; label: "Kill"; text: "6" }
        }

        Button {
            text: "Save RC Mapping"
            visible: accessManager.can("can_configure_rc")
            enabled: !manufacturerVehicleManager.loading && manufacturerVehicleManager.selectedProfile.id !== undefined
            onClicked: manufacturerVehicleManager.saveRcMapping(manufacturerVehicleManager.selectedProfile.id, {
                roll: rollField.text,
                pitch: pitchField.text,
                throttle: throttleField.text,
                yaw: yawField.text,
                mode: modeField.text,
                kill: killField.text
            })
        }

        StatusFooter {}
    }

    component MapField: ColumnLayout {
        property string label: ""
        property alias text: input.text
        Layout.fillWidth: true
        spacing: 4
        Text { text: label; color: "#706a7e"; font.pixelSize: 11; font.bold: true }
        TextField { id: input; Layout.fillWidth: true }
    }
}
