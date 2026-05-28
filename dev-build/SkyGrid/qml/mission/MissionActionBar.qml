import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SkyGrid 1.0
import "../controls"

GridLayout {
    id: root
    columns: width > 720 ? 4 : 2
    rowSpacing: 8
    columnSpacing: 10

    function hasUploadRoute() {
        return !missionStore.plan.boundaryOnly && missionStore.plan.generatedRoute.length >= 2
    }

    function backendReadyForUpload() {
        return !missionStore.plan.createdLocally
                && missionStore.plan.backendSyncReady
                && missionStore.plan.backendUploadEligible
                && missionStore.plan.backendValidationState !== "blocked"
    }

    function weatherBlocksUpload() {
        return weatherManager.available && windCheckManager.result === "Do Not Fly"
    }

    function aircraftId() {
        return missionSyncManager.assignedAircraft.length > 0
            ? String(missionSyncManager.assignedAircraft[0].id)
            : ""
    }

    function missionCenter() {
        var source = missionStore.plan.polygon.length > 0
            ? missionStore.plan.polygon
            : missionStore.plan.waypoints
        if (missionStore.plan.hasPoi) {
            return { latitude: missionStore.plan.poi.latitude, longitude: missionStore.plan.poi.longitude }
        }
        if (source.length === 0) {
            return { latitude: mapState.centerLatitude, longitude: mapState.centerLongitude }
        }
        var lat = 0
        var lon = 0
        for (var i = 0; i < source.length; ++i) {
            lat += source[i].latitude
            lon += source[i].longitude
        }
        return { latitude: lat / source.length, longitude: lon / source.length }
    }

    function runValidation() {
        var center = missionCenter()
        weatherSyncManager.refreshForMission(center.latitude, center.longitude, missionStore.plan.altitude, aircraftId(), "MISSION")
        missionSyncManager.validateActiveMission()
        preflightChecklistManager.runChecklist(true)
    }

    function uploadReason() {
        if (!sessionManager.operationsAllowed) return "Device approval is required."
        if (missionStore.plan.boundaryOnly) return "Virtual Fence syncs as a Control Center boundary, not an aircraft route."
        if (missionStore.plan.createdLocally) return "Save and sync with Control Center before upload."
        if (!missionStore.plan.backendSyncReady || !missionStore.plan.backendUploadEligible) return "Run Control Center validation before upload."
        if (!vehicleManager.connected) return "No connected aircraft."
        if (preflightChecklistManager.blocked || preflightChecklistManager.cautionRequired) {
            return preflightChecklistManager.blockReason()
        }
        if (!missionStore.plan.missionReady) return "Resolve critical validation checks first."
        if (root.weatherBlocksUpload()) return "Control Center weather check is Do Not Fly."
        if (!root.hasUploadRoute()) return "Mission needs at least two route items."
        if (missionUploadManager.uploading) return "Upload already in progress."
        return "Upload mission to aircraft."
    }

    function startReason() {
        if (!sessionManager.operationsAllowed) return "Device approval is required."
        if (missionStore.plan.boundaryOnly) return "Virtual Fence is a Control Center boundary, not a startable aircraft mission."
        if (!vehicleManager.connected) return "No connected aircraft."
        if (!preflightChecklistManager.canStartMission) return preflightChecklistManager.blockReason()
        if (missionStore.plan.uploadState !== "uploaded") return "Upload mission before starting."
        if (missionExecutionManager.executing) return "Mission is already running."
        return "Start uploaded mission."
    }

    PrimaryButton {
        Layout.fillWidth: true
        secondary: true
        text: "Save Mission"
        iconSource: AssetRegistry.icons.boxicons_save
        enabled: sessionManager.operationsAllowed && missionStore.plan.missionState !== "EMPTY"
        opacity: enabled ? 1 : 0.55
        onClicked: missionSyncManager.saveActiveMission()
        ToolTip.visible: hovered && !enabled
        ToolTip.text: sessionManager.operationsAllowed ? "Create a mission first." : "Device approval is required."
    }
    PrimaryButton {
        Layout.fillWidth: true
        secondary: true
        enabled: sessionManager.operationsAllowed && (root.hasUploadRoute() || missionStore.plan.boundaryOnly)
        text: sessionManager.operationsAllowed
            ? (missionSyncManager.syncing ? "Validating..." : "Validate")
            : "Device Approval Required"
        iconSource: AssetRegistry.icons.boxicons_play
        opacity: enabled ? 1 : 0.55
        onClicked: root.runValidation()
        ToolTip.visible: hovered && !enabled
        ToolTip.text: sessionManager.operationsAllowed ? "Add mission geometry before validation." : "Device approval is required."
    }
    PrimaryButton {
        Layout.fillWidth: true
        enabled: sessionManager.operationsAllowed && root.backendReadyForUpload() && vehicleManager.connected && preflightChecklistManager.canUpload && !root.weatherBlocksUpload() && root.hasUploadRoute() && !missionUploadManager.uploading
        text: missionStore.plan.boundaryOnly ? "Boundary Synced" : (missionUploadManager.uploading ? ("Uploading " + missionUploadManager.progress + "%") : "Upload to Aircraft")
        iconSource: AssetRegistry.icons.cloud
        opacity: enabled ? 1 : 0.55
        onClicked: missionUploadManager.uploadActiveMission()
        ToolTip.visible: hovered && !enabled
        ToolTip.text: root.uploadReason()
    }
    PrimaryButton {
        Layout.fillWidth: true
        enabled: sessionManager.operationsAllowed && !missionStore.plan.boundaryOnly && vehicleManager.connected && preflightChecklistManager.canStartMission && !missionExecutionManager.executing
        text: missionExecutionManager.executing ? "Executing..." : "Start Mission"
        iconSource: AssetRegistry.icons.boxicons_play
        opacity: enabled ? 1 : 0.55
        onClicked: missionExecutionManager.startMission()
        ToolTip.visible: hovered && !enabled
        ToolTip.text: root.startReason()
    }
}
