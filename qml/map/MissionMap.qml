import QtQuick
import SkyGrid 1.0
import "overlays"

Item {
    id: root
    objectName: "missionMap"
    property real routeDashOffset: 0
    readonly property string activeMissionType: missionStore.plan.missionType.length > 0 ? missionStore.plan.missionType : appState.currentMissionType
    readonly property bool mapInteractionActive: ["select", "takeoff", "point", "poi", "route", "polygon", "rawWaypoint", "fence", "rally"].indexOf(appState.selectedTool) !== -1
    readonly property bool showGeneratedSurvey: root.activeMissionType !== "photomap"
                                             || (missionStore.plan.polygon.length >= 3 && appState.selectedTool !== "polygon")
    readonly property bool fastMode: String(performanceMode) === "fast"
    readonly property bool routeVisible: !missionStore.plan.boundaryOnly && missionStore.plan.generatedRoute.length > 1
    readonly property bool routeAnimationActive: !fastMode
                                             && visible
                                             && routeVisible
                                             && (appState.selectedTool === "route"
                                                 || appState.selectedTool === "point"
                                                 || missionUploadManager.uploading
                                                 || missionExecutionManager.executing)

    Timer {
        interval: root.fastMode ? 100 : 66
        running: root.routeAnimationActive
        repeat: true
        onTriggered: {
            root.routeDashOffset = (root.routeDashOffset + 0.7) % 28
            geometryCanvas.requestPaint()
        }
    }

    function pointFor(latitude, longitude) {
        return mapSurface.item && mapSurface.item.pointFor
            ? mapSurface.item.pointFor(latitude, longitude)
            : Qt.point(width / 2, height / 2)
    }

    function coordinateFor(point) {
        return mapSurface.item && mapSurface.item.coordinateFor
            ? mapSurface.item.coordinateFor(point)
            : { latitude: mapState.centerLatitude, longitude: mapState.centerLongitude }
    }

    function rowLatitude(row) {
        if (row.latitude !== undefined)
            return Number(row.latitude)
        var raw = Number(row.x)
        return Math.abs(raw) > 1000000 ? raw / 10000000 : raw
    }

    function rowLongitude(row) {
        if (row.longitude !== undefined)
            return Number(row.longitude)
        var raw = Number(row.y)
        return Math.abs(raw) > 1000000 ? raw / 10000000 : raw
    }

    function validCoordinate(row) {
        var lat = rowLatitude(row)
        var lon = rowLongitude(row)
        return !isNaN(lat) && !isNaN(lon) && Math.abs(lat) > 0.000001 && Math.abs(lon) > 0.000001
    }

    Connections {
        target: telemetryStore
        function onTelemetryChanged() {
            if (mapState.followAircraft
                    && telemetryStore.connected
                    && Math.abs(telemetryStore.latitude) > 0.000001
                    && Math.abs(telemetryStore.longitude) > 0.000001) {
                mapState.centerLatitude = telemetryStore.latitude
                mapState.centerLongitude = telemetryStore.longitude
            }
        }
    }

    Loader {
        id: mapSurface
        objectName: "missionMapSurfaceLoader"
        anchors.fill: parent
        source: "TileMapSurface.qml"
    }

    Canvas {
        id: geometryCanvas
        objectName: "missionGeometryCanvas"
        anchors.fill: parent
        z: 2
        antialiasing: !root.fastMode

        Connections {
            target: missionStore.plan
            function onPlanChanged() { geometryCanvas.requestPaint() }
            function onGeometryChanged() { geometryCanvas.requestPaint() }
        }
        Connections { target: appState; function onMissionChanged() { geometryCanvas.requestPaint() } }
        Connections { target: appState; function onToolChanged() { geometryCanvas.requestPaint() } }
        Connections { target: mapState; function onOverlayChanged() { geometryCanvas.requestPaint() } }
        Connections { target: mapState; function onZoomChanged() { geometryCanvas.requestPaint() } }
        Connections { target: mapState; function onCenterChanged() { geometryCanvas.requestPaint() } }
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            var polygon = missionStore.plan.boundaryOnly && missionStore.plan.boundaryPreview.length > 0
                    ? missionStore.plan.boundaryPreview
                    : missionStore.plan.polygon
            var waypoints = missionStore.plan.waypoints
            var takeoff = missionStore.plan.takeoffPoint
            var uploadRoute = missionStore.plan.generatedRoute

            if (root.activeMissionType === "photomap" || root.activeMissionType === "map3dArea" || root.activeMissionType === "virtualFence" || root.activeMissionType === "towerInspection") {
                ctx.beginPath()
                for (var i = 0; i < polygon.length; ++i) {
                    var pp = root.pointFor(polygon[i].latitude, polygon[i].longitude)
                    if (i === 0) ctx.moveTo(pp.x, pp.y)
                    else ctx.lineTo(pp.x, pp.y)
                }
                ctx.closePath()
                ctx.fillStyle = root.activeMissionType === "virtualFence" ? "rgba(247,201,72,0.13)"
                              : (root.activeMissionType === "photomap" ? "rgba(0,188,212,0.10)" : "rgba(75,18,139,0.18)")
                ctx.strokeStyle = root.activeMissionType === "virtualFence" ? "#f7c948"
                                : (root.activeMissionType === "photomap" ? "#25d6f5" : "rgba(255,255,255,0.92)")
                ctx.lineWidth = 3
                ctx.fill()
                ctx.stroke()
                ctx.strokeStyle = root.activeMissionType === "virtualFence" ? "rgba(247,201,72,0.32)"
                                : (root.activeMissionType === "photomap" ? "rgba(37,214,245,0.28)" : "rgba(124,69,184,0.55)")
                ctx.lineWidth = 8
                ctx.stroke()
            }

            if (!missionStore.plan.boundaryOnly && uploadRoute.length > 1 && root.showGeneratedSurvey) {
                ctx.lineJoin = "round"
                ctx.lineCap = "round"
                ctx.beginPath()
                for (var j = 0; j < uploadRoute.length; ++j) {
                    var wp = root.pointFor(uploadRoute[j].latitude, uploadRoute[j].longitude)
                    if (j === 0) ctx.moveTo(wp.x, wp.y)
                    else ctx.lineTo(wp.x, wp.y)
                }
                ctx.strokeStyle = root.activeMissionType === "photomap" ? "rgba(0,86,48,0.62)" : "rgba(44,0,87,0.78)"
                ctx.lineWidth = root.activeMissionType === "photomap" ? 8 : 10
                ctx.stroke()
                ctx.strokeStyle = root.activeMissionType === "photomap" ? "#32d36b" : "rgba(255,255,255,0.96)"
                ctx.lineWidth = root.activeMissionType === "photomap" ? 3 : 4
                if (root.routeAnimationActive) {
                    ctx.setLineDash([18, 10])
                    ctx.lineDashOffset = -root.routeDashOffset
                }
                ctx.stroke()
                ctx.setLineDash([])
            }

            if (missionStore.plan.hasTakeoffPoint) {
                var tp = root.pointFor(takeoff.latitude, takeoff.longitude)
                ctx.beginPath()
                ctx.arc(tp.x, tp.y, 18, 0, Math.PI * 2)
                ctx.fillStyle = "rgba(34,197,94,0.88)"
                ctx.fill()
                ctx.strokeStyle = "#ffffff"
                ctx.lineWidth = 3
                ctx.stroke()
                ctx.fillStyle = "#ffffff"
                ctx.font = "bold 12px sans-serif"
                ctx.textAlign = "center"
                ctx.textBaseline = "middle"
                ctx.fillText("TO", tp.x, tp.y)
            }
        }
    }

    Canvas {
        id: livePathCanvas
        objectName: "missionLivePathCanvas"
        anchors.fill: parent
        z: 2.5
        antialiasing: !root.fastMode

        Connections { target: telemetryStore; function onTelemetryChanged() { livePathCanvas.requestPaint() } }
        Connections { target: mapState; function onZoomChanged() { livePathCanvas.requestPaint() } }
        Connections { target: mapState; function onCenterChanged() { livePathCanvas.requestPaint() } }
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            var livePath = telemetryStore.livePath
            if (livePath.length > 1) {
                ctx.lineJoin = "round"
                ctx.lineCap = "round"
                ctx.beginPath()
                var start = Math.max(0, livePath.length - (root.fastMode ? 150 : 240))
                for (var lp = start; lp < livePath.length; ++lp) {
                    var livePoint = root.pointFor(livePath[lp].latitude, livePath[lp].longitude)
                    if (lp === start) ctx.moveTo(livePoint.x, livePoint.y)
                    else ctx.lineTo(livePoint.x, livePoint.y)
                }
                ctx.strokeStyle = "rgba(0,255,128,0.82)"
                ctx.lineWidth = 3
                ctx.stroke()
            }
        }
    }

    Canvas {
        id: advancedMissionCanvas
        objectName: "advancedMissionCanvas"
        anchors.fill: parent
        z: 2.7
        antialiasing: !root.fastMode
        visible: advancedMissionManager.useForUpload
                 || appState.selectedTool === "rawWaypoint"
                 || appState.selectedTool === "fence"
                 || appState.selectedTool === "rally"
                 || advancedMissionManager.geofenceItems.length > 0
                 || advancedMissionManager.rallyItems.length > 0

        Connections { target: advancedMissionManager; function onMissionChanged() { advancedMissionCanvas.requestPaint() } }
        Connections { target: appState; function onToolChanged() { advancedMissionCanvas.requestPaint() } }
        Connections { target: mapState; function onZoomChanged() { advancedMissionCanvas.requestPaint() } }
        Connections { target: mapState; function onCenterChanged() { advancedMissionCanvas.requestPaint() } }
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            var missionRows = advancedMissionManager.missionItems
            var first = true
            ctx.beginPath()
            for (var i = 0; i < missionRows.length; ++i) {
                if (!root.validCoordinate(missionRows[i]))
                    continue
                var mp = root.pointFor(root.rowLatitude(missionRows[i]), root.rowLongitude(missionRows[i]))
                if (first) {
                    ctx.moveTo(mp.x, mp.y)
                    first = false
                } else {
                    ctx.lineTo(mp.x, mp.y)
                }
            }
            if (!first) {
                ctx.strokeStyle = "rgba(95,22,197,0.88)"
                ctx.lineWidth = 4
                ctx.setLineDash([10, 8])
                ctx.stroke()
                ctx.setLineDash([])
            }

            var fenceRows = advancedMissionManager.geofenceItems
            if (fenceRows.length > 1) {
                ctx.beginPath()
                var fenceStarted = false
                for (var f = 0; f < fenceRows.length; ++f) {
                    if (!root.validCoordinate(fenceRows[f]))
                        continue
                    var fp = root.pointFor(root.rowLatitude(fenceRows[f]), root.rowLongitude(fenceRows[f]))
                    if (!fenceStarted) {
                        ctx.moveTo(fp.x, fp.y)
                        fenceStarted = true
                    } else {
                        ctx.lineTo(fp.x, fp.y)
                    }
                }
                if (fenceStarted) {
                    ctx.closePath()
                    ctx.fillStyle = "rgba(247,201,72,0.12)"
                    ctx.strokeStyle = "rgba(161,92,0,0.92)"
                    ctx.lineWidth = 3
                    ctx.fill()
                    ctx.stroke()
                }
            }
        }
    }

    OverlayLayer {
        objectName: "missionOverlayLayer"
        anchors.fill: parent
        mapItem: root
        z: 3
    }

    Repeater {
        model: missionStore.plan.polygon
        delegate: Rectangle {
            id: edgeInsertHandle
            readonly property int nextIndex: missionStore.plan.polygon.length > 0 ? (index + 1) % missionStore.plan.polygon.length : 0
            readonly property var nextPoint: missionStore.plan.polygon.length > nextIndex ? missionStore.plan.polygon[nextIndex] : modelData
            readonly property var pointA: root.pointFor(modelData.latitude, modelData.longitude)
            readonly property var pointB: root.pointFor(nextPoint.latitude, nextPoint.longitude)
            z: 4.8
            width: 18
            height: 18
            radius: 9
            visible: appState.selectedTool === "polygon"
                     && missionStore.plan.polygon.length > 1
                     && (missionStore.plan.polygon.length > 2 || index === 0)
            x: (pointA.x + pointB.x) / 2 - width / 2
            y: (pointA.y + pointB.y) / 2 - height / 2
            color: insertArea.containsMouse ? "#25d6f5" : "#ffffff"
            border.color: "#25d6f5"
            border.width: 2
            scale: insertArea.containsMouse ? 1.25 : 1.0
            Text {
                anchors.centerIn: parent
                text: "+"
                color: insertArea.containsMouse ? "#ffffff" : "#25d6f5"
                font.pixelSize: 14
                font.bold: true
            }
            MouseArea {
                id: insertArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    var c = root.coordinateFor(Qt.point(edgeInsertHandle.x + edgeInsertHandle.width / 2,
                                                        edgeInsertHandle.y + edgeInsertHandle.height / 2))
                    missionStore.plan.insertPolygonVertex(index, c.latitude, c.longitude)
                    appState.selectedPolygonIndex = index + 1
                    appState.selectedTool = "polygon"
                }
            }
            Behavior on scale { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
            Behavior on color { ColorAnimation { duration: Theme.animFast } }
        }
    }

    Repeater {
        model: missionStore.plan.polygon
        delegate: Rectangle {
            id: polygonHandle
            z: 5
            width: 24
            height: 24
            radius: 12
            scale: handleArea.containsMouse || handleArea.pressed ? 1.2 : 1
            visible: missionStore.plan.polygon.length > 0
            color: root.activeMissionType === "photomap"
                   ? (handleArea.containsMouse ? "#dffbff" : "#102c3a")
                   : (handleArea.containsMouse ? Theme.amber : Theme.white)
            border.color: root.activeMissionType === "photomap" ? "#25d6f5" : Theme.purple
            border.width: 3
            x: root.pointFor(modelData.latitude, modelData.longitude).x - width / 2
            y: root.pointFor(modelData.latitude, modelData.longitude).y - height / 2
            Behavior on color { ColorAnimation { duration: Theme.animFast } }
            Behavior on scale { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
            Text {
                anchors.centerIn: parent
                text: String(index + 1)
                color: root.activeMissionType === "photomap" ? "#ffffff" : Theme.purple
                font.pixelSize: 11
                font.bold: true
            }
            MouseArea {
                id: handleArea
                anchors.fill: parent
                hoverEnabled: true
                drag.target: parent
                onPressed: {
                    appState.selectedPolygonIndex = index
                    appState.selectedTool = "polygon"
                }
                onPositionChanged: {
                    if (pressed) {
                        var c = root.coordinateFor(Qt.point(parent.x + parent.width / 2, parent.y + parent.height / 2))
                        missionStore.plan.movePolygonVertex(index, c.latitude, c.longitude)
                    }
                }
            }
        }
    }

    Repeater {
        model: missionStore.plan.waypoints
        delegate: WaypointOverlay {
            id: waypoint
            z: 5
            indexText: String(index + 1)
            hovered: waypointArea.containsMouse
            selected: appState.selectedWaypointIndex === index || missionStore.plan.activeWaypointIndex === index
            visible: missionStore.plan.waypoints.length > 0
            x: root.pointFor(modelData.latitude, modelData.longitude).x - width / 2
            y: root.pointFor(modelData.latitude, modelData.longitude).y - height / 2
            MouseArea {
                id: waypointArea
                anchors.fill: parent
                hoverEnabled: true
                drag.target: parent
                onPressed: {
                    appState.selectedWaypointIndex = index
                    appState.selectedPolygonIndex = -1
                }
                onPositionChanged: {
                    if (pressed) {
                        var c = root.coordinateFor(Qt.point(parent.x + parent.width / 2, parent.y + parent.height / 2))
                        missionStore.plan.moveWaypoint(index, c.latitude, c.longitude)
                    }
                }
            }
        }
    }

    PoiMarkerOverlay {
        id: poi
        z: 5
        visible: missionStore.plan.hasPoi
        hovered: poiArea.containsMouse
        x: root.pointFor(missionStore.plan.poi.latitude, missionStore.plan.poi.longitude).x - width / 2
        y: root.pointFor(missionStore.plan.poi.latitude, missionStore.plan.poi.longitude).y - height / 2
        MouseArea {
            id: poiArea
            anchors.fill: parent
            hoverEnabled: true
            drag.target: parent
            onPositionChanged: {
                if (pressed) {
                    var c = root.coordinateFor(Qt.point(parent.x + parent.width / 2, parent.y + parent.height / 2))
                    missionStore.plan.setPoi(c.latitude, c.longitude)
                }
            }
        }
    }

    Repeater {
        model: advancedMissionManager.missionItems
        delegate: Rectangle {
            z: 5.5
            width: 26
            height: 26
            radius: 13
            visible: root.validCoordinate(modelData)
                     && (advancedMissionManager.useForUpload || appState.selectedTool === "rawWaypoint")
            color: "#ffffff"
            border.color: "#5f16c5"
            border.width: 3
            x: root.pointFor(root.rowLatitude(modelData), root.rowLongitude(modelData)).x - width / 2
            y: root.pointFor(root.rowLatitude(modelData), root.rowLongitude(modelData)).y - height / 2
            Text {
                anchors.centerIn: parent
                text: "M" + String(modelData.seq)
                color: "#5f16c5"
                font.pixelSize: 9
                font.bold: true
            }
        }
    }

    Repeater {
        model: advancedMissionManager.geofenceItems
        delegate: Rectangle {
            z: 5.4
            width: 24
            height: 24
            radius: 12
            visible: root.validCoordinate(modelData)
            color: "#fff7d6"
            border.color: "#a15c00"
            border.width: 3
            x: root.pointFor(root.rowLatitude(modelData), root.rowLongitude(modelData)).x - width / 2
            y: root.pointFor(root.rowLatitude(modelData), root.rowLongitude(modelData)).y - height / 2
            Text {
                anchors.centerIn: parent
                text: "F" + String(modelData.seq)
                color: "#7a4300"
                font.pixelSize: 9
                font.bold: true
            }
        }
    }

    Repeater {
        model: advancedMissionManager.rallyItems
        delegate: Rectangle {
            z: 5.4
            width: 24
            height: 24
            radius: 12
            visible: root.validCoordinate(modelData)
            color: "#e7fbef"
            border.color: "#167a3a"
            border.width: 3
            x: root.pointFor(root.rowLatitude(modelData), root.rowLongitude(modelData)).x - width / 2
            y: root.pointFor(root.rowLatitude(modelData), root.rowLongitude(modelData)).y - height / 2
            Text {
                anchors.centerIn: parent
                text: "R" + String(modelData.seq)
                color: "#167a3a"
                font.pixelSize: 9
                font.bold: true
            }
        }
    }

    Rectangle {
        id: radiusHandle
        z: 5
        width: 22
        height: 22
        radius: 11
        visible: missionStore.plan.hasPoi && root.activeMissionType === "map3dPoi"
        color: radiusArea.containsMouse || radiusArea.pressed ? Theme.white : Theme.amber
        border.color: Theme.white
        border.width: 2
        scale: radiusArea.containsMouse || radiusArea.pressed ? 1.22 : 1
        x: root.pointFor(missionStore.plan.poi.latitude, missionStore.plan.poi.longitude).x + Math.max(58, Math.min(220, missionStore.plan.radius / 4)) - width / 2
        y: root.pointFor(missionStore.plan.poi.latitude, missionStore.plan.poi.longitude).y - height / 2
        MouseArea {
            id: radiusArea
            anchors.fill: parent
            hoverEnabled: true
            drag.target: parent
            onPositionChanged: {
                if (pressed) {
                    var center = root.pointFor(missionStore.plan.poi.latitude, missionStore.plan.poi.longitude)
                    var dx = parent.x + parent.width / 2 - center.x
                    var dy = parent.y + parent.height / 2 - center.y
                    missionStore.plan.setPoiRadius(Math.sqrt(dx * dx + dy * dy) * 4)
                }
            }
        }
        Behavior on scale { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
        Behavior on color { ColorAnimation { duration: Theme.animFast } }
    }

    Rectangle {
        id: takeoffHandle
        z: 6
        width: 30
        height: 30
        radius: 15
        visible: missionStore.plan.hasTakeoffPoint
        color: takeoffArea.containsMouse || takeoffArea.pressed ? Theme.green : "#ffffff"
        border.color: Theme.green
        border.width: 3
        x: root.pointFor(missionStore.plan.takeoffPoint.latitude, missionStore.plan.takeoffPoint.longitude).x - width / 2
        y: root.pointFor(missionStore.plan.takeoffPoint.latitude, missionStore.plan.takeoffPoint.longitude).y - height / 2
        Text {
            anchors.centerIn: parent
            text: "TO"
            color: takeoffArea.containsMouse || takeoffArea.pressed ? Theme.white : Theme.green
            font.pixelSize: 10
            font.bold: true
        }
        MouseArea {
            id: takeoffArea
            anchors.fill: parent
            hoverEnabled: true
            drag.target: parent
            onPressed: appState.selectedTool = "takeoff"
            onPositionChanged: {
                if (pressed) {
                    var c = root.coordinateFor(Qt.point(parent.x + parent.width / 2, parent.y + parent.height / 2))
                    missionStore.plan.setTakeoffPoint(c.latitude, c.longitude)
                }
            }
        }
    }

    MouseArea {
        objectName: "missionMapInteractionArea"
        anchors.fill: parent
        z: 1
        enabled: root.mapInteractionActive
        acceptedButtons: Qt.LeftButton
        propagateComposedEvents: true
        property real startX: 0
        property real startY: 0
        property bool panning: false
        onPressed: {
            startX = mouse.x
            startY = mouse.y
            panning = false
        }
        onPositionChanged: {
            if (pressed && appState.selectedTool === "select" && Math.abs(mouse.x - startX) + Math.abs(mouse.y - startY) > 3) {
                panning = true
                var a = root.coordinateFor(Qt.point(startX, startY))
                var b = root.coordinateFor(Qt.point(mouse.x, mouse.y))
                mapState.centerLatitude += a.latitude - b.latitude
                mapState.centerLongitude += a.longitude - b.longitude
                mapState.followAircraft = false
                startX = mouse.x
                startY = mouse.y
            }
        }
        onClicked: {
            if (panning) {
                return
            }
            var c = root.coordinateFor(Qt.point(mouse.x, mouse.y))
            if (appState.selectedTool === "rawWaypoint") {
                advancedMissionManager.addMissionCommand(16, 3, 0, 0, 0, 0, c.latitude, c.longitude, missionStore.plan.altitude, 0)
            } else if (appState.selectedTool === "fence") {
                advancedMissionManager.addGeofencePoint(c.latitude, c.longitude, missionStore.plan.altitude, 5001)
            } else if (appState.selectedTool === "rally") {
                advancedMissionManager.addRallyPoint(c.latitude, c.longitude, missionStore.plan.altitude)
            } else if (appState.selectedTool === "takeoff"
                       || (!missionStore.plan.hasTakeoffPoint && ["point", "poi", "route", "polygon"].indexOf(appState.selectedTool) !== -1)) {
                missionStore.plan.setTakeoffPoint(c.latitude, c.longitude)
            } else if (appState.selectedTool === "point" || appState.selectedTool === "route") {
                missionStore.plan.addWaypoint(c.latitude, c.longitude)
            } else if (appState.selectedTool === "poi") {
                missionStore.plan.setPoi(c.latitude, c.longitude)
            } else if (appState.selectedTool === "polygon") {
                missionStore.plan.addPolygonVertex(c.latitude, c.longitude)
            }
        }
        onWheel: function(wheel) {
            if (wheel.angleDelta.y > 0) mapState.zoomIn()
            else mapState.zoomOut()
        }
    }
}
