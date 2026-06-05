import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Item {
    id: root
    anchors.fill: parent
    visible: false
    z: 100

    signal dismissed()
    signal checklistCompleted()

    function aircraftId() {
        return missionSyncManager.assignedAircraft.length > 0 ? String(missionSyncManager.assignedAircraft[0].id) : ""
    }

    function missionCenter() {
        var source = missionStore.plan.polygon.length > 0 ? missionStore.plan.polygon : missionStore.plan.waypoints
        if (missionStore.plan.hasPoi) return { latitude: missionStore.plan.poi.latitude, longitude: missionStore.plan.poi.longitude }
        if (source.length === 0) return { latitude: mapState.centerLatitude, longitude: mapState.centerLongitude }
        var lat = 0
        var lon = 0
        for (var i = 0; i < source.length; ++i) {
            lat += source[i].latitude
            lon += source[i].longitude
        }
        return { latitude: lat / source.length, longitude: lon / source.length }
    }

    function routeItems() {
        return missionStore.plan.generatedRoute
    }

    function runAndContinue() {
        var center = missionCenter()
        weatherSyncManager.refreshForMission(center.latitude, center.longitude, missionStore.plan.altitude, aircraftId(), "MISSION")
        missionStore.plan.validateMission()
        preflightChecklistManager.runChecklist(true)

        if (missionStore.plan.boundaryOnly) {
            missionSyncManager.saveActiveMission()
            return
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#00000078"
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        preventStealing: true
        onWheel: function(wheel) { wheel.accepted = true }
    }

    Rectangle {
        id: card
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 780)
        height: Math.min(parent.height - 56, 930)
        radius: 4
        color: "#ffffff"
        clip: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 72
                color: Theme.purple
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 22
                    anchors.rightMargin: 16
                    Text {
                        Layout.fillWidth: true
                        text: "Pre-flight Checklist"
                        color: Theme.white
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: 24
                        font.bold: true
                    }
                    IconButton {
                        implicitWidth: 34
                        implicitHeight: 34
                        iconText: "X"
                        active: true
                        iconOnAccent: true
                        onClicked: root.dismissed()
                        ToolTip.visible: hovered
                        ToolTip.text: "Dismiss checklist"
                    }
                }
            }

            Flickable {
                id: flick
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                contentWidth: width
                contentHeight: checklistColumn.implicitHeight
                boundsBehavior: Flickable.StopAtBounds

                ColumnLayout {
                    id: checklistColumn
                    width: flick.width
                    spacing: 0

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 54
                        color: "#ffffff"
                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 68
                            anchors.rightMargin: 68
                            Text {
                                Layout.fillWidth: true
                                text: preflightChecklistManager.status
                                color: Theme.muted
                                font.pixelSize: 14
                                elide: Text.ElideRight
                            }
                            Text {
                                text: preflightChecklistManager.blocked ? "FAIL"
                                      : (preflightChecklistManager.cautionRequired ? "CAUTION" : "READY")
                                color: preflightChecklistManager.blocked ? Theme.red
                                     : (preflightChecklistManager.cautionRequired ? "#ffc400" : "#009e18")
                                font.pixelSize: 13
                                font.bold: true
                            }
                        }
                    }

                    Repeater {
                        model: preflightChecklistManager.checks
                        delegate: ChecklistRow {
                            width: checklistColumn.width
                            title: modelData.label || modelData.category || "Check"
                            value: modelData.message || ""
                            severity: modelData.passed ? "ok" : (modelData.severity || "warning")
                            muted: !modelData.passed && modelData.severity !== "critical" && modelData.severity !== "warning"
                            onAcknowledge: preflightChecklistManager.acknowledgeCheck(modelData.id)
                            canAcknowledge: !modelData.passed && modelData.severity === "warning" && !modelData.acknowledged
                        }
                    }

                    Rectangle {
                        visible: preflightChecklistManager.blocked || preflightChecklistManager.cautionRequired
                        Layout.fillWidth: true
                        Layout.leftMargin: 68
                        Layout.rightMargin: 68
                        Layout.topMargin: 8
                        implicitHeight: warningText.implicitHeight + 22
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
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                        }
                    }

                    PrimaryButton {
                        Layout.fillWidth: true
                        Layout.leftMargin: 68
                        Layout.rightMargin: 68
                        Layout.topMargin: 28
                        Layout.bottomMargin: 36
                        Layout.preferredHeight: 56
                        text: missionStore.plan.boundaryOnly ? "SYNC BOUNDARY" : "START FLYING"
                        enabled: sessionManager.operationsAllowed && !missionUploadManager.uploading && !missionExecutionManager.executing
                        onClicked: root.runAndContinue()
                    }
                }
            }
        }
    }

    component ChecklistRow: Rectangle {
        id: row
        property string title: ""
        property string value: ""
        property string severity: "ok"
        property bool muted: false
        property bool canAcknowledge: false
        signal acknowledge()

        Layout.fillWidth: true
        implicitHeight: Math.max(82, valueText.implicitHeight + 34)
        color: "#ffffff"

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 68
            anchors.rightMargin: 68
            height: 1
            color: "#e4dfeb"
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 78
            anchors.rightMargin: 68
            spacing: 20

            Text {
                text: row.severity === "critical" ? "!"
                      : (row.severity === "warning" ? "!" : "✓")
                color: row.severity === "critical" ? Theme.red
                     : (row.severity === "warning" ? "#ffc400" : "#009e18")
                font.pixelSize: 17
                font.bold: true
            }

            Text {
                Layout.preferredWidth: 250
                text: row.title
                color: row.muted ? "#bdb9c5"
                     : (row.severity === "warning" ? "#ffc400" : (row.severity === "critical" ? Theme.red : "#009e18"))
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
            }

            Text {
                id: valueText
                Layout.fillWidth: true
                text: row.value
                color: "#85808a"
                font.pixelSize: 16
                wrapMode: Text.WordWrap
                maximumLineCount: 3
                elide: Text.ElideRight
            }

            Button {
                visible: row.canAcknowledge
                text: "Ack"
                onClicked: row.acknowledge()
                background: Rectangle { radius: 5; color: parent.hovered ? "#eee8f7" : "#f8f5fb"; border.color: "#ddd2e9" }
            }
        }
    }
}
