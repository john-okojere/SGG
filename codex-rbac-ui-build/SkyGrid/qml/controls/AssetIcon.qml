import QtQuick
import SkyGrid 1.0

Item {
    id: root

    property url source: ""
    property real iconSize: Math.min(width, height)
    property bool active: false
    property bool hovered: false
    property real inactiveOpacity: 0.76
    readonly property string sourceText: source.toString()
    readonly property bool isSvgSource: sourceText.indexOf(".svg") !== -1
    readonly property bool fastMode: String(performanceMode).toLowerCase() === "fast"
    readonly property real rasterScale: fastMode || iconSize <= 32 ? 1 : 2

    function svgProviderSource() {
        var clean = sourceText.split("?")[0]
        return "image://svgicon/" + clean.substring(clean.lastIndexOf("/") + 1)
    }

    function glyphForSource() {
        var s = sourceText.toLowerCase()
        if (s.indexOf("battery") !== -1) return "▰"
        if (s.indexOf("camera") !== -1) return "◉"
        if (s.indexOf("cursor") !== -1) return "⌖"
        if (s.indexOf("filter") !== -1) return "≡"
        if (s.indexOf("search") !== -1) return "⌕"
        if (s.indexOf("git-branch") !== -1 || s.indexOf("route") !== -1) return "⌁"
        if (s.indexOf("play") !== -1) return "▷"
        if (s.indexOf("save") !== -1) return "▣"
        if (s.indexOf("trash") !== -1) return "⌫"
        if (s.indexOf("undo") !== -1) return "↶"
        if (s.indexOf("wifi") !== -1) return "≋"
        if (s.indexOf("wind") !== -1) return "≈"
        if (s.indexOf("cloud") !== -1) return "☁"
        if (s.indexOf("compass") !== -1) return "⌾"
        if (s.indexOf("cube") !== -1) return "◇"
        if (s.indexOf("edit") !== -1) return "✎"
        if (s.indexOf("cancel") !== -1) return "×"
        if (s.indexOf("building") !== -1) return "▥"
        if (s.indexOf("grid") !== -1) return "▦"
        if (s.indexOf("mountain") !== -1) return "△"
        if (s.indexOf("satellite") !== -1) return "◌"
        if (s.indexOf("waypoint") !== -1) return "⌘"
        if (s.indexOf("person") !== -1) return "◍"
        if (s.indexOf("check") !== -1) return "✓"
        if (s.indexOf("home") !== -1) return "⌂"
        if (s.indexOf("pin") !== -1) return "●"
        if (s.indexOf("plane") !== -1) return "✈"
        if (s.indexOf("plus") !== -1) return "+"
        return "•"
    }

    implicitWidth: 24
    implicitHeight: 24

    Image {
        id: image
        anchors.centerIn: parent
        width: Math.min(root.width, root.iconSize)
        height: Math.min(root.height, root.iconSize)
        source: root.isSvgSource ? root.svgProviderSource() : root.source
        visible: root.sourceText.length > 0
        fillMode: Image.PreserveAspectFit
        smooth: !root.fastMode && root.iconSize > 18
        mipmap: !root.fastMode && root.iconSize > 32
        sourceSize.width: Math.ceil(width * root.rasterScale)
        sourceSize.height: Math.ceil(height * root.rasterScale)
        opacity: root.enabled ? (root.active || root.hovered ? 1 : root.inactiveOpacity) : 0.42
        Behavior on opacity { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
    }

    Text {
        anchors.centerIn: parent
        visible: root.isSvgSource && image.status === Image.Error
        text: root.glyphForSource()
        color: root.active || root.hovered ? Theme.purple : Theme.ink
        opacity: root.enabled ? (root.active || root.hovered ? 1 : root.inactiveOpacity) : 0.42
        font.pixelSize: Math.max(10, Math.min(root.width, root.height, root.iconSize) * 0.82)
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        Behavior on opacity { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
    }
}
