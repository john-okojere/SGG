import QtQuick
import QtQuick.Layouts
import "../../controls"

ColumnLayout {
    spacing: 10
    CollapsibleSection {
        Layout.fillWidth: true
        title: "Virtual Fence Settings"
        SelectRow { Layout.fillWidth: true; label: "Fence Shape"; value: "Polygon"; options: ["Polygon", "Circle", "Corridor"] }
        SelectRow { Layout.fillWidth: true; label: "Boundary Type"; value: "Inclusion"; options: ["Inclusion", "Exclusion", "Warning"] }
        ParameterSlider { Layout.fillWidth: true; label: "Flight Speed"; from: 2; to: 140; value: missionStore.plan.speed; unit: "m/s"; onValueEdited: missionStore.plan.speed = value }
        ParameterSlider { Layout.fillWidth: true; label: "Safe Margin"; from: 1; to: 100; value: missionStore.plan.safeMargin; decimals: 1; unit: "m"; onValueEdited: missionStore.plan.safeMargin = value }
        ParameterSlider { Layout.fillWidth: true; label: "Min Alt."; from: 20; to: 160; value: missionStore.plan.altitude; decimals: 1; unit: "ft"; onValueEdited: missionStore.plan.altitude = value }
        SelectRow { Layout.fillWidth: true; label: "Breach Action"; value: "Return to Home"; options: ["Return to Home", "Hover", "Land", "Notify Only"] }
        SelectRow { Layout.fillWidth: true; label: "Warning Action"; value: "Alert Pilot"; options: ["Alert Pilot", "Notify Control Center", "Log Only"] }
        SelectRow { Layout.fillWidth: true; label: "Active Time"; value: "Always"; options: ["Always", "Mission Only", "Timed Window"] }
        ToggleRow { Layout.fillWidth: true; label: "Notifications"; checked: true }
    }
}
