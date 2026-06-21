import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#f6f4fa"

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: Math.max(0, root.width - (root.width < 520 ? 32 : 48))
            x: root.width < 520 ? 16 : 24
            y: root.width < 520 ? 16 : 24
            spacing: 14

            Text { Layout.fillWidth: true; text: "Manual Test Mode"; color: "#14111d"; font.pixelSize: root.width < 520 ? 20 : 24; font.bold: true; elide: Text.ElideRight }
            Text { Layout.fillWidth: true; text: "Manufacturer-only manual test controls use existing local vehicle action guards. Backend test session recording is not connected yet."; color: "#706a7e"; font.pixelSize: 13; wrapMode: Text.WordWrap }

            StatusRow { label: "Permission"; value: accessManager.can("can_fly_manual_test") ? "Allowed" : "Blocked" }
            StatusRow { label: "Session"; value: sessionManager.operationsAllowed ? "Trusted" : sessionManager.blockReason }
            StatusRow { label: "Aircraft Link"; value: vehicleManager.connected ? "Connected" : "Not connected" }
            StatusRow { label: "Backend Workflow"; value: "Not connected yet" }

            Flow {
                Layout.fillWidth: true
                Layout.preferredHeight: childrenRect.height
                spacing: 10
                Button {
                    text: "Hold"
                    visible: accessManager.can("can_fly_manual_test")
                    enabled: sessionManager.operationsAllowed && vehicleManager.connected
                    onClicked: {
                        manualControlManager.neutral()
                        vehicleActionManager.holdPosition()
                    }
                }
                Button {
                    text: "Land"
                    visible: accessManager.can("can_fly_manual_test")
                    enabled: sessionManager.operationsAllowed && vehicleManager.connected
                    onClicked: vehicleActionManager.land()
                }
            }

            StatusFooter {}
            Item { Layout.preferredHeight: root.width < 520 ? 16 : 24 }
        }
    }

    component StatusRow: RowLayout {
        property string label: ""
        property string value: ""
        Layout.fillWidth: true
        Text { Layout.preferredWidth: root.width < 520 ? 112 : 150; text: label; color: "#706a7e"; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
        Text { Layout.fillWidth: true; text: value; color: "#14111d"; font.pixelSize: 13; elide: Text.ElideRight }
    }
}
