import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    radius: 8
    color: "#ffffff"
    border.color: "#e2dceb"
    implicitHeight: profileContent.implicitHeight + 32
    readonly property bool narrow: width < 720

    function loadActive() {
        var profile = vehicleProfileManager.activeProfile || {};
        nameField.text = profile.name || "";
        modelField.text = profile.model || "";
        serialField.text = profile.serial_number || profile.serial || "";
        firmwareField.text = profile.firmware_version || "";
        airframeField.text = profile.airframe_type || "";
    }

    ColumnLayout {
        id: profileContent
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text {
                    Layout.fillWidth: true
                    text: "Step 2 / 3 - Vehicle Identity & Profile"
                    color: "#171222"
                    font.pixelSize: 16
                    font.bold: true
                }
                Text {
                    Layout.fillWidth: true
                    text: vehicleProfileManager.status + " - saved profiles are returned to pilots after release."
                    color: "#706a7e"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
            }
            Button {
                Layout.preferredWidth: 82
                Layout.preferredHeight: 32
                text: "Refresh"
                enabled: permissionManager.canViewFleet || permissionManager.canConfigureVehicle
                onClicked: vehicleProfileManager.refreshProfiles()
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? "#f5f0fb" : "#ffffff"
                    border.color: "#d8ceeb"
                }
                contentItem: Text {
                    text: parent.text
                    color: "#3b0787"
                    font.pixelSize: 11
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: root.narrow ? 1 : 2
            rowSpacing: 12
            columnSpacing: 12
            ListView {
                Layout.fillWidth: root.narrow
                Layout.preferredWidth: root.narrow ? Math.max(220, root.width - 32) : 150
                Layout.preferredHeight: root.narrow ? 132 : 226
                clip: true
                model: vehicleProfileManager.profiles
                delegate: Rectangle {
                    width: ListView.view.width
                    height: 42
                    radius: 8
                    color: String(modelData.id) === vehicleProfileManager.activeProfileId() ? "#f0e8fb" : (profileMouse.containsMouse ? "#fbf8ff" : "#ffffff")
                    border.color: "#e2dceb"
                    Text {
                        anchors.fill: parent
                        anchors.margins: 8
                        text: modelData.name || modelData.model || ("Profile " + modelData.id)
                        color: "#171222"
                        font.pixelSize: 11
                        font.bold: true
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                    MouseArea {
                        id: profileMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: vehicleProfileManager.selectProfile(String(modelData.id))
                    }
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: root.width < 520 ? 1 : 2
                rowSpacing: 8
                columnSpacing: 10
                FieldLabel {
                    text: "Name"
                }
                TextField {
                    id: nameField
                    Layout.fillWidth: true
                    placeholderText: "SkyGrid X8 profile"
                }
                FieldLabel {
                    text: "Model"
                }
                TextField {
                    id: modelField
                    Layout.fillWidth: true
                    placeholderText: "X8"
                }
                FieldLabel {
                    text: "Serial"
                }
                TextField {
                    id: serialField
                    Layout.fillWidth: true
                    placeholderText: "SGX8-001"
                }
                FieldLabel {
                    text: "Firmware"
                }
                TextField {
                    id: firmwareField
                    Layout.fillWidth: true
                    placeholderText: "PX4 1.14"
                }
                FieldLabel {
                    text: "Airframe"
                }
                TextField {
                    id: airframeField
                    Layout.fillWidth: true
                    placeholderText: "multirotor"
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                text: vehicleProfileManager.activeProfileId().length > 0 ? "Save Profile" : "Create Profile"
                enabled: permissionManager.canEditVehicleProfile || permissionManager.canRegisterVehicle
                onClicked: {
                    var profile = {};
                    if (vehicleProfileManager.activeProfileId().length > 0)
                        profile.id = vehicleProfileManager.activeProfileId();
                    profile.name = nameField.text;
                    profile.model = modelField.text;
                    profile.serial_number = serialField.text;
                    profile.firmware_version = firmwareField.text;
                    profile.airframe_type = airframeField.text;
                    vehicleProfileManager.saveProfile(profile);
                }
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? "#4f0aa6" : "#3b0787"
                }
                contentItem: Text {
                    text: parent.text
                    color: "#ffffff"
                    font.pixelSize: 11
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Button {
                Layout.preferredWidth: 110
                Layout.preferredHeight: 36
                text: "New"
                enabled: permissionManager.canRegisterVehicle
                onClicked: {
                    vehicleProfileManager.selectProfile("");
                    nameField.text = "";
                    modelField.text = "";
                    serialField.text = "";
                    firmwareField.text = "";
                    airframeField.text = "";
                }
                background: Rectangle {
                    radius: 8
                    color: parent.hovered ? "#f5f0fb" : "#ffffff"
                    border.color: "#d8ceeb"
                }
                contentItem: Text {
                    text: parent.text
                    color: "#3b0787"
                    font.pixelSize: 11
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    Component.onCompleted: loadActive()

    Connections {
        target: vehicleProfileManager
        function onProfilesChanged() {
            if (!nameField.activeFocus && !modelField.activeFocus && !serialField.activeFocus && !firmwareField.activeFocus && !airframeField.activeFocus)
                root.loadActive();
        }
    }

    component FieldLabel: Label {
        color: "#706a7e"
        font.pixelSize: 11
        font.bold: true
        verticalAlignment: Text.AlignVCenter
    }
}
