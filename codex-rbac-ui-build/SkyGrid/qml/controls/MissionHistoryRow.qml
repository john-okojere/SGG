import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    property string name: ""
    property string date: ""
    property string operatorName: ""
    property string type: ""
    property string status: ""
    property url iconSource: AssetRegistry.icons.lucide_route

    height: 82
    color: "transparent"

    RowLayout {
        anchors.fill: parent
        spacing: 14
        Rectangle {
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
            radius: 24
            color: Theme.purple
            AssetIcon {
                anchors.centerIn: parent
                width: 25
                height: 25
                source: root.iconSource
                active: true
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Text { text: root.name; color: Theme.ink; font.pixelSize: 18; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
            Text { text: root.date + " · " + root.operatorName; color: Theme.muted; font.pixelSize: 13 }
            Text { text: root.type; color: Theme.purple; font.pixelSize: 13; font.bold: true }
        }
        Rectangle {
            Layout.preferredWidth: 34
            Layout.preferredHeight: 34
            radius: 17
            color: root.status === "Warning" ? "#fff5db" : "#f5f1fa"
            border.color: root.status === "Warning" ? Theme.amber : Theme.line
            AssetIcon {
                anchors.centerIn: parent
                width: 18
                height: 18
                source: root.status === "Synced" ? AssetRegistry.icons.cloud : (root.status === "Warning" ? AssetRegistry.icons.iconoir_cancel : AssetRegistry.icons.boxicons_save)
                active: true
            }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        color: "#ded6e8"
    }
}
