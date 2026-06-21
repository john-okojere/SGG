import QtQuick
import "../screens"
import "../mission"
import "../pilot"
import "../vehicle"
import "../manufacturer"
import "../tools"
import "../help"

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
        active: appState.currentScreen === "planner" && appState.operationalMode !== "pilot" && moduleAccessManager.missionWorkspaceAllowed
        visible: active
        sourceComponent: plannerComponent
    }

    Loader {
        id: pilotLoader
        anchors.fill: parent
        active: appState.currentScreen === "planner" && appState.operationalMode === "pilot" && moduleAccessManager.pilotWorkspaceAllowed
        visible: active
        sourceComponent: pilotComponent
    }

    Loader {
        id: vehicleConfigLoader
        anchors.fill: parent
        active: appState.currentScreen === "vehicleConfiguration" && moduleAccessManager.vehicleConfigurationWorkspaceAllowed
        visible: active
        sourceComponent: vehicleConfigComponent
    }

    Loader {
        id: manufacturerTestFlightLoader
        anchors.fill: parent
        active: appState.currentScreen === "manufacturerTestFlight" && moduleAccessManager.manufacturerTestFlightWorkspaceAllowed
        visible: active
        sourceComponent: manufacturerTestFlightComponent
    }

    Loader {
        id: manufacturerLoader
        anchors.fill: parent
        active: appState.currentScreen === "manufacturer"
        visible: active
        sourceComponent: manufacturerComponent
    }

    Loader {
        id: gcsToolsLoader
        anchors.fill: parent
        active: appState.currentScreen === "gcsTools" && moduleAccessManager.gcsToolsWorkspaceAllowed
        visible: active
        sourceComponent: gcsToolsComponent
    }

    Loader {
        id: helpLoader
        anchors.fill: parent
        active: appState.currentScreen === "help"
        visible: active
        sourceComponent: helpComponent
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

    PostMissionSummaryModal {
        anchors.fill: parent
        visible: postMissionSummaryManager.visible
        enabled: postMissionSummaryManager.visible
        z: 40
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
        id: vehicleConfigComponent
        VehicleConfigurationScreen {
            anchors.fill: parent
        }
    }

    Component {
        id: manufacturerTestFlightComponent
        ManufacturerTestFlightScreen {
            anchors.fill: parent
        }
    }

    Component {
        id: manufacturerComponent
        ManufacturerWorkspaceScreen {
            anchors.fill: parent
        }
    }

    Component {
        id: gcsToolsComponent
        GcsToolsWorkspace {
            anchors.fill: parent
        }
    }

    Component {
        id: helpComponent
        HelpCenterScreen {
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
