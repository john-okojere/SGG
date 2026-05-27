import QtQuick
import QtQuick.Layouts
import "../../controls"

ColumnLayout {
    spacing: 10
    SegmentedControl { Layout.fillWidth: true; options: ["Basic", "Advanced"] }
    SelectRow { Layout.fillWidth: true; label: "Camera Model"; value: "Phantom 4 Pro Camera"; options: ["Phantom 4 Pro Camera", "Mavic 3T", "Zenmuse H20T"] }
    SelectRow { Layout.fillWidth: true; label: "Shooting Angle"; value: "Parallel To Main Path"; options: ["Parallel To Main Path", "Toward Structure", "Oblique"] }
    SelectRow { Layout.fillWidth: true; label: "Capture Mode"; value: "Capture at Equal Dist. Interval"; options: ["Capture at Equal Dist. Interval", "Video", "Hover Capture"] }
    SelectRow { Layout.fillWidth: true; label: "Flight Course Mode"; value: "Inside Mode"; options: ["Inside Mode", "Outside Mode", "Vertical Scan"] }
    ParameterSlider { Layout.fillWidth: true; label: "Speed"; from: 2; to: 140; value: missionStore.plan.speed; unit: "m/s"; onValueEdited: missionStore.plan.speed = value }
    ParameterSlider { Layout.fillWidth: true; label: "Min Alt."; from: 20; to: 160; value: missionStore.plan.altitude; decimals: 1; unit: "ft"; onValueEdited: missionStore.plan.altitude = value }
    SelectRow { Layout.fillWidth: true; label: "Latitude"; value: "34.15413572" }
    SelectRow { Layout.fillWidth: true; label: "Longitude"; value: "-84.21239517" }
    SelectRow { Layout.fillWidth: true; label: "Finish Action"; value: "Return to Home"; options: ["Return to Home", "Hover", "Land"] }
}
