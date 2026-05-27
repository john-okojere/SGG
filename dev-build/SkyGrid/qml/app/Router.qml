import QtQuick
import "../screens"
import "../mission"
import "../pilot"

Item {
    id: root

    Loader {
        id: homeLoader
        anchors.fill: parent
        active: appState.currentScreen === "home" || appState.currentScreen === "missionSelector"
        visible: active
        sourceComponent: homeComponent
    }

    Loader {
        id: plannerLoader
        anchors.fill: parent
        active: appState.currentScreen === "planner" && appState.operationalMode !== "pilot"
        visible: active
        sourceComponent: plannerComponent
    }

    Loader {
        id: pilotLoader
        anchors.fill: parent
        active: appState.currentScreen === "planner" && appState.operationalMode === "pilot"
        visible: active
        sourceComponent: pilotComponent
    }

    Loader {
        id: modalLoader
        anchors.fill: parent
        active: appState.currentScreen === "missionSelector"
        visible: active
        opacity: active ? 1 : 0
        z: 20
        sourceComponent: modalComponent
    }

    Component {
        id: homeComponent
        HomeScreen {
            anchors.fill: parent
        }
    }

    Component {
        id: plannerComponent
        MissionPlannerScreen {
            anchors.fill: parent
        }
    }

    Component {
        id: pilotComponent
        ManualFlightScreen {
            anchors.fill: parent
        }
    }

    Component {
        id: modalComponent
        MissionSelectionModal {
            anchors.fill: parent
        }
    }
}
