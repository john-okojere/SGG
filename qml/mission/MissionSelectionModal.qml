import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    color: "#000000"

    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: root.visible ? 1 : 0
        Behavior on opacity { NumberAnimation { duration: Theme.anim } }
        MouseArea { anchors.fill: parent }
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.72, 1040)
        spacing: 42

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "New Mission"
            color: Theme.white
            font.pixelSize: 34
            font.bold: true
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 6
            rowSpacing: 28
            columnSpacing: 28

            Repeater {
                model: missionStore.missionTypes
                delegate: Item {
                    id: card
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    property bool hovered: false

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10

                        Rectangle {
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 70
                            radius: 35
                            color: card.hovered ? Theme.purple2 : Theme.purple
                            border.color: card.hovered ? "#ffffff99" : "#00000000"
                            y: card.hovered ? -5 : 0
                            Rectangle {
                                anchors.centerIn: parent
                                width: icon === "waypoint" ? 44 : 52
                                height: width
                                radius: width / 2
                                color: icon === "waypoint" ? "#ffffff" : "#00000000"
                                AssetIcon {
                                    anchors.centerIn: parent
                                    width: icon === "waypoint" ? 28 : 34
                                    height: width
                                    source: AssetRegistry.icons[icon] || ""
                                    active: true
                                }
                            }
                            Behavior on y { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
                            Behavior on color { ColorAnimation { duration: Theme.animFast } }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: name
                            color: Theme.white
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 20
                            font.bold: true
                            wrapMode: Text.WordWrap
                        }
                        Text {
                            Layout.fillWidth: true
                            text: description
                            color: "#ddd4e7"
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 15
                            wrapMode: Text.WordWrap
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: card.hovered = true
                        onExited: card.hovered = false
                        onClicked: appState.startMission(key)
                    }
                }
            }
        }

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 62
            height: 62
            radius: 31
            color: closeArea.containsMouse ? "#ffd64f" : "#f7c948"
            border.color: closeArea.containsMouse ? "#ffffffcc" : "#ffffff55"
            AssetIcon {
                anchors.centerIn: parent
                width: 24
                height: 24
                source: AssetRegistry.icons.iconoir_cancel
                active: true
            }
            MouseArea {
                id: closeArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: Qt.quit()
            }
            Behavior on color { ColorAnimation { duration: Theme.animFast } }
            Behavior on border.color { ColorAnimation { duration: Theme.animFast } }
        }
    }
}
