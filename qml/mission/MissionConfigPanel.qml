import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

Rectangle {
    id: root
    radius: 10
    color: "#f5f5f7"
    clip: true
    border.color: "#d8cbe9"
    border.width: 1

    readonly property string activeMissionType: missionStore.plan.missionType.length > 0 ? missionStore.plan.missionType : appState.currentMissionType
    readonly property bool isWaypointRoute: activeMissionType === "waypointRoute"
    readonly property bool isVirtualFence: activeMissionType === "virtualFence"
    readonly property bool isPoi: activeMissionType === "map3dPoi"
    readonly property bool isPolygonMission: activeMissionType === "photomap"
                                            || activeMissionType === "map3dArea"
                                            || activeMissionType === "towerInspection"
                                            || activeMissionType === "virtualFence"
    readonly property bool compactLayout: width < 370 || height < 760
    readonly property int labelSize: compactLayout ? 13 : 14
    readonly property int inputSize: compactLayout ? 12 : 13
    readonly property int helperSize: 10
    readonly property int controlHeight: compactLayout ? 32 : 34
    readonly property int rowSpacing: compactLayout ? 8 : 10
    property int activeTab: 0
    property int waypointTab: 0
    property bool editingTitle: false
    signal requestPreflight()
    signal requestPrepareAndStart()

    onActiveTabChanged: formFlick.contentY = 0
    onWaypointTabChanged: formFlick.contentY = 0

    function missionTitle() {
        if (missionStore.plan.name.length > 0 && missionStore.plan.name.indexOf("Untitled") !== 0) return missionStore.plan.name
        if (activeMissionType === "photomap") return "PhotoMap"
        if (activeMissionType === "virtualFence") return "Virtual Fence"
        if (activeMissionType === "map3dArea") return "3D Map Area"
        if (activeMissionType === "map3dPoi") return "3D Map POI"
        if (activeMissionType === "waypointRoute") return "Waypoint Route"
        if (activeMissionType === "towerInspection") return "Tower Inspection"
        return missionStore.plan.name
    }

    function routeItems() { return missionStore.plan.generatedRoute }
    function routeDistanceFt() { return missionStore.plan.routeDistanceKm * 3280.84 }
    function selectedWaypoint() { return missionStore.plan.waypointAt(appState.selectedWaypointIndex) }
    function safeWaypointIndex() { return appState.selectedWaypointIndex >= 0 ? appState.selectedWaypointIndex : 0 }

    function metricOneLabel() { return "Waypoint Qty" }
    function metricOneValue() {
        if (activeMissionType === "virtualFence") return String(missionStore.plan.polygon.length)
        return String(Math.max(missionStore.plan.waypoints.length, routeItems().length))
    }
    function metricOneUnit() { return "pts" }
    function metricTwoLabel() {
        if (activeMissionType === "virtualFence") return "Area"
        return isWaypointRoute || activeMissionType === "map3dPoi" ? "Distance" : "Flight Length"
    }
    function metricTwoValue() {
        if (activeMissionType === "virtualFence") return Number(missionStore.plan.missionAreaHa).toFixed(0)
        if (missionStore.plan.routeDistanceKm >= 1.0 && isWaypointRoute) return Number(missionStore.plan.routeDistanceKm).toFixed(2)
        return Number(routeDistanceFt()).toFixed(0)
    }
    function metricTwoUnit() {
        if (activeMissionType === "virtualFence") return "ha"
        if (missionStore.plan.routeDistanceKm >= 1.0 && isWaypointRoute) return "km"
        return "ft"
    }
    function metricFourLabel() {
        if (isWaypointRoute) return "Layers"
        if (activeMissionType === "virtualFence") return ""
        return isPoi ? "MainPath No" : "MainPath No"
    }
    function metricFourValue() {
        if (isWaypointRoute) return "1"
        if (activeMissionType === "virtualFence") return ""
        return String(routeItems().length)
    }
    function metricFourUnit() { return isWaypointRoute ? "Layer" : "Lines" }

    function aircraftId() {
        return missionSyncManager.assignedAircraft.length > 0 ? String(missionSyncManager.assignedAircraft[0].id) : ""
    }

    function missionCenter() {
        var source = missionStore.plan.polygon.length > 0 ? missionStore.plan.polygon : missionStore.plan.waypoints
        if (missionStore.plan.hasPoi) return { latitude: missionStore.plan.poi.latitude, longitude: missionStore.plan.poi.longitude }
        if (source.length === 0) return { latitude: mapState.centerLatitude, longitude: mapState.centerLongitude }
        var lat = 0
        var lon = 0
        for (var i = 0; i < source.length; ++i) {
            lat += source[i].latitude
            lon += source[i].longitude
        }
        return { latitude: lat / source.length, longitude: lon / source.length }
    }

    function editablePrimaryCoordinate() {
        if (appState.selectedPolygonIndex >= 0 && appState.selectedPolygonIndex < missionStore.plan.polygon.length) {
            return missionStore.plan.polygon[appState.selectedPolygonIndex]
        }
        return { latitude: missionStore.plan.primaryLatitude, longitude: missionStore.plan.primaryLongitude }
    }

    function applyCoordinate(latText, lonText, selectedOnly) {
        var lat = Number(latText)
        var lon = Number(lonText)
        if (isNaN(lat) || isNaN(lon)) return
        if (selectedOnly && appState.selectedWaypointIndex >= 0) {
            missionStore.plan.moveWaypoint(appState.selectedWaypointIndex, lat, lon)
        } else if (!selectedOnly && appState.selectedPolygonIndex >= 0 && appState.selectedPolygonIndex < missionStore.plan.polygon.length) {
            missionStore.plan.movePolygonVertex(appState.selectedPolygonIndex, lat, lon)
        } else {
            missionStore.plan.setPrimaryCoordinate(lat, lon)
        }
    }

    function selectedPolygonPoint() {
        if (appState.selectedPolygonIndex >= 0 && appState.selectedPolygonIndex < missionStore.plan.polygon.length) {
            return missionStore.plan.polygon[appState.selectedPolygonIndex]
        }
        return {}
    }

    function insertPolygonNearSelected(after) {
        if (missionStore.plan.polygon.length === 0) {
            missionStore.plan.addPolygonVertex(mapState.centerLatitude, mapState.centerLongitude)
            appState.selectedPolygonIndex = 0
            appState.selectedTool = "polygon"
            return
        }
        var baseIndex = appState.selectedPolygonIndex >= 0 ? appState.selectedPolygonIndex : missionStore.plan.polygon.length - 1
        var afterIndex = after ? baseIndex : Math.max(0, baseIndex - 1)
        var a = missionStore.plan.polygon[afterIndex]
        var b = missionStore.plan.polygon[(afterIndex + 1) % missionStore.plan.polygon.length]
        var lat = b ? (a.latitude + b.latitude) / 2 : a.latitude
        var lon = b ? (a.longitude + b.longitude) / 2 : a.longitude
        missionStore.plan.insertPolygonVertex(afterIndex, lat, lon)
        appState.selectedPolygonIndex = afterIndex + 1
        appState.selectedTool = "polygon"
    }

    function deleteSelectedPolygonPoint() {
        if (appState.selectedPolygonIndex < 0 || appState.selectedPolygonIndex >= missionStore.plan.polygon.length) return
        var nextIndex = Math.min(appState.selectedPolygonIndex, missionStore.plan.polygon.length - 2)
        missionStore.plan.deletePolygonVertex(appState.selectedPolygonIndex)
        appState.selectedPolygonIndex = nextIndex
        appState.selectedTool = "polygon"
    }

    function templateDescription() {
        if (activeMissionType === "photomap") return "Creates mapped photo coverage from a polygon, camera settings, overlap, altitude, and capture mode."
        if (activeMissionType === "virtualFence") return "Defines a safe operational boundary for the UAV and syncs it to Control Center."
        if (activeMissionType === "map3dArea") return "Builds a 3D area capture path from boundary, overlap, camera, and altitude settings."
        if (activeMissionType === "map3dPoi") return "Creates an orbit or scan around a point of interest with radius, camera, and altitude controls."
        if (activeMissionType === "waypointRoute") return "Plans a route from editable waypoints, each with independent flight and camera actions."
        if (activeMissionType === "towerInspection") return "Plans a structured inspection route around a tower or vertical asset."
        return "Configure mission geometry, command sequence, validation, and aircraft execution settings."
    }

    function applyTakeoff(latText, lonText) {
        var lat = Number(latText)
        var lon = Number(lonText)
        if (!isNaN(lat) && !isNaN(lon)) {
            missionStore.plan.setTakeoffPoint(lat, lon)
        }
    }

    function runValidationThenPreflight() {
        var center = missionCenter()
        weatherSyncManager.refreshForMission(center.latitude, center.longitude, missionStore.plan.altitude, aircraftId(), "MISSION")
        missionStore.plan.validateMission()
        missionSyncManager.validateActiveMission()
        requestPreflight()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: root.compactLayout ? 12 : 14
        spacing: root.compactLayout ? 8 : 10

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: root.compactLayout ? 34 : 38
            spacing: 8

            TextField {
                visible: root.editingTitle
                Layout.fillWidth: true
                text: missionStore.plan.name
                selectByMouse: true
                font.pixelSize: root.compactLayout ? 17 : 18
                font.bold: true
                color: Theme.purple
                onEditingFinished: {
                    missionStore.plan.name = text
                    root.editingTitle = false
                }
                background: Rectangle { radius: 6; color: "#ffffff"; border.color: "#c9b8df" }
            }

            Text {
                visible: !root.editingTitle
                Layout.fillWidth: true
                text: root.missionTitle()
                color: Theme.purple
                font.pixelSize: root.compactLayout ? 17 : 18
                font.bold: true
                elide: Text.ElideRight
            }

            IconButton {
                implicitWidth: root.compactLayout ? 30 : 32
                implicitHeight: root.compactLayout ? 30 : 32
                iconSource: AssetRegistry.icons.edit
                onClicked: root.editingTitle = !root.editingTitle
                ToolTip.visible: hovered
                ToolTip.text: "Rename mission"
            }
        }

        MissionMetricsRow {
            Layout.fillWidth: true
            Layout.preferredHeight: root.compactLayout ? 50 : 56
            Layout.minimumHeight: root.compactLayout ? 50 : 56
            Layout.maximumHeight: root.compactLayout ? 50 : 56
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: descriptionText.implicitHeight + 18
            radius: 7
            color: "#eee8f7"
            border.color: "#d8cbe9"
            Text {
                id: descriptionText
                anchors.fill: parent
                anchors.margins: 8
                text: root.templateDescription()
                color: "#4f465b"
                font.pixelSize: root.compactLayout ? 10 : 11
                wrapMode: Text.WordWrap
            }
        }

        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#ded5ea" }

        PolygonWorkflowSection { visible: root.isPolygonMission }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.controlHeight
            radius: 6
            color: "#f0eef3"
            visible: !root.isVirtualFence

            RowLayout {
                anchors.fill: parent
                anchors.margins: 3
                spacing: 0
                SegmentedButton { Layout.fillWidth: true; text: "Basic"; active: root.activeTab === 0; onClicked: root.activeTab = 0 }
                SegmentedButton { Layout.fillWidth: true; text: "Advanced"; active: root.activeTab === 1; onClicked: root.activeTab = 1 }
            }
        }

        Flickable {
            id: formFlick
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: width
            contentHeight: contentColumn.implicitHeight
            boundsBehavior: Flickable.StopAtBounds

            ColumnLayout {
                id: contentColumn
                width: parent.width
                spacing: 12

                TakeoffSection {}
                WaypointRouteForm { visible: root.isWaypointRoute }
                VirtualFenceForm { visible: root.isVirtualFence }
                PoiForm { visible: root.isPoi }
                SurveyForm { visible: !root.isWaypointRoute && !root.isVirtualFence && !root.isPoi }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: root.controlHeight
            spacing: 8

            ActionButton {
                Layout.fillWidth: true
                text: "Preflight Checklist"
                iconText: "✓"
                primary: false
                enabled: sessionManager.operationsAllowed
                onClicked: root.requestPreflight()
            }
            Text {
                Layout.fillWidth: true
                text: missionSyncManager.syncing ? "Saving mission..." : missionStore.plan.operationStatus
                color: Theme.muted
                font.pixelSize: 10
                elide: Text.ElideRight
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: root.controlHeight
            spacing: root.compactLayout ? 12 : 18

            ActionButton {
                Layout.fillWidth: true
                text: "Save Mission"
                iconText: "▣"
                primary: false
                enabled: sessionManager.operationsAllowed && missionStore.plan.missionState !== "EMPTY"
                onClicked: missionSyncManager.saveActiveMission()
            }

            ActionButton {
                Layout.fillWidth: true
                text: root.isVirtualFence ? "Validate & Sync" : "Start Flying"
                iconText: "▷"
                primary: true
                enabled: sessionManager.operationsAllowed
                         && (root.isVirtualFence || root.routeItems().length >= 2)
                         && !missionUploadManager.uploading
                         && !missionExecutionManager.executing
                onClicked: root.isVirtualFence ? root.runValidationThenPreflight() : root.requestPrepareAndStart()
            }
        }
    }

    component MissionMetricsRow: RowLayout {
        spacing: 0
        Metric { label: root.metricOneLabel(); value: root.metricOneValue(); unit: root.metricOneUnit() }
        Separator {}
        Metric { label: root.metricTwoLabel(); value: root.metricTwoValue(); unit: root.metricTwoUnit() }
        Separator {}
        Metric { label: "Est. Flight Time"; value: missionStore.plan.estimatedTime; unit: "" }
        Separator { visible: root.metricFourLabel().length > 0 }
        Metric { visible: root.metricFourLabel().length > 0; label: root.metricFourLabel(); value: root.metricFourValue(); unit: root.metricFourUnit() }
    }

    component PolygonWorkflowSection: ColumnLayout {
        width: parent.width
        spacing: 8

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.controlHeight
            radius: 6
            color: "#f0eef3"
            RowLayout {
                anchors.fill: parent
                anchors.margins: 3
                spacing: 0
                SegmentedButton {
                    Layout.fillWidth: true
                    text: "Edit Polygon"
                    active: appState.selectedTool === "polygon"
                    onClicked: appState.selectedTool = "polygon"
                }
                SegmentedButton {
                    Layout.fillWidth: true
                    text: "Survey Preview"
                    active: appState.selectedTool === "survey"
                    enabled: missionStore.plan.polygon.length >= 3
                    onClicked: appState.selectedTool = "survey"
                }
            }
        }

        Text {
            Layout.fillWidth: true
            text: missionStore.plan.polygon.length < 3
                  ? "Add at least 3 polygon vertices before survey lanes are generated."
                  : (appState.selectedTool === "polygon"
                     ? "Polygon edit mode: drag numbered vertices or click + handles on edges to insert points."
                     : "Survey preview mode: generated lanes are clipped to the polygon and used for upload.")
            color: missionStore.plan.polygon.length < 3 ? Theme.amber : Theme.muted
            font.pixelSize: root.helperSize
            wrapMode: Text.WordWrap
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            ActionButton {
                Layout.fillWidth: true
                text: "Insert Before"
                iconText: "+"
                primary: false
                enabled: missionStore.plan.polygon.length > 0
                onClicked: root.insertPolygonNearSelected(false)
            }
            ActionButton {
                Layout.fillWidth: true
                text: "Insert After"
                iconText: "+"
                primary: false
                enabled: missionStore.plan.polygon.length > 0
                onClicked: root.insertPolygonNearSelected(true)
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            ActionButton {
                Layout.fillWidth: true
                text: "Delete Vertex"
                iconText: "×"
                primary: false
                enabled: appState.selectedPolygonIndex >= 0 && appState.selectedPolygonIndex < missionStore.plan.polygon.length
                onClicked: root.deleteSelectedPolygonPoint()
            }
            Text {
                Layout.fillWidth: true
                text: appState.selectedPolygonIndex >= 0 && appState.selectedPolygonIndex < missionStore.plan.polygon.length
                      ? ("Selected vertex " + (appState.selectedPolygonIndex + 1))
                      : "No vertex selected"
                color: "#4f465b"
                font.pixelSize: root.helperSize
                elide: Text.ElideRight
            }
        }
    }

    component SurveyForm: ColumnLayout {
        width: parent.width
        spacing: root.rowSpacing
        BasicSurveyFields { visible: root.activeTab === 0 }
        AdvancedSurveyFields { visible: root.activeTab === 1 }
    }

    component TakeoffSection: ColumnLayout {
        width: parent.width
        spacing: root.rowSpacing
        RowLayout {
            Layout.fillWidth: true
            Text {
                Layout.fillWidth: true
                text: "Takeoff Command"
                color: missionStore.plan.hasTakeoffPoint ? Theme.green : Theme.red
                font.pixelSize: root.labelSize
                font.bold: true
            }
            Text {
                text: missionStore.plan.hasTakeoffPoint ? "READY" : "REQUIRED"
                color: missionStore.plan.hasTakeoffPoint ? Theme.green : Theme.red
                font.pixelSize: 11
                font.bold: true
            }
        }
        Text {
            Layout.fillWidth: true
            text: "Set the first mission action by clicking the Takeoff tool on the map or entering launch coordinates."
            color: Theme.muted
            font.pixelSize: root.helperSize
            wrapMode: Text.WordWrap
        }
        CoordinateInput {
            id: takeoffLat
            label: "Takeoff Lat."
            text: missionStore.plan.hasTakeoffPoint ? Number(missionStore.plan.takeoffPoint.latitude).toFixed(8) : Number(mapState.centerLatitude).toFixed(8)
            help: "Latitude for the required first mission action."
            onAcceptedText: root.applyTakeoff(text, takeoffLon.text)
        }
        CoordinateInput {
            id: takeoffLon
            label: "Takeoff Lon."
            text: missionStore.plan.hasTakeoffPoint ? Number(missionStore.plan.takeoffPoint.longitude).toFixed(8) : Number(mapState.centerLongitude).toFixed(8)
            help: "Longitude for the required first mission action."
            onAcceptedText: root.applyTakeoff(takeoffLat.text, text)
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            ActionButton {
                Layout.fillWidth: true
                text: "Use Map Center"
                iconText: "+"
                primary: false
                onClicked: missionStore.plan.setTakeoffPoint(mapState.centerLatitude, mapState.centerLongitude)
            }
            ActionButton {
                Layout.fillWidth: true
                text: "Clear Takeoff"
                iconText: "×"
                primary: false
                enabled: missionStore.plan.hasTakeoffPoint
                onClicked: missionStore.plan.clearTakeoffPoint()
            }
        }
        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#ded5ea" }
    }

    component PoiForm: ColumnLayout {
        width: parent.width
        spacing: root.rowSpacing
        ColumnLayout {
            visible: root.activeTab === 0
            width: parent.width
            spacing: root.rowSpacing
            SelectInput { label: "Camera Model"; help: "Camera profile used for footprint, GSD, and capture estimates."; value: missionStore.plan.cameraModel; options: ["Zenmuse X5S 70mm", "Phantom 4 Pro Camera", "Mavic 3E Camera"]; onAcceptedValue: missionStore.plan.cameraModel = value }
            SelectInput { label: "Flight Course Mode"; help: "Pattern style used to generate the POI flight path."; value: missionStore.plan.flightCourseMode; options: ["Circle Mode", "Spiral Mode", "Vertical Scan"]; onAcceptedValue: missionStore.plan.flightCourseMode = value }
            SelectInput { label: "Capture Mode"; help: "How the camera captures during the generated path."; value: missionStore.plan.captureMode; options: ["Capture at Equal Dist. Interval", "Timed Capture", "Hover Capture"]; onAcceptedValue: missionStore.plan.captureMode = value }
            SliderRow { label: "Speed"; help: "Target aircraft speed for this mission."; value: missionStore.plan.speed; from: 1; to: 35; suffix: "m/s"; onEdited: missionStore.plan.speed = value }
            SliderRow { label: "Flight Rad."; help: "Orbit radius around the point of interest in meters."; value: missionStore.plan.radius; from: 20; to: 1200; suffix: "m"; onEdited: missionStore.plan.radius = value }
            SliderRow { label: "Building Rad."; help: "Approximate subject radius in meters used to maintain clearance."; value: missionStore.plan.buildingRadius; from: 5; to: 600; suffix: "m"; onEdited: missionStore.plan.buildingRadius = value }
            AltitudeRangeRow {}
            CoordinatePair { selectedOnly: false }
        }
        AdvancedSurveyFields { visible: root.activeTab === 1 }
    }

    component BasicSurveyFields: ColumnLayout {
        width: parent.width
        spacing: root.rowSpacing
        SelectInput { label: "Camera Model"; help: "Camera profile used for footprint, GSD, overlap, and image count estimates."; value: missionStore.plan.cameraModel; options: ["Phantom 4 Pro Camera", "Mavic 3E Camera", "Zenmuse X5S", "Zenmuse X5S 70mm"]; onAcceptedValue: missionStore.plan.cameraModel = value }
        SelectInput { visible: root.activeMissionType === "towerInspection"; label: "Shooting Angle"; help: "Camera orientation relative to the inspection path."; value: missionStore.plan.shootingAngle; options: ["Parallel To Main Path", "Nadir", "Oblique", "Toward POI"]; onAcceptedValue: missionStore.plan.shootingAngle = value }
        SelectInput { label: "Capture Mode"; help: "How photos or video are captured during the mission."; value: missionStore.plan.captureMode; options: ["Capture at Equal Dist. Interval", "Timed Capture", "Hover Capture"]; onAcceptedValue: missionStore.plan.captureMode = value }
        SelectInput { label: "Flight Course Mode"; help: "Route pattern used to cover the mission geometry."; value: missionStore.plan.flightCourseMode; options: ["Inside Mode", "Circle Mode", "Parallel", "Crosshatch", "Perimeter First"]; onAcceptedValue: missionStore.plan.flightCourseMode = value }
        SelectInput { visible: root.activeMissionType === "map3dArea"; label: "Inside Mode"; help: "Internal lane layout for area capture."; value: missionStore.plan.insideMode; options: ["Parallel", "Crosshatch", "Perimeter"]; onAcceptedValue: missionStore.plan.insideMode = value }
        SliderRow { label: "Speed"; help: "Target aircraft speed for route generation and estimates."; value: missionStore.plan.speed; from: 1; to: 35; suffix: "m/s"; onEdited: missionStore.plan.speed = value }
        AltitudeRangeRow {}
        CoordinatePair { selectedOnly: false }
        SelectInput { label: "Finish Action"; help: "Aircraft behavior after mission execution completes."; value: missionStore.plan.finishAction; options: ["Return to Home", "Hover", "Land"]; onAcceptedValue: missionStore.plan.finishAction = value }
    }

    component AdvancedSurveyFields: ColumnLayout {
        width: parent.width
        spacing: root.rowSpacing
        SliderRow { label: "Front Overlap"; help: "Forward image overlap used for survey capture spacing."; value: missionStore.plan.frontOverlap; from: 10; to: 95; suffix: "%"; onEdited: missionStore.plan.frontOverlap = Math.round(value) }
        SliderRow { label: "Side Overlap"; help: "Side-to-side overlap used between generated lanes."; value: missionStore.plan.sideOverlap; from: 10; to: 95; suffix: "%"; onEdited: missionStore.plan.sideOverlap = Math.round(value) }
        SliderRow { visible: root.activeMissionType !== "map3dPoi"; label: "Course Angle"; help: "Heading angle for generated survey lanes."; value: missionStore.plan.courseAngle; from: 0; to: 360; suffix: "°"; onEdited: missionStore.plan.courseAngle = value }
        SliderRow { visible: root.activeMissionType !== "map3dPoi"; label: "Margin"; help: "Inset spacing from the polygon boundary used when generating PhotoMap lanes."; value: missionStore.plan.margin; from: 0; to: 250; suffix: "m"; onEdited: missionStore.plan.margin = value }
        SliderRow { label: "Gimbal Pitch"; help: "Camera pitch angle during capture."; value: missionStore.plan.gimbalPitch; from: -90; to: 45; suffix: "°"; onEdited: missionStore.plan.gimbalPitch = value }
        SelectInput { visible: root.activeMissionType === "photomap" || root.activeMissionType === "map3dPoi" || root.activeMissionType === "map3dArea"; label: "Flight Direction"; help: "Direction used for generated survey or orbit paths."; value: missionStore.plan.flightDirection; options: ["Clockwise", "Counter Clockwise", "North Up"]; onAcceptedValue: missionStore.plan.flightDirection = value }
        SelectInput { visible: root.activeMissionType === "map3dPoi" || root.activeMissionType === "map3dArea"; label: "Finish Action"; help: "Aircraft behavior after mission execution completes."; value: missionStore.plan.finishAction; options: ["Return to Home", "Hover", "Land"]; onAcceptedValue: missionStore.plan.finishAction = value }
        CoordinatePair { selectedOnly: false }
        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#ded5ea" }
        ReadonlyField { label: "GSD"; help: "Estimated ground sampling distance from altitude and camera profile."; value: Number(missionStore.plan.routeEstimates.gsd_cm_px || missionStore.plan.gsd).toFixed(1) + " cm/px" }
        ReadonlyField { label: "Lane Spacing"; help: "Distance between generated survey lanes from side overlap and camera footprint."; value: Number(missionStore.plan.routeEstimates.lane_spacing_m || 0).toFixed(1) + " m" }
        ReadonlyField { label: "Shot Spacing"; help: "Distance between photo capture points from front overlap and camera footprint."; value: Number(missionStore.plan.routeEstimates.shot_spacing_m || 0).toFixed(1) + " m" }
        ReadonlyField { label: "Est. Photos"; help: "Approximate number of photos from route length and capture spacing."; value: String(Math.max(1, Math.round(missionStore.plan.routeEstimates.estimated_photos || root.routeItems().length * 3))) }
        ReadonlyField { label: "Est. Batteries"; help: "Battery sets estimated from planned energy use."; value: Number(Math.max(1, Math.ceil(missionStore.plan.estimatedBattery / 70))).toFixed(0) + " Set Approx." }
    }

    component VirtualFenceForm: ColumnLayout {
        width: parent.width
        spacing: root.rowSpacing
        RowLayout {
            Layout.fillWidth: true
            Text { Layout.fillWidth: true; text: "Virtual Fence Settings"; color: "#0f0b14"; font.pixelSize: root.labelSize; font.bold: true }
            Text { text: "⌃"; color: "#0f0b14"; font.pixelSize: 18; font.bold: true }
        }
        SelectInput { label: "Fence Shape"; help: "Geometry used for the Control Center safety boundary."; value: missionStore.plan.fenceShape; options: ["Polygon", "Circle", "Corridor"]; onAcceptedValue: missionStore.plan.fenceShape = value }
        SelectInput { label: "Boundary Type"; help: "Whether the aircraft must stay inside, outside, or only warn near this boundary."; value: missionStore.plan.boundaryType; options: ["Inclusion", "Exclusion", "Warning Only"]; onAcceptedValue: missionStore.plan.boundaryType = value }
        SliderRow { label: "Flight Speed"; help: "Reference speed for boundary planning and estimates."; value: missionStore.plan.speed; from: 1; to: 35; suffix: "m/s"; onEdited: missionStore.plan.speed = value }
        SliderRow { label: "Safe Margin"; help: "Warning margin around the fence boundary."; value: missionStore.plan.safeMargin; from: 1; to: 100; suffix: "m"; onEdited: missionStore.plan.safeMargin = value }
        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#ded5ea" }
        AltitudeRangeRow {}
        SelectInput { label: "Breach Action"; help: "Action requested if the aircraft breaches the boundary."; value: missionStore.plan.breachAction; options: ["Return to Home", "Hold Position", "Land", "Alert Only"]; onAcceptedValue: missionStore.plan.breachAction = value }
        SelectInput { label: "Warning Action"; help: "Alert behavior before a boundary breach."; value: missionStore.plan.warningAction; options: ["Alert Pilot", "Control Center Alert", "Audio Warning"]; onAcceptedValue: missionStore.plan.warningAction = value }
        SliderRow { label: "Capture Interval"; help: "Optional camera sampling interval while monitoring this boundary."; value: missionStore.plan.captureInterval; from: 0.5; to: 10; suffix: "s"; onEdited: missionStore.plan.captureInterval = value }
        SliderRow { label: "Flight Alt."; help: "Reference altitude in meters for fence validation and display."; value: missionStore.plan.altitude; from: 10; to: 160; suffix: "m"; onEdited: missionStore.plan.altitude = value }
        SliderRow { label: "GSD"; help: "Estimated imagery resolution if capture is enabled."; value: missionStore.plan.gsd; from: 0.5; to: 12; suffix: "cm/px"; onEdited: missionStore.plan.gsd = value }
    }

    component WaypointRouteForm: ColumnLayout {
        width: parent.width
        spacing: root.rowSpacing
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.controlHeight
            radius: 6
            color: "#f0eef3"
            RowLayout {
                anchors.fill: parent
                anchors.margins: 3
                spacing: 0
                SegmentedButton { Layout.fillWidth: true; text: "All Points"; active: root.waypointTab === 0; onClicked: root.waypointTab = 0 }
                SegmentedButton { Layout.fillWidth: true; text: "Selected point"; active: root.waypointTab === 1; onClicked: root.waypointTab = 1 }
            }
        }

        ColumnLayout {
            visible: root.waypointTab === 0
            width: parent.width
            spacing: root.rowSpacing
            SliderRow { label: "Speed"; help: "Default speed inherited by new route waypoints."; value: missionStore.plan.speed; from: 1; to: 35; suffix: "m/s"; onEdited: missionStore.plan.speed = value }
            AltitudeRangeRow {}
            SelectInput { label: "Heading Mode"; help: "How aircraft heading is assigned along the route."; value: missionStore.plan.headingMode; options: ["Define Per Point", "Follow Route", "Course Locked"]; onAcceptedValue: missionStore.plan.headingMode = value }
            SelectInput { label: "Aircraft Rot."; help: "Default aircraft rotation behavior between points."; value: missionStore.plan.aircraftRotation; options: ["Auto", "Manual", "Counter Clockwise", "Clockwise"]; onAcceptedValue: missionStore.plan.aircraftRotation = value }
            SelectInput { label: "Gimbal Pitch"; help: "Default camera pitch mode for route points."; value: "Defined Per Point"; options: ["Defined Per Point", "Nadir", "45° Oblique"]; onAcceptedValue: missionStore.plan.gimbalPitch = value === "Nadir" ? -90 : missionStore.plan.gimbalPitch }
            SelectInput { label: "Cornering Style"; help: "How the aircraft turns through route corners."; value: missionStore.plan.corneringStyle; options: ["Curved", "Stop and Turn", "Straight"]; onAcceptedValue: missionStore.plan.corneringStyle = value }
            CoordinatePair { selectedOnly: false }
            SelectInput { label: "Finish Action"; help: "Aircraft behavior after the route completes."; value: missionStore.plan.finishAction; options: ["Return to Home", "Hover", "Land"]; onAcceptedValue: missionStore.plan.finishAction = value }
        }

        ColumnLayout {
            visible: root.waypointTab === 1
            width: parent.width
            spacing: root.rowSpacing
            readonly property var point: root.selectedWaypoint()
            Text {
                Layout.fillWidth: true
                text: appState.selectedWaypointIndex >= 0 ? ("Waypoint " + (appState.selectedWaypointIndex + 1)) : "Select a waypoint"
                color: "#0f0b14"
                font.pixelSize: root.labelSize
                font.bold: true
            }
            SliderRow { label: "Altitude"; help: "Altitude override in meters for this waypoint."; value: parent.point.altitude || missionStore.plan.altitude; from: 10; to: 160; suffix: "m"; enabled: appState.selectedWaypointIndex >= 0; onEdited: missionStore.plan.setWaypointAltitude(appState.selectedWaypointIndex, value) }
            SliderRow { label: "Speed"; help: "Speed override for this waypoint."; value: parent.point.speed || missionStore.plan.speed; from: 1; to: 35; suffix: "m/s"; enabled: appState.selectedWaypointIndex >= 0; onEdited: missionStore.plan.setWaypointSpeed(appState.selectedWaypointIndex, value) }
            SliderRow { label: "Heading"; help: "Aircraft heading at this waypoint."; value: parent.point.heading || 0; from: 0; to: 359; suffix: "°"; enabled: appState.selectedWaypointIndex >= 0; onEdited: missionStore.plan.setWaypointHeading(appState.selectedWaypointIndex, value) }
            SelectInput { label: "Rotation Direc."; help: "Rotation behavior used at the selected point."; value: missionStore.plan.aircraftRotation; options: ["Counter Clockwise", "Clockwise", "Auto"]; enabled: appState.selectedWaypointIndex >= 0; onAcceptedValue: missionStore.plan.aircraftRotation = value }
            SliderRow { label: "Gimbal Pitch"; help: "Camera gimbal pitch at this waypoint."; value: parent.point.gimbal_pitch || missionStore.plan.gimbalPitch; from: -90; to: 45; suffix: "°"; enabled: appState.selectedWaypointIndex >= 0; onEdited: missionStore.plan.setWaypointGimbalPitch(appState.selectedWaypointIndex, value) }
            SelectInput { label: "Camera Mode"; help: "Camera action override for this waypoint."; value: parent.point.camera_mode || missionStore.plan.captureMode; options: ["Capture at Equal Dist. Interval", "Take Photo", "Start Recording", "Stop Recording", "No Camera"]; enabled: appState.selectedWaypointIndex >= 0; onAcceptedValue: missionStore.plan.setWaypointCameraMode(appState.selectedWaypointIndex, value) }
            SelectInput { label: "Waypoint Act."; help: "Mission command performed at this waypoint."; value: parent.point.action || "No Action"; options: ["No Action", "Take Photo", "Start Recording", "Stop Recording", "Hover", "Q-loiter", "Return-to-Home"]; enabled: appState.selectedWaypointIndex >= 0; onAcceptedValue: missionStore.plan.setWaypointAction(appState.selectedWaypointIndex, value) }
            SliderRow { label: "Q-loiter"; help: "Loiter duration if Q-loiter is selected."; value: parent.point.loiter_seconds || 0; from: 0; to: 600; suffix: "s"; enabled: appState.selectedWaypointIndex >= 0; onEdited: missionStore.plan.setWaypointLoiterSeconds(appState.selectedWaypointIndex, value) }
            SliderRow { label: "Hover"; help: "Hover duration if Hover is selected."; value: parent.point.hover_seconds || 0; from: 0; to: 600; suffix: "s"; enabled: appState.selectedWaypointIndex >= 0; onEdited: missionStore.plan.setWaypointHoverSeconds(appState.selectedWaypointIndex, value) }
            CoordinatePair { selectedOnly: true }
        }
    }

    component AltitudeRangeRow: ColumnLayout {
        width: parent.width
        spacing: 6
        RowLayout {
            Layout.fillWidth: true
            spacing: root.rowSpacing
            FieldLabel { text: "Min Alt."; help: "Minimum planned altitude in meters for validation and generated routes." }
            ValueBox { Layout.preferredWidth: root.compactLayout ? 76 : 84; help: "Minimum planned altitude in meters for this mission."; text: Number(missionStore.plan.minAltitude).toFixed(1); suffix: "m"; onAcceptedValue: missionStore.plan.minAltitude = Number(value) }
            Item { Layout.fillWidth: true }
            FieldLabel { Layout.preferredWidth: root.compactLayout ? 64 : 74; text: "Max Alt."; help: "Maximum planned altitude in meters for validation and generated routes." }
            ValueBox { Layout.preferredWidth: root.compactLayout ? 76 : 84; help: "Maximum planned altitude in meters for this mission."; text: Number(missionStore.plan.maxAltitude).toFixed(1); suffix: "m"; onAcceptedValue: missionStore.plan.maxAltitude = Number(value) }
        }
        SliderBox {
            Layout.fillWidth: true
            from: 20
            to: 160
            value: missionStore.plan.altitude
            onMoved: missionStore.plan.altitude = value
            ToolTip.visible: hovered
            ToolTip.text: "Default altitude in meters applied to generated routes and new waypoints."
        }
    }

    component CoordinatePair: ColumnLayout {
        property bool selectedOnly: false
        readonly property var point: selectedOnly ? root.selectedWaypoint() : ({})
        readonly property var primaryPoint: root.editablePrimaryCoordinate()
        width: parent.width
        spacing: root.rowSpacing
        CoordinateInput {
            id: latInput
            label: "Latitude"
            help: selectedOnly ? "Latitude for the selected waypoint." : (appState.selectedPolygonIndex >= 0 ? "Latitude for the selected boundary point." : "Primary mission latitude used for geometry and weather checks.")
            text: Number(selectedOnly && appState.selectedWaypointIndex >= 0 ? point.latitude : primaryPoint.latitude).toFixed(8)
            onAcceptedText: root.applyCoordinate(text, lonInput.text, selectedOnly)
        }
        CoordinateInput {
            id: lonInput
            label: "Longitude"
            help: selectedOnly ? "Longitude for the selected waypoint." : (appState.selectedPolygonIndex >= 0 ? "Longitude for the selected boundary point." : "Primary mission longitude used for geometry and weather checks.")
            text: Number(selectedOnly && appState.selectedWaypointIndex >= 0 ? point.longitude : primaryPoint.longitude).toFixed(8)
            onAcceptedText: root.applyCoordinate(latInput.text, text, selectedOnly)
        }
    }

    component SliderRow: RowLayout {
        id: row
        property string label: ""
        property real value: 0
        property real from: 0
        property real to: 100
        property string suffix: ""
        property string help: label.length > 0 ? "Adjust " + label + " for this mission command." : "Adjust this mission parameter."
        signal edited(real value)
        Layout.fillWidth: true
        spacing: root.rowSpacing
        FieldLabel { text: row.label; help: row.help }
        SliderBox {
            Layout.fillWidth: true
            from: row.from
            to: row.to
            value: row.value
            enabled: row.enabled
            onMoved: row.edited(value)
            ToolTip.visible: hovered
            ToolTip.text: row.help
        }
        ValueBox { Layout.preferredWidth: root.compactLayout ? 78 : 88; help: row.help; text: Number(row.value).toFixed(row.suffix === "m/s" || row.suffix === "%" || row.suffix === "°" ? 0 : 1); suffix: row.suffix; enabled: row.enabled; onAcceptedValue: row.edited(Number(value)) }
    }

    component SelectInput: RowLayout {
        id: row
        property string label: ""
        property string value: ""
        property var options: []
        property string help: label.length > 0 ? "Choose " + label + " for the current mission." : "Choose a mission option."
        signal acceptedValue(string value)
        Layout.fillWidth: true
        spacing: root.rowSpacing
        FieldLabel { text: row.label; help: row.help }
        ComboBox {
            Layout.fillWidth: true
            model: row.options
            currentIndex: Math.max(0, row.options.indexOf(row.value))
            font.pixelSize: root.inputSize
            implicitHeight: root.controlHeight
            enabled: row.enabled
            onActivated: row.acceptedValue(currentText)
            ToolTip.visible: hovered
            ToolTip.text: row.help
            contentItem: Text { text: parent.displayText; color: "#141017"; font.pixelSize: root.inputSize; verticalAlignment: Text.AlignVCenter; leftPadding: 10; elide: Text.ElideRight }
            indicator: Text { anchors.right: parent.right; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter; text: "›"; color: "#0f0b14"; font.pixelSize: 21; font.bold: true }
            background: Rectangle { radius: 5; color: "#f8f6fb"; border.color: "#cdb9e6"; border.width: 1; opacity: row.enabled ? 1 : 0.55 }
        }
    }

    component CoordinateInput: RowLayout {
        id: row
        property string label: ""
        property alias text: coordinateField.text
        property string help: "Edit " + label + " manually."
        signal acceptedText()
        Layout.fillWidth: true
        spacing: root.rowSpacing
        FieldLabel { text: row.label; help: row.help }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.controlHeight
            radius: 5
            color: "#f8f6fb"
            border.color: "#cdb9e6"
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                TextField {
                    id: coordinateField
                    Layout.fillWidth: true
                    color: "#141017"
                    font.pixelSize: root.inputSize
                    selectByMouse: true
                    validator: DoubleValidator {}
                    onEditingFinished: row.acceptedText()
                    background: Item {}
                    ToolTip.visible: hovered
                    ToolTip.text: row.help
                }
                Text { text: "✎"; color: "#0f0b14"; font.pixelSize: 15; font.bold: true }
            }
        }
    }

    component ReadonlyField: RowLayout {
        property string label: ""
        property string value: ""
        property string help: "Read-only mission estimate."
        Layout.fillWidth: true
        spacing: root.rowSpacing
        FieldLabel { text: label; help: help }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.controlHeight
            radius: 5
            color: "#f8f6fb"
            border.color: "#cdb9e6"
            Text { anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10; text: value; color: "#141017"; font.pixelSize: root.inputSize; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight }
            ToolTip.visible: estimateHover.containsMouse
            ToolTip.text: help
            MouseArea { id: estimateHover; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.NoButton }
        }
    }

    component FieldLabel: Text {
        property string help: text.length > 0 ? "Configure " + text + " for this mission." : ""
        Layout.preferredWidth: root.compactLayout ? 98 : 112
        color: "#161119"
        font.pixelSize: root.labelSize
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        ToolTip.visible: labelHover.containsMouse && help.length > 0
        ToolTip.text: help
        MouseArea { id: labelHover; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.NoButton }
    }

    component ValueBox: Rectangle {
        id: box
        property alias text: valueField.text
        property string suffix: ""
        property string help: "Enter a numeric value."
        signal acceptedValue(string value)
        height: root.controlHeight
        implicitHeight: root.controlHeight
        radius: 5
        color: "#f8f6fb"
        border.color: "#cdb9e6"
        opacity: enabled ? 1 : 0.55
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            spacing: 6
            TextField {
                id: valueField
                Layout.fillWidth: true
                color: "#141017"
                font.pixelSize: root.inputSize
                selectByMouse: true
                horizontalAlignment: TextInput.AlignRight
                validator: DoubleValidator {}
                onEditingFinished: box.acceptedValue(text)
                background: Item {}
            }
            Text { text: box.suffix; color: "#141017"; font.pixelSize: root.inputSize }
        }
        ToolTip.visible: boxHover.containsMouse && box.help.length > 0
        ToolTip.text: box.help
        MouseArea { id: boxHover; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.NoButton }
    }

    component SliderBox: Slider {
        id: slider
        implicitHeight: root.controlHeight
        live: true
        background: Rectangle {
            x: slider.leftPadding + 8
            y: slider.topPadding + slider.availableHeight / 2 - 1.5
            width: slider.availableWidth - 16
            height: 3
            radius: 2
            color: "#77727d"
            Rectangle { width: slider.visualPosition * parent.width; height: parent.height; radius: 2; color: Theme.purple }
        }
        handle: Rectangle {
            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            width: 7
            height: 7
            radius: 4
            color: "#f7f3fb"
            border.color: Theme.purple
        }
    }

    component Metric: ColumnLayout {
        property string label: ""
        property string value: ""
        property string unit: ""
        Layout.fillWidth: true
        spacing: 2
        Text { Layout.fillWidth: true; text: label; color: "#2f2935"; horizontalAlignment: Text.AlignHCenter; font.pixelSize: 11; elide: Text.ElideRight }
        Text { Layout.fillWidth: true; text: value + (unit.length > 0 ? unit : ""); color: "#05040a"; horizontalAlignment: Text.AlignHCenter; font.pixelSize: root.compactLayout ? 17 : 18; font.bold: true; elide: Text.ElideRight }
    }

    component Separator: Rectangle {
        Layout.preferredWidth: 1
        Layout.preferredHeight: root.compactLayout ? 42 : 48
        Layout.alignment: Qt.AlignVCenter
        color: "#cdbfe0"
    }

    component SegmentedButton: Button {
        id: tab
        property bool active: false
        implicitHeight: root.controlHeight - 4
        hoverEnabled: true
        WheelHandler {
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: function(event) { event.accepted = true }
        }
        contentItem: Text { text: tab.text; color: tab.active ? Theme.white : "#0f0b14"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.pixelSize: 13; font.bold: true }
        background: Rectangle { radius: 4; color: tab.active ? Theme.purple : "#00000000" }
    }

    component ActionButton: Button {
        id: button
        property bool primary: false
        property string iconText: ""
        implicitHeight: root.controlHeight
        hoverEnabled: true
        opacity: enabled ? 1 : 0.45
        WheelHandler {
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: function(event) { event.accepted = true }
        }
        contentItem: Row {
            anchors.centerIn: parent
            spacing: 8
            Text { text: button.iconText; color: button.primary ? Theme.white : "#141017"; font.pixelSize: 16; font.bold: true }
            Text { text: button.text; color: button.primary ? Theme.white : "#141017"; font.pixelSize: 13; elide: Text.ElideRight }
        }
        background: Rectangle { radius: 5; color: button.primary ? (button.hovered ? Theme.purple2 : Theme.purple) : "#f8f6fb"; border.color: button.primary ? Theme.purple : "#cdb9e6"; border.width: 1 }
    }
}
