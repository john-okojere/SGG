pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    color: "#f6f4fa"

    property string query: ""
    property string selectedTitle: ""
    readonly property bool compact: width < 980
    readonly property color ink: "#14111d"
    readonly property color muted: "#706a7e"
    readonly property color line: "#e2dceb"
    readonly property color purple: "#3b0787"

    function resultsModel() {
        return root.query.length > 1 ? helpCenterManager.search(root.query) : helpCenterManager.sections
    }

    function selectSection(title) {
        root.selectedTitle = title
    }

    function selectedText() {
        if (root.selectedTitle.length > 0)
            return helpCenterManager.sectionText(root.selectedTitle)
        return helpCenterManager.documentText
    }

    Component.onCompleted: {
        var quick = helpCenterManager.quickStarts
        if (quick.length > 0)
            root.selectedTitle = quick[0].title
        else if (helpCenterManager.sections.length > 0)
            root.selectedTitle = helpCenterManager.sections[0].title
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.compact ? 96 : 78
            color: "#ffffff"
            border.color: root.line

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: root.compact ? 14 : 24
                anchors.rightMargin: root.compact ? 14 : 24
                spacing: root.compact ? 10 : 16

                Image {
                    Layout.preferredWidth: root.compact ? 128 : 190
                    Layout.preferredHeight: 44
                    source: AssetRegistry.logos.full_logo
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    mipmap: true
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.fillWidth: true
                        text: "Help Center"
                        color: root.ink
                        font.pixelSize: root.compact ? 20 : 24
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Text {
                        Layout.fillWidth: true
                        text: helpCenterManager.status
                        color: root.muted
                        font.pixelSize: 11
                        elide: Text.ElideMiddle
                    }
                }

                Rectangle {
                    Layout.preferredWidth: root.compact ? 210 : 340
                    Layout.preferredHeight: 42
                    radius: 8
                    color: "#fbfaff"
                    border.color: root.line

                    AssetIcon {
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        width: 17
                        height: 17
                        source: AssetRegistry.icons.boxicons_search_big
                        active: true
                    }

                    TextField {
                        anchors.fill: parent
                        anchors.leftMargin: 38
                        anchors.rightMargin: 10
                        placeholderText: "Search guide"
                        text: root.query
                        onTextChanged: root.query = text
                        color: root.ink
                        font.pixelSize: 13
                        background: Item {}
                    }
                }

                Button {
                    Layout.preferredWidth: 104
                    Layout.preferredHeight: 40
                    text: "Dashboard"
                    hoverEnabled: true
                    ToolTip.text: "Return to Dashboard"
                    ToolTip.visible: hovered
                    onClicked: appState.goHome()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                Layout.preferredWidth: root.compact ? 280 : 342
                Layout.fillHeight: true
                visible: width > 0
                color: "#fbfaff"
                border.color: root.line

                ScrollView {
                    anchors.fill: parent
                    contentWidth: availableWidth

                    ColumnLayout {
                        x: 14
                        y: 14
                        width: parent.width - 28
                        spacing: 12

                        SectionHeader { text: "Role Guides" }
                        Repeater {
                            model: helpCenterManager.roleGuides
                            HelpNavRow {
                                width: parent.width
                                title: modelData.title
                                detail: modelData.excerpt
                                active: root.selectedTitle === modelData.title
                                onClicked: root.selectSection(modelData.title)
                            }
                        }

                        SectionHeader { text: "Quick Starts" }
                        Repeater {
                            model: helpCenterManager.quickStarts
                            HelpNavRow {
                                width: parent.width
                                title: modelData.title
                                detail: modelData.excerpt
                                active: root.selectedTitle === modelData.title
                                onClicked: root.selectSection(modelData.title)
                            }
                        }

                        SectionHeader { text: root.query.length > 1 ? "Search Results" : "All Sections" }
                        Repeater {
                            model: root.resultsModel()
                            HelpNavRow {
                                width: parent.width
                                title: modelData.title
                                detail: modelData.excerpt
                                active: root.selectedTitle === modelData.title
                                onClicked: root.selectSection(modelData.title)
                            }
                        }

                        SectionHeader { text: "FAQ" }
                        Repeater {
                            model: helpCenterManager.faq
                            HelpNavRow {
                                width: parent.width
                                title: modelData.title
                                detail: modelData.excerpt
                                active: root.selectedTitle === modelData.title
                                onClicked: root.selectSection(modelData.title)
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#ffffff"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: root.compact ? 18 : 28
                    spacing: 14

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 3
                            Text {
                                Layout.fillWidth: true
                                text: root.selectedTitle.length > 0 ? root.selectedTitle : "SkyGrid GCS Guide"
                                color: root.ink
                                font.pixelSize: root.compact ? 24 : 30
                                font.bold: true
                                wrapMode: Text.WordWrap
                            }
                            Text {
                                Layout.fillWidth: true
                                text: root.query.length > 1 ? root.resultsModel().length + " matching sections" : helpCenterManager.sections.length + " guide sections"
                                color: root.muted
                                font.pixelSize: 12
                                elide: Text.ElideRight
                            }
                        }
                        Button {
                            Layout.preferredWidth: 90
                            Layout.preferredHeight: 38
                            text: "Reload"
                            hoverEnabled: true
                            ToolTip.text: "Reload the guide from disk or resource"
                            ToolTip.visible: hovered
                            onClicked: helpCenterManager.reload()
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: root.line
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        contentWidth: availableWidth
                        clip: true

                        Text {
                            width: parent.width
                            text: root.selectedText()
                            textFormat: Text.PlainText
                            wrapMode: Text.WordWrap
                            color: root.ink
                            font.pixelSize: 14
                            lineHeight: 1.22
                        }
                    }
                }
            }
        }
    }

    component SectionHeader: Text {
        Layout.fillWidth: true
        text: ""
        color: "#30006f"
        font.pixelSize: 11
        font.bold: true
    }

    component HelpNavRow: Rectangle {
        id: navRow
        property string title: ""
        property string detail: ""
        property bool active: false
        signal clicked()

        height: Math.max(58, titleText.implicitHeight + detailText.implicitHeight + 22)
        radius: 8
        color: active ? "#efe7fb" : (mouse.containsMouse ? "#f6f1fb" : "#ffffff")
        border.color: active ? root.purple : root.line
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 3
            Text {
                id: titleText
                Layout.fillWidth: true
                text: navRow.title
                color: root.ink
                font.pixelSize: 13
                font.bold: true
                elide: Text.ElideRight
            }
            Text {
                id: detailText
                Layout.fillWidth: true
                text: navRow.detail
                color: root.muted
                font.pixelSize: 11
                maximumLineCount: 2
                elide: Text.ElideRight
                wrapMode: Text.WordWrap
            }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: navRow.clicked()
        }
    }
}
