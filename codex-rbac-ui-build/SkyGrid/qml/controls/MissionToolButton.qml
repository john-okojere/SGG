import QtQuick
import QtQuick.Controls
import SkyGrid 1.0

IconButton {
    id: root
    property string tool: ""
    property string label: tool
    active: appState.selectedTool === tool
    iconOnAccent: true
    onClicked: appState.selectedTool = tool
    ToolTip.visible: hovered
    ToolTip.text: label
}
