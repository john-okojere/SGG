import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root
    radius: 7
    color: "#ffffff"
    border.color: "#e2d8ee"

    property real maxAltitude: 120
    property real maxSpeed: 8.0
    property string flightMode: "Stabilized"
    signal moreSettingsRequested()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 13

        Text { text: "MANUAL SETTINGS"; color: "#3b294d"; font.pixelSize: 11; font.bold: true }
        CompactSlider { label: "Max Altitude"; from: 20; to: 180; value: root.maxAltitude; unit: "m"; onMovedValue: root.maxAltitude = value }
        CompactSlider { label: "Speed"; from: 1; to: 16; value: root.maxSpeed; unit: "m/s"; decimals: 1; onMovedValue: root.maxSpeed = value }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 34
            Text { text: "Flight Mode"; color: "#111111"; font.pixelSize: 11; Layout.fillWidth: true }
            ComboBox {
                Layout.preferredWidth: 150
                Layout.preferredHeight: 32
                model: ["Stabilized", "Position", "Altitude", "Manual"]
                currentIndex: 0
                font.pixelSize: 11
                onActivated: root.flightMode = currentText
                background: Rectangle { radius: 4; color: "#fbf9fe"; border.color: "#e2d8ee" }
            }
        }
        Item { Layout.fillHeight: true }
        RowLayout {
            id: moreRow
            Layout.fillWidth: true
            Text { text: "More Settings"; color: "#2e005f"; font.pixelSize: 11; font.bold: true; Layout.fillWidth: true }
            Text { text: "›"; color: "#2e005f"; font.pixelSize: 18; font.bold: true }
            TapHandler { onTapped: root.moreSettingsRequested() }
        }
    }

    component CompactSlider: ColumnLayout {
        id: sliderRow
        property string label: ""
        property real from: 0
        property real to: 100
        property real value: 0
        property string unit: ""
        property int decimals: 0
        signal movedValue(real value)
        Layout.fillWidth: true
        spacing: 7
        RowLayout {
            Layout.fillWidth: true
            Text { text: sliderRow.label; color: "#111111"; font.pixelSize: 11; Layout.fillWidth: true }
            Rectangle {
                Layout.preferredWidth: 62
                Layout.preferredHeight: 30
                radius: 4
                color: "#fbf9fe"
                border.color: "#e2d8ee"
                Text {
                    anchors.centerIn: parent
                    text: Number(sliderRow.value).toFixed(sliderRow.decimals) + " " + sliderRow.unit
                    color: "#111111"
                    font.pixelSize: 11
                    font.bold: true
                }
            }
        }
        Slider {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            from: sliderRow.from
            to: sliderRow.to
            value: sliderRow.value
            onMoved: sliderRow.movedValue(value)
            background: Rectangle {
                x: parent.leftPadding
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                width: parent.availableWidth
                height: 3
                radius: 2
                color: "#ece7f1"
                Rectangle { width: parent.parent.visualPosition * parent.width; height: parent.height; radius: 2; color: "#2e005f" }
            }
            handle: Rectangle {
                x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                width: 10
                height: 10
                radius: 5
                color: "#2e005f"
            }
        }
    }
}
