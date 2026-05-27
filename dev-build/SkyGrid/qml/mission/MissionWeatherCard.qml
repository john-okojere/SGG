import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    Layout.fillWidth: true
    implicitHeight: 116
    radius: 8
    color: "#ffffff"
    border.color: weatherBorder()

    function weatherBorder() {
        if (windCheckManager.severity === "critical") return "#ef233c66"
        if (windCheckManager.severity === "warning") return "#f5c54288"
        if (weatherManager.available) return "#18b83f66"
        return "#e2d8ee"
    }

    function statusColor() {
        if (windCheckManager.severity === "critical") return "#ef233c"
        if (windCheckManager.severity === "warning") return "#b77900"
        if (weatherManager.available) return "#148a34"
        return Theme.muted
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 10
        columns: 4
        rowSpacing: 8
        columnSpacing: 10

        RowLayout {
            Layout.columnSpan: 3
            Layout.fillWidth: true
            spacing: 8
            AssetIcon {
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                source: AssetRegistry.icons.boxicons_wind_filled
                active: true
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0
                Text {
                    text: "Weather / UAV Forecast"
                    color: Theme.ink
                    font.pixelSize: 12
                    font.bold: true
                }
                Text {
                    text: weatherSyncManager.syncing ? "Checking Control Center weather..." : windCheckManager.status
                    color: Theme.muted
                    font.pixelSize: 10
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: 92
            Layout.preferredHeight: 30
            radius: 5
            color: statusColor() + "18"
            border.color: statusColor() + "55"
            Text {
                anchors.centerIn: parent
                text: weatherManager.available ? windCheckManager.result : "Unavailable"
                color: statusColor()
                font.pixelSize: 10
                font.bold: true
            }
        }

        Metric {
            label: "Wind"
            value: weatherManager.available ? Number(weatherManager.windSpeedMps).toFixed(1) : "--"
            unit: "m/s"
        }
        Metric {
            label: "Gust"
            value: weatherManager.available ? Number(weatherManager.windGustMps).toFixed(1) : "--"
            unit: "m/s"
        }
        Metric {
            label: "Dir"
            value: weatherManager.available ? Number(weatherManager.windDirectionDeg).toFixed(0) : "--"
            unit: "deg"
        }
        Metric {
            label: "Visibility"
            value: weatherManager.available ? Number(weatherManager.visibilityKm).toFixed(1) : "--"
            unit: "km"
        }
        Metric {
            label: "Cloud"
            value: weatherManager.available ? Number(weatherManager.cloudCover).toFixed(0) : "--"
            unit: "%"
        }
        Metric {
            label: "Precip"
            value: weatherManager.available ? Number(weatherManager.precipitationProbability).toFixed(0) : "--"
            unit: "%"
        }
        Metric {
            label: "Temp"
            value: weatherManager.available ? Number(weatherManager.temperatureC).toFixed(0) : "--"
            unit: "C"
        }
        Metric {
            label: "Source"
            value: weatherManager.available ? weatherManager.sourceState : "Waiting"
            unit: ""
        }
    }

    component Metric: ColumnLayout {
        property string label: ""
        property string value: "--"
        property string unit: ""
        spacing: 0
        Text {
            text: label
            color: Theme.muted
            font.pixelSize: 9
            font.bold: true
            elide: Text.ElideRight
            Layout.fillWidth: true
        }
        RowLayout {
            spacing: 2
            Text {
                text: value
                color: Theme.ink
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
            }
            Text {
                text: unit
                visible: unit.length > 0
                color: Theme.muted
                font.pixelSize: 9
                font.bold: true
            }
        }
    }
}
