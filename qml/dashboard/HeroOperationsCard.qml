import QtQuick 2.15
import QtQuick.Layouts 1.15
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root

    property url backgroundSource: ""
    property string title: "Mission Operations Center"
    property string subtitle: "Real-time overview of your operations and system status"
    property var metrics: []
    readonly property bool compact: width < 900
    readonly property bool narrow: width < 620

    signal openRequested

    implicitHeight: root.narrow ? 330 : (root.compact ? 282 : 206)
    radius: 10
    clip: true
    color: "#180955"

    Image {
        anchors.fill: parent
        source: root.backgroundSource
        fillMode: Image.PreserveAspectCrop
        smooth: true
        mipmap: true
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#21066fdd"
            }
            GradientStop {
                position: 0.62
                color: "#21066f66"
            }
            GradientStop {
                position: 1.0
                color: "#09032744"
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: root.narrow ? 14 : 20
        spacing: root.narrow ? 10 : 14

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 6

                Text {
                    Layout.fillWidth: true
                    text: root.title
                    color: "#ffffff"
                    font.pixelSize: root.narrow ? 18 : 22
                    font.bold: true
                    wrapMode: Text.WordWrap
                    maximumLineCount: root.narrow ? 2 : 1
                    elide: root.narrow ? Text.ElideNone : Text.ElideRight
                }

                Text {
                    Layout.fillWidth: true
                    text: root.subtitle
                    color: "#eee9ff"
                    font.pixelSize: root.narrow ? 10 : 11
                    font.bold: true
                    wrapMode: Text.WordWrap
                    maximumLineCount: root.narrow ? 3 : 1
                    elide: root.narrow ? Text.ElideNone : Text.ElideRight
                }
            }

            Rectangle {
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                Layout.alignment: Qt.AlignTop
                radius: 6
                color: openMouse.containsMouse ? "#ffffff33" : "#ffffff18"
                border.color: "#ffffff40"

                Text {
                    anchors.centerIn: parent
                    text: ">"
                    color: "#ffffff"
                    font.pixelSize: 14
                    font.bold: true
                }

                MouseArea {
                    id: openMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.openRequested()
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Rectangle {
            id: metricsPanel
            Layout.fillWidth: true
            Layout.preferredHeight: metricGrid.implicitHeight + (root.narrow ? 18 : 20)
            radius: 8
            color: "#200b68cc"
            border.color: "#ffffff18"
            border.width: 1

            GridLayout {
                id: metricGrid
                anchors.fill: parent
                anchors.leftMargin: root.narrow ? 10 : 16
                anchors.rightMargin: root.narrow ? 10 : 16
                anchors.topMargin: 10
                anchors.bottomMargin: 10
                columns: root.narrow ? 2 : (root.compact ? 3 : Math.max(1, root.metrics.length))
                rowSpacing: 8
                columnSpacing: 0

                Repeater {
                    model: root.metrics
                    delegate: Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 46

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            spacing: 4

                            Text {
                                Layout.fillWidth: true
                                text: modelData.label
                                color: "#d8d1f5"
                                font.pixelSize: root.narrow ? 7 : 8
                                font.bold: true
                                elide: Text.ElideRight
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.value
                                    color: modelData.stateColor || "#ffffff"
                                    font.pixelSize: root.narrow ? 15 : 18
                                    font.bold: true
                                    elide: Text.ElideRight
                                }

                                Item {
                                    visible: !root.narrow
                                    Layout.fillWidth: true
                                }

                                AssetIcon {
                                    Layout.preferredWidth: root.narrow ? 15 : 18
                                    Layout.preferredHeight: root.narrow ? 15 : 18
                                    iconSize: root.narrow ? 15 : 18
                                    source: modelData.iconSource || ""
                                    active: true
                                    inactiveOpacity: 1
                                    visible: source.toString().length > 0
                                }

                                Text {
                                    visible: !(modelData.iconSource && String(modelData.iconSource).length > 0)
                                    text: modelData.icon || "OK"
                                    color: modelData.stateColor || "#ffffff"
                                    font.pixelSize: root.narrow ? 13 : 15
                                }
                            }
                        }

                        Rectangle {
                            visible: !root.compact && index > 0
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            width: 1
                            height: parent.height - 14
                            color: "#ffffff20"
                        }
                    }
                }
            }
        }
    }
}
