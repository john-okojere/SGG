import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    radius: 24
    color: enabled ? "#4b0f8178" : "#4b0f813f"
    border.color: enabled ? "#ffffff8f" : "#ffffff55"
    border.width: 1
    opacity: enabled ? 1 : 0.72

    property real leftX: 0
    property real leftY: 0
    property real rightX: 0
    property real rightY: 0
    property double lastManualActionAt: 0

    function sendInput() {
        if (!enabled) {
            manualControlManager.neutral()
            return
        }
        manualControlManager.setInput(-leftY, leftX, -rightY, rightX)
        recordManualInput(-leftY, leftX, -rightY, rightX)
    }

    function recordManualInput(forward, lateral, vertical, yaw) {
        var now = Date.now()
        if (now - lastManualActionAt < 250) return
        lastManualActionAt = now
        if (typeof pilotActionSyncManager !== "undefined") {
            pilotActionSyncManager.recordAction("manual_input_active", "Pilot joystick input active", {
                forward: forward,
                lateral: lateral,
                vertical: vertical,
                yaw: yaw
            })
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 22

        Stick {
            Layout.preferredWidth: 140
            Layout.preferredHeight: 148
            label: "ROLL / PITCH"
            xValue: root.leftX
            yValue: root.leftY
            onValuesChanged: function(x, y) {
                root.leftX = x
                root.leftY = y
                root.sendInput()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 132
            radius: 10
            color: "#2e005f55"
            border.color: "#ffffff70"
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 9
                Button {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 156
                    Layout.preferredHeight: 36
                    enabled: root.enabled
                    onClicked: {
                        root.leftX = root.leftY = root.rightX = root.rightY = 0
                        manualControlManager.neutral()
                        vehicleActionManager.holdPosition()
                    }
                    background: Rectangle {
                        radius: 18
                        color: parent.hovered ? "#ffffffee" : "#ffffffc8"
                        border.color: "#ffffff"
                    }
                    contentItem: Row {
                        spacing: 8
                        anchors.centerIn: parent
                        Text { text: "◉"; color: "#5b22a8"; font.pixelSize: 13; anchors.verticalCenter: parent.verticalCenter }
                        Text { text: "Hold position"; color: "#5b22a8"; font.pixelSize: 12; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                StatValue { label: "ALTITUDE (AGL)"; value: Number(telemetryStore.altitude).toFixed(1) + " m"; icon: "↑" }
                StatValue { label: "SPEED (GROUND)"; value: Number(telemetryStore.speed).toFixed(1) + " m/s"; icon: "" }
            }
        }

        Stick {
            Layout.preferredWidth: 140
            Layout.preferredHeight: 148
            label: "YAW / THROTTLE"
            xValue: root.rightX
            yValue: root.rightY
            onValuesChanged: function(x, y) {
                root.rightX = x
                root.rightY = y
                root.sendInput()
            }
        }
    }

    component StatValue: ColumnLayout {
        property string label: ""
        property string value: ""
        property string icon: ""
        Layout.fillWidth: true
        spacing: 0
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: (icon.length > 0 ? icon + " " : "") + value
            color: "#ffffff"
            font.pixelSize: 19
            font.bold: true
        }
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: label
            color: "#ffffffdd"
            font.pixelSize: 10
            font.bold: true
        }
    }

    component Stick: Item {
        id: stick
        property string label: ""
        property real xValue: 0
        property real yValue: 0
        signal valuesChanged(real x, real y)
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            text: stick.label
            color: "#ffffff"
            font.pixelSize: 10
            font.bold: true
        }
        Rectangle {
            id: ring
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            width: 124
            height: 124
            radius: 62
            color: "#6d2eb85c"
            border.color: "#ffffffdd"
            border.width: 1
            Rectangle {
                anchors.centerIn: parent
                width: 90
                height: 90
                radius: 45
                color: "transparent"
                border.color: "#ffffff88"
            }
            Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.topMargin: 9; text: "⌃"; color: "#ffffff"; font.pixelSize: 22 }
            Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 8; text: "⌄"; color: "#ffffff"; font.pixelSize: 22 }
            Text { anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; anchors.leftMargin: 13; text: "‹"; color: "#ffffff"; font.pixelSize: 26 }
            Text { anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; anchors.rightMargin: 13; text: "›"; color: "#ffffff"; font.pixelSize: 26 }
            Rectangle {
                id: thumb
                width: 38
                height: 38
                radius: 19
                color: "#ffffff"
                border.color: "#e2d8ee"
                x: ring.width / 2 - width / 2 + stick.xValue * 38
                y: ring.height / 2 - height / 2 + stick.yValue * 38
                Behavior on x { NumberAnimation { duration: 100; easing.type: Easing.OutCubic } }
                Behavior on y { NumberAnimation { duration: 100; easing.type: Easing.OutCubic } }
            }
            MouseArea {
                anchors.fill: parent
                onPressed: update(mouse.x, mouse.y)
                onPositionChanged: if (pressed) update(mouse.x, mouse.y)
                onReleased: {
                    stick.valuesChanged(0, 0)
                }
                function update(mx, my) {
                    var dx = mx - ring.width / 2
                    var dy = my - ring.height / 2
                    var maxR = 42
                    var len = Math.sqrt(dx * dx + dy * dy)
                    if (len > maxR) {
                        dx = dx / len * maxR
                        dy = dy / len * maxR
                    }
                    stick.valuesChanged(dx / maxR, dy / maxR)
                }
            }
        }
    }
}
