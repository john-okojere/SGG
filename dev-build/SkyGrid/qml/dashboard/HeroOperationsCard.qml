import QtQuick 2.15
import QtQuick.Layouts 1.15
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root

    property url backgroundSource: ""
    property var metrics: []

    signal openRequested()

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
            GradientStop { position: 0.0; color: "#21066fdd" }
            GradientStop { position: 0.62; color: "#21066f66" }
            GradientStop { position: 1.0; color: "#09032744" }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 14

        RowLayout {
            Layout.fillWidth: true

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 6
                Text {
                    Layout.fillWidth: true
                    text: "Mission Operations Center"
                    color: "#ffffff"
                    font.pixelSize: 22
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    Layout.fillWidth: true
                    text: "Real-time overview of your operations and system status"
                    color: "#eee9ff"
                    font.pixelSize: 11
                    font.bold: true
                    elide: Text.ElideRight
                }
            }

            Rectangle {
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                radius: 6
                color: openMouse.containsMouse ? "#ffffff33" : "#ffffff18"
                border.color: "#ffffff40"
                Text {
                    anchors.centerIn: parent
                    text: "↗"
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

        Item { Layout.fillHeight: true }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 68
            radius: 8
            color: "#200b68cc"
            border.color: "#ffffff18"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 0

                Repeater {
                    model: root.metrics
                    delegate: Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            anchors.topMargin: 10
                            anchors.bottomMargin: 10
                            spacing: 4

                            Text {
                                Layout.fillWidth: true
                                text: modelData.label
                                color: "#d8d1f5"
                                font.pixelSize: 8
                                font.bold: true
                                elide: Text.ElideRight
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    text: modelData.value
                                    color: modelData.stateColor || "#ffffff"
                                    font.pixelSize: 18
                                    font.bold: true
                                }
                                Item { Layout.fillWidth: true }
                                AssetIcon {
                                    width: 18
                                    height: 18
                                    iconSize: 18
                                    source: modelData.iconSource || ""
                                    active: true
                                    inactiveOpacity: 1
                                    visible: source.toString().length > 0
                                }
                                Text {
                                    visible: !(modelData.iconSource && String(modelData.iconSource).length > 0)
                                    text: modelData.icon || "OK"
                                    color: modelData.stateColor || "#ffffff"
                                    font.pixelSize: 15
                                }
                            }
                        }

                        Rectangle {
                            visible: index > 0
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            width: 1
                            height: parent.height - 28
                            color: "#ffffff20"
                        }
                    }
                }
            }
        }
    }
}
