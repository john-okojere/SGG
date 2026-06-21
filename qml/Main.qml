import QtQuick
import QtQuick.Controls
import SkyGrid 1.0
import "app"
import "controls"

ApplicationWindow {
    id: rootWindow
    width: 1280
    height: 720
    minimumWidth: 900
    minimumHeight: 540
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.CustomizeWindowHint
    title: "SkyGrid GCS"
    color: Theme.surface
    visible: true

    AppShell {
        anchors.fill: parent
    }

    WindowChrome {
        targetWindow: rootWindow
        controlsVisible: typeof authManager === "undefined"
                         || !authManager.authenticated
                         || authManager.devicePending
    }
}
