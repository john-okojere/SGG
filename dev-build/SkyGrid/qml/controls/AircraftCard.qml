import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    property string name: ""
    property string role: ""
    property string battery: ""
    property string status: ""

    radius: 8
    color: "#ffffff"
    border.color: "#e2d9ec"
    height: 126

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8
        RowLayout {
            Layout.fillWidth: true
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                AssetIcon {
                    Layout.preferredWidth: 22
                    Layout.preferredHeight: 22
                    source: AssetRegistry.icons.plane
                    active: true
                }
                Text { text: root.name; color: Theme.ink; font.pixelSize: 19; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
            }
            StatusPill { value: root.status; iconSource: root.status === "Maintenance" ? AssetRegistry.icons.iconoir_cancel : AssetRegistry.icons.boxicons_wifi; accent: root.status === "Maintenance" ? Theme.amber : Theme.green; implicitWidth: 126; implicitHeight: 34 }
        }
        Text { text: root.role; color: Theme.muted; font.pixelSize: 14 }
        Text { text: "Battery " + root.battery; color: Theme.purple; font.pixelSize: 20; font.bold: true }
    }
}
