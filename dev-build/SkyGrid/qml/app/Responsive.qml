pragma Singleton

import QtQuick

QtObject {
    readonly property int smallScreenWidth: 1180
    readonly property int mediumScreenWidth: 1440
    readonly property int largeScreenWidth: 1720

    function isSmallScreen(width) {
        return width < smallScreenWidth
    }

    function isMediumScreen(width) {
        return width >= smallScreenWidth && width < largeScreenWidth
    }

    function isLargeScreen(width) {
        return width >= largeScreenWidth
    }

    function pageMargin(width) {
        if (isSmallScreen(width))
            return 14
        if (width < mediumScreenWidth)
            return 18
        return 24
    }

    function gap(width) {
        return isSmallScreen(width) ? 12 : 16
    }

    function sidebarWidth(width) {
        if (width < 900)
            return 64
        if (isSmallScreen(width))
            return 84
        return Math.max(280, Math.min(340, width * 0.2))
    }

    function panelWidth(width) {
        if (isSmallScreen(width))
            return Math.max(300, Math.min(380, width * 0.42))
        return Math.max(390, Math.min(520, width * 0.28))
    }

    function cardColumns(width, preferredWidth) {
        var usable = Math.max(1, width - pageMargin(width) * 2)
        var columns = Math.floor((usable + gap(width)) / (preferredWidth + gap(width)))
        if (isSmallScreen(width))
            return 1
        if (width < mediumScreenWidth)
            return Math.max(1, Math.min(2, columns))
        if (width < largeScreenWidth)
            return Math.max(2, Math.min(3, columns))
        return Math.max(3, Math.min(4, columns))
    }

    function inputHeight(width) {
        return isSmallScreen(width) ? 46 : 54
    }

    function headerHeight(width) {
        return isSmallScreen(width) ? 64 : 74
    }

    function readableTextWidth(width) {
        return Math.max(320, Math.min(720, width - pageMargin(width) * 2))
    }
}
