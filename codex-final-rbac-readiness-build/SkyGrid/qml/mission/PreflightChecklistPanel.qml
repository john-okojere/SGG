import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

ColumnLayout {
    id: root
    spacing: 8

    function aircraftId() {
        return missionSyncManager.assignedAircraft.length > 0
            ? String(missionSyncManager.assignedAircraft[0].id)
            : ""
    }

    function missionCenter() {
        var source = missionStore.plan.polygon.length > 0
            ? missionStore.plan.polygon
            : missionStore.plan.waypoints
        if (missionStore.plan.hasPoi) {
            return { latitude: missionStore.plan.poi.latitude, longitude: missionStore.plan.poi.longitude }
        }
        if (source.length === 0) {
            return { latitude: mapState.centerLatitude, longitude: mapState.centerLongitude }
        }
        var lat = 0
        var lon = 0
        for (var i = 0; i < source.length; ++i) {
            lat += source[i].latitude
            lon += source[i].longitude
        }
        return { latitude: lat / source.length, longitude: lon / source.length }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 8
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Text {
                text: "Pre-flight Checklist"
                color: Theme.ink
                font.pixelSize: 18
                font.bold: true
            }
            Text {
                text: preflightChecklistManager.status
                color: Theme.muted
                font.pixelSize: 11
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }
        Text {
            text: preflightChecklistManager.blocked ? "BLOCKED"
                 : (preflightChecklistManager.cautionRequired ? "CAUTION" : "READY")
            color: preflightChecklistManager.blocked ? Theme.red
                 : (preflightChecklistManager.cautionRequired ? Theme.amber : Theme.green)
            font.pixelSize: 11
            font.bold: true
        }
    }

    Rectangle {
        Layout.fillWidth: true
        height: 10
        radius: 5
        color: "#d8cde5"
        Rectangle {
            width: parent.width * preflightChecklistManager.readinessPercent / 100
            height: parent.height
            radius: 5
            color: preflightChecklistManager.blocked ? Theme.red
                 : (preflightChecklistManager.cautionRequired ? Theme.amber : Theme.green)
            Behavior on width { NumberAnimation { duration: Theme.anim; easing.type: Easing.OutCubic } }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 8
        PrimaryButton {
            Layout.fillWidth: true
            secondary: true
            text: "Run Pre-flight"
            iconSource: AssetRegistry.icons.boxicons_play
            onClicked: {
                if (typeof weatherSyncManager !== "undefined" && missionStore.plan) {
                    var center = root.missionCenter()
                    weatherSyncManager.refreshForMission(center.latitude, center.longitude, missionStore.plan.altitude, root.aircraftId(), "MISSION")
                }
                if (missionStore.plan) {
                    missionStore.plan.validateMission()
                }
                preflightChecklistManager.runChecklist(true)
            }
        }
        PrimaryButton {
            visible: preflightChecklistManager.cautionRequired
            Layout.preferredWidth: 150
            secondary: true
            text: "Acknowledge"
            onClicked: preflightChecklistManager.acknowledgeAllCautions()
        }
    }

    Repeater {
        model: preflightChecklistManager.checks
        delegate: Rectangle {
            Layout.fillWidth: true
            implicitHeight: 52
            radius: 6
            color: modelData.passed ? "#edf9f1"
                 : (modelData.severity === "critical" ? "#ffe9e9" : (modelData.severity === "warning" ? "#fff6d8" : "#edf6ff"))
            border.color: modelData.passed ? "#b9e8c6"
                 : (modelData.severity === "critical" ? "#efaaaa" : (modelData.severity === "warning" ? "#efd06c" : "#b8d6ee"))

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 2
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    Text {
                        text: modelData.category
                        color: Theme.purple
                        font.pixelSize: 10
                        font.bold: true
                    }
                    Text {
                        Layout.fillWidth: true
                        text: modelData.label
                        color: Theme.ink
                        font.pixelSize: 13
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Text {
                        text: modelData.passed ? "PASS" : modelData.severity.toUpperCase()
                        color: Theme.muted
                        font.pixelSize: 10
                        font.bold: true
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        Layout.fillWidth: true
                        text: modelData.message
                        color: Theme.muted
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                    }
                    Button {
                        visible: !modelData.passed && modelData.severity === "warning" && !modelData.acknowledged
                        text: "Ack"
                        onClicked: preflightChecklistManager.acknowledgeCheck(modelData.id)
                        background: Rectangle { radius: 4; color: parent.hovered ? "#eee8f7" : "#f8f5fb"; border.color: "#ddd2e9" }
                    }
                }
            }
        }
    }

    Rectangle {
        visible: preflightChecklistManager.blocked || preflightChecklistManager.cautionRequired
        Layout.fillWidth: true
        implicitHeight: warningText.implicitHeight + 20
        radius: 6
        color: preflightChecklistManager.blocked ? "#ffe9e9" : "#fff6d8"
        border.color: preflightChecklistManager.blocked ? "#efaaaa" : "#efd06c"
        Text {
            id: warningText
            anchors.fill: parent
            anchors.margins: 10
            text: preflightChecklistManager.blocked
                ? ("Flight operations blocked: " + preflightChecklistManager.blockReason())
                : "Caution items require acknowledgement before upload, arm, or mission start."
            color: Theme.ink
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }
    }
}
