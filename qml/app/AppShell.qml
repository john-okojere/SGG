import QtQuick
import SkyGrid 1.0

Item {
    id: root
    readonly property bool showLogin: !authManager.authenticated || authManager.devicePending
    readonly property bool showStartup: authManager.authenticated && !authManager.devicePending && !appStartupManager.startupComplete

    Rectangle {
        anchors.fill: parent
        color: Theme.surface
    }

    Loader {
        id: loginLoader
        anchors.fill: parent
        active: root.showLogin
        visible: active
        source: "../screens/LoginScreen.qml"
        z: 100
    }

    Loader {
        id: startupLoader
        anchors.fill: parent
        active: root.showStartup
        visible: active
        source: "../screens/StartupLoaderScreen.qml"
        z: 90
        onLoaded: {
            item.message = appStartupManager.startupMessage;
            item.progress = appStartupManager.startupProgress;
            item.offline = appStartupManager.startupState === "offline" || appStartupManager.startupState === "cached_workspace";
        }
        Connections {
            target: appStartupManager
            function onStartupChanged() {
                if (startupLoader.item) {
                    startupLoader.item.message = appStartupManager.startupMessage;
                    startupLoader.item.progress = appStartupManager.startupProgress;
                    startupLoader.item.offline = appStartupManager.startupState === "offline" || appStartupManager.startupState === "cached_workspace";
                }
            }
        }
    }

    TelemetryBar {
        id: telemetryBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: Math.min(Theme.topBarHeight, 64)
        z: 20
        visible: !root.showLogin && !root.showStartup && appState.currentScreen !== "home" && appState.currentScreen !== "help" && appState.operationalMode !== "pilot"
    }

    Router {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: telemetryBar.visible ? telemetryBar.bottom : parent.top
        anchors.bottom: parent.bottom
        visible: !root.showLogin && !root.showStartup
    }
}
