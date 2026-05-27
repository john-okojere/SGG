pragma Singleton

import QtQuick

QtObject {
    readonly property color bg: "#07050c"
    readonly property color surface: "#f4f1f8"
    readonly property color surfaceAlt: "#ebe7f2"
    readonly property color ink: "#0e0b14"
    readonly property color muted: "#766f82"
    readonly property color line: "#d7cde5"
    readonly property color purple: "#2c0057"
    readonly property color purple2: "#4b128b"
    readonly property color purpleSoft: "#7c45b8"
    readonly property color glass: "#160720d9"
    readonly property color glassSoft: "#210b32c8"
    readonly property color green: "#22c55e"
    readonly property color amber: "#f7c948"
    readonly property color red: "#ef4444"
    readonly property color white: "#ffffff"

    readonly property int topBarHeight: 96
    readonly property int toolRailWidth: 96
    readonly property int panelRadius: 8
    readonly property int controlRadius: 6
    readonly property int spacing: 14
    readonly property int animFast: 140
    readonly property int anim: 220

    readonly property string fontFamily: "Inter, Arial, sans-serif"
}
