import QtQuick
import SkyGrid 1.0

Item {
    id: root
    readonly property bool showLogin: !authManager.authenticated || authManager.devicePending

    Rectangle {
        anchors.fill: parent
        color: Theme.surface
    }

    Loader {
        anchors.fill: parent
        active: root.showLogin
        visible: active
        source: "../screens/LoginScreen.qml"
        z: 100
    }

    TelemetryBar {
        id: telemetryBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: Math.min(Theme.topBarHeight, 64)
        z: 20
        visible: !root.showLogin && appState.currentScreen !== "home" && appState.operationalMode !== "pilot"
    }

    Router {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: telemetryBar.visible ? telemetryBar.bottom : parent.top
        anchors.bottom: parent.bottom
        visible: !root.showLogin
    }
}
