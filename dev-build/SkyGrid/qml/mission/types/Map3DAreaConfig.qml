import QtQuick
import QtQuick.Layouts
import "../../controls"

ColumnLayout {
    spacing: 10
    SegmentedControl { Layout.fillWidth: true; options: ["Basic", "Advanced"] }
    SelectRow { Layout.fillWidth: true; label: "Camera Model"; value: "Zenmuse X5S 70mm"; options: ["Zenmuse X5S 70mm", "Phantom 4 Pro Camera", "Mavic 3E Camera"] }
    SelectRow { Layout.fillWidth: true; label: "Capture Mode"; value: "Oblique Grid Capture"; options: ["Oblique Grid Capture", "Nadir Grid", "Double Grid"] }
    SelectRow { Layout.fillWidth: true; label: "Layer Mode"; value: "3 Layers"; options: ["1 Layer", "2 Layers", "3 Layers", "5 Layers"] }
    ParameterSlider { Layout.fillWidth: true; label: "Speed"; from: 2; to: 140; value: missionStore.plan.speed; unit: "m/s"; onValueEdited: missionStore.plan.speed = value }
    ParameterSlider { Layout.fillWidth: true; label: "Flight Alt."; from: 20; to: 160; value: missionStore.plan.altitude; decimals: 1; unit: "ft"; onValueEdited: missionStore.plan.altitude = value }
    ParameterSlider { Layout.fillWidth: true; label: "Front Overlap"; from: 10; to: 95; value: missionStore.plan.frontOverlap; unit: "%"; onValueEdited: missionStore.plan.frontOverlap = Math.round(value) }
    ParameterSlider { Layout.fillWidth: true; label: "Side Overlap"; from: 10; to: 95; value: missionStore.plan.sideOverlap; unit: "%"; onValueEdited: missionStore.plan.sideOverlap = Math.round(value) }
    SelectRow { Layout.fillWidth: true; label: "Finish Action"; value: "Return to Home"; options: ["Return to Home", "Hover", "Land"] }
}
