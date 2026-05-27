import QtQuick
import QtQuick.Layouts
import "../../controls"

ColumnLayout {
    spacing: 10
    SelectRow { Layout.fillWidth: true; label: "Flight Course Mode"; value: "Circle Mode"; options: ["Circle Mode", "Parallel", "Smart Oblique"] }
    SelectRow { Layout.fillWidth: true; label: "Camera Type"; value: "Phantom 4 Pro Camera"; options: ["Phantom 4 Pro Camera", "Zenmuse X5S", "Mavic 3E Camera"] }
    SelectRow { Layout.fillWidth: true; label: "Inside Mode"; value: "Parallel"; options: ["Parallel", "Crosshatch", "Perimeter First"] }
    SelectRow { Layout.fillWidth: true; label: "Capture Mode"; value: "Capture at Equal Dist. Interval"; options: ["Capture at Equal Dist. Interval", "Time Interval", "Hover Capture"] }
    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#d8cde5" }
    ParameterSlider { Layout.fillWidth: true; label: "Flight Speed"; from: 2; to: 140; value: missionStore.plan.speed; unit: "m/s"; onValueEdited: missionStore.plan.speed = value }
    ParameterSlider { Layout.fillWidth: true; label: "Capture Interval"; from: 0.5; to: 10; value: missionStore.plan.captureInterval; decimals: 1; unit: "m/s"; onValueEdited: missionStore.plan.captureInterval = value }
    ParameterSlider { Layout.fillWidth: true; label: "Flight Alt."; from: 20; to: 160; value: missionStore.plan.altitude; decimals: 1; unit: "ft"; onValueEdited: missionStore.plan.altitude = value }
    ParameterSlider { Layout.fillWidth: true; label: "GSD"; from: 0.5; to: 12; value: missionStore.plan.gsd; decimals: 1; unit: "cm/px"; onValueEdited: missionStore.plan.gsd = value }
    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#d8cde5" }
    ParameterSlider { Layout.fillWidth: true; label: "Front Overlap"; from: 10; to: 95; value: missionStore.plan.frontOverlap; unit: "%"; onValueEdited: missionStore.plan.frontOverlap = Math.round(value) }
    ParameterSlider { Layout.fillWidth: true; label: "Side Overlap"; from: 10; to: 95; value: missionStore.plan.sideOverlap; unit: "%"; onValueEdited: missionStore.plan.sideOverlap = Math.round(value) }
    SelectRow { Layout.fillWidth: true; label: "Finish Action"; value: "Return to Home"; options: ["Return to Home", "Hover", "Land"] }
}
