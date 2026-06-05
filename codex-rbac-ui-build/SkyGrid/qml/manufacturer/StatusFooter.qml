import QtQuick
import QtQuick.Layouts

ColumnLayout {
    Layout.fillWidth: true
    spacing: 6
    Text {
        Layout.fillWidth: true
        text: manufacturerVehicleManager.error.length > 0 ? manufacturerVehicleManager.error : manufacturerVehicleManager.status
        color: manufacturerVehicleManager.error.length > 0 ? "#b00020" : "#706a7e"
        font.pixelSize: 12
        wrapMode: Text.WordWrap
    }
    Text {
        Layout.fillWidth: true
        text: manufacturerVehicleManager.loading ? "Working..." : ""
        color: "#30006f"
        font.pixelSize: 11
        font.bold: true
    }
}
