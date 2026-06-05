import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root
    modal: true
    focus: true
    width: 360
    height: 330
    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    signal acceptedTarget(real latitude, real longitude, real altitude, real speed)

    background: Rectangle {
        radius: 10
        color: "#ffffff"
        border.color: "#e2d8ee"
    }

    function submit() {
        var lat = Number(latField.text)
        var lon = Number(lonField.text)
        var alt = Number(altField.text)
        var speed = Number(speedField.text)
        if (isNaN(lat) || isNaN(lon) || isNaN(alt) || isNaN(speed)
                || lat < -90 || lat > 90 || lon < -180 || lon > 180 || alt < 0 || speed <= 0) {
            errorText.text = "Enter a valid latitude, longitude, altitude, and speed."
            return
        }
        manualControlManager.goToCoordinate(lat, lon, alt, speed)
        eventLogManager.logEvent("go_to_coordinate_requested", "info", "Pilot entered a go-to coordinate", {
            latitude: lat,
            longitude: lon,
            altitude_m: alt,
            speed_mps: speed
        })
        acceptedTarget(lat, lon, alt, speed)
        close()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 12

        Text { text: "GO TO COORDINATE"; color: "#2e005f"; font.pixelSize: 14; font.bold: true }
        FieldRow { id: latField; label: "Latitude"; text: telemetryStore.connected ? Number(telemetryStore.latitude).toFixed(7) : "9.0765000" }
        FieldRow { id: lonField; label: "Longitude"; text: telemetryStore.connected ? Number(telemetryStore.longitude).toFixed(7) : "7.3986000" }
        FieldRow { id: altField; label: "Altitude"; text: "30"; suffix: "m" }
        FieldRow { id: speedField; label: "Speed"; text: "5"; suffix: "m/s" }
        Text {
            id: errorText
            Layout.fillWidth: true
            color: "#ef233c"
            font.pixelSize: 10
            wrapMode: Text.WordWrap
        }
        Item { Layout.fillHeight: true }
        RowLayout {
            Layout.fillWidth: true
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 38
                text: "Cancel"
                onClicked: root.close()
                background: Rectangle { radius: 6; color: "#fbf9fe"; border.color: "#e2d8ee" }
                contentItem: Text { text: parent.text; color: "#2e005f"; font.pixelSize: 11; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            }
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 38
                text: "Go"
                onClicked: root.submit()
                background: Rectangle { radius: 6; color: "#2e005f" }
                contentItem: Text { text: parent.text; color: "#ffffff"; font.pixelSize: 11; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            }
        }
    }

    component FieldRow: RowLayout {
        property alias text: input.text
        property string label: ""
        property string suffix: ""
        Layout.fillWidth: true
        Text { text: label; color: "#111111"; font.pixelSize: 11; Layout.preferredWidth: 90 }
        TextField {
            id: input
            Layout.fillWidth: true
            Layout.preferredHeight: 34
            font.pixelSize: 11
            selectByMouse: true
            background: Rectangle { radius: 5; color: "#fbf9fe"; border.color: "#e2d8ee" }
        }
        Text { text: suffix; color: "#5f5a66"; font.pixelSize: 10; Layout.preferredWidth: suffix.length > 0 ? 28 : 0 }
    }
}
