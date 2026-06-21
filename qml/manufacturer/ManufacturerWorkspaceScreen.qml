import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    color: "#f6f4fa"
    readonly property bool narrow: width < 920
    readonly property bool stacked: width < 1040

    readonly property var tools: [
        { key: "gcsTools", title: "GCS Tools", description: "Setup, logs and diagnostics", icon: AssetRegistry.icons.lucide_grid_3x3, global: true },
        { key: "vehicleConfiguration", title: "Profile Setup", description: "Airframe and serial metadata", icon: AssetRegistry.icons.cube, permission: "can_configure_vehicle", page: vehicleConfigurationPage },
        { key: "vehicleProfile", title: "Aircraft Manager", description: "Create and update profiles", icon: AssetRegistry.icons.plane, permission: "can_edit_vehicle_profile", alternate: "can_register_vehicle", page: vehicleProfilePage },
        { key: "flightControllerBinding", title: "Controller Binding", description: "Bind controller UID to profile", icon: AssetRegistry.icons.lucide_satellite, permission: "can_bind_flight_controller", page: flightControllerBindingPage },
        { key: "vehicleReleaseLock", title: "Release / Lock", description: "Scope and audit state", icon: AssetRegistry.icons.cloud, permission: "can_release_vehicle_to_organization", alternate: "can_edit_vehicle_profile", page: vehicleReleaseLockPage }
    ]

    function allowed(tool) {
        if (tool.global)
            return typeof gcsToolCatalog !== "undefined" && gcsToolCatalog.availableCount > 0
        return accessManager.can(tool.permission) || (!!tool.alternate && accessManager.can(tool.alternate))
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

    function clamp(value, minValue, maxValue) {
        var numeric = Number(value)
        if (isNaN(numeric))
            return minValue
        return Math.max(minValue, Math.min(maxValue, numeric))
    }

    function percentText(value) {
        return Math.round(clamp(value, 0, 100)) + "%"
    }

    function liveBatteryPercent() {
        return telemetryStore.connected ? clamp(telemetryStore.battery, 0, 100) : 0
    }

    function liveGpsPercent() {
        if (!telemetryStore.connected && !vehicleManager.connected)
            return 0
        return clamp((Number(telemetryStore.satellites) / 12) * 100, 0, 100)
    }

    function liveRcPercent() {
        if (!rcCalibrationManager.rcAvailable)
            return 0
        if (Number(telemetryStore.rcSignal) > 0)
            return clamp(telemetryStore.rcSignal, 0, 100)
        return 100
    }

    Component.onCompleted: manufacturerVehicleManager.fetchVehicleProfiles()

    GridLayout {
        anchors.fill: parent
        columns: root.stacked ? 1 : 2
        rowSpacing: 0
        columnSpacing: 0

        Rectangle {
            Layout.fillWidth: root.stacked
            Layout.preferredWidth: root.stacked ? root.width : (root.narrow ? 276 : 332)
            Layout.preferredHeight: root.stacked ? Math.min(280, Math.max(210, root.height * 0.38)) : root.height
            Layout.minimumWidth: 0
            Layout.minimumHeight: 0
            Layout.fillHeight: !root.stacked
            color: "#ffffff"
            border.color: "#e2dceb"
            clip: true

            ScrollView {
                id: manufacturerNavScroll
                anchors.fill: parent
                contentWidth: availableWidth
                contentHeight: manufacturerNavContent.implicitHeight + 32
                clip: true

                ColumnLayout {
                    id: manufacturerNavContent
                    width: Math.max(0, manufacturerNavScroll.availableWidth - 32)
                    x: 16
                    y: 16
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
                GridLayout {
                    id: toolsGrid
                    Layout.fillWidth: true
                    columns: root.narrow ? 1 : 2
                    rowSpacing: 8
                    columnSpacing: 8

                    Repeater {
                        model: root.tools
                        delegate: Button {
                            id: toolCard
                            Layout.fillWidth: true
                            Layout.preferredHeight: visible ? (root.stacked ? 74 : 86) : 0
                            Layout.maximumHeight: visible ? (root.stacked ? 74 : 86) : 0
                            visible: root.allowed(modelData)
                            hoverEnabled: true
                            onClicked: {
                                if (modelData.global)
                                    appState.openGcsTools()
                                else
                                    appState.openManufacturerTool(modelData.key)
                            }
                            ToolTip.visible: hovered
                            ToolTip.text: modelData.title + ": " + modelData.description
                            background: Rectangle {
                                radius: 8
                                color: appState.currentManufacturerTool === modelData.key ? "#f0e7fb" : (toolCard.hovered ? "#faf7ff" : "#ffffff")
                                border.color: appState.currentManufacturerTool === modelData.key ? "#8b62c4" : "#e2dceb"
                            }
                            contentItem: ColumnLayout {
                                spacing: 4
                                anchors.margins: 8
                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 6
                                    AssetIcon {
                                        Layout.preferredWidth: 18
                                        Layout.preferredHeight: 18
                                        iconSize: 18
                                        source: modelData.icon
                                        active: modelData.global || appState.currentManufacturerTool === modelData.key
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.title
                                        color: appState.currentManufacturerTool === modelData.key ? "#30006f" : "#14111d"
                                        font.pixelSize: 11
                                        font.bold: true
                                        elide: Text.ElideRight
                                    }
                                }
                                Text {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    text: modelData.description
                                    color: "#706a7e"
                                    font.pixelSize: 10
                                    wrapMode: Text.WordWrap
                                    maximumLineCount: root.stacked ? 1 : 2
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }
                }

                LiveConfigStrip {
                    Layout.fillWidth: true
                    Layout.minimumHeight: implicitHeight
                }

                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#e2dceb" }
                Text { Layout.fillWidth: true; text: "Scoped Profiles"; color: "#706a7e"; font.pixelSize: 11; font.bold: true }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    Repeater {
                        model: manufacturerVehicleManager.profiles
                        delegate: Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 62
                            radius: 7
                            color: manufacturerVehicleManager.selectedProfile.id === modelData.id ? "#f0e7fb" : "#fbfaff"
                            border.color: "#e2dceb"
                            Column {
                                anchors.fill: parent
                                anchors.margins: 9
                                spacing: 3
                                Text { width: parent.width; text: root.profileName(modelData); color: "#14111d"; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
                                Text { width: parent.width; text: (modelData.serial_number || "No serial") + " - " + (modelData.status || "draft"); color: "#706a7e"; font.pixelSize: 10; elide: Text.ElideRight }
                            }
                            MouseArea { anchors.fill: parent; onClicked: manufacturerVehicleManager.selectProfile(modelData.id) }
                        }
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
                Item { Layout.preferredHeight: 16 }
            }
        }
        }

        Loader {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 0
            Layout.minimumHeight: 0
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

    component LiveConfigStrip: Rectangle {
        radius: 8
        color: "#fbfaff"
        border.color: "#e2dceb"
        implicitHeight: liveConfigContent.implicitHeight + 18

        ColumnLayout {
            id: liveConfigContent
            anchors.fill: parent
            anchors.margins: 9
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Text {
                    Layout.fillWidth: true
                    text: "Live Setup Health"
                    color: "#14111d"
                    font.pixelSize: 11
                    font.bold: true
                    elide: Text.ElideRight
                }
                Rectangle {
                    Layout.preferredWidth: 8
                    Layout.preferredHeight: 8
                    radius: 4
                    color: vehicleManager.connected ? "#2fb344" : "#d99000"
                }
                Text {
                    text: vehicleManager.connected ? "Live" : "Idle"
                    color: vehicleManager.connected ? "#14692a" : "#715400"
                    font.pixelSize: 9
                    font.bold: true
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: 7
                columnSpacing: 7
                MiniGauge {
                    label: "Ready"
                    value: productionReadinessManager.readinessPercent
                    valueText: root.percentText(productionReadinessManager.readinessPercent)
                    ok: productionReadinessManager.readyForProduction
                }
                MiniGauge {
                    label: "Power"
                    value: root.liveBatteryPercent()
                    valueText: telemetryStore.connected ? root.percentText(telemetryStore.battery) : "--"
                    ok: telemetryStore.connected && telemetryStore.battery >= 30
                }
                MiniGauge {
                    label: "GPS"
                    value: root.liveGpsPercent()
                    valueText: telemetryStore.connected ? telemetryStore.satellites + " sat" : "--"
                    ok: telemetryStore.connected && telemetryStore.satellites >= 8
                }
                MiniGauge {
                    label: "RC"
                    value: root.liveRcPercent()
                    valueText: rcCalibrationManager.rcAvailable ? root.percentText(root.liveRcPercent()) : "--"
                    ok: rcCalibrationManager.rcAvailable
                }
            }

            StatusLine {
                label: "Controller"
                value: vehicleManager.connected ? vehicleManager.autopilot + " " + vehicleManager.systemId : "Not connected"
                ok: vehicleManager.connected
            }
            StatusLine {
                label: "Firmware"
                value: firmwareUpdateManager.state.length > 0 ? firmwareUpdateManager.state : firmwareUpdateManager.status
                ok: firmwareUpdateManager.supported || firmwareUpdateManager.state === "completed"
            }
            StatusLine {
                label: "Video"
                value: videoStreamManager.connected ? "RTSP/H264 connected" : "No stream"
                ok: videoStreamManager.connected
            }
        }
    }

    component MiniGauge: Rectangle {
        property string label: ""
        property real value: 0
        property string valueText: ""
        property bool ok: false
        Layout.fillWidth: true
        Layout.preferredHeight: 46
        radius: 7
        color: "#ffffff"
        border.color: "#e2dceb"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 7
            spacing: 4
            RowLayout {
                Layout.fillWidth: true
                spacing: 5
                Text {
                    Layout.fillWidth: true
                    text: label
                    color: "#706a7e"
                    font.pixelSize: 9
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    text: valueText
                    color: ok ? "#14692a" : "#171222"
                    font.pixelSize: 10
                    font.bold: true
                    elide: Text.ElideRight
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 6
                radius: 3
                color: "#eee8f5"
                Rectangle {
                    width: parent.width * root.clamp(value, 0, 100) / 100
                    height: parent.height
                    radius: 3
                    color: ok ? "#2fb344" : "#8b62c4"
                }
            }
        }
    }

    component StatusLine: RowLayout {
        property string label: ""
        property string value: ""
        property bool ok: false
        spacing: 6
        Rectangle {
            Layout.preferredWidth: 7
            Layout.preferredHeight: 7
            radius: 4
            color: ok ? "#2fb344" : "#d99000"
        }
        Text {
            Layout.preferredWidth: 62
            text: label
            color: "#706a7e"
            font.pixelSize: 9
            font.bold: true
            elide: Text.ElideRight
        }
        Text {
            Layout.fillWidth: true
            text: value.length > 0 ? value : "Waiting"
            color: "#14111d"
            font.pixelSize: 9
            elide: Text.ElideRight
        }
    }
}
