import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Item {
    id: root

    readonly property bool authorized: permissionManager.canConfigureVehicle || moduleAccessManager.vehicleConfigurationWorkspaceAllowed
    readonly property bool compact: width < 1180
    readonly property bool wide: width >= 1500
    readonly property bool medium: width >= 1180 && width < 1500
    property string toast: ""

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
        if (!vehicleManager.connected && !telemetryStore.connected)
            return 0
        return clamp((Number(telemetryStore.satellites) / 12) * 100, 0, 100)
    }

    function liveLinkPercent() {
        if (!vehicleManager.connected && !telemetryStore.connected)
            return 0
        if (Number(telemetryStore.rcSignal) > 0)
            return clamp(telemetryStore.rcSignal, 0, 100)
        return telemetryStore.connected ? 75 : 40
    }

    function liveRcPercent() {
        if (!rcCalibrationManager.rcAvailable)
            return 0
        if (Number(telemetryStore.rcSignal) > 0)
            return clamp(telemetryStore.rcSignal, 0, 100)
        return 100
    }

    function channelPercent(channel) {
        var value = Number(channel.value)
        if (!channel.seen || isNaN(value) || value <= 0)
            return 0
        return clamp(((value - 1000) / 1000) * 100, 0, 100)
    }

    function shortNumber(value, suffix) {
        var numeric = Number(value)
        if (isNaN(numeric))
            return "--" + suffix
        return numeric.toFixed(Math.abs(numeric) >= 10 ? 0 : 1) + suffix
    }

    Rectangle {
        anchors.fill: parent
        color: "#f6f4fa"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 72
            color: "#ffffff"
            border.color: "#e2dceb"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: root.compact ? 16 : 26
                anchors.rightMargin: root.compact ? 16 : 26
                spacing: 14

                Button {
                    id: backButton
                    Layout.preferredWidth: 42
                    Layout.preferredHeight: 38
                    visible: moduleAccessManager.defaultWorkspace !== "vehicleConfiguration"
                    text: "<"
                    onClicked: appState.goHome()
                    background: Rectangle {
                        radius: 8
                        color: backButton.hovered ? "#f1eafa" : "#ffffff"
                        border.color: "#d8ceeb"
                    }
                    contentItem: Text {
                        text: backButton.text
                        color: "#3b0787"
                        font.pixelSize: 16
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    ToolTip.visible: hovered
                    ToolTip.text: "Back to dashboard"
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.fillWidth: true
                        text: "Vehicle Configuration"
                        color: "#171222"
                        font.pixelSize: root.compact ? 18 : 22
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Text {
                        Layout.fillWidth: true
                        text: roleAccessManager.currentUserRole.length > 0 ? roleAccessManager.currentUserRole + " workspace" : "Role-controlled workspace"
                        color: "#706a7e"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
                }

                Rectangle {
                    Layout.preferredWidth: root.compact ? 150 : 230
                    Layout.preferredHeight: 38
                    radius: 8
                    color: permissionManager.canConfigureVehicle ? "#ecf8ef" : "#fff6d8"
                    border.color: permissionManager.canConfigureVehicle ? "#bfe8c8" : "#efd06c"
                    Text {
                        anchors.centerIn: parent
                        width: parent.width - 18
                        text: permissionManager.canConfigureVehicle ? "CONFIG ACCESS" : "READ ONLY"
                        color: permissionManager.canConfigureVehicle ? "#14692a" : "#715400"
                        font.pixelSize: 11
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }
                }

                Button {
                    id: testFlightButton
                    Layout.preferredWidth: root.compact ? 130 : 160
                    Layout.preferredHeight: 38
                    visible: accessManager.canPerform("manual_flight")
                    text: "Pilot Mode"
                    onClicked: appState.startPilotMode()
                    background: Rectangle {
                        radius: 8
                        color: testFlightButton.hovered ? "#4f0aa6" : "#3b0787"
                    }
                    contentItem: Text {
                        text: testFlightButton.text
                        color: "#ffffff"
                        font.pixelSize: 12
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }
            }
        }

        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width
            contentHeight: Math.max(height, content.implicitHeight + (root.compact ? 32 : 48))
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            ColumnLayout {
                id: content
                width: parent.width - (root.compact ? 32 : 48)
                spacing: 16
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: root.compact ? 16 : 24

                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: heroStack.implicitHeight + 36
                    implicitHeight: heroStack.implicitHeight + 36
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e2dceb"

                    ColumnLayout {
                        id: heroStack
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 14

                        Text {
                            Layout.fillWidth: true
                            text: "Vehicle Setup & Release"
                            color: "#171222"
                            font.pixelSize: root.compact ? 22 : 26
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            visible: root.wide
                            spacing: 18
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 8
                                Text {
                                    Layout.fillWidth: true
                                    text: "Connect the controller, identify the vehicle, calibrate RC input, capture parameters, then review release/audit state."
                                    color: "#706a7e"
                                    font.pixelSize: 13
                                    wrapMode: Text.WordWrap
                                }
                                Text {
                                    Layout.fillWidth: true
                                    text: vehicleManager.connected ? "Flight controller connected. Continue with profile binding, RC calibration, and parameter capture." : "Start by connecting a USB flight controller or Gazebo/UDP endpoint. This workspace never sends arm, takeoff, motor test, or mission-start commands."
                                    color: vehicleManager.connected ? "#14692a" : "#715400"
                                    font.pixelSize: 12
                                    wrapMode: Text.WordWrap
                                }
                            }

                            Button {
                                id: heroPrimaryButtonWide
                                Layout.preferredWidth: 190
                                Layout.preferredHeight: 42
                                Layout.alignment: Qt.AlignVCenter
                                text: vehicleManager.connected ? "Bind Profile" : "Connect Vehicle"
                                enabled: permissionManager.canBindFlightController
                                onClicked: vehicleManager.connected ? vehicleConfigManager.bindFlightController() : vehicleConfigManager.connectVehicle()
                                background: Rectangle {
                                    radius: 8
                                    color: heroPrimaryButtonWide.hovered ? "#4f0aa6" : "#3b0787"
                                }
                                contentItem: Text {
                                    text: heroPrimaryButtonWide.text
                                    color: "#ffffff"
                                    font.pixelSize: 12
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            visible: !root.wide
                            spacing: 10
                            Text {
                                Layout.fillWidth: true
                                text: "Connect the controller, identify the vehicle, calibrate RC input, capture parameters, then review release/audit state."
                                color: "#706a7e"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }
                            Text {
                                Layout.fillWidth: true
                                text: vehicleManager.connected ? "Flight controller connected. Continue with profile binding, RC calibration, and parameter capture." : "Start by connecting a USB flight controller or Gazebo/UDP endpoint. This workspace never sends arm, takeoff, motor test, or mission-start commands."
                                color: vehicleManager.connected ? "#14692a" : "#715400"
                                font.pixelSize: 12
                                wrapMode: Text.WordWrap
                            }
                            Button {
                                id: heroPrimaryButtonStacked
                                Layout.preferredWidth: 190
                                Layout.preferredHeight: 42
                                text: vehicleManager.connected ? "Bind Profile" : "Connect Vehicle"
                                enabled: permissionManager.canBindFlightController
                                onClicked: vehicleManager.connected ? vehicleConfigManager.bindFlightController() : vehicleConfigManager.connectVehicle()
                                background: Rectangle {
                                    radius: 8
                                    color: heroPrimaryButtonStacked.hovered ? "#4f0aa6" : "#3b0787"
                                }
                                contentItem: Text {
                                    text: heroPrimaryButtonStacked.text
                                    color: "#ffffff"
                                    font.pixelSize: 12
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: root.compact ? 1 : (root.wide ? 5 : 3)
                    rowSpacing: 12
                    columnSpacing: 12

                    WorkflowCard {
                        step: "1"
                        title: "Connect FC"
                        detail: vehicleManager.connected ? vehicleManager.autopilot + " detected" : "USB/Serial, UDP, or TCP"
                        ok: vehicleManager.connected
                        action: "Connect"
                        onTriggered: vehicleConfigManager.connectVehicle()
                    }
                    WorkflowCard {
                        step: "2"
                        title: "Profile & Bind"
                        detail: vehicleProfileManager.activeProfileId().length > 0 ? (vehicleManager.connected ? "Ready to bind" : "Profile selected") : "Create or select profile"
                        ok: vehicleProfileManager.activeProfileId().length > 0 && vehicleManager.connected
                        action: vehicleProfileManager.activeProfileId().length > 0 ? "Bind" : "New Profile"
                        onTriggered: vehicleProfileManager.activeProfileId().length > 0 ? vehicleConfigManager.bindFlightController() : vehicleProfileManager.selectProfile("")
                    }
                    WorkflowCard {
                        step: "3"
                        title: "RC Calibrate"
                        detail: rcCalibrationManager.rcAvailable ? "Receiver input detected" : "Monitor receiver channels"
                        ok: rcCalibrationManager.rcAvailable
                        action: rcCalibrationManager.monitoring ? "Monitoring" : "Start"
                        onTriggered: vehicleManager.connected ? rcCalibrationManager.startMonitoring() : vehicleConfigManager.connectVehicle()
                    }
                    WorkflowCard {
                        step: "4"
                        title: "Parameters"
                        detail: JSON.stringify(vehicleConfigManager.parameterSnapshot).length > 2 ? "Snapshot captured" : "Read parameters"
                        ok: JSON.stringify(vehicleConfigManager.parameterSnapshot).length > 2
                        action: "Read"
                        onTriggered: vehicleConfigManager.readParameterSnapshot()
                    }
                    WorkflowCard {
                        step: "5"
                        title: "Release / Audit"
                        detail: vehicleProfileManager.activeProfileId().length > 0 ? "Review audit below" : "Select profile first"
                        ok: vehicleProfileManager.activeProfileId().length > 0 && JSON.stringify(vehicleConfigManager.parameterSnapshot).length > 2
                        action: "Review"
                        onTriggered: root.toast = "Review configuration audit below, then release the profile from Command Center."
                    }
                }

                VisualBenchPanel {
                    Layout.fillWidth: true
                    Layout.minimumHeight: implicitHeight
                    enabled: root.authorized
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: root.compact ? 1 : 2
                    rowSpacing: 16
                    columnSpacing: 16

                    VehicleConnectionPanel {
                        Layout.fillWidth: true
                        Layout.minimumHeight: implicitHeight
                        enabled: root.authorized
                    }

                    VehicleProfilePanel {
                        Layout.fillWidth: true
                        Layout.minimumHeight: implicitHeight
                        enabled: root.authorized
                    }

                    HealthPanel {
                        Layout.fillWidth: true
                        Layout.minimumHeight: implicitHeight
                        enabled: root.authorized
                    }

                    SafetyPanel {
                        Layout.fillWidth: true
                        Layout.minimumHeight: implicitHeight
                        enabled: root.authorized
                    }

                    ControllerMappingPanel {
                        Layout.fillWidth: true
                        Layout.columnSpan: root.compact ? 1 : (root.wide ? 1 : 2)
                        Layout.minimumHeight: implicitHeight
                        enabled: root.authorized
                    }

                    ParameterSnapshotPanel {
                        Layout.fillWidth: true
                        Layout.columnSpan: root.compact ? 1 : (root.wide ? 1 : 2)
                        Layout.minimumHeight: implicitHeight
                        enabled: root.authorized
                    }

                    VehicleConfigAuditPanel {
                        Layout.fillWidth: true
                        Layout.columnSpan: root.compact ? 1 : 2
                        Layout.minimumHeight: implicitHeight
                        enabled: root.authorized
                    }
                }
            }
        }
    }

    component VisualBenchPanel: Rectangle {
        radius: 8
        color: "#ffffff"
        border.color: "#e2dceb"
        implicitHeight: visualBenchContent.implicitHeight + 32

        ColumnLayout {
            id: visualBenchContent
            anchors.fill: parent
            anchors.margins: 16
            spacing: 14

            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.fillWidth: true
                        text: "Live Vehicle Configuration Bench"
                        color: "#171222"
                        font.pixelSize: 17
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Text {
                        Layout.fillWidth: true
                        text: vehicleManager.connected ? vehicleManager.connectionUrl : "Waiting for a flight controller connection"
                        color: vehicleManager.connected ? "#14692a" : "#706a7e"
                        font.pixelSize: 11
                        elide: Text.ElideRight
                    }
                }
                Rectangle {
                    Layout.preferredWidth: root.compact ? 118 : 170
                    Layout.preferredHeight: 30
                    radius: 8
                    color: vehicleManager.connected ? "#ecf8ef" : "#fff6d8"
                    border.color: vehicleManager.connected ? "#bfe8c8" : "#efd06c"
                    Text {
                        anchors.centerIn: parent
                        width: parent.width - 16
                        text: vehicleManager.connected ? "LIVE DATA" : "NOT CONNECTED"
                        color: vehicleManager.connected ? "#14692a" : "#715400"
                        font.pixelSize: 10
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: root.compact ? 1 : (root.wide ? 4 : 2)
                rowSpacing: 12
                columnSpacing: 12

                GaugeCard {
                    title: "Readiness"
                    value: productionReadinessManager.readinessPercent
                    valueLabel: root.percentText(productionReadinessManager.readinessPercent)
                    detail: productionReadinessManager.status.length > 0 ? productionReadinessManager.status : "Readiness model loaded"
                    available: true
                    accent: productionReadinessManager.readyForProduction ? "#14692a" : "#d99000"
                }
                GaugeCard {
                    title: "Power"
                    value: root.liveBatteryPercent()
                    valueLabel: telemetryStore.connected ? root.percentText(telemetryStore.battery) : "--"
                    detail: telemetryStore.connected ? root.shortNumber(telemetryStore.batteryVoltage, " V") : "Waiting for battery telemetry"
                    available: telemetryStore.connected
                    accent: telemetryStore.battery >= 30 ? "#14692a" : "#b00020"
                }
                GaugeCard {
                    title: "GPS / Link"
                    value: root.liveGpsPercent()
                    valueLabel: telemetryStore.connected ? telemetryStore.satellites + " sat" : "--"
                    detail: telemetryStore.connected ? (telemetryStore.gpsMode + " / HDOP " + root.shortNumber(telemetryStore.hdop, "")) : "Waiting for GPS telemetry"
                    available: telemetryStore.connected
                    accent: telemetryStore.satellites >= 8 ? "#14692a" : "#d99000"
                }
                GaugeCard {
                    title: "RC Input"
                    value: root.liveRcPercent()
                    valueLabel: rcCalibrationManager.rcAvailable ? root.percentText(root.liveRcPercent()) : "--"
                    detail: rcCalibrationManager.rcAvailable ? rcCalibrationManager.sampleCount + " channel samples" : "Receiver input not detected"
                    available: rcCalibrationManager.rcAvailable
                    accent: rcCalibrationManager.rcAvailable ? "#14692a" : "#d99000"
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: root.compact ? 1 : (root.wide ? 3 : 2)
                rowSpacing: 12
                columnSpacing: 12

                AttitudeMiniPanel {
                    Layout.fillWidth: true
                    Layout.minimumHeight: implicitHeight
                }
                RcChannelGraph {
                    Layout.fillWidth: true
                    Layout.minimumHeight: implicitHeight
                }
                StatusMatrix {
                    Layout.fillWidth: true
                    Layout.minimumHeight: implicitHeight
                    Layout.columnSpan: root.compact ? 1 : (root.wide ? 1 : 2)
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: root.compact ? 2 : (root.wide ? 6 : 3)
                rowSpacing: 8
                columnSpacing: 8

                MetricTile { label: "Speed"; value: telemetryStore.connected ? root.shortNumber(telemetryStore.speed, " m/s") : "--"; ok: telemetryStore.connected }
                MetricTile { label: "Altitude"; value: telemetryStore.connected ? root.shortNumber(telemetryStore.altitude, " m") : "--"; ok: telemetryStore.connected }
                MetricTile { label: "V Speed"; value: telemetryStore.connected ? root.shortNumber(telemetryStore.verticalSpeed, " m/s") : "--"; ok: telemetryStore.connected }
                MetricTile { label: "Heading"; value: telemetryStore.connected ? root.shortNumber(telemetryStore.heading, " deg") : "--"; ok: telemetryStore.connected }
                MetricTile { label: "Latency"; value: telemetryStore.connected ? root.shortNumber(telemetryStore.latency, " ms") : "--"; ok: telemetryStore.connected }
                MetricTile { label: "Mode"; value: vehicleManager.connected ? vehicleManager.flightMode : "--"; ok: vehicleManager.connected }
            }
        }
    }

    component GaugeCard: Rectangle {
        id: gaugeCard
        property string title: ""
        property real value: 0
        property string valueLabel: ""
        property string detail: ""
        property bool available: false
        property color accent: "#3b0787"
        Layout.fillWidth: true
        Layout.preferredHeight: 160
        radius: 8
        color: gaugeCard.available ? "#fbfaff" : "#f8f6fb"
        border.color: "#e2dceb"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 12

            Canvas {
                id: gaugeCanvas
                Layout.preferredWidth: 86
                Layout.preferredHeight: 86
                property real normalizedValue: root.clamp(gaugeCard.value, 0, 100)
                onNormalizedValueChanged: requestPaint()
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    var center = width / 2
                    var radius = Math.min(width, height) / 2 - 7
                    ctx.lineWidth = 8
                    ctx.strokeStyle = "#e7e1ef"
                    ctx.beginPath()
                    ctx.arc(center, center, radius, Math.PI * 0.75, Math.PI * 2.25)
                    ctx.stroke()
                    ctx.strokeStyle = gaugeCard.available ? gaugeCard.accent : "#bdb6c8"
                    ctx.beginPath()
                    ctx.arc(center, center, radius, Math.PI * 0.75, Math.PI * (0.75 + 1.5 * normalizedValue / 100))
                    ctx.stroke()
                }
                Component.onCompleted: requestPaint()

                Text {
                    anchors.centerIn: parent
                    width: parent.width - 16
                    text: gaugeCard.valueLabel.length > 0 ? gaugeCard.valueLabel : root.percentText(gaugeCard.value)
                    color: gaugeCard.available ? "#171222" : "#706a7e"
                    font.pixelSize: 15
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 7
                Text {
                    Layout.fillWidth: true
                    text: gaugeCard.title
                    color: "#171222"
                    font.pixelSize: 13
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    Layout.fillWidth: true
                    text: gaugeCard.detail
                    color: "#706a7e"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                    maximumLineCount: 3
                    elide: Text.ElideRight
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 8
                    radius: 4
                    color: "#eee8f5"
                    Rectangle {
                        width: parent.width * root.clamp(gaugeCard.value, 0, 100) / 100
                        height: parent.height
                        radius: 4
                        color: gaugeCard.available ? gaugeCard.accent : "#bdb6c8"
                    }
                }
            }
        }
    }

    component AttitudeMiniPanel: Rectangle {
        radius: 8
        color: "#fbfaff"
        border.color: "#e2dceb"
        implicitHeight: 232

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 10
            Text {
                Layout.fillWidth: true
                text: "Attitude"
                color: "#171222"
                font.pixelSize: 14
                font.bold: true
                elide: Text.ElideRight
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                radius: 8
                color: "#ece7f4"
                border.color: "#d8ceeb"
                clip: true

                Item {
                    anchors.centerIn: parent
                    width: parent.width * 1.5
                    height: parent.height * 1.5
                    rotation: telemetryStore.connected ? root.clamp(telemetryStore.roll, -60, 60) : 0
                    y: parent.height / 2 - height / 2 + (telemetryStore.connected ? root.clamp(telemetryStore.pitch, -25, 25) : 0)

                    Rectangle {
                        x: 0
                        y: 0
                        width: parent.width
                        height: parent.height / 2
                        color: "#8fc3ff"
                    }
                    Rectangle {
                        x: 0
                        y: parent.height / 2
                        width: parent.width
                        height: parent.height / 2
                        color: "#8b6a47"
                    }
                }
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width - 36
                    height: 2
                    color: "#ffffff"
                }
                Rectangle {
                    anchors.centerIn: parent
                    width: 10
                    height: 10
                    radius: 5
                    color: "#ffffff"
                    border.color: "#171222"
                }
                Text {
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottomMargin: 8
                    text: telemetryStore.connected ? telemetryStore.flightMode : "No telemetry"
                    color: "#ffffff"
                    font.pixelSize: 10
                    font.bold: true
                }
            }
            GridLayout {
                Layout.fillWidth: true
                columns: 3
                columnSpacing: 8
                rowSpacing: 8
                MetricTile { label: "Roll"; value: telemetryStore.connected ? root.shortNumber(telemetryStore.roll, " deg") : "--"; ok: telemetryStore.connected }
                MetricTile { label: "Pitch"; value: telemetryStore.connected ? root.shortNumber(telemetryStore.pitch, " deg") : "--"; ok: telemetryStore.connected }
                MetricTile { label: "Yaw"; value: telemetryStore.connected ? root.shortNumber(telemetryStore.yaw, " deg") : "--"; ok: telemetryStore.connected }
            }
        }
    }

    component RcChannelGraph: Rectangle {
        radius: 8
        color: "#fbfaff"
        border.color: "#e2dceb"
        implicitHeight: 232

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 8
            RowLayout {
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    text: "RC Channel Monitor"
                    color: "#171222"
                    font.pixelSize: 14
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    text: rcCalibrationManager.monitoring ? "Monitoring" : "Idle"
                    color: rcCalibrationManager.rcAvailable ? "#14692a" : "#706a7e"
                    font.pixelSize: 10
                    font.bold: true
                }
            }
            Repeater {
                model: rcCalibrationManager.channels
                delegate: ChannelBar {
                    Layout.fillWidth: true
                    label: modelData.name
                    rawValue: modelData.seen ? modelData.value : 0
                    percent: root.channelPercent(modelData)
                    seen: modelData.seen
                }
            }
        }
    }

    component ChannelBar: RowLayout {
        property string label: ""
        property real rawValue: 0
        property real percent: 0
        property bool seen: false
        spacing: 8
        Text {
            Layout.preferredWidth: 34
            text: label
            color: seen ? "#171222" : "#8b8498"
            font.pixelSize: 10
            font.bold: true
            elide: Text.ElideRight
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 9
            radius: 4
            color: "#eee8f5"
            Rectangle {
                width: parent.width * root.clamp(percent, 0, 100) / 100
                height: parent.height
                radius: 4
                color: seen ? "#3b0787" : "#c8bfd5"
            }
        }
        Text {
            Layout.preferredWidth: 42
            text: seen ? Math.round(rawValue).toString() : "--"
            color: seen ? "#171222" : "#8b8498"
            font.pixelSize: 10
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideRight
        }
    }

    component StatusMatrix: Rectangle {
        radius: 8
        color: "#fbfaff"
        border.color: "#e2dceb"
        implicitHeight: statusMatrixContent.implicitHeight + 28

        ColumnLayout {
            id: statusMatrixContent
            anchors.fill: parent
            anchors.margins: 14
            spacing: 10
            Text {
                Layout.fillWidth: true
                text: "Configuration Systems"
                color: "#171222"
                font.pixelSize: 14
                font.bold: true
                elide: Text.ElideRight
            }
            GridLayout {
                Layout.fillWidth: true
                columns: root.compact ? 1 : 2
                rowSpacing: 8
                columnSpacing: 8

                StatusPillTile {
                    label: "Flight Controller"
                    value: vehicleManager.connected ? vehicleManager.autopilot + " / " + vehicleManager.systemId : "Not connected"
                    ok: vehicleManager.connected
                }
                StatusPillTile {
                    label: "Firmware"
                    value: firmwareUpdateManager.state.length > 0 ? firmwareUpdateManager.state : firmwareUpdateManager.status
                    ok: firmwareUpdateManager.supported || firmwareUpdateManager.state === "completed"
                }
                StatusPillTile {
                    label: "Parameters"
                    value: parameterManager.busy ? "Reading" : (parameterManager.changedCount > 0 ? parameterManager.changedCount + " changed" : parameterManager.status)
                    ok: !parameterManager.busy && parameterManager.changedCount === 0
                }
                StatusPillTile {
                    label: "Setup"
                    value: initialSetupManager.running ? initialSetupManager.activeStep : initialSetupManager.status
                    ok: !initialSetupManager.running
                }
                StatusPillTile {
                    label: "Payload / Video"
                    value: videoStreamManager.connected ? "RTSP/H264 connected" : payloadManager.status
                    ok: videoStreamManager.connected || payloadManager.status.length > 0
                }
                StatusPillTile {
                    label: "Optional Hardware"
                    value: optionalHardwareManager.activeTool.length > 0 ? optionalHardwareManager.activeTool : optionalHardwareManager.status
                    ok: optionalHardwareManager.status.length > 0
                }
            }
        }
    }

    component StatusPillTile: Rectangle {
        property string label: ""
        property string value: ""
        property bool ok: false
        Layout.fillWidth: true
        Layout.preferredHeight: 58
        radius: 8
        color: ok ? "#f0f8f2" : "#ffffff"
        border.color: ok ? "#bfe8c8" : "#e2dceb"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8
            Rectangle {
                Layout.preferredWidth: 8
                Layout.preferredHeight: 8
                radius: 4
                color: ok ? "#2fb344" : "#d99000"
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text {
                    Layout.fillWidth: true
                    text: label
                    color: "#706a7e"
                    font.pixelSize: 10
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    Layout.fillWidth: true
                    text: value.length > 0 ? value : "Waiting"
                    color: "#171222"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
            }
        }
    }

    component MetricTile: Rectangle {
        property string label: ""
        property string value: ""
        property bool ok: false
        Layout.fillWidth: true
        Layout.preferredHeight: 52
        radius: 8
        color: ok ? "#ffffff" : "#f8f6fb"
        border.color: "#e2dceb"
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 9
            spacing: 3
            Text {
                Layout.fillWidth: true
                text: label
                color: "#706a7e"
                font.pixelSize: 10
                font.bold: true
                elide: Text.ElideRight
            }
            Text {
                Layout.fillWidth: true
                text: value
                color: ok ? "#171222" : "#8b8498"
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
            }
        }
    }

    component WorkflowCard: Rectangle {
        id: workflowCard
        property string step: ""
        property string title: ""
        property string detail: ""
        property string action: ""
        property bool ok: false
        signal triggered
        Layout.fillWidth: true
        Layout.preferredHeight: 108
        radius: 8
        color: workflowCard.ok ? Theme.greenSoft : Theme.surfaceRaised
        border.color: workflowCard.ok ? "#bfe8c8" : Theme.lineSoft

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8
            RowLayout {
                Layout.fillWidth: true
                Rectangle {
                    Layout.preferredWidth: 26
                    Layout.preferredHeight: 26
                    radius: 13
                    color: workflowCard.ok ? "#ffffff" : Theme.purpleWash
                    Text {
                        anchors.centerIn: parent
                        text: workflowCard.step
                        color: workflowCard.ok ? "#14692a" : Theme.purple
                        font.pixelSize: 11
                        font.bold: true
                    }
                }
                Text {
                    Layout.fillWidth: true
                    text: workflowCard.title
                    color: Theme.ink
                    font.pixelSize: 13
                    font.bold: true
                    elide: Text.ElideRight
                }
            }
            Text {
                Layout.fillWidth: true
                text: workflowCard.detail
                color: Theme.muted
                font.pixelSize: 11
                wrapMode: Text.WordWrap
                maximumLineCount: 2
                elide: Text.ElideRight
            }
            Button {
                id: workflowButton
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                text: workflowCard.ok ? "Open" : workflowCard.action
                onClicked: workflowCard.triggered()
                background: Rectangle {
                    radius: 8
                    color: workflowButton.hovered ? Theme.purpleWash : Theme.surfaceRaised
                    border.color: Theme.lineSoft
                }
                contentItem: Text {
                    text: workflowButton.text
                    color: Theme.purple
                    font.pixelSize: 10
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }
        }
    }

    component HealthPanel: Rectangle {
        radius: 8
        color: "#ffffff"
        border.color: "#e2dceb"
        implicitHeight: healthContent.implicitHeight + 32

        ColumnLayout {
            id: healthContent
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Text {
                Layout.fillWidth: true
                text: "Vehicle Health"
                color: "#171222"
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
            }
            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: 8
                columnSpacing: 10
                HealthRow {
                    label: "Telemetry"
                    value: vehicleManager.connected ? "Connected" : "Disconnected"
                    ok: vehicleManager.connected
                }
                HealthRow {
                    label: "Autopilot"
                    value: vehicleManager.autopilot
                    ok: vehicleManager.autopilot !== "Unknown"
                }
                HealthRow {
                    label: "System ID"
                    value: vehicleManager.systemId.length > 0 ? vehicleManager.systemId : "Not detected"
                    ok: vehicleManager.systemId.length > 0
                }
                HealthRow {
                    label: "Flight mode"
                    value: vehicleManager.flightMode
                    ok: vehicleManager.connected
                }
                HealthRow {
                    label: "Armed"
                    value: vehicleManager.armed ? "Armed" : "Disarmed"
                    ok: !vehicleManager.armed
                }
                HealthRow {
                    label: "Health"
                    value: vehicleManager.health
                    ok: vehicleManager.health === "Ready"
                }
            }
        }
    }

    component SafetyPanel: Rectangle {
        radius: 8
        color: "#ffffff"
        border.color: "#e2dceb"
        implicitHeight: safetyContent.implicitHeight + 32

        ColumnLayout {
            id: safetyContent
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Text {
                Layout.fillWidth: true
                text: "Safety Setup"
                color: "#171222"
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
            }
            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: 8
                columnSpacing: 10
                HealthRow {
                    label: "Return altitude"
                    value: "30 m default"
                    ok: true
                }
                HealthRow {
                    label: "Mission altitude"
                    value: "50 m default"
                    ok: true
                }
                HealthRow {
                    label: "Low battery"
                    value: "25% threshold"
                    ok: true
                }
                HealthRow {
                    label: "Geofence"
                    value: "Checked at mission upload"
                    ok: true
                }
                HealthRow {
                    label: "Failsafe"
                    value: vehicleManager.health
                    ok: vehicleManager.health === "Ready"
                }
                HealthRow {
                    label: "Hardware gate"
                    value: "No auto-arm / no auto-takeoff"
                    ok: true
                }
            }
        }
    }

    component HealthRow: RowLayout {
        id: healthRow
        property string label: ""
        property string value: ""
        property bool ok: false
        Layout.fillWidth: true
        Rectangle {
            Layout.preferredWidth: 8
            Layout.preferredHeight: 8
            radius: 4
            color: healthRow.ok ? "#2fb344" : "#d99000"
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1
            Text {
                Layout.fillWidth: true
                text: healthRow.label
                color: "#706a7e"
                font.pixelSize: 10
                font.bold: true
                elide: Text.ElideRight
            }
            Text {
                Layout.fillWidth: true
                text: healthRow.value
                color: "#171222"
                font.pixelSize: 12
                elide: Text.ElideRight
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        visible: !root.authorized
        color: "#f6f4facc"
        z: 20

        Rectangle {
            width: Math.min(parent.width - 48, 520)
            height: 180
            anchors.centerIn: parent
            radius: 8
            color: "#ffffff"
            border.color: "#e2dceb"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 22
                spacing: 12
                Text {
                    Layout.fillWidth: true
                    text: "Vehicle configuration is locked"
                    color: "#171222"
                    font.pixelSize: 20
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    Layout.fillWidth: true
                    text: "Your current Control Center role does not allow vehicle configuration. Mission and observation workspaces remain available based on assigned permissions."
                    color: "#706a7e"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
                Button {
                    id: dashboardButton
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 130
                    Layout.preferredHeight: 36
                    text: "Dashboard"
                    onClicked: appState.goHome()
                    background: Rectangle {
                        radius: 8
                        color: dashboardButton.hovered ? "#4f0aa6" : "#3b0787"
                    }
                    contentItem: Text {
                        text: dashboardButton.text
                        color: "#ffffff"
                        font.pixelSize: 12
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}
