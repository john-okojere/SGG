import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    radius: 7
    color: "#ffffff"
    border.color: "#e2d8ee"
    border.width: 1
    property int activeIndex: appState.operationalMode === "pilot" ? 1 : 0

    RowLayout {
        anchors.fill: parent
        spacing: 0

        TabButton {
            Layout.fillWidth: true
            label: "MISSION"
            iconSource: AssetRegistry.icons.boxicons_save
            active: root.activeIndex === 0
            onClicked: appState.goHome()
        }
        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#ece4f3" }
        TabButton {
            Layout.fillWidth: true
            label: "MANUAL FLIGHT"
            iconSource: AssetRegistry.icons.plane
            active: root.activeIndex === 1
            onClicked: appState.startPilotMode()
        }
    }

    component TabButton: Button {
        id: tab
        property string label: ""
        property url iconSource: ""
        property bool active: false
        hoverEnabled: true
        background: Rectangle {
            color: tab.active ? "#f7f1fd" : (tab.hovered ? "#fbf8fe" : "#ffffff")
            radius: 7
            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 2
                color: tab.active ? "#2e005f" : "transparent"
            }
        }
        contentItem: Row {
            spacing: 8
            anchors.centerIn: parent
            AssetIcon {
                width: 16
                height: 16
                source: tab.iconSource
                active: tab.active
                hovered: tab.hovered
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: tab.label
                color: tab.active ? "#2e005f" : "#111111"
                font.pixelSize: 12
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
