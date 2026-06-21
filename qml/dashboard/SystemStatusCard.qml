import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property var rows: []

    signal detailsRequested()

    readonly property string footerStatus: statusFromRows()
    readonly property color footerColor: footerStatus === "OK" ? "#28b947" : (footerStatus === "REVIEW" ? "#f4b000" : "#7e778a")
    readonly property string footerText: footerStatus === "OK" ? "Systems ready" : (footerStatus === "REVIEW" ? "Review required" : "Status pending")

    function statusFromRows() {
        var items = root.rows || []
        if (items.length === 0)
            return "PENDING"
        for (var i = 0; i < items.length; ++i) {
            if (!items[i].ok)
                return "REVIEW"
        }
        return "OK"
    }

    radius: 10
    color: hover.hovered ? "#fbfaff" : "#ffffff"
    border.color: hover.hovered ? "#cfc4eb" : "#e2dceb"
    border.width: 1
    clip: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 10

        Text {
            text: "System Status"
            color: "#14111d"
            font.pixelSize: 16
            font.bold: true
        }

        ScrollView {
            id: statusRows
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: availableWidth
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ColumnLayout {
                width: statusRows.availableWidth
                spacing: 6

                Repeater {
                    model: root.rows
                    delegate: RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 22
                        Text {
                            Layout.fillWidth: true
                            text: modelData.label
                            color: "#332c42"
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }
                        Text {
                            text: modelData.value
                            color: "#211a30"
                            font.pixelSize: 10
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        Rectangle {
                            Layout.preferredWidth: 7
                            Layout.preferredHeight: 7
                            radius: 4
                            color: modelData.ok ? "#28b947" : "#f4b000"
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: root.footerStatus
                color: root.footerColor
                font.pixelSize: 12
                font.bold: true
            }
            Text {
                Layout.fillWidth: true
                text: root.footerText
                color: root.footerColor
                font.pixelSize: 12
                font.bold: true
            }
        }
    }

    HoverHandler {
        id: hover
    }

    TapHandler {
        onTapped: root.detailsRequested()
    }
}
