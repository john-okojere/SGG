import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    color: "#f6f4fa"

    readonly property var tools: [
        { key: "vehicleConfiguration", title: "Vehicle Configuration", permission: "can_configure_vehicle", page: vehicleConfigurationPage },
        { key: "vehicleProfile", title: "Vehicle Profile", permission: "can_edit_vehicle_profile", alternate: "can_register_vehicle", page: vehicleProfilePage },
        { key: "flightControllerBinding", title: "Flight Controller Binding", permission: "can_bind_flight_controller", page: flightControllerBindingPage },
        { key: "vehicleParameters", title: "Vehicle Parameters", permission: "can_read_vehicle_parameters", page: vehicleParametersPage },
        { key: "rcMapping", title: "RC Mapping", permission: "can_configure_rc", page: rcMappingPage },
        { key: "manufacturerTestFlight", title: "Manufacturer Test Flight", permission: "can_run_manufacturer_test_flight", page: manufacturerTestFlightPage },
        { key: "manualTestMode", title: "Manual Test Mode", permission: "can_fly_manual_test", page: manualTestModePage },
        { key: "vehicleReleaseLock", title: "Release / Lock Status", permission: "can_release_vehicle_to_organization", alternate: "can_edit_vehicle_profile", page: vehicleReleaseLockPage },
        { key: "firmwareManager", title: "Firmware Manager", permission: "can_configure_vehicle", page: firmwareManagerPage }
    ]

    function allowed(tool) {
        return accessManager.can(tool.permission) || (tool.alternate && accessManager.can(tool.alternate))
    }

    function currentTool() {
        for (var i = 0; i < tools.length; ++i) {
            if (tools[i].key === appState.currentManufacturerTool)
                return tools[i]
        }
        return tools[0]
    }

    function profileName(profile) {
        if (!profile)
            return "Untitled profile"
        return profile.name || profile.model || profile.serial_number || ("Profile " + profile.id)
    }

    Component.onCompleted: manufacturerVehicleManager.fetchVehicleProfiles()

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 332
            Layout.fillHeight: true
            color: "#ffffff"
            border.color: "#e2dceb"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    AssetIcon { Layout.preferredWidth: 24; Layout.preferredHeight: 24; iconSize: 24; source: AssetRegistry.icons.cube; active: true }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text { Layout.fillWidth: true; text: "Manufacturer"; color: "#14111d"; font.pixelSize: 16; font.bold: true; elide: Text.ElideRight }
                        Text { Layout.fillWidth: true; text: missionSyncManager.manufacturer.name || "Authorized workspace"; color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
                    }
                    Button {
                        Layout.preferredWidth: 78
                        Layout.preferredHeight: 32
                        text: "Refresh"
                        enabled: !manufacturerVehicleManager.loading
                        onClicked: manufacturerVehicleManager.fetchVehicleProfiles()
                    }
                }

                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#e2dceb" }

                Text { Layout.fillWidth: true; text: "Tools"; color: "#706a7e"; font.pixelSize: 11; font.bold: true }
                Repeater {
                    model: root.tools
                    delegate: Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 38
                        visible: root.allowed(modelData)
                        text: modelData.title
                        hoverEnabled: true
                        onClicked: appState.openManufacturerTool(modelData.key)
                        background: Rectangle {
                            radius: 7
                            color: appState.currentManufacturerTool === modelData.key ? "#f0e7fb" : (parent.hovered ? "#faf7ff" : "#ffffff")
                            border.color: appState.currentManufacturerTool === modelData.key ? "#8b62c4" : "#e2dceb"
                        }
                        contentItem: Text {
                            text: parent.text
                            color: appState.currentManufacturerTool === modelData.key ? "#30006f" : "#14111d"
                            font.pixelSize: 12
                            font.bold: true
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#e2dceb" }
                Text { Layout.fillWidth: true; text: "Scoped Profiles"; color: "#706a7e"; font.pixelSize: 11; font.bold: true }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 8
                    model: manufacturerVehicleManager.profiles
                    delegate: Rectangle {
                        width: ListView.view.width
                        height: 62
                        radius: 7
                        color: manufacturerVehicleManager.selectedProfile.id === modelData.id ? "#f0e7fb" : "#fbfaff"
                        border.color: "#e2dceb"
                        Column {
                            anchors.fill: parent
                            anchors.margins: 9
                            spacing: 3
                            Text { width: parent.width; text: root.profileName(modelData); color: "#14111d"; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
                            Text { width: parent.width; text: (modelData.serial_number || "No serial") + " • " + (modelData.status || "draft"); color: "#706a7e"; font.pixelSize: 10; elide: Text.ElideRight }
                        }
                        MouseArea { anchors.fill: parent; onClicked: manufacturerVehicleManager.selectProfile(modelData.id) }
                    }
                }

                Text {
                    Layout.fillWidth: true
                    text: manufacturerVehicleManager.error.length > 0 ? manufacturerVehicleManager.error : manufacturerVehicleManager.status
                    color: manufacturerVehicleManager.error.length > 0 ? "#b00020" : "#706a7e"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 38
                    text: "Back to Dashboard"
                    onClicked: appState.goHome()
                }
            }
        }

        Loader {
            Layout.fillWidth: true
            Layout.fillHeight: true
            sourceComponent: root.currentTool().page
        }
    }

    Component { id: vehicleConfigurationPage; VehicleConfigurationPage {} }
    Component { id: vehicleProfilePage; VehicleProfilePage {} }
    Component { id: flightControllerBindingPage; FlightControllerBindingPage {} }
    Component { id: vehicleParametersPage; VehicleParametersPage {} }
    Component { id: rcMappingPage; RcMappingPage {} }
    Component { id: manufacturerTestFlightPage; ManufacturerTestFlightPage {} }
    Component { id: manualTestModePage; ManualTestModePage {} }
    Component { id: vehicleReleaseLockPage; VehicleReleaseLockPage {} }
    Component { id: firmwareManagerPage; FirmwareManagerPage {} }
}
