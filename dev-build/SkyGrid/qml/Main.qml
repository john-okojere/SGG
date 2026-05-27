import QtQuick
import QtQuick.Controls
import SkyGrid 1.0
import "app"

ApplicationWindow {
    id: window
    width: 1600
    height: 980
    minimumWidth: 1180
    minimumHeight: 760
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.CustomizeWindowHint
    title: "SkyGrid GCS"
    color: Theme.surface
    visible: true

    AppShell {
        anchors.fill: parent
    }
}
