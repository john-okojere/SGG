import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import SkyGrid 1.0

Rectangle {
    id: root

    signal closeRequested()

    property int activeTab: 0
    property int commandIndex: 0
    property int frameIndex: 3
    property string param1Draft: "0"
    property string param2Draft: "0"
    property string param3Draft: "0"
    property string param4Draft: "0"
    property string latitudeDraft: Number(mapState.centerLatitude).toFixed(7)
    property string longitudeDraft: Number(mapState.centerLongitude).toFixed(7)
    property string altitudeDraft: Number(missionStore.plan.altitude).toFixed(1)
    property int fenceCommand: 5001

    radius: 8
    color: "#ffffff"
    border.color: "#d8cde7"
    border.width: 1
    clip: true

    function numberValue(value, fallback) {
        var parsed = Number(value)
        return isNaN(parsed) ? fallback : parsed
    }

    function rawCoordinate(value) {
        var parsed = Number(value)
        if (isNaN(parsed))
            return 0
        return Math.abs(parsed) > 1000000 ? parsed / 10000000 : parsed
    }

    function commandAt(index) {
        if (index < 0 || index >= advancedMissionManager.commandCatalog.length)
            return advancedMissionManager.commandCatalog.length > 0 ? advancedMissionManager.commandCatalog[0] : { command: 16, frame: 3, name: "NAV_WAYPOINT" }
        return advancedMissionManager.commandCatalog[index]
    }

    function commandIndexFor(command) {
        for (var i = 0; i < advancedMissionManager.commandCatalog.length; ++i) {
            if (Number(advancedMissionManager.commandCatalog[i].command) === Number(command))
                return i
        }
        return 0
    }

    function frameIndexFor(frame) {
        for (var i = 0; i < advancedMissionManager.frameCatalog.length; ++i) {
            if (Number(advancedMissionManager.frameCatalog[i].frame) === Number(frame))
                return i
        }
        return 0
    }

    function frameAt(index) {
        if (index < 0 || index >= advancedMissionManager.frameCatalog.length)
            return advancedMissionManager.frameCatalog.length > 0 ? advancedMissionManager.frameCatalog[0] : { frame: 3, name: "GLOBAL_RELATIVE_ALT" }
        return advancedMissionManager.frameCatalog[index]
    }

    function addDraftCommand() {
        var command = commandAt(commandIndex)
        var frame = frameAt(frameIndex)
        advancedMissionManager.addMissionCommand(Number(command.command),
                                                 Number(frame.frame),
                                                 numberValue(param1Draft, 0),
                                                 numberValue(param2Draft, 0),
                                                 numberValue(param3Draft, 0),
                                                 numberValue(param4Draft, 0),
                                                 numberValue(latitudeDraft, mapState.centerLatitude),
                                                 numberValue(longitudeDraft, mapState.centerLongitude),
                                                 numberValue(altitudeDraft, missionStore.plan.altitude),
                                                 0)
    }

    function uploadSourceLabel() {
        return advancedMissionManager.useForUpload && advancedMissionManager.missionItems.length > 0
               ? "RAW MAVLink"
               : "SkyGrid Plan"
    }

    function editorSafetyText() {
        if (advancedMissionManager.useForUpload && advancedMissionManager.missionItems.length > 0)
            return "Raw upload is selected. Review row order, frames, altitude, fence, and rally data before upload."
        return "SkyGrid plan remains the upload source. Build or enable raw only for engineering command workflows."
    }

    FileDialog {
        id: saveDialog
        title: "Save Mission File"
        fileMode: FileDialog.SaveFile
        nameFilters: ["QGroundControl plan (*.plan)", "QGC WPL (*.waypoints *.txt)", "SkyGrid raw mission (*.json)"]
        onAccepted: advancedMissionManager.saveMissionFile(selectedFile)
    }

    FileDialog {
        id: loadDialog
        title: "Load Mission File"
        fileMode: FileDialog.OpenFile
        nameFilters: ["Mission files (*.plan *.waypoints *.txt *.json)", "All files (*)"]
        onAccepted: advancedMissionManager.loadMissionFile(selectedFile)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text {
                    Layout.fillWidth: true
                    text: "Advanced MAVLink Mission Editor"
                    color: "#14111d"
                    font.pixelSize: 18
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    Layout.fillWidth: true
                    text: advancedMissionManager.status + "  " + advancedMissionManager.compareStatus
                    color: "#706a7e"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
            }

            RowLayout {
                spacing: 8
                Text {
                    text: "Use for upload"
                    color: "#14111d"
                    font.pixelSize: 11
                    font.bold: true
                }
                Switch {
                    checked: advancedMissionManager.useForUpload
                    onToggled: advancedMissionManager.useForUpload = checked
                    ToolTip.visible: hovered
                    ToolTip.text: "When enabled, Mission Upload sends this raw MAVLink table instead of the generated SkyGrid route."
                }
            }

            ToolButton {
                text: "x"
                onClicked: root.closeRequested()
                ToolTip.visible: hovered
                ToolTip.text: "Close advanced editor"
            }
        }

        MissionSummaryStrip { Layout.fillWidth: true }

        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 38
            contentWidth: actionRow.implicitWidth
            contentHeight: actionRow.implicitHeight
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            RowLayout {
                id: actionRow
                height: 34
                spacing: 8
                ActionButton { text: "Build From Plan"; onClicked: advancedMissionManager.buildFromSkyGridRoute(missionStore.plan.generatedRoute, missionStore.plan.takeoffPoint, missionStore.plan.finishAction) }
                ActionButton { text: "Read Aircraft"; onClicked: advancedMissionManager.readMissionFromAircraft(); enabled: !advancedMissionManager.busy }
                ActionButton { text: "Compare"; onClicked: advancedMissionManager.compareLocalToAircraft(); enabled: !advancedMissionManager.busy }
                ActionButton { text: "Save"; onClicked: saveDialog.open() }
                ActionButton { text: "Load"; onClicked: loadDialog.open() }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 34
            radius: 7
            color: "#f0eef3"
            RowLayout {
                anchors.fill: parent
                anchors.margins: 3
                spacing: 0
                TabButton { label: "Mission Items"; active: root.activeTab === 0; onClicked: root.activeTab = 0 }
                TabButton { label: "Geofence"; active: root.activeTab === 1; onClicked: root.activeTab = 1 }
                TabButton { label: "Rally Points"; active: root.activeTab === 2; onClicked: root.activeTab = 2 }
                TabButton { label: "Catalog"; active: root.activeTab === 3; onClicked: root.activeTab = 3 }
            }
        }

        AddCommandRow {
            visible: root.activeTab === 0
            Layout.fillWidth: true
        }

        FenceActionRow {
            visible: root.activeTab === 1
            Layout.fillWidth: true
        }

        RallyActionRow {
            visible: root.activeTab === 2
            Layout.fillWidth: true
        }

        Loader {
            Layout.fillWidth: true
            Layout.fillHeight: true
            sourceComponent: root.activeTab === 0 ? missionTable
                             : (root.activeTab === 1 ? fenceTable
                             : (root.activeTab === 2 ? rallyTable : catalogTable))
        }
    }

    Component {
        id: missionTable
        MissionTable {
            rows: advancedMissionManager.missionItems
            tableWidth: Math.max(root.width - 28, 1280)
        }
    }

    Component {
        id: fenceTable
        RawPointTable {
            rows: advancedMissionManager.geofenceItems
            tableWidth: Math.max(root.width - 28, 980)
            mode: "fence"
        }
    }

    Component {
        id: rallyTable
        RawPointTable {
            rows: advancedMissionManager.rallyItems
            tableWidth: Math.max(root.width - 28, 860)
            mode: "rally"
        }
    }

    Component {
        id: catalogTable
        ScrollView {
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 8
                Repeater {
                    model: advancedMissionManager.commandCatalog
                    delegate: Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: catalogContent.implicitHeight + 16
                        radius: 7
                        color: index % 2 === 0 ? "#ffffff" : "#fbfaff"
                        border.color: "#e2dceb"
                        RowLayout {
                            id: catalogContent
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.margins: 8
                            spacing: 10
                            Text { Layout.preferredWidth: 92; text: modelData.category; color: "#3b0787"; font.pixelSize: 11; font.bold: true; elide: Text.ElideRight }
                            Text { Layout.preferredWidth: 230; text: modelData.name + " (" + modelData.command + ")"; color: "#14111d"; font.pixelSize: 12; font.bold: true; elide: Text.ElideRight }
                            Text { Layout.fillWidth: true; text: modelData.detail; color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
                            ActionButton {
                                Layout.preferredWidth: 72
                                text: "Add"
                                onClicked: advancedMissionManager.addMissionCommand(modelData.command, modelData.frame, 0, 0, 0, 0, mapState.centerLatitude, mapState.centerLongitude, missionStore.plan.altitude, modelData.category === "FENCE" ? 1 : (modelData.category === "RALLY" ? 2 : 0))
                            }
                        }
                    }
                }
            }
        }
    }

    component MissionSummaryStrip: Rectangle {
        implicitHeight: summaryContent.childrenRect.height + 14
        radius: 7
        color: advancedMissionManager.useForUpload ? "#fff8df" : "#fbfaff"
        border.color: advancedMissionManager.useForUpload ? "#e8cf7c" : "#e2dceb"
        Flow {
            id: summaryContent
            anchors.fill: parent
            anchors.margins: 7
            spacing: 8
            SummaryTile { label: "Upload Source"; value: root.uploadSourceLabel(); accent: advancedMissionManager.useForUpload ? "#a86c00" : "#3b0787" }
            SummaryTile { label: "Mission Items"; value: String(advancedMissionManager.missionItems.length); accent: "#14111d" }
            SummaryTile { label: "Geofence"; value: String(advancedMissionManager.geofenceItems.length); accent: "#14111d" }
            SummaryTile { label: "Rally"; value: String(advancedMissionManager.rallyItems.length); accent: "#14111d" }
            SummaryTile { label: "Aircraft"; value: vehicleManager.connected ? "ONLINE" : "OFFLINE"; accent: vehicleManager.connected ? "#178a3f" : "#b00020" }
            Text {
                width: summaryContent.width < 760 ? summaryContent.width : Math.max(180, summaryContent.width - 620)
                text: root.editorSafetyText()
                color: "#4f465b"
                font.pixelSize: 11
                wrapMode: Text.WordWrap
            }
        }
    }

    component SummaryTile: Rectangle {
        property string label: ""
        property string value: ""
        property color accent: "#14111d"
        width: Math.min(116, parent ? parent.width : 116)
        Layout.preferredWidth: 116
        implicitHeight: 42
        radius: 6
        color: "#ffffff"
        border.color: "#e2dceb"
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 6
            spacing: 1
            Text {
                Layout.fillWidth: true
                text: label
                color: "#706a7e"
                font.pixelSize: 9
                elide: Text.ElideRight
            }
            Text {
                Layout.fillWidth: true
                text: value
                color: accent
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
            }
        }
    }

    component AddCommandRow: Rectangle {
        radius: 7
        color: "#fbfaff"
        border.color: "#e2dceb"
        implicitHeight: 56
        ScrollView {
            anchors.fill: parent
            anchors.margins: 8
            contentWidth: addCommandRow.implicitWidth
            contentHeight: addCommandRow.implicitHeight
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            RowLayout {
                id: addCommandRow
                height: 40
                spacing: 8
                ComboBox {
                    Layout.preferredWidth: 220
                    model: advancedMissionManager.commandCatalog
                    textRole: "name"
                    currentIndex: root.commandIndex
                    onActivated: {
                        root.commandIndex = index
                        root.frameIndex = root.frameIndexFor(root.commandAt(index).frame)
                    }
                }
                ComboBox {
                    Layout.preferredWidth: 170
                    model: advancedMissionManager.frameCatalog
                    textRole: "name"
                    currentIndex: root.frameIndex
                    onActivated: root.frameIndex = index
                }
                SmallField { Layout.preferredWidth: 72; placeholderText: "P1"; text: root.param1Draft; onTextChanged: root.param1Draft = text }
                SmallField { Layout.preferredWidth: 72; placeholderText: "P2"; text: root.param2Draft; onTextChanged: root.param2Draft = text }
                SmallField { Layout.preferredWidth: 72; placeholderText: "P3"; text: root.param3Draft; onTextChanged: root.param3Draft = text }
                SmallField { Layout.preferredWidth: 72; placeholderText: "P4"; text: root.param4Draft; onTextChanged: root.param4Draft = text }
                SmallField { Layout.preferredWidth: 112; placeholderText: "Lat"; text: root.latitudeDraft; onTextChanged: root.latitudeDraft = text }
                SmallField { Layout.preferredWidth: 112; placeholderText: "Lon"; text: root.longitudeDraft; onTextChanged: root.longitudeDraft = text }
                SmallField { Layout.preferredWidth: 78; placeholderText: "Alt"; text: root.altitudeDraft; onTextChanged: root.altitudeDraft = text }
                ActionButton { Layout.preferredWidth: 92; text: "Add Row"; onClicked: root.addDraftCommand() }
                ActionButton { Layout.preferredWidth: 104; text: "Write Mission"; onClicked: advancedMissionManager.writeMissionToAircraft(); enabled: !advancedMissionManager.busy && advancedMissionManager.missionItems.length > 0 }
                ActionButton {
                    Layout.preferredWidth: 104
                    text: appState.selectedTool === "rawWaypoint" ? "Map Tool On" : "Map Row Tool"
                    onClicked: appState.selectedTool = "rawWaypoint"
                }
            }
        }
    }

    component FenceActionRow: Rectangle {
        radius: 7
        color: "#fbfaff"
        border.color: "#e2dceb"
        implicitHeight: 48
        ScrollView {
            anchors.fill: parent
            anchors.margins: 8
            contentWidth: fenceActionContent.implicitWidth
            contentHeight: fenceActionContent.implicitHeight
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            RowLayout {
                id: fenceActionContent
                height: 32
                spacing: 8
                ComboBox {
                    Layout.preferredWidth: 220
                    model: ["Inclusion Polygon", "Exclusion Polygon", "Inclusion Circle", "Exclusion Circle", "Return Point"]
                    onActivated: root.fenceCommand = index === 1 ? 5002 : (index === 2 ? 5003 : (index === 3 ? 5004 : (index === 4 ? 5000 : 5001)))
                }
                ActionButton { text: "Add Map Center"; onClicked: advancedMissionManager.addGeofencePoint(mapState.centerLatitude, mapState.centerLongitude, missionStore.plan.altitude, root.fenceCommand) }
                ActionButton {
                    text: appState.selectedTool === "fence" ? "GF Tool On" : "GF Map Tool"
                    onClicked: appState.selectedTool = "fence"
                }
                ActionButton { text: "Read Fence"; onClicked: advancedMissionManager.readGeofenceFromAircraft(); enabled: !advancedMissionManager.busy }
                ActionButton { text: "Upload Fence"; onClicked: advancedMissionManager.writeGeofenceToAircraft(); enabled: !advancedMissionManager.busy && advancedMissionManager.geofenceItems.length > 0 }
                ActionButton { text: "Clear Fence"; onClicked: advancedMissionManager.clearGeofenceItems() }
                Text { Layout.preferredWidth: 300; text: "Select the GF map tool, then click the map to place geofence vertices."; color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
            }
        }
    }

    component RallyActionRow: Rectangle {
        radius: 7
        color: "#fbfaff"
        border.color: "#e2dceb"
        implicitHeight: 48
        ScrollView {
            anchors.fill: parent
            anchors.margins: 8
            contentWidth: rallyActionContent.implicitWidth
            contentHeight: rallyActionContent.implicitHeight
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            RowLayout {
                id: rallyActionContent
                height: 32
                spacing: 8
                ActionButton { text: "Add Map Center"; onClicked: advancedMissionManager.addRallyPoint(mapState.centerLatitude, mapState.centerLongitude, missionStore.plan.altitude) }
                ActionButton {
                    text: appState.selectedTool === "rally" ? "RP Tool On" : "RP Map Tool"
                    onClicked: appState.selectedTool = "rally"
                }
                ActionButton { text: "Read Rally"; onClicked: advancedMissionManager.readRallyPointsFromAircraft(); enabled: !advancedMissionManager.busy }
                ActionButton { text: "Upload Rally"; onClicked: advancedMissionManager.writeRallyPointsToAircraft(); enabled: !advancedMissionManager.busy && advancedMissionManager.rallyItems.length > 0 }
                ActionButton { text: "Clear Rally"; onClicked: advancedMissionManager.clearRallyItems() }
                Text { Layout.preferredWidth: 300; text: "Select the RP map tool, then click the map to place rally points."; color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
            }
        }
    }

    component MissionTable: ScrollView {
        property var rows: []
        property int tableWidth: 1280
        contentWidth: tableWidth
        clip: true
        Column {
            width: tableWidth
            spacing: 2
            TableHeader { width: parent.width; headers: ["Seq", "Command", "Frame", "P1", "P2", "P3", "P4", "Lat", "Lon", "Alt", "Cur", "Auto", "Tools"] }
            Repeater {
                model: rows
                delegate: MissionRow {
                    width: parent.width
                    rowIndex: index
                    rowData: modelData
                }
            }
        }
    }

    component MissionRow: Rectangle {
        property int rowIndex: 0
        property var rowData: ({})
        height: 44
        color: rowIndex % 2 === 0 ? "#ffffff" : "#fbfaff"
        border.color: "#eee8f5"
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            spacing: 6
            Text { Layout.preferredWidth: 36; text: String(rowData.seq); color: "#706a7e"; font.pixelSize: 11; elide: Text.ElideRight }
            ComboBox {
                Layout.preferredWidth: 190
                model: advancedMissionManager.commandCatalog
                textRole: "name"
                currentIndex: root.commandIndexFor(rowData.command)
                onActivated: advancedMissionManager.updateMissionItem(rowIndex, { command: root.commandAt(index).command, frame: root.commandAt(index).frame })
            }
            ComboBox {
                Layout.preferredWidth: 145
                model: advancedMissionManager.frameCatalog
                textRole: "name"
                currentIndex: root.frameIndexFor(rowData.frame)
                onActivated: advancedMissionManager.updateMissionItem(rowIndex, { frame: root.frameAt(index).frame })
            }
            EditCell { value: rowData.param1; widthHint: 62; onCommitted: advancedMissionManager.updateMissionItem(rowIndex, { param1: Number(valueText) }) }
            EditCell { value: rowData.param2; widthHint: 62; onCommitted: advancedMissionManager.updateMissionItem(rowIndex, { param2: Number(valueText) }) }
            EditCell { value: rowData.param3; widthHint: 62; onCommitted: advancedMissionManager.updateMissionItem(rowIndex, { param3: Number(valueText) }) }
            EditCell { value: rowData.param4; widthHint: 62; onCommitted: advancedMissionManager.updateMissionItem(rowIndex, { param4: Number(valueText) }) }
            EditCell { value: rowData.latitude !== undefined ? rowData.latitude : root.rawCoordinate(rowData.x); widthHint: 98; decimals: 7; onCommitted: advancedMissionManager.updateMissionItem(rowIndex, { latitude: Number(valueText) }) }
            EditCell { value: rowData.longitude !== undefined ? rowData.longitude : root.rawCoordinate(rowData.y); widthHint: 98; decimals: 7; onCommitted: advancedMissionManager.updateMissionItem(rowIndex, { longitude: Number(valueText) }) }
            EditCell { value: rowData.altitude !== undefined ? rowData.altitude : rowData.z; widthHint: 72; decimals: 1; onCommitted: advancedMissionManager.updateMissionItem(rowIndex, { altitude: Number(valueText) }) }
            CheckBox {
                Layout.preferredWidth: 42
                checked: Number(rowData.current) !== 0
                onToggled: advancedMissionManager.updateMissionItem(rowIndex, { current: checked ? 1 : 0 })
            }
            CheckBox {
                Layout.preferredWidth: 48
                checked: Number(rowData.autocontinue) !== 0
                onToggled: advancedMissionManager.updateMissionItem(rowIndex, { autocontinue: checked ? 1 : 0 })
            }
            RowLayout {
                Layout.preferredWidth: 154
                spacing: 4
                TinyButton { text: "^"; enabled: rowIndex > 0; onClicked: advancedMissionManager.moveMissionItem(rowIndex, rowIndex - 1) }
                TinyButton { text: "v"; enabled: rowIndex < advancedMissionManager.missionItems.length - 1; onClicked: advancedMissionManager.moveMissionItem(rowIndex, rowIndex + 1) }
                TinyButton { text: "+"; onClicked: advancedMissionManager.duplicateMissionItem(rowIndex) }
                TinyButton { text: "-"; onClicked: advancedMissionManager.removeMissionItem(rowIndex) }
            }
        }
    }

    component RawPointTable: ScrollView {
        id: rawPointTable
        property var rows: []
        property int tableWidth: 860
        property string mode: "fence"
        contentWidth: tableWidth
        clip: true
        Column {
            width: tableWidth
            spacing: 2
            TableHeader { width: parent.width; headers: mode === "fence" ? ["Seq", "Command", "Frame", "Lat", "Lon", "Alt", "Radius/P1", "Tools"] : ["Seq", "Lat", "Lon", "Alt", "Tools"] }
            Repeater {
                model: rows
                delegate: PointRow {
                    width: parent.width
                    rowIndex: index
                    rowData: modelData
                    mode: rawPointTable.mode
                }
            }
        }
    }

    component PointRow: Rectangle {
        property int rowIndex: 0
        property var rowData: ({})
        property string mode: "fence"
        height: 44
        color: rowIndex % 2 === 0 ? "#ffffff" : "#fbfaff"
        border.color: "#eee8f5"
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            spacing: 6
            Text { Layout.preferredWidth: 40; text: String(rowData.seq); color: "#706a7e"; font.pixelSize: 11 }
            ComboBox {
                visible: mode === "fence"
                Layout.preferredWidth: 210
                model: ["5001 Inclusion Polygon", "5002 Exclusion Polygon", "5003 Inclusion Circle", "5004 Exclusion Circle", "5000 Return Point"]
                currentIndex: Number(rowData.command) === 5002 ? 1 : (Number(rowData.command) === 5003 ? 2 : (Number(rowData.command) === 5004 ? 3 : (Number(rowData.command) === 5000 ? 4 : 0)))
                onActivated: advancedMissionManager.updateGeofenceItem(rowIndex, { command: index === 1 ? 5002 : (index === 2 ? 5003 : (index === 3 ? 5004 : (index === 4 ? 5000 : 5001))) })
            }
            EditCell {
                visible: mode === "fence"
                value: rowData.frame
                widthHint: 64
                decimals: 0
                onCommitted: advancedMissionManager.updateGeofenceItem(rowIndex, { frame: Number(valueText) })
            }
            EditCell {
                value: rowData.latitude !== undefined ? rowData.latitude : root.rawCoordinate(rowData.x)
                widthHint: 112
                decimals: 7
                onCommitted: mode === "fence" ? advancedMissionManager.updateGeofenceItem(rowIndex, { latitude: Number(valueText) }) : advancedMissionManager.updateRallyItem(rowIndex, { latitude: Number(valueText) })
            }
            EditCell {
                value: rowData.longitude !== undefined ? rowData.longitude : root.rawCoordinate(rowData.y)
                widthHint: 112
                decimals: 7
                onCommitted: mode === "fence" ? advancedMissionManager.updateGeofenceItem(rowIndex, { longitude: Number(valueText) }) : advancedMissionManager.updateRallyItem(rowIndex, { longitude: Number(valueText) })
            }
            EditCell {
                value: rowData.altitude !== undefined ? rowData.altitude : rowData.z
                widthHint: 78
                decimals: 1
                onCommitted: mode === "fence" ? advancedMissionManager.updateGeofenceItem(rowIndex, { altitude: Number(valueText) }) : advancedMissionManager.updateRallyItem(rowIndex, { altitude: Number(valueText) })
            }
            EditCell {
                visible: mode === "fence"
                value: rowData.param1
                widthHint: 86
                onCommitted: advancedMissionManager.updateGeofenceItem(rowIndex, { param1: Number(valueText) })
            }
            RowLayout {
                Layout.preferredWidth: 76
                TinyButton { text: "-"; onClicked: mode === "fence" ? advancedMissionManager.removeGeofenceItem(rowIndex) : advancedMissionManager.removeRallyItem(rowIndex) }
            }
            Item { Layout.fillWidth: true }
        }
    }

    component TableHeader: Rectangle {
        id: tableHeader
        property var headers: []
        height: 30
        color: "#eee8f7"
        radius: 5
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            spacing: 6
            Repeater {
                model: tableHeader.headers
                delegate: Text {
                    Layout.preferredWidth: index === 0 ? 40 : (modelData === "Tools" ? 154 : (modelData === "Command" ? 190 : (modelData === "Frame" ? 145 : 70)))
                    Layout.fillWidth: index === tableHeader.headers.length - 1 && modelData !== "Tools"
                    text: modelData
                    color: "#4a4259"
                    font.pixelSize: 10
                    font.bold: true
                    elide: Text.ElideRight
                }
            }
        }
    }

    component EditCell: TextField {
        id: editCell
        property real value: 0
        property int widthHint: 72
        property int decimals: 2
        signal committed(string valueText)
        Layout.preferredWidth: widthHint
        text: Number(value).toFixed(decimals)
        color: "#050505"
        selectByMouse: true
        font.pixelSize: 10
        horizontalAlignment: Text.AlignRight
        validator: DoubleValidator {}
        onEditingFinished: committed(text)
        background: Rectangle { radius: 4; color: "#ffffff"; border.color: editCell.activeFocus ? "#8b62c4" : "#d8cde7" }
    }

    component SmallField: TextField {
        id: smallField
        color: "#050505"
        selectByMouse: true
        font.pixelSize: 11
        validator: DoubleValidator {}
        background: Rectangle { radius: 5; color: "#ffffff"; border.color: smallField.activeFocus ? "#8b62c4" : "#d8cde7" }
    }

    component TinyButton: Button {
        implicitWidth: 28
        implicitHeight: 26
        font.pixelSize: 10
        font.bold: true
        background: Rectangle { radius: 5; color: parent.enabled ? "#f8f6fb" : "#eeeef3"; border.color: "#d8cde7" }
    }

    component ActionButton: Button {
        id: actionButton
        Layout.preferredHeight: 30
        font.pixelSize: 11
        font.bold: true
        background: Rectangle {
            radius: 6
            color: actionButton.enabled ? (actionButton.hovered ? "#f0e7fb" : "#ffffff") : "#eeeef3"
            border.color: "#d8cde7"
        }
        contentItem: Text {
            text: actionButton.text
            color: actionButton.enabled ? "#3b0787" : "#8b8498"
            font: actionButton.font
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    component TabButton: Button {
        id: tabButton
        property string label: ""
        property bool active: false
        Layout.fillWidth: true
        text: label
        background: Rectangle { radius: 5; color: tabButton.active ? "#3b0787" : "transparent" }
        contentItem: Text {
            text: tabButton.text
            color: tabButton.active ? "#ffffff" : "#3b0787"
            font.pixelSize: 11
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
}
