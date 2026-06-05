import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

ColumnLayout {
    id: root
    spacing: 8

    function backendLabel() {
        if (missionStore.plan.backendUploadEligible || missionStore.plan.boundaryOnly) return "Control Center ready"
        if (missionStore.plan.backendValidationState === "blocked") return "Control Center review needed"
        return "Backend validation required"
    }

    Text {
        text: "Mission Ready: " + missionStore.plan.missionReadiness + "%"
        color: Theme.ink
        font.pixelSize: 18
        font.bold: true
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 8
        StatusPill {
            Layout.fillWidth: true
            iconText: "●"
            value: root.backendLabel()
            accent: missionStore.plan.backendUploadEligible || missionStore.plan.boundaryOnly ? Theme.green
                 : (missionStore.plan.backendValidationState === "blocked" ? Theme.red : Theme.amber)
        }
        StatusPill {
            Layout.fillWidth: true
            iconText: "●"
            value: preflightChecklistManager.blocked ? "Preflight blocked"
                 : (preflightChecklistManager.cautionRequired ? "Caution required" : "Preflight ready")
            accent: preflightChecklistManager.blocked ? Theme.red
                 : (preflightChecklistManager.cautionRequired ? Theme.amber : Theme.green)
        }
    }

    Rectangle {
        Layout.fillWidth: true
        height: 10
        radius: 5
        color: "#d8cde5"
        Rectangle {
            width: parent.width * missionStore.plan.missionReadiness / 100
            height: parent.height
            radius: 5
            color: missionStore.plan.missionReadiness >= 85 ? Theme.green : (missionStore.plan.missionReadiness >= 65 ? Theme.amber : Theme.red)
            Behavior on width { NumberAnimation { duration: Theme.anim; easing.type: Easing.OutCubic } }
        }
    }

    Repeater {
        model: missionStore.plan.validationChecks
        delegate: Rectangle {
            Layout.fillWidth: true
            height: 34
            radius: 6
            color: modelData.passed ? "#edf9f1" : (modelData.severity === "critical" ? "#ffe9e9" : (modelData.severity === "warning" ? "#fff6d8" : "#edf6ff"))
            border.color: modelData.passed ? "#b9e8c6" : (modelData.severity === "critical" ? "#efaaaa" : (modelData.severity === "warning" ? "#efd06c" : "#b8d6ee"))
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                AssetIcon {
                    Layout.preferredWidth: 18
                    Layout.preferredHeight: 18
                    source: modelData.passed ? AssetRegistry.icons.boxicons_save : (modelData.severity === "critical" ? AssetRegistry.icons.iconoir_cancel : AssetRegistry.icons.boxicons_filter)
                    active: true
                }
                Text { text: modelData.label; color: Theme.ink; font.pixelSize: 13; font.bold: true; Layout.fillWidth: true }
                Text { text: modelData.passed ? "PASS" : modelData.severity.toUpperCase(); color: Theme.muted; font.pixelSize: 11; font.bold: true }
            }
        }
    }

    Repeater {
        model: missionStore.plan.validationWarnings
        delegate: Rectangle {
            Layout.fillWidth: true
            height: 46
            radius: 6
            color: modelData.severity === "critical" ? "#ffe9e9" : (modelData.severity === "warning" ? "#fff6d8" : "#edf6ff")
            border.color: modelData.severity === "critical" ? "#efaaaa" : (modelData.severity === "warning" ? "#efd06c" : "#b8d6ee")
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                text: modelData.text
                color: Theme.ink
                font.pixelSize: 13
                elide: Text.ElideRight
            }
        }
    }

    Rectangle {
        visible: missionStore.plan.validationWarnings.length === 0
        Layout.fillWidth: true
        height: 42
        radius: 6
        color: "#edf9f1"
        border.color: "#b9e8c6"
        Text { anchors.centerIn: parent; text: "All critical checks are passing."; color: "#1f7a3c"; font.pixelSize: 14; font.bold: true }
    }
}
