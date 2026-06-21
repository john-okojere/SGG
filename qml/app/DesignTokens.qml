import QtQuick

QtObject {
    property int pageMargin: 24
    property int denseMargin: 12
    property int panelWidth: 560
    property int metricHeight: 76
    property int viewportWidth: 1600
    property int viewportHeight: 900

    readonly property bool compactMode: viewportWidth < 1280 || viewportHeight < 720
    readonly property bool mediumMode: viewportWidth >= 1280 && viewportWidth < 1600
    readonly property bool wideMode: viewportWidth >= 1600

    readonly property int responsiveSidebarWidth: wideMode ? 332 : (mediumMode ? 292 : 260)
    readonly property int responsivePanelWidth: wideMode ? 420 : (mediumMode ? 370 : 320)
    readonly property int fontTiny: compactMode ? 9 : 10
    readonly property int fontSmall: compactMode ? 10 : 11
    readonly property int fontBody: compactMode ? 12 : 13
    readonly property int fontLabel: compactMode ? 11 : 12
    readonly property int fontTitle: compactMode ? 16 : (mediumMode ? 17 : 18)
    readonly property int fontHero: compactMode ? 24 : (mediumMode ? 27 : 30)
}
