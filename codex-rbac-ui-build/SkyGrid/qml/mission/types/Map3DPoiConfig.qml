import QtQuick
import QtQuick.Layouts
import "../../controls"

ColumnLayout {
    spacing: 10
    SegmentedControl { Layout.fillWidth: true; options: ["Basic", "Advanced"] }
    SelectRow { Layout.fillWidth: true; label: "Camera Model"; value: "Zenmuse X5S 70mm"; options: ["Zenmuse X5S 70mm", "Phantom 4 Pro Camera", "Mavic 3E Camera"] }
    SelectRow { Layout.fillWidth: true; label: "Flight Course Mode"; value: "Circle Mode"; options: ["Circle Mode", "Spiral", "Orbit Stack"] }
    SelectRow { Layout.fillWidth: true; label: "Capture Mode"; value: "Capture at Equal Dist. Interval"; options: ["Capture at Equal Dist. Interval", "Timed Capture", "Video Orbit"] }
    ParameterSlider { Layout.fillWidth: true; label: "Speed"; from: 2; to: 140; value: missionStore.plan.speed; unit: "m/s"; onValueEdited: missionStore.plan.speed = value }
    ParameterSlider { Layout.fillWidth: true; label: "Flight Rad."; from: 30; to: 1200; value: missionStore.plan.radius; decimals: 1; unit: "ft"; onValueEdited: missionStore.plan.radius = value }
    ParameterSlider { Layout.fillWidth: true; label: "Building Rad."; from: 5; to: 600; value: missionStore.plan.buildingRadius; decimals: 1; unit: "ft"; onValueEdited: missionStore.plan.buildingRadius = value }
    ParameterSlider { Layout.fillWidth: true; label: "Min Alt."; from: 20; to: 160; value: missionStore.plan.altitude; decimals: 1; unit: "ft"; onValueEdited: missionStore.plan.altitude = value }
    SelectRow { Layout.fillWidth: true; label: "Latitude"; value: "34.15413572" }
    SelectRow { Layout.fillWidth: true; label: "Longitude"; value: "-84.21239517" }
}
