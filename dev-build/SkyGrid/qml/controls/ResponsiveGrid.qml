import QtQuick
import QtQuick.Layouts
import "../app"

GridLayout {
    id: root

    property int availableWidth: width
    property int preferredCardWidth: 360
    property int maxColumns: 4
    readonly property bool isSmallScreen: Responsive.isSmallScreen(availableWidth)
    readonly property bool isMediumScreen: Responsive.isMediumScreen(availableWidth)
    readonly property bool isLargeScreen: Responsive.isLargeScreen(availableWidth)

    columns: Math.max(1, Math.min(maxColumns, Responsive.cardColumns(availableWidth, preferredCardWidth)))
    rowSpacing: Responsive.gap(availableWidth)
    columnSpacing: Responsive.gap(availableWidth)
}
