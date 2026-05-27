import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root
    modal: false
    focus: true
    width: Math.min(520, parent ? parent.width * 0.34 : 520)
    height: parent ? parent.height - 120 : 720
    x: parent ? parent.width - width - 24 : 0
    y: 92
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property string filter: "all"

    background: Rectangle {
        radius: 10
        color: "#ffffff"
        border.color: "#e2d8ee"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10
        RowLayout {
            Layout.fillWidth: true
            Text { text: "EVENT LOG"; color: "#2e005f"; font.pixelSize: 14; font.bold: true; Layout.fillWidth: true }
            ComboBox {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 30
                model: ["all", "info", "warning", "error"]
                currentIndex: 0
                font.pixelSize: 10
                onActivated: root.filter = currentText
                background: Rectangle { radius: 5; color: "#fbf9fe"; border.color: "#e2d8ee" }
            }
            Button {
                Layout.preferredWidth: 28
                Layout.preferredHeight: 28
                text: "×"
                onClicked: root.close()
                background: Rectangle { radius: 14; color: parent.hovered ? "#f3eef8" : "transparent" }
            }
        }
        Text {
            Layout.fillWidth: true
            text: gcsEventSyncManager.status
            color: "#6d6377"
            font.pixelSize: 10
            elide: Text.ElideRight
        }
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 8
            model: eventLogManager.events
            delegate: Rectangle {
                width: ListView.view.width
                height: (root.filter === "all" || modelData.severity === root.filter) ? 68 : 0
                visible: height > 0
                radius: 7
                color: "#fbf9fe"
                border.color: "#ece3f3"
                property color sevColor: modelData.severity === "error" ? "#ef233c" : (modelData.severity === "warning" ? "#f5c542" : "#5b22a8")
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10
                    Rectangle { Layout.preferredWidth: 8; Layout.preferredHeight: 8; radius: 4; color: sevColor }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text {
                            Layout.fillWidth: true
                            text: modelData.message || modelData.event_type
                            color: "#111111"
                            font.pixelSize: 11
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        Text {
                            Layout.fillWidth: true
                            text: (modelData.event_type || "") + " · " + (modelData.recorded_at || "")
                            color: "#6d6377"
                            font.pixelSize: 9
                            elide: Text.ElideRight
                        }
                    }
                    Text {
                        text: modelData.severity || "info"
                        color: sevColor
                        font.pixelSize: 9
                        font.bold: true
                    }
                }
            }
        }
    }
}
