import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#f6f4fa"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: root.width < 520 ? 16 : 24
        spacing: 14

        Text { Layout.fillWidth: true; text: "Vehicle Release / Lock Status"; color: "#14111d"; font.pixelSize: root.width < 520 ? 20 : 24; font.bold: true; elide: Text.ElideRight }
        Text { Layout.fillWidth: true; text: "Release the selected profile to an organization, lock it, or fetch configuration audit records."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }
        Text { Layout.fillWidth: true; text: "Current status: " + (manufacturerVehicleManager.selectedProfile.status || "No profile selected"); color: "#14111d"; font.pixelSize: 13; font.bold: true; elide: Text.ElideRight }

        TextField { id: orgField; Layout.fillWidth: true; placeholderText: "Organization id" }
        TextField { id: notesField; Layout.fillWidth: true; placeholderText: "Release notes" }

        Flow {
            Layout.fillWidth: true
            Layout.preferredHeight: childrenRect.height
            spacing: 10
            Button {
                text: "Release"
                visible: accessManager.can("can_release_vehicle_to_organization")
                enabled: !manufacturerVehicleManager.loading && manufacturerVehicleManager.selectedProfile.id !== undefined && orgField.text.length > 0
                onClicked: manufacturerVehicleManager.releaseVehicleToOrganization(manufacturerVehicleManager.selectedProfile.id, orgField.text, notesField.text)
            }
            Button {
                text: "Lock"
                visible: accessManager.can("can_edit_vehicle_profile")
                enabled: !manufacturerVehicleManager.loading && manufacturerVehicleManager.selectedProfile.id !== undefined
                onClicked: manufacturerVehicleManager.lockVehicle(manufacturerVehicleManager.selectedProfile.id)
            }
            Button {
                text: "Fetch Audit"
                visible: accessManager.can("can_view_vehicle_audit")
                enabled: !manufacturerVehicleManager.loading && manufacturerVehicleManager.selectedProfile.id !== undefined
                onClicked: manufacturerVehicleManager.fetchVehicleAudit(manufacturerVehicleManager.selectedProfile.id)
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: manufacturerVehicleManager.auditRecords
            delegate: Rectangle {
                width: ListView.view.width
                height: 54
                radius: 7
                color: "#ffffff"
                border.color: "#e2dceb"
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    Text { Layout.fillWidth: true; text: modelData.action || modelData.event_type || "Audit record"; color: "#14111d"; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
                    Text { text: modelData.created_at || modelData.timestamp || ""; color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
                }
            }
        }

        StatusFooter {}
    }
}
