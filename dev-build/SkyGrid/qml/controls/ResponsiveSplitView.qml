import QtQuick
import QtQuick.Controls
import "../app"

SplitView {
    id: root

    property int availableWidth: width
    property int availableHeight: height
    property int sidePanelWidth: Responsive.panelWidth(availableWidth)
    readonly property bool isSmallScreen: Responsive.isSmallScreen(availableWidth)
    readonly property bool isMediumScreen: Responsive.isMediumScreen(availableWidth)
    readonly property bool isLargeScreen: Responsive.isLargeScreen(availableWidth)

    orientation: isSmallScreen ? Qt.Vertical : Qt.Horizontal
}
