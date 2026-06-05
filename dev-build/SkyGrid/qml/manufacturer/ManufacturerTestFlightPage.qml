import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: "#f6f4fa"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 14

        Text { text: "Manufacturer Test Flight"; color: "#14111d"; font.pixelSize: 24; font.bold: true }
        Text { Layout.fillWidth: true; text: "Uses current vehicle/session state for readiness. A dedicated SGG_CC test-flight workflow is not connected yet."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }

        StatusRow { label: "Permission"; value: accessManager.can("can_run_manufacturer_test_flight") ? "Allowed" : "Blocked" }
        StatusRow { label: "Session"; value: sessionManager.operationsAllowed ? "Trusted" : sessionManager.blockReason }
        StatusRow { label: "Aircraft Link"; value: vehicleManager.connected ? "Connected" : "Not connected" }
        StatusRow { label: "Backend Workflow"; value: "Not connected yet" }

        RowLayout {
            Layout.fillWidth: true
            Button {
                text: "Refresh Profiles"
                visible: accessManager.can("can_run_manufacturer_test_flight")
                enabled: !manufacturerVehicleManager.loading
                onClicked: manufacturerVehicleManager.fetchVehicleProfiles()
            }
            Button {
                text: "Start Manual Test Mode"
                visible: accessManager.can("can_fly_manual_test")
                enabled: sessionManager.operationsAllowed && vehicleManager.connected
                onClicked: appState.openManufacturerTool("manualTestMode")
            }
        }

        StatusFooter {}
    }

    component StatusRow: RowLayout {
        property string label: ""
        property string value: ""
        Layout.fillWidth: true
        Text { Layout.preferredWidth: 150; text: label; color: "#706a7e"; font.pixelSize: 12; font.bold: true }
        Text { Layout.fillWidth: true; text: value; color: "#14111d"; font.pixelSize: 13; elide: Text.ElideRight }
    }
}
