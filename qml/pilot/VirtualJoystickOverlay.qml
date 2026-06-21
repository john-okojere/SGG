import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    visible: accessManager.can("can_fly_manual")
    radius: root.compact ? 18 : 24
    color: enabled ? "#4b0f8178" : "#4b0f813f"
    border.color: enabled ? "#ffffff8f" : "#ffffff55"
    border.width: 1
    opacity: enabled ? 1 : 0.72
    clip: true

    property real leftX: 0
    property real leftY: 0
    property real rightX: 0
    property real rightY: 0
    property double lastManualActionAt: 0
    readonly property bool compact: width < 470 || height < 160
    readonly property int stickWidth: compact ? 108 : 140
    readonly property int stickHeight: compact ? 118 : 148
    readonly property int ringSize: compact ? 94 : 124
    readonly property int innerRingSize: compact ? 68 : 90
    readonly property int thumbSize: compact ? 30 : 38
    readonly property real thumbTravel: compact ? 30 : 38
    readonly property real maxStickRadius: compact ? 32 : 42

    function sendInput() {
        if (!enabled || !accessManager.canPerform("manual_flight")) {
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
        anchors.margins: root.compact ? 14 : 24
        spacing: root.compact ? 12 : 22

        Stick {
            Layout.preferredWidth: root.stickWidth
            Layout.preferredHeight: root.stickHeight
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
            Layout.preferredHeight: root.compact ? 112 : 132
            radius: 10
            color: "#2e005f55"
            border.color: "#ffffff70"
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: root.compact ? 8 : 12
                spacing: root.compact ? 6 : 9
                Button {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: root.compact ? 128 : 156
                    Layout.preferredHeight: root.compact ? 32 : 36
                    visible: accessManager.can("can_fly_manual")
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
                        spacing: 6
                        anchors.centerIn: parent
                        Text { text: "o"; color: "#5b22a8"; font.pixelSize: 13; anchors.verticalCenter: parent.verticalCenter }
                        Text { text: "Hold"; color: "#5b22a8"; font.pixelSize: root.compact ? 11 : 12; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                StatValue { label: "ALTITUDE"; value: Number(telemetryStore.altitude).toFixed(1) + " m"; icon: "^" }
                StatValue { label: "GROUND SPEED"; value: Number(telemetryStore.speed).toFixed(1) + " m/s"; icon: "" }
            }
        }

        Stick {
            Layout.preferredWidth: root.stickWidth
            Layout.preferredHeight: root.stickHeight
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
            font.pixelSize: root.compact ? 15 : 19
            font.bold: true
            elide: Text.ElideRight
        }
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: label
            color: "#ffffffdd"
            font.pixelSize: root.compact ? 8 : 10
            font.bold: true
            elide: Text.ElideRight
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
            font.pixelSize: root.compact ? 8 : 10
            font.bold: true
            elide: Text.ElideRight
        }
        Rectangle {
            id: ring
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            width: root.ringSize
            height: root.ringSize
            radius: width / 2
            color: "#6d2eb85c"
            border.color: "#ffffffdd"
            border.width: 1
            Rectangle {
                anchors.centerIn: parent
                width: root.innerRingSize
                height: root.innerRingSize
                radius: width / 2
                color: "transparent"
                border.color: "#ffffff88"
            }
            Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.topMargin: root.compact ? 6 : 9; text: "^"; color: "#ffffff"; font.pixelSize: root.compact ? 16 : 22 }
            Text { anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: root.compact ? 6 : 8; text: "v"; color: "#ffffff"; font.pixelSize: root.compact ? 16 : 22 }
            Text { anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; anchors.leftMargin: root.compact ? 8 : 13; text: "<"; color: "#ffffff"; font.pixelSize: root.compact ? 20 : 26 }
            Text { anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; anchors.rightMargin: root.compact ? 8 : 13; text: ">"; color: "#ffffff"; font.pixelSize: root.compact ? 20 : 26 }
            Rectangle {
                id: thumb
                width: root.thumbSize
                height: root.thumbSize
                radius: width / 2
                color: "#ffffff"
                border.color: "#e2d8ee"
                x: ring.width / 2 - width / 2 + stick.xValue * root.thumbTravel
                y: ring.height / 2 - height / 2 + stick.yValue * root.thumbTravel
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
                    var maxR = root.maxStickRadius
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
