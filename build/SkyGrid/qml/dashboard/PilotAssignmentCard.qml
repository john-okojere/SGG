import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property string pilotName: "John Okojere"
    property string organization: "Nigerian Air Force"
    property string unitSystem: "METRIC"
    property string assignment: "Unassigned"
    property string clearance: "Level 3"
    property string lastLogin: "May 20, 2026 - 08:21"

    signal pilotModeRequested()
    signal changePilotRequested()

    radius: 10
    color: "#ffffff"
    border.color: "#e2dceb"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 14

        Text {
            text: "Pilot Assignment"
            color: "#14111d"
            font.pixelSize: 16
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            Rectangle {
                Layout.preferredWidth: 46
                Layout.preferredHeight: 46
                radius: 23
                gradient: Gradient {
                    GradientStop { position: 0; color: "#6e20d3" }
                    GradientStop { position: 1; color: "#28056f" }
                }
                Text { anchors.centerIn: parent; text: "P"; color: "#ffffff"; font.pixelSize: 20; font.bold: true }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text { Layout.fillWidth: true; text: root.pilotName; color: "#14111d"; font.pixelSize: 15; font.bold: true; elide: Text.ElideRight }
                Text { Layout.fillWidth: true; text: root.organization; color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
            }
        }

        DetailRow { label: "Unit System"; value: root.unitSystem }
        DetailRow { label: "Current Assignment"; value: root.assignment }
        DetailRow { label: "Clearance Level"; value: root.clearance }
        DetailRow { label: "Last Login"; value: root.lastLogin }

        Item { Layout.fillHeight: true }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            ActionButton {
                Layout.fillWidth: true
                text: "Pilot Mode"
                primary: true
                onClicked: root.pilotModeRequested()
            }
            ActionButton {
                Layout.fillWidth: true
                text: "Change Pilot"
                onClicked: root.changePilotRequested()
            }
        }
    }

    component DetailRow: RowLayout {
        id: detail
        property string label: ""
        property string value: ""
        Layout.fillWidth: true
        Layout.preferredHeight: 24
        Text { Layout.fillWidth: true; text: detail.label; color: "#4a4359"; font.pixelSize: 11; font.bold: true }
        Text { text: detail.value; color: "#332c42"; font.pixelSize: 11; font.bold: true; elide: Text.ElideRight }
    }

    component ActionButton: Rectangle {
        id: action
        property string text: ""
        property bool primary: false
        signal clicked()
        Layout.preferredHeight: 36
        radius: 7
        color: primary ? (buttonMouse.containsMouse ? "#4f0aa6" : "#3b0787") : (buttonMouse.containsMouse ? "#f7f3fb" : "#ffffff")
        border.color: primary ? "#3b0787" : "#cfc4eb"
        Text { anchors.centerIn: parent; text: action.text; color: action.primary ? "#ffffff" : "#3b0787"; font.pixelSize: 11; font.bold: true }
        MouseArea { id: buttonMouse; anchors.fill: parent; hoverEnabled: true; onClicked: action.clicked() }
    }
}
