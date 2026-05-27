import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property url imageSource: ""
    property string aircraftName: "SkyGrid X8-01"
    property string tailNumber: "SGX8-01"
    property real battery: 78
    property string linkQuality: "Strong"
    property string lastSync: "2 min ago"
    property bool ready: true

    signal clicked()

    radius: 8
    color: mouse.containsMouse ? "#fbfaff" : "#ffffff"
    border.color: mouse.containsMouse ? "#cfc4eb" : "#e0d8eb"
    border.width: 1
    clip: true

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 86
            color: "#f5f3f8"
            Image {
                anchors.fill: parent
                anchors.margins: 8
                source: root.imageSource
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 12
            spacing: 7

            Rectangle {
                Layout.preferredWidth: readyLabel.implicitWidth + 18
                Layout.preferredHeight: 20
                radius: 10
                color: root.ready ? "#dff8e5" : "#fff4d8"
                Text {
                    id: readyLabel
                    anchors.centerIn: parent
                    text: root.ready ? "Ready" : "Caution"
                    color: root.ready ? "#168a35" : "#a36b00"
                    font.pixelSize: 10
                    font.bold: true
                }
            }

            Text {
                Layout.fillWidth: true
                text: root.aircraftName
                color: "#14111d"
                font.pixelSize: 15
                font.bold: true
                elide: Text.ElideRight
            }

            Text {
                Layout.fillWidth: true
                text: "Tail #: " + root.tailNumber
                color: "#706a7e"
                font.pixelSize: 11
                elide: Text.ElideRight
            }

            MetricBar { label: "Battery"; value: root.battery; suffix: "%" }
            MetricBar { label: "Link"; value: root.linkQuality === "Strong" ? 96 : 72; suffix: ""; textValue: root.linkQuality }

            RowLayout {
                Layout.fillWidth: true
                Text { text: "Last Sync"; color: "#706a7e"; font.pixelSize: 10; Layout.fillWidth: true }
                Text { text: root.lastSync; color: "#171222"; font.pixelSize: 10; font.bold: true }
            }
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }

    component MetricBar: ColumnLayout {
        id: metric
        property string label: ""
        property real value: 0
        property string suffix: "%"
        property string textValue: ""
        spacing: 3

        RowLayout {
            Layout.fillWidth: true
            Text { text: metric.label; color: "#2b2638"; font.pixelSize: 10; Layout.fillWidth: true }
            Text {
                text: metric.textValue.length > 0 ? metric.textValue : Math.round(metric.value) + metric.suffix
                color: "#2b2638"
                font.pixelSize: 10
                font.bold: true
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 4
            radius: 2
            color: "#eee9f5"
            Rectangle {
                width: parent.width * Math.max(0, Math.min(100, metric.value)) / 100
                height: parent.height
                radius: 2
                color: metric.value > 35 ? "#3d16a5" : "#ef4444"
            }
        }
    }
}
