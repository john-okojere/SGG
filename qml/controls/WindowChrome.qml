pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Window
import SkyGrid 1.0

Item {
    id: root

    property var targetWindow
    property int chromeHeight: 42
    property int resizeMargin: 6
    property color controlColor: "#ffffff"
    property color controlHoverColor: "#efe7fb"
    property color closeHoverColor: "#d92d20"
    property bool controlsVisible: true

    anchors.fill: parent
    z: 10000

    function isMaximized() {
        return targetWindow && targetWindow.visibility === Window.Maximized;
    }

    function minimizeWindow() {
        if (targetWindow && targetWindow.showMinimized)
            targetWindow.showMinimized();
    }

    function toggleMaximize() {
        if (!targetWindow)
            return;
        if (isMaximized())
            targetWindow.showNormal();
        else
            targetWindow.showMaximized();
    }

    function closeWindow() {
        if (targetWindow && targetWindow.close)
            targetWindow.close();
        else
            Qt.quit();
    }

    function startMove(mouse) {
        if (!targetWindow)
            return;
        if (targetWindow.startSystemMove && targetWindow.startSystemMove())
            return;
        if (mouse)
            dragFallback.start(mouse);
    }

    function startResize(edges) {
        if (!targetWindow || isMaximized())
            return;
        if (targetWindow.startSystemResize)
            targetWindow.startSystemResize(edges);
    }

    QtObject {
        id: dragFallback
        property bool active: false
        property real startMouseX: 0
        property real startMouseY: 0
        property real startWindowX: 0
        property real startWindowY: 0

        function start(mouse) {
            if (!root.targetWindow)
                return;
            active = true;
            startMouseX = mouse.screenX;
            startMouseY = mouse.screenY;
            startWindowX = root.targetWindow.x;
            startWindowY = root.targetWindow.y;
        }

        function update(mouse) {
            if (!active || !root.targetWindow)
                return;
            root.targetWindow.x = startWindowX + mouse.screenX - startMouseX;
            root.targetWindow.y = startWindowY + mouse.screenY - startMouseY;
        }

        function stop() {
            active = false;
        }
    }

    MouseArea {
        id: dragArea
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: root.controlsVisible ? controls.left : parent.right
        height: root.chromeHeight
        acceptedButtons: Qt.LeftButton
        hoverEnabled: false
        propagateComposedEvents: true
        onPressed: function (mouse) {
            root.startMove(mouse);
        }
        onPositionChanged: function (mouse) {
            dragFallback.update(mouse);
        }
        onReleased: dragFallback.stop()
        onCanceled: dragFallback.stop()
        onDoubleClicked: function (mouse) {
            root.toggleMaximize();
            mouse.accepted = true;
        }
    }

    Row {
        id: controls
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 6
        anchors.rightMargin: 8
        spacing: 6
        height: 32
        z: 5
        visible: root.controlsVisible
        enabled: root.controlsVisible

        ChromeButton {
            label: "-"
            tooltip: "Minimize"
            onClicked: root.minimizeWindow()
        }

        ChromeButton {
            label: root.isMaximized() ? "[]" : "^"
            tooltip: root.isMaximized() ? "Restore" : "Maximize"
            onClicked: root.toggleMaximize()
        }

        ChromeButton {
            label: "X"
            tooltip: "Close"
            closeButton: true
            onClicked: root.closeWindow()
        }
    }

    ResizeHandle {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: root.resizeMargin
        edges: Qt.LeftEdge
        cursorShape: Qt.SizeHorCursor
    }

    ResizeHandle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: root.resizeMargin
        edges: Qt.RightEdge
        cursorShape: Qt.SizeHorCursor
    }

    ResizeHandle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: root.resizeMargin
        edges: Qt.TopEdge
        cursorShape: Qt.SizeVerCursor
    }

    ResizeHandle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: root.resizeMargin
        edges: Qt.BottomEdge
        cursorShape: Qt.SizeVerCursor
    }

    ResizeHandle {
        anchors.left: parent.left
        anchors.top: parent.top
        width: root.resizeMargin * 2
        height: root.resizeMargin * 2
        edges: Qt.LeftEdge | Qt.TopEdge
        cursorShape: Qt.SizeFDiagCursor
    }

    ResizeHandle {
        anchors.right: parent.right
        anchors.top: parent.top
        width: root.resizeMargin * 2
        height: root.resizeMargin * 2
        edges: Qt.RightEdge | Qt.TopEdge
        cursorShape: Qt.SizeBDiagCursor
    }

    ResizeHandle {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: root.resizeMargin * 2
        height: root.resizeMargin * 2
        edges: Qt.LeftEdge | Qt.BottomEdge
        cursorShape: Qt.SizeBDiagCursor
    }

    ResizeHandle {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: root.resizeMargin * 2
        height: root.resizeMargin * 2
        edges: Qt.RightEdge | Qt.BottomEdge
        cursorShape: Qt.SizeFDiagCursor
    }

    component ChromeButton: Button {
        id: buttonRoot
        property string label: ""
        property string tooltip: ""
        property bool closeButton: false

        width: 32
        height: 32
        focusPolicy: Qt.NoFocus
        hoverEnabled: true
        padding: 0

        ToolTip.visible: hovered && tooltip.length > 0
        ToolTip.text: tooltip
        ToolTip.delay: 500

        background: Rectangle {
            radius: 8
            color: buttonRoot.closeButton && buttonRoot.hovered ? root.closeHoverColor : buttonRoot.hovered ? root.controlHoverColor : root.controlColor
            border.color: buttonRoot.closeButton && buttonRoot.hovered ? root.closeHoverColor : buttonRoot.hovered ? Theme.purple : "#d9cce8"
            border.width: 1
            Behavior on color {
                ColorAnimation {
                    duration: Theme.animFast
                }
            }
            Behavior on border.color {
                ColorAnimation {
                    duration: Theme.animFast
                }
            }
        }

        contentItem: Text {
            text: buttonRoot.label
            color: buttonRoot.closeButton && buttonRoot.hovered ? "#ffffff" : Theme.purple
            font.pixelSize: buttonRoot.label === "-" ? 18 : 12
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    component ResizeHandle: MouseArea {
        property int edges: Qt.LeftEdge
        acceptedButtons: Qt.LeftButton
        hoverEnabled: true
        enabled: root.targetWindow && !root.isMaximized()
        preventStealing: true
        z: 2
        onPressed: root.startResize(edges)
    }
}
