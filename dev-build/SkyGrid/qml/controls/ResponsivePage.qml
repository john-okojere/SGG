import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../app"

ScrollView {
    id: root

    property alias content: contentLayout.data
    property int pageWidth: width
    property int margin: Responsive.pageMargin(pageWidth)
    property int gap: Responsive.gap(pageWidth)
    readonly property bool isSmallScreen: Responsive.isSmallScreen(pageWidth)
    readonly property bool isMediumScreen: Responsive.isMediumScreen(pageWidth)
    readonly property bool isLargeScreen: Responsive.isLargeScreen(pageWidth)

    clip: true
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AsNeeded
    contentWidth: availableWidth

    ColumnLayout {
        id: contentLayout
        width: Math.max(0, root.availableWidth - root.margin * 2)
        x: root.margin
        spacing: root.gap
    }
}
