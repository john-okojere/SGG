import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    radius: 7
    color: "#ffffff"
    border.color: "#e2d8ee"

    readonly property bool canCommand: vehicleManager.connected && sessionManager.operationsAllowed && !vehicleActionManager.busy
    readonly property bool preflightReady: !preflightChecklistManager.blocked && !preflightChecklistManager.cautionRequired
    readonly property bool canArm: canCommand && preflightReady && !telemetryStore.armed
    readonly property bool canTakeoff: canCommand && telemetryStore.armed && !telemetryStore.inAir
    readonly property bool canAirborne: canCommand && telemetryStore.inAir

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10
        Text { text: "QUICK ACTIONS"; color: "#3b294d"; font.pixelSize: 11; font.bold: true }
        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 10
            columnSpacing: 10
            ActionButton {
                text: telemetryStore.armed ? "DISARM" : "ARM"
                iconText: telemetryStore.armed ? "⌁" : "▣"
                enabled: root.canCommand
                accent: "#2e005f"
                onClicked: {
                    preflightChecklistManager.runChecklist(true)
                    if (typeof pilotActionSyncManager !== "undefined") {
                        pilotActionSyncManager.recordAction(telemetryStore.armed ? "vehicle_disarm" : "vehicle_arm",
                                                            telemetryStore.armed ? "Pilot requested disarm" : "Pilot requested arm")
                    }
                    telemetryStore.armed ? vehicleActionManager.disarm() : vehicleActionManager.arm()
                }
            }
            ActionButton {
                text: "HOLD / HOVER"
                iconText: "◉"
                enabled: root.canAirborne
                onClicked: {
                    pilotActionSyncManager.recordAction("vehicle_hold", "Pilot requested hold / hover")
                    manualControlManager.neutral()
                    vehicleActionManager.holdPosition()
                }
            }
            ActionButton {
                text: "TAKEOFF"
                iconText: "↑"
                enabled: root.canTakeoff
                onClicked: {
                    preflightChecklistManager.runChecklist(true)
                    flightStatsManager.startSession()
                    if (typeof pilotActionSyncManager !== "undefined") {
                        pilotActionSyncManager.recordAction("takeoff_started", "Pilot requested takeoff")
                    }
                    vehicleActionManager.takeoff()
                }
            }
            ActionButton {
                text: "LAND"
                iconText: "↓"
                enabled: root.canAirborne || (root.canCommand && telemetryStore.armed)
                onClicked: {
                    pilotActionSyncManager.recordAction("land_requested", "Pilot requested landing")
                    vehicleActionManager.land()
                }
            }
            ActionButton {
                text: "RETURN TO HOME"
                iconText: "⌂"
                enabled: root.canAirborne
                onClicked: {
                    pilotActionSyncManager.recordAction("return_to_home_requested", "Pilot requested return to home")
                    vehicleActionManager.returnToLaunch()
                }
            }
            ActionButton {
                text: "EMERGENCY STOP"
                iconText: "△"
                danger: true
                enabled: root.canCommand
                onClicked: {
                    pilotActionSyncManager.recordAction("emergency_stop_requested", "Pilot requested emergency stop")
                    vehicleActionManager.emergencyStop()
                }
            }
        }
    }

    component ActionButton: Button {
        id: button
        property string iconText: ""
        property color accent: "#2e005f"
        property bool danger: false
        Layout.fillWidth: true
        Layout.preferredHeight: 42
        hoverEnabled: true
        opacity: enabled ? 1 : 0.42
        contentItem: Row {
            spacing: 8
            anchors.centerIn: parent
            Text {
                text: button.iconText
                color: button.danger ? "#ef233c" : button.accent
                font.pixelSize: 15
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: button.text
                color: button.danger ? "#ef233c" : "#111111"
                font.pixelSize: 10
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        background: Rectangle {
            radius: 5
            color: button.hovered && button.enabled ? (button.danger ? "#fff1f3" : "#f7f1fd") : "#ffffff"
            border.color: button.danger ? "#ef233c" : "#d9cbe9"
            border.width: button.danger || button.hovered ? 1.5 : 1
            Behavior on color { ColorAnimation { duration: 130 } }
        }
    }
}
