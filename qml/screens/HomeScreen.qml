import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SkyGrid 1.0
import "../dashboard"
import "../controls"

Item {
    id: root

    property string activeTab: "mission"
    property string searchText: ""
    property string toast: ""
    property string modalTitle: ""
    property string modalBody: ""
    property bool modalOpen: false

    readonly property bool phone: width < 720
    readonly property bool narrow: width < 980
    readonly property bool compact: width < 1180
    readonly property bool tablet: width < 1440
    readonly property bool showLeftSidebar: width >= 1180 && height >= 680
    readonly property bool showRightSidebar: false
    readonly property int topBarHeight: phone ? 96 : (narrow ? 86 : 74)
    readonly property int chromeReserveWidth: 0
    readonly property int leftSidebarWidth: Math.max(374, Math.min(450, Math.round(width * 0.18) + 150))
    readonly property int rightSidebarWidth: Math.max(280, Math.min(300, Math.round(width * 0.2)))
    readonly property int dashboardOuterMargin: narrow ? 12 : 18
    readonly property int dashboardGap: narrow ? 12 : 16
    readonly property int dashboardMaxWidth: 1440
    readonly property int centerContentWidth: Math.max(0, width - (showLeftSidebar ? leftSidebarWidth : 0) - (showRightSidebar ? rightSidebarWidth : 0) - (dashboardOuterMargin * 2) - dashboardGap)
    readonly property bool onePageDashboard: !phone && centerContentWidth >= 820 && height >= 680
    readonly property bool wideDashboardGrid: centerContentWidth >= 820
    readonly property int summaryGridColumns: centerContentWidth < 720 ? 1 : (centerContentWidth < 820 ? 2 : 3)
    readonly property int operationsGridColumns: centerContentWidth < 820 ? 1 : 2
    readonly property int quickActionColumns: phone || centerContentWidth < 760 ? 1 : 2
    readonly property int assignedAircraftTargetWidth: 500
    readonly property bool dashboardHasSideCard: wideDashboardGrid && dashboardActionCount() > 0
    readonly property int dashboardGridColumns: dashboardHasSideCard ? 2 : 1
    readonly property int aircraftColumns: 1
    readonly property int heroCardHeight: phone ? 124 : (height < 760 ? 150 : (height < 820 ? 172 : (compact ? 172 : 190)))
    readonly property int mainCardHeight: phone ? 320 : (height < 760 ? 238 : (height < 820 ? 278 : (compact ? 278 : 300)))
    readonly property int summaryCardHeight: phone ? 176 : (height < 760 ? 172 : (height < 820 ? 176 : 196))
    readonly property int quickActionTileHeight: height < 760 ? 74 : 86
    readonly property bool showSecondaryOperations: height >= 860
    readonly property int assignedAircraftHeight: mainCardHeight
    readonly property color purple: "#4B3DA0"
    readonly property color purpleDark: "#30006f"
    readonly property color surface: "#f6f4fa"
    readonly property color ink: "#14111d"
    readonly property color muted: "#706a7e"
    readonly property color line: "#e2dceb"

    function hasObject(name) {
        if (name === "telemetryStore")
            return typeof telemetryStore !== "undefined";
        if (name === "missionSyncManager")
            return typeof missionSyncManager !== "undefined";
        if (name === "missionStore")
            return typeof missionStore !== "undefined";
        if (name === "profileManager")
            return typeof profileManager !== "undefined";
        if (name === "weatherManager")
            return typeof weatherManager !== "undefined";
        if (name === "weatherSyncManager")
            return typeof weatherSyncManager !== "undefined";
        if (name === "eventLogManager")
            return typeof eventLogManager !== "undefined";
        if (name === "appState")
            return typeof appState !== "undefined";
        if (name === "authManager")
            return typeof authManager !== "undefined";
        if (name === "localSyncCache")
            return typeof localSyncCache !== "undefined";
        if (name === "sessionManager")
            return typeof sessionManager !== "undefined";
        if (name === "preferencesManager")
            return typeof preferencesManager !== "undefined";
        if (name === "flightStatsManager")
            return typeof flightStatsManager !== "undefined";
        if (name === "preflightChecklistManager")
            return typeof preflightChecklistManager !== "undefined";
        if (name === "permissionManager")
            return typeof permissionManager !== "undefined";
        if (name === "moduleAccessManager")
            return typeof moduleAccessManager !== "undefined";
        if (name === "permissionGuard")
            return typeof permissionGuard !== "undefined";
        if (name === "roleAccessManager")
            return typeof roleAccessManager !== "undefined";
        if (name === "accessManager")
            return typeof accessManager !== "undefined";
        if (name === "gcsToolCatalog")
            return typeof gcsToolCatalog !== "undefined";
        return false;
    }

    function textValue(item, keys, fallback) {
        if (!item)
            return fallback;
        for (var i = 0; i < keys.length; ++i) {
            var value = item[keys[i]];
            if (value !== undefined && value !== null && String(value).length > 0)
                return String(value);
        }
        return fallback;
    }

    function numberValue(item, keys, fallback) {
        var text = textValue(item, keys, "");
        return text.length > 0 && !isNaN(Number(text)) ? Number(text) : fallback;
    }

    function safeList(value) {
        return value && value.length !== undefined ? value : [];
    }

    function profileName() {
        if (hasObject("profileManager") && profileManager.displayName.length > 0)
            return profileManager.displayName;
        var user = hasObject("roleAccessManager") ? roleAccessManager.user : ({});
        return textValue(user, ["display_name", "name", "full_name", "email"], "Signed-out operator");
    }

    function organizationName() {
        if (hasObject("profileManager") && profileManager.organizationName.length > 0 && profileManager.organizationName !== "SkyGrid Operations")
            return profileManager.organizationName;
        var organization = hasObject("missionSyncManager") ? missionSyncManager.organization : ({});
        return textValue(organization, ["name", "display_name"], "No organization");
    }

    function telemetryConnected() {
        return hasObject("telemetryStore") && telemetryStore.connected;
    }

    function canViewTelemetryData() {
        return accessManager.can("can_stream_telemetry")
            || accessManager.can("can_view_telemetry")
            || accessManager.can("can_use_flight_data");
    }

    function isManufacturer() {
        return hasObject("roleAccessManager") && roleAccessManager.isManufacturer;
    }

    function dashboardTitle() {
        return isManufacturer() ? "Manufacturer Operations Dashboard" : "Mission Operations Center";
    }

    function dashboardSubtitle() {
        return isManufacturer() ? "Configure, plan, test, validate, and release authorized vehicles" : "Real-time overview of your operations and system status";
    }

    function missionActionTitle() {
        return isManufacturer() ? "Test Mission Plan" : "New Mission";
    }

    function pilotActionTitle() {
        return "Pilot Mode";
    }

    function missionItems() {
        if (!accessManager.can("can_plan_mission"))
            return []
        var active = hasObject("missionSyncManager") ? safeList(missionSyncManager.activeMissions) : []
        var history = hasObject("missionSyncManager") ? safeList(missionSyncManager.missionHistory) : []
        var live = active.length > 0 ? active : (hasObject("missionSyncManager") ? safeList(missionSyncManager.approvedMissions) : [])
        var cached = hasObject("missionStore") ? safeList(missionStore.missionHistory) : []
        var source = history.length > 0 ? history : (live.length > 0 ? live : cached)
        return source
    }

    function activeMissionItems() {
        var all = missionItems();
        var out = [];
        for (var i = 0; i < all.length; ++i) {
            if (missionIsActive(all[i]))
                out.push(all[i]);
        }
        return out;
    }

    function filteredMissions() {
        var all = missionItems();
        var query = root.searchText.toLowerCase();
        if (query.length === 0)
            return all;
        var out = [];
        for (var i = 0; i < all.length; ++i) {
            var name = textValue(all[i], ["name", "title", "mission_name"], "").toLowerCase();
            var type = textValue(all[i], ["mission_type", "type", "kind"], "").toLowerCase();
            if (name.indexOf(query) >= 0 || type.indexOf(query) >= 0)
                out.push(all[i]);
        }
        return out;
    }

    function aircraftItems() {
        if (!accessManager.can("can_view_fleet") && !root.canViewTelemetryData())
            return []
        var assigned = hasObject("missionSyncManager") ? safeList(missionSyncManager.assignedAircraft) : []
        if (assigned.length > 0)
            return assigned
        if (telemetryConnected())
            return [{ name: telemetryStore.aircraftId, tail_number: telemetryStore.aircraftId, battery_percent: telemetryStore.battery, status: "Connected", link_quality: telemetryStore.transmission + "%" }]
        return []
    }

    function hasManufacturerAccess() {
        return accessManager.canModule("manufacturer_dashboard")
            || accessManager.canModule("manufacturer_test_flight")
            || accessManager.canModule("vehicle_profiles")
            || accessManager.can("can_configure_vehicle")
            || accessManager.can("can_run_manufacturer_test_flight")
            || accessManager.can("can_fly_manual_test")
            || accessManager.can("can_edit_vehicle_profile")
            || accessManager.can("can_register_vehicle")
    }

    function gcsToolsAvailable() {
        return hasObject("gcsToolCatalog")
            && gcsToolCatalog.availableCount > 0
            && root.canModule("gcs_tools")
            && (!hasObject("accessManager") || accessManager.canPerform("gcs_tools"))
    }

    function canOpenPilotMode() {
        return !hasObject("accessManager") || accessManager.canPerform("manual_flight")
    }

    function canOpenMissionPlanner() {
        return !hasObject("accessManager") || accessManager.canPerform("mission_planning")
    }

    function actionDenial(action, fallback) {
        if (hasObject("accessManager")) {
            var reason = accessManager.denialReasonForAction(action)
            if (reason.length > 0)
                return reason
        }
        return fallback
    }

    function pilotModeDenial() {
        return actionDenial("manual_flight", "Missing manual flight authority")
    }

    function missionPlannerDenial() {
        return actionDenial("mission_planning", "Missing mission planning authority")
    }

    function manufacturerName() {
        var manufacturer = hasObject("missionSyncManager") ? missionSyncManager.manufacturer : ({})
        return textValue(manufacturer, ["name", "display_name", "manufacturer_display"], "Manufacturer Workspace")
    }

    function vehicleProfileItems() {
        return hasObject("missionSyncManager") ? safeList(missionSyncManager.vehicleProfiles) : []
    }

    function onlineAircraftCount() {
        var items = aircraftItems();
        if (telemetryConnected())
            return Math.max(1, onlineAircraftFromList(items));
        return onlineAircraftFromList(items);
    }

    function onlineAircraftFromList(items) {
        var count = 0;
        for (var i = 0; i < items.length; ++i) {
            var status = textValue(items[i], ["telemetry_status", "connection_status", "status", "health_status", "readiness"], "").toLowerCase();
            var online = items[i].is_online === true || items[i].connected === true
                    || status.indexOf("online") >= 0
                    || status.indexOf("connected") >= 0
                    || status.indexOf("active") >= 0
                    || status.indexOf("healthy") >= 0;
            if (online)
                count += 1;
        }
        return count;
    }

    function missionStatus(mission) {
        return textValue(mission, ["execution_state", "status", "sync_state", "upload_state"], "").toLowerCase();
    }

    function truthyValue(value) {
        return value === true || String(value).toLowerCase() === "true" || String(value) === "1";
    }

    function missionIsActive(mission) {
        var status = missionStatus(mission);
        return status.indexOf("active") >= 0
            || status.indexOf("execut") >= 0
            || status.indexOf("progress") >= 0
            || status.indexOf("started") >= 0;
    }

    function missionIsReady(mission) {
        var status = missionStatus(mission);
        var validation = textValue(mission, ["validation_state", "backend_validation_state"], "").toLowerCase();
        return truthyValue(mission.sync_ready)
            || truthyValue(mission.upload_eligible)
            || validation === "valid"
            || status.indexOf("approved") >= 0
            || status.indexOf("ready") >= 0;
    }

    function readyMissionItems() {
        var all = missionItems();
        var out = [];
        for (var i = 0; i < all.length; ++i) {
            if (missionIsReady(all[i]))
                out.push(all[i]);
        }
        return out;
    }

    function notificationCount() {
        return hasObject("profileManager") ? safeList(profileManager.notifications).length : 0;
    }

    function currentAircraft() {
        var items = aircraftItems();
        return items.length > 0 ? items[0] : ({});
    }

    function telemetryBatteryLabel() {
        if (telemetryConnected())
            return telemetryStore.battery + "%";
        var aircraft = currentAircraft();
        var battery = numberValue(aircraft, ["battery_percent", "battery"], -1);
        return battery >= 0 ? Math.round(battery) + "%" : "--";
    }

    function telemetrySatellitesLabel() {
        if (telemetryConnected())
            return String(telemetryStore.satellites);
        var aircraft = currentAircraft();
        var satellites = numberValue(aircraft, ["satellites", "gps_satellites"], -1);
        return satellites >= 0 ? String(Math.round(satellites)) : "--";
    }

    function telemetryLinkLabel() {
        if (telemetryConnected())
            return telemetryStore.transmission + "%";
        return textValue(currentAircraft(), ["link_quality", "rc_quality", "telemetry_status", "status"], "Offline");
    }

    function telemetryCameraLabel() {
        if (telemetryConnected())
            return telemetryStore.recordingState;
        return textValue(currentAircraft(), ["camera_status", "fpv_status"], "--");
    }

    function telemetrySpeedLabel() {
        return telemetryConnected() ? Number(telemetryStore.speed).toFixed(1) + " m/s" : "--";
    }

    function telemetryAltitudeLabel() {
        return telemetryConnected() ? Number(telemetryStore.altitude).toFixed(0) + " m" : "--";
    }

    function systemStateLabel() {
        if (telemetryConnected() && telemetryStore.armed)
            return "ARMED";
        if (hasObject("sessionManager") && sessionManager.operationsAllowed)
            return "READY";
        return "WAIT";
    }

    function controlCenterStatusLabel() {
        if (!hasObject("sessionManager"))
            return "Unavailable";
        if (sessionManager.controlCenterReachable)
            return "Online";
        if (sessionManager.operationsAllowed)
            return "Trusted cache";
        return "Sign in";
    }

    function syncStatusLabel() {
        if (hasObject("missionSyncManager") && missionSyncManager.status.length > 0)
            return missionSyncManager.status;
        return controlCenterStatusLabel();
    }

    function deviceStatusLabel() {
        var sessionStatus = hasObject("missionSyncManager") ? missionSyncManager.sessionStatus : ({});
        var device = hasObject("missionSyncManager") ? missionSyncManager.deviceSummary : ({});
        if (hasObject("sessionManager") && sessionManager.operationsAllowed)
            return "Trusted";
        return textValue(sessionStatus, ["device_status", "status", "reason"],
                         textValue(device, ["status", "device_status"], hasObject("sessionManager") ? sessionManager.blockReason : "Sign in required"));
    }

    function operatorRoleLabel() {
        if (hasObject("accessManager") && accessManager.role.length > 0)
            return accessManager.role.replace(/_/g, " ");
        if (hasObject("roleAccessManager") && roleAccessManager.currentRole.length > 0)
            return roleAccessManager.currentRole.replace(/_/g, " ");
        return "No role";
    }

    function missionPlanStatusLabel() {
        if (root.hasObject("missionStore") && missionStore.plan && missionStore.plan.name.length > 0)
            return missionStore.plan.name;
        var ready = readyMissionItems();
        if (ready.length > 0)
            return ready.length + " ready";
        return missionItems().length > 0 ? "Needs validation" : "No mission";
    }

    function pilotAssignmentLabel() {
        var aircraft = currentAircraft();
        return textValue(aircraft, ["name", "tail_number", "serial_number", "aircraft_id"], "Unassigned");
    }

    function lastSessionLabel() {
        if (hasObject("sessionManager") && sessionManager.operationsAllowed)
            return "Current session active";
        return deviceStatusLabel();
    }

    function dashboardActionCount() {
        var count = 0;
        if (!root.showLeftSidebar && root.gcsToolsAvailable())
            count += 1;
        if (!root.showLeftSidebar && root.hasManufacturerAccess())
            count += 1;
        if (root.hasManufacturerAccess())
            count += 2;
        if (!root.showLeftSidebar && !root.hasManufacturerAccess() && root.canPermission("can_plan_mission") && root.canModule("mission_planning"))
            count += 1;
        if (!root.showLeftSidebar && !root.hasManufacturerAccess() && root.canPermission("can_configure_vehicle") && root.canModule("vehicle_configuration"))
            count += 1;
        return count;
    }

    function dashboardActionRows() {
        var count = root.dashboardActionCount()
        if (count <= 0)
            return 0
        return Math.ceil(count / Math.max(1, root.quickActionColumns))
    }

    function dashboardActionPanelHeight() {
        var rows = root.dashboardActionRows()
        return rows <= 0 ? 0 : (rows * 106 + 72)
    }

    function weatherStatus() {
        if (!hasObject("weatherManager") || !weatherManager.available)
            return "SOURCE_UNAVAILABLE";
        var status = String(weatherManager.status).toUpperCase();
        if (status.indexOf("DO_NOT") >= 0 || status.indexOf("D/NF") >= 0)
            return "DO_NOT_FLY";
        if (status.indexOf("CAUTION") >= 0)
            return "CAUTION";
        if (status.indexOf("GOOD") >= 0)
            return "GOOD_TO_FLY";
        return "GOOD_TO_FLY";
    }

    function weatherLabel() {
        var status = weatherStatus();
        if (status === "SOURCE_UNAVAILABLE")
            return "Pending";
        if (status === "DO_NOT_FLY")
            return "Blocked";
        if (status === "CAUTION")
            return "Caution";
        return "Good";
    }

    function weatherDirectionLabel(deg) {
        var labels = ["N", "NE", "E", "SE", "S", "SW", "W", "NW"];
        var index = Math.round((((deg % 360) + 360) % 360) / 45) % 8;
        return labels[index];
    }

    function missionIcon(type) {
        var normalized = String(type || "").toLowerCase();
        if (normalized.indexOf("photo") >= 0 || normalized.indexOf("grid") >= 0)
            return AssetRegistry.icons.lucide_grid_3x3;
        if (normalized.indexOf("fence") >= 0 || normalized.indexOf("geo") >= 0)
            return AssetRegistry.icons.cube;
        if (normalized.indexOf("poi") >= 0 || normalized.indexOf("building") >= 0)
            return AssetRegistry.icons.lucide_building_2;
        if (normalized.indexOf("3d") >= 0 || normalized.indexOf("terrain") >= 0 || normalized.indexOf("map") >= 0)
            return AssetRegistry.icons.lucide_mountain;
        if (normalized.indexOf("route") >= 0 || normalized.indexOf("waypoint") >= 0)
            return AssetRegistry.icons.lucide_route;
        if (normalized.indexOf("tower") >= 0 || normalized.indexOf("inspection") >= 0)
            return AssetRegistry.icons.waypoint;
        return AssetRegistry.icons.lucide_waypoints;
    }

    property int readinessRevision: 0
    property alias flickable: flickable

    function readinessPercent() {
        root.readinessRevision;
        if (root.hasObject("preflightChecklistManager"))
            return preflightChecklistManager.readinessPercent;
        var checks = readinessFallbackItems();
        var passed = 0;
        for (var i = 0; i < checks.length; ++i) {
            if (checks[i].ok)
                passed += 1;
        }
        return checks.length > 0 ? Math.round((passed * 100) / checks.length) : 0;
    }

    function readinessStatusText() {
        var percent = readinessPercent();
        if (root.hasObject("preflightChecklistManager") && preflightChecklistManager.status.length > 0)
            return preflightChecklistManager.status;
        if (percent >= 90)
            return "Ready for mission review";
        if (percent >= 70)
            return "Caution items need review";
        return "Blocked until key systems are ready";
    }

    function readinessCheckItems() {
        root.readinessRevision;
        if (root.hasObject("preflightChecklistManager")) {
            var items = [];
            var checks = preflightChecklistManager.checks || [];
            for (var i = 0; i < checks.length; ++i) {
                var check = checks[i];
                items.push({
                    label: check.category || check.label || "Check",
                    value: check.message || check.label || "",
                    ok: !!check.passed
                });
            }
            if (items.length > 0)
                return items;
        }
        return readinessFallbackItems();
    }

    function readinessFallbackItems() {
        var aircraftCount = aircraftItems().length;
        var connected = telemetryConnected();
        var missionCount = missionItems().length;
        var readyMissions = readyMissionItems().length;
        return [
            {
                label: "Aircraft readiness",
                value: connected ? telemetryStore.aircraftReadiness : (aircraftCount > 0 ? aircraftCount + " assigned" : "No assigned aircraft"),
                ok: connected ? telemetryStore.aircraftReady : aircraftCount > 0
            },
            {
                label: "Telemetry readiness",
                value: connected ? "Live telemetry" : "No live aircraft link",
                ok: connected
            },
            {
                label: "Battery / power",
                value: telemetryBatteryLabel(),
                ok: !connected || telemetryStore.battery >= 35
            },
            {
                label: "GPS / link",
                value: connected ? telemetryStore.satellites + " sats, " + telemetryStore.transmission + "%" : telemetryLinkLabel(),
                ok: connected ? telemetryStore.satellites >= 8 && telemetryStore.transmission >= 50 : aircraftCount > 0
            },
            {
                label: "Mission plan",
                value: readyMissions > 0 ? readyMissions + " ready" : (missionCount > 0 ? "Needs validation" : "No mission plan"),
                ok: readyMissions > 0 || (root.hasObject("missionStore") && missionStore.plan && missionStore.plan.missionReady)
            },
            {
                label: "Operator / device",
                value: deviceStatusLabel(),
                ok: hasObject("sessionManager") && sessionManager.operationsAllowed
            },
            {
                label: "Weather",
                value: weatherLabel(),
                ok: weatherStatus() === "GOOD_TO_FLY"
            }
        ];
    }

    function readinessDetailText() {
        var checks = readinessCheckItems();
        var lines = [
            "Mission readiness: " + readinessPercent() + "%",
            readinessStatusText(),
            ""
        ];
        for (var i = 0; i < checks.length; ++i) {
            lines.push(checks[i].label + ": " + checks[i].value + " - " + (checks[i].ok ? "OK" : "Review"));
        }
        var blockers = [];
        for (var j = 0; j < checks.length; ++j) {
            if (!checks[j].ok)
                blockers.push(checks[j].label);
        }
        lines.push("");
        lines.push(blockers.length > 0 ? "Warnings/blockers: " + blockers.join(", ") : "Warnings/blockers: none");
        if (root.hasObject("preflightChecklistManager")) {
            var reason = preflightChecklistManager.blockReason();
            if (reason.length > 0)
                lines.push("Block reason: " + reason);
        }
        return lines.join("\n");
    }

    function dashboardDetailText() {
        return [
            "Control Center: " + controlCenterStatusLabel(),
            "Device: " + deviceStatusLabel(),
            "Aircraft: " + onlineAircraftCount() + " online / " + aircraftItems().length + " assigned",
            "Missions: " + activeMissionItems().length + " active / " + readyMissionItems().length + " ready / " + missionItems().length + " visible",
            "Telemetry: " + (telemetryConnected() ? telemetryStore.aircraftId + " live" : "No live aircraft link"),
            "Sync: " + syncStatusLabel()
        ].join("\n");
    }

    function aircraftDetailText(aircraft) {
        return [
            "Aircraft: " + textValue(aircraft, ["name", "model", "tail_number", "serial_number", "aircraft_id"], "Unknown"),
            "Identifier: " + textValue(aircraft, ["tail_number", "serial_number", "aircraft_id", "id"], "--"),
            "Battery: " + textValue(aircraft, ["battery_percent", "battery"], telemetryConnected() ? telemetryStore.battery + "%" : "--"),
            "Link: " + textValue(aircraft, ["link_quality", "rc_quality", "telemetry_status", "status"], telemetryLinkLabel()),
            "Last sync: " + textValue(aircraft, ["last_sync", "last_seen", "updated_at"], "No recent sync"),
            "Readiness: " + textValue(aircraft, ["readiness", "health_status", "status"], telemetryConnected() ? telemetryStore.aircraftReadiness : "Pending telemetry")
        ].join("\n");
    }

    function fleetDetailText() {
        var items = aircraftItems();
        var lines = ["Assigned aircraft: " + items.length, "Online aircraft: " + onlineAircraftCount(), ""];
        for (var i = 0; i < items.length; ++i) {
            lines.push(textValue(items[i], ["name", "model", "tail_number", "serial_number", "aircraft_id"], "Aircraft") + " - " + textValue(items[i], ["telemetry_status", "status", "health_status"], "status pending"));
        }
        if (items.length === 0)
            lines.push("No assigned aircraft are available in the current role scope.");
        return lines.join("\n");
    }

    function systemDetailText() {
        var rows = systemStatusRows();
        var lines = [];
        for (var i = 0; i < rows.length; ++i)
            lines.push(rows[i].label + ": " + rows[i].value + " - " + (rows[i].ok ? "OK" : "Review"));
        return lines.join("\n");
    }

    function systemStatusRows() {
        return [
            { label: "Command & Control", value: hasObject("sessionManager") && sessionManager.operationsAllowed ? "Operational" : deviceStatusLabel(), ok: hasObject("sessionManager") && sessionManager.operationsAllowed },
            { label: "Control Center", value: controlCenterStatusLabel(), ok: hasObject("sessionManager") && sessionManager.controlCenterReachable },
            { label: "Aircraft Link", value: telemetryConnected() ? telemetryStore.aircraftId : "Offline", ok: telemetryConnected() },
            { label: "GNSS", value: telemetryConnected() ? telemetryStore.satellites + " satellites" : "Waiting for telemetry", ok: telemetryConnected() && telemetryStore.satellites >= 8 },
            { label: "Mission Plan", value: missionPlanStatusLabel(), ok: readyMissionItems().length > 0 || (root.hasObject("missionStore") && missionStore.plan && missionStore.plan.missionReady) },
            { label: "Sync", value: syncStatusLabel(), ok: hasObject("missionSyncManager") && !missionSyncManager.syncing }
        ];
    }

    function activityDetailText() {
        var items = activityItems();
        var lines = [
            "Recent activity: " + items.length,
            "Notifications: " + notificationCount(),
            "Mission history: " + missionItems().length,
            ""
        ];
        for (var i = 0; i < items.length; ++i)
            lines.push(items[i].time + " - " + items[i].message);
        if (items.length === 0)
            lines.push("No recent activity is available for the current role scope.");
        return lines.join("\n");
    }

    function activityItems() {
        if (!accessManager.can("can_view_vehicle_audit"))
            return []
        var events = hasObject("eventLogManager") ? safeList(eventLogManager.events) : []
        var recent = hasObject("profileManager") ? safeList(profileManager.recentActivity) : []
        var source = events.length > 0 ? events : recent
        if (source.length > 0) {
            var out = [];
            for (var i = 0; i < Math.min(source.length, 8); ++i) {
                out.push({
                    message: textValue(source[i], ["message", "title", "event_type"], "Operational event received"),
                    time: textValue(source[i], ["time", "recorded_at", "created_at"], "just now"),
                    color: textValue(source[i], ["severity"], "info") === "warning" ? "#f4b000" : "#4B3DA0"
                });
            }
            return out;
        }
        return []
    }

    function recentMissionSummaryItems() {
        var all = root.filteredMissions()
        var out = []
        for (var i = 0; i < Math.min(all.length, 4); ++i) {
            out.push({
                label: root.textValue(all[i], ["name", "title", "mission_name"], "Untitled Mission"),
                detail: root.textValue(all[i], ["mission_type", "type", "kind"], "Mission"),
                value: root.textValue(all[i], ["status", "execution_state", "sync_state"], "Pending"),
                color: "#4B3DA0"
            })
        }
        return out
    }

    function notificationSummaryItems() {
        var notes = root.hasObject("profileManager") ? root.safeList(profileManager.notifications) : []
        var out = []
        for (var i = 0; i < Math.min(notes.length, 4); ++i) {
            out.push({
                label: root.textValue(notes[i], ["title", "message", "type"], "Notification"),
                detail: root.textValue(notes[i], ["message", "body", "detail"], ""),
                value: root.textValue(notes[i], ["time", "created_at", "recorded_at"], ""),
                color: root.textValue(notes[i], ["severity", "level"], "info") === "warning" ? "#f4b000" : "#4B3DA0"
            })
        }
        return out
    }

    function showPanel(title, body) {
        var action = panelAccessAction(title)
        if (action.length > 0 && !accessManager.canPerform(action)) {
            blockPanel(title, action)
            return
        }
        root.modalTitle = title
        root.modalBody = body
        root.modalOpen = true
        root.toast = title
    }

    function panelAccessAction(title) {
        if (title === "Activity Feed" || title === "Flight Logs")
            return "security_audit"
        if (title === "Readiness Details" || title === "Operations Center" || title === "Control Center")
            return "telemetry_stream"
        if (title === "Change Pilot")
            return "settings"
        if (title === "Fleet Center" || title === "Aircraft Details")
            return "aircraft_profile_access"
        if (title === "Dashboard Settings")
            return "settings"
        if (title === "Mission Filters")
            return "mission_open"
        return ""
    }

    function canShowPanel(title) {
        var action = panelAccessAction(title)
        return action.length === 0 || accessManager.canPerform(action)
    }

    function blockPanel(title, action) {
        root.toast = title + " blocked by local permissions."
        accessManager.recordBlocked(action, title + " panel blocked by local permissions.", { panel: title })
    }

    function logAction(type, message) {
        if (hasObject("eventLogManager"))
            eventLogManager.logEvent(type, "info", message);
        root.toast = message;
    }

    function canPermission(scope) {
        if (hasObject("accessManager") && accessManager.accessLoaded)
            return accessManager.can(scope);
        return hasObject("permissionManager") && permissionManager.hasPermission(scope);
    }

    function canModule(moduleName) {
        if (hasObject("accessManager") && accessManager.accessLoaded && accessManager.canModule(moduleName))
            return true;
        if (hasObject("moduleAccessManager") && moduleAccessManager.moduleAllowed(moduleName))
            return true;
        if (moduleName === "manufacturer_dashboard")
            return canPermission("can_configure_vehicle") || canPermission("can_run_manufacturer_test_flight");
        if (moduleName === "mission_planning")
            return canPermission("can_plan_mission") || canPermission("can_upload_mission") || canPermission("can_start_mission") || canPermission("can_view_mission_logs");
        if (moduleName === "pilot_operations" || moduleName === "pilot_mode")
            return canPermission("can_fly_manual");
        if (moduleName === "vehicle_configuration")
            return canPermission("can_configure_vehicle");
        if (moduleName === "manufacturer_test_flight")
            return canPermission("can_run_manufacturer_test_flight");
        if (moduleName === "release_vehicle")
            return canPermission("can_release_vehicle_to_organization");
        if (moduleName === "fleet")
            return canPermission("can_view_fleet") || canPermission("can_assign_aircraft") || canPermission("can_assign_pilots");
        return false;
    }

    function redirectToDefaultWorkspace() {
        if (!hasObject("appState") || !hasObject("moduleAccessManager"))
            return;
        if ((appState.currentScreen === "home" || appState.currentScreen === "missionSelector") && moduleAccessManager.defaultWorkspace !== "home")
            appState.openDefaultWorkspace(moduleAccessManager.defaultWorkspace);
    }

    function deny(message) {
        root.toast = message;
    }

    function startPilotMode() {
        if (typeof accessManager !== "undefined" && !accessManager.canPerform("manual_flight")) {
            root.toast = "Pilot Mode blocked by local permissions."
            return
        }
        logAction("pilot_mode_opened", isManufacturer() ? "Opening manufacturer test flight" : "Opening Pilot Mode");
        if (hasObject("appState"))
            appState.startPilotMode();
    }

    function openNewMission() {
        if (typeof accessManager !== "undefined" && !accessManager.canPerform("mission_planning")) {
            root.toast = "Mission planning blocked by local permissions."
            return
        }
        logAction("new_mission_opened", isManufacturer() ? "Opening manufacturer test mission planner" : "Opening mission planner");
        if (hasObject("appState"))
            appState.openMissionSelector();
    }

    function openVehicleConfiguration() {
        if (root.hasManufacturerAccess()) {
            root.openManufacturerWorkspace();
            return;
        }
        if (hasObject("permissionGuard") && (!permissionGuard.requirePermission("can_configure_vehicle") || !permissionGuard.requireModule("vehicle_configuration"))) {
            deny(permissionGuard.lastDenial);
            return;
        }
        if (!canPermission("can_configure_vehicle") || !canModule("vehicle_configuration")) {
            deny("Vehicle configuration is not available for this role.");
            return;
        }
        logAction("vehicle_configuration_opened", "Opening vehicle configuration");
        if (hasObject("appState"))
            appState.openVehicleConfiguration();
    }

    function openManufacturerWorkspace() {
        if (!root.hasManufacturerAccess()) {
            deny("Manufacturer workspace is not available for this role.");
            return;
        }
        logAction("manufacturer_workspace_opened", "Opening manufacturer workspace");
        if (hasObject("appState"))
            appState.openManufacturerWorkspace();
    }

    function openGcsTools() {
        if (!root.gcsToolsAvailable()) {
            deny("GCS tools are not available for this role.");
            return;
        }
        logAction("gcs_tools_opened", "Opening GCS tools");
        if (hasObject("appState"))
            appState.openGcsTools();
    }

    function openHelpCenter() {
        if (hasObject("appState"))
            appState.openHelpCenter();
    }

    function missionRecordId(mission) {
        return textValue(mission, ["id", "local_id"], "");
    }

    function openMissionInPlanner(mission) {
        if (!canModule("mission_planning")) {
            root.toast = "Mission workspace is not available for this role.";
            return;
        }
        if (!mission)
            return;
        var missionId = missionRecordId(mission);
        if (missionId.length === 0) {
            root.toast = "This sample mission cannot be opened. Sync missions from Control Center or create a new mission.";
            return;
        }
        if (typeof accessManager !== "undefined"
            && (!accessManager.canPerform("mission_open") || !accessManager.canAccessMission(missionId))) {
            root.toast = "Mission open blocked by local permissions."
            return
        }
        if (!hasObject("missionSyncManager")) {
            root.toast = "Mission sync is unavailable.";
            return;
        }
        logAction("mission_opened", "Opening mission in planner: " + textValue(mission, ["name", "title", "mission_name"], missionId));
        missionSyncManager.openMission(mission);
    }

    function refreshWeather() {
        if (hasObject("weatherSyncManager") && hasObject("telemetryStore") && Math.abs(telemetryStore.latitude) > 0.000001)
            weatherSyncManager.refreshForCoordinate(telemetryStore.latitude, telemetryStore.longitude);
        else
            root.toast = "Weather will sync when a trusted session and location are available";
    }

    function refreshControlCenterDashboard(manual) {
        if (!hasObject("sessionManager")) {
            return;
        }
        if (!sessionManager.operationsAllowed) {
            sessionManager.validateSession();
            if (manual)
                root.toast = "Checking Control Center session...";
            return;
        }
        if (hasObject("missionSyncManager") && !missionSyncManager.syncing) {
            missionSyncManager.bootstrap();
        }
        if (hasObject("weatherSyncManager") && hasObject("telemetryStore") && Math.abs(telemetryStore.latitude) > 0.000001) {
            weatherSyncManager.refreshForCoordinate(telemetryStore.latitude, telemetryStore.longitude);
        }
        if (manual)
            root.toast = sessionManager.controlCenterReachable ? "Control Center sync refreshed" : "Control Center unreachable; using local cache";
    }

    Timer {
        id: toastTimer
        interval: 2600
        repeat: false
        onTriggered: root.toast = ""
    }

    Component.onCompleted: Qt.callLater(root.redirectToDefaultWorkspace)

    Connections {
        target: moduleAccessManager
        function onModulesChanged() {
            root.redirectToDefaultWorkspace();
        }
    }

    onToastChanged: {
        if (toast.length > 0)
            toastTimer.restart();
    }

    Rectangle {
        anchors.fill: parent
        color: root.surface
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.topBarHeight
            color: "#ffffff"
            border.color: root.line
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 0
                anchors.rightMargin: root.chromeReserveWidth
                spacing: root.narrow ? 8 : 14

                Image {
                    Layout.preferredWidth: root.narrow ? 132 : (root.compact ? 166 : 220)
                    Layout.preferredHeight: root.narrow ? 36 : 44
                    source: AssetRegistry.logos.full_logo
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    mipmap: true
                }

                Flickable {
                    id: flickable
                    width: 800
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.topBarHeight
                    contentWidth: Math.max(width, pillRow.implicitWidth)
                    contentHeight: height
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    Row {
                        id: pillRow
                        height: 48
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 10

                        TopStatusPill {
                            compact: root.narrow
                            iconSource: AssetRegistry.icons.plane
                            value: root.onlineAircraftCount() + "/" + root.aircraftItems().length
                            label: "Aircraft"
                            accent: root.purple
                            onClicked: root.showPanel("Fleet Center", root.fleetDetailText())
                        }
                        TopStatusPill {
                            compact: root.narrow
                            iconSource: AssetRegistry.icons.boxicons_battery_3
                            value: root.telemetryBatteryLabel()
                            label: "Battery"
                            accent: root.telemetryConnected() && telemetryStore.battery < 35 ? "#ef4444" : "#28b947"
                            onClicked: root.showPanel("Readiness Details", root.readinessDetailText())
                        }
                        TopStatusPill {
                            compact: root.narrow
                            iconSource: AssetRegistry.icons.lucide_satellite
                            value: root.telemetrySatellitesLabel()
                            label: "GPS"
                            accent: root.purple
                            onClicked: root.showPanel("Readiness Details", root.readinessDetailText())
                        }
                        TopStatusPill {
                            compact: root.narrow
                            iconSource: AssetRegistry.icons.boxicons_wifi
                            value: root.telemetryLinkLabel()
                            label: "Link"
                            accent: "#28b947"
                            onClicked: root.showPanel("System Status", root.systemDetailText())
                        }
                        TopStatusPill {
                            compact: root.narrow
                            iconSource: AssetRegistry.icons.boxicons_camera
                            value: root.telemetryCameraLabel()
                            label: "Camera"
                            accent: "#171222"
                            onClicked: root.showPanel("System Status", root.systemDetailText())
                        }
                        TopStatusPill {
                            compact: root.narrow
                            iconSource: AssetRegistry.icons.boxicons_wind_filled
                            value: root.telemetrySpeedLabel()
                            label: "Speed"
                            accent: "#171222"
                            onClicked: root.showPanel("Aircraft Details", root.aircraftDetailText(root.currentAircraft()))
                        }
                        TopStatusPill {
                            compact: root.narrow
                            iconSource: AssetRegistry.icons.lucide_mountain
                            value: root.telemetryAltitudeLabel()
                            label: "Alt"
                            accent: "#171222"
                            onClicked: root.showPanel("Aircraft Details", root.aircraftDetailText(root.currentAircraft()))
                        }
                        TopStatusPill {
                            compact: root.narrow
                            iconSource: AssetRegistry.icons.mdi_check_circle
                            value: root.systemStateLabel()
                            label: "System"
                            accent: "#28b947"
                            onClicked: root.showPanel("System Status", root.systemDetailText())
                        }
                    }
                }

                Rectangle {
                    Layout.preferredWidth: root.narrow ? 46 : 92
                    Layout.preferredHeight: 46
                    radius: 23
                    color: helpMouse.containsMouse ? "#f2ecfb" : "#ffffff"
                    border.color: root.line
                    ToolTip.text: "Open Help Center"
                    ToolTip.visible: helpMouse.containsMouse

                    Row {
                        anchors.centerIn: parent
                        spacing: 6
                        Text {
                            text: "?"
                            color: root.purple
                            font.pixelSize: 18
                            font.bold: true
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            visible: !root.narrow
                            text: "Help"
                            color: root.ink
                            font.pixelSize: 12
                            font.bold: true
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MouseArea {
                        id: helpMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: root.openHelpCenter()
                    }
                }

                Rectangle {
                    Layout.preferredWidth: root.narrow ? 48 : (root.compact ? 58 : 230)
                    Layout.preferredHeight: 50
                    radius: 28
                    color: profileMouse.containsMouse ? "#f7f3fb" : "#ffffff"
                    border.color: "#e1dceb"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 10
                        spacing: 10

                        Rectangle {
                            Layout.preferredWidth: 36
                            Layout.preferredHeight: 36
                            radius: 18
                            gradient: Gradient {
                                GradientStop {
                                    position: 0
                                    color: "#5f16c5"
                                }
                                GradientStop {
                                    position: 1
                                    color: "#2f006b"
                                }
                            }
                            AssetIcon {
                                anchors.centerIn: parent
                                width: 19
                                height: 19
                                iconSize: 19
                                source: AssetRegistry.icons.mdi_person_outline
                                active: true
                                inactiveOpacity: 1
                            }
                        }

                        ColumnLayout {
                            visible: !root.compact
                            Layout.fillWidth: true
                            spacing: 1
                            Text {
                                Layout.fillWidth: true
                                text: root.profileName()
                                color: root.ink
                                font.pixelSize: 12
                                font.bold: true
                                elide: Text.ElideRight
                            }
                            Text {
                                Layout.fillWidth: true
                                text: root.organizationName()
                                color: root.muted
                                font.pixelSize: 9
                                elide: Text.ElideRight
                            }
                        }

                        Text {
                            visible: !root.compact
                            text: "v"
                            color: root.purple
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }

                    MouseArea {
                        id: profileMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: profileMenu.open()
                    }

                    Popup {
                        id: profileMenu
                        x: parent.width - width
                        y: parent.height + 8
                        width: 190
                        height: 104
                        modal: false
                        focus: true
                        background: Rectangle {
                            radius: 8
                            color: "#ffffff"
                            border.color: root.line
                        }
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            Text {
                                text: "Profile Settings"
                                color: root.ink
                                font.pixelSize: 12
                                font.bold: true
                                Layout.fillWidth: true
                            }
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 1
                                color: root.line
                            }
                            Text {
                                text: "Logout"
                                color: root.purple
                                font.pixelSize: 12
                                font.bold: true
                                Layout.fillWidth: true
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        profileMenu.close();
                                        if (root.hasObject("authManager"))
                                            authManager.logout();
                                        else
                                            root.toast = "Logout requested";
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                visible: root.showLeftSidebar
                Layout.preferredWidth: root.leftSidebarWidth
                Layout.fillHeight: true
                color: "#fbfaff"
                border.color: root.line
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 14

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        rowSpacing: 6
                        columnSpacing: 6
                        SidebarTabButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            visible: root.canModule("mission_planning") && accessManager.can("can_plan_mission")
                            text: "Mission"
                            iconSource: AssetRegistry.icons.lucide_route
                            active: root.activeTab === "mission"
                            enabled: root.canModule("mission_planning") && accessManager.can("can_plan_mission")
                            onClicked: root.activeTab = "mission"
                        }
                        SidebarTabButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            visible: root.hasManufacturerAccess() || (root.canModule("vehicle_configuration") && accessManager.can("can_configure_vehicle"))
                            text: root.hasManufacturerAccess() ? "Manufacturer" : "Vehicle"
                            iconSource: AssetRegistry.icons.cube
                            active: root.activeTab === "vehicle"
                            enabled: root.hasManufacturerAccess() || (root.canModule("vehicle_configuration") && accessManager.can("can_configure_vehicle"))
                            onClicked: {
                                root.activeTab = "vehicle";
                                if (root.hasManufacturerAccess())
                                    root.openManufacturerWorkspace();
                                else
                                    root.openVehicleConfiguration();
                            }
                        }
                        SidebarTabButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            visible: root.gcsToolsAvailable()
                            text: "Tools"
                            iconSource: AssetRegistry.icons.lucide_grid_3x3
                            active: root.activeTab === "tools"
                            enabled: root.gcsToolsAvailable()
                            onClicked: {
                                root.activeTab = "tools";
                                root.openGcsTools();
                            }
                        }
                        SidebarTabButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            text: "Map"
                            iconSource: AssetRegistry.icons.lucide_satellite
                            active: root.activeTab === "map"
                            onClicked: root.activeTab = "map"
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        visible: root.canModule("mission_planning")
                        spacing: 8
                        Rectangle {
                            visible: accessManager.can("can_plan_mission")
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            radius: 8
                            color: "#ffffff"
                            border.color: root.line
                            TextInput {
                                anchors.fill: parent
                                anchors.leftMargin: 38
                                anchors.rightMargin: 12
                                verticalAlignment: TextInput.AlignVCenter
                                text: root.searchText
                                color: root.ink
                                font.pixelSize: 12
                                clip: true
                                onTextChanged: root.searchText = text
                            }
                            Text {
                                visible: root.searchText.length === 0
                                anchors.left: parent.left
                                anchors.leftMargin: 38
                                anchors.verticalCenter: parent.verticalCenter
                                text: "Search missions..."
                                color: "#9b94a6"
                                font.pixelSize: 12
                            }
                            AssetIcon {
                                anchors.left: parent.left
                                anchors.leftMargin: 13
                                anchors.verticalCenter: parent.verticalCenter
                                width: 16
                                height: 16
                                iconSize: 16
                                source: AssetRegistry.icons.boxicons_search_big
                                active: true
                            }
                        }
                        Rectangle {
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 44
                            radius: 8
                            color: filterMouse.containsMouse ? "#f2ecfb" : "#ffffff"
                            border.color: root.line
                            AssetIcon {
                                anchors.centerIn: parent
                                width: 18
                                height: 18
                                iconSize: 18
                                source: AssetRegistry.icons.boxicons_filter
                                active: true
                            }
                            MouseArea {
                                id: filterMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: root.showPanel("Mission Filters", "Filter panel ready for mission type, status, date range, and sync state.")
                            }
                        }
                    }

                    Text {
                        visible: root.canModule("mission_planning")
                        text: "MISSION HISTORY"
                        color: root.ink
                        font.pixelSize: 11
                        font.bold: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: root.canModule("mission_planning")
                        clip: true
                        spacing: 10
                        model: root.filteredMissions()
                        delegate: MissionHistoryItem {
                            width: ListView.view.width
                            missionName: root.textValue(modelData, ["name", "title", "mission_name"], "Untitled Mission")
                            category: root.textValue(modelData, ["mission_type", "type", "kind"], "Surveillance")
                            timestamp: root.textValue(modelData, ["date", "updated_at", "created_at"], "Cached locally")
                            status: root.textValue(modelData, ["status", "execution_state", "sync_state"], "Completed")
                            badgeColor: category === "Logistics" ? "#e3efff" : (category === "Reconnaissance" ? "#ffe8d5" : (category === "Training" ? "#e4efff" : "#efe2ff"))
                            badgeTextColor: category === "Logistics" ? "#1f64c8" : (category === "Reconnaissance" ? "#d46c00" : (category === "Training" ? "#276be8" : "#5b22a8"))
                            iconSource: root.missionIcon(category)
                            onClicked: root.openMissionInPlanner(modelData)
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            visible: root.canPermission("can_plan_mission") && root.canModule("mission_planning")
                            radius: 8
                            opacity: 1
                            color: newMissionMouse.containsMouse ? "#4f0aa6" : "#3b0787"
                            Row {
                                anchors.centerIn: parent
                                spacing: 8
                                AssetIcon {
                                    width: 17
                                    height: 17
                                    iconSize: 17
                                    source: AssetRegistry.icons.plus
                                    active: true
                                    inactiveOpacity: 1
                                }
                                Text {
                                    text: "New Mission"
                                    color: "#ffffff"
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                            }
                            MouseArea {
                                id: newMissionMouse
                                anchors.fill: parent
                                enabled: true
                                hoverEnabled: true
                                onClicked: root.openNewMission()
                            }
                        }
                        Rectangle {
                            visible: accessManager.can("can_view_reports")
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 44
                            radius: 8
                            color: settingsMouse.containsMouse ? "#f2ecfb" : "#ffffff"
                            border.color: root.line
                            AssetIcon {
                                anchors.centerIn: parent
                                width: 18
                                height: 18
                                iconSize: 18
                                source: AssetRegistry.icons.edit
                                active: true
                            }
                            MouseArea {
                                id: settingsMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: root.showPanel("Dashboard Settings", "Mission sidebar preferences, filters, and cached mission controls are ready.")
                            }
                        }
                    }
                }
            }

            Flickable {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                contentWidth: width
                contentHeight: root.onePageDashboard ? height : dashboardContent.implicitHeight + (root.dashboardOuterMargin * 2)
                interactive: !root.onePageDashboard
                boundsBehavior: Flickable.StopAtBounds

                ColumnLayout {
                    id: dashboardContent
                    width: Math.max(0, Math.min(parent.width - (root.dashboardOuterMargin * 2), root.dashboardMaxWidth))
                    x: Math.max(root.dashboardOuterMargin, Math.round((parent.width - width) / 2))
                    y: root.dashboardOuterMargin
                    spacing: root.dashboardGap

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: root.dashboardGap

                        ColumnLayout {
                            Layout.preferredWidth: root.showRightSidebar ? root.centerContentWidth : dashboardContent.width
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignTop
                            spacing: 16

                            HeroOperationsCard {
                                Layout.fillWidth: true
                                Layout.preferredHeight: root.heroCardHeight
                                backgroundSource: AssetRegistry.dashboard.mission_operations_center_bg
                                title: root.dashboardTitle()
                                subtitle: root.dashboardSubtitle()
                                metrics: [
                                    {
                                        label: "ACTIVE MISSIONS",
                                        value: String(root.activeMissionItems().length),
                                        iconSource: AssetRegistry.icons.lucide_grid_3x3,
                                        stateColor: "#ffffff"
                                    },
                                    {
                                        label: "AIRCRAFT ONLINE",
                                        value: root.onlineAircraftCount() + " / " + root.aircraftItems().length,
                                        iconSource: AssetRegistry.icons.plane,
                                        stateColor: "#ffffff"
                                    },
                                    {
                                        label: "ACTIVE FLIGHTS",
                                        value: root.hasObject("flightStatsManager") && flightStatsManager.active ? "1" : "0",
                                        iconSource: AssetRegistry.icons.lucide_route,
                                        stateColor: "#ffffff"
                                    },
                                    {
                                        label: "NOTIFICATIONS",
                                        value: String(root.notificationCount()),
                                        iconSource: AssetRegistry.icons.cloud,
                                        stateColor: "#ffffff"
                                    },
                                    {
                                        label: "WEATHER READY",
                                        value: root.weatherLabel(),
                                        iconSource: AssetRegistry.icons.boxicons_wind_filled,
                                        stateColor: root.weatherStatus() === "GOOD_TO_FLY" ? "#32e86a" : "#f4b000"
                                    },
                                    {
                                        label: "SYNC STATUS",
                                        value: root.controlCenterStatusLabel(),
                                        iconSource: AssetRegistry.icons.boxicons_wifi,
                                        stateColor: root.hasObject("sessionManager") && sessionManager.controlCenterReachable ? "#32e86a" : "#f4b000"
                                    }
                                ]
                                onOpenRequested: root.showPanel(root.dashboardTitle(), root.dashboardDetailText())
                            }

                            GridLayout {
                                visible: root.phone && !root.showLeftSidebar
                                Layout.fillWidth: true
                                columns: root.phone ? 1 : 2
                                rowSpacing: 10
                                columnSpacing: 10

                                SidebarTabButton {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 44
                                    visible: root.canModule("mission_planning")
                                    text: "Mission"
                                    iconSource: AssetRegistry.icons.lucide_route
                                    active: root.activeTab === "mission"
                                    enabled: root.canModule("mission_planning")
                                    onClicked: {
                                        root.activeTab = "mission";
                                        if (root.filteredMissions().length > 0)
                                            root.openMissionInPlanner(root.filteredMissions()[0]);
                                        else
                                            root.openNewMission();
                                    }
                                }

                                SidebarTabButton {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 44
                                    visible: root.hasManufacturerAccess() || root.canModule("vehicle_configuration")
                                    text: root.hasManufacturerAccess() ? "Manufacturer" : "Vehicle"
                                    iconSource: AssetRegistry.icons.cube
                                    active: root.activeTab === "vehicle"
                                    enabled: root.hasManufacturerAccess() || root.canModule("vehicle_configuration")
                                    onClicked: {
                                        root.activeTab = "vehicle";
                                        if (root.hasManufacturerAccess())
                                            root.openManufacturerWorkspace();
                                        else
                                            root.openVehicleConfiguration();
                                    }
                                }

                                SidebarTabButton {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 44
                                    visible: root.gcsToolsAvailable()
                                    text: "Tools"
                                    iconSource: AssetRegistry.icons.lucide_grid_3x3
                                    active: root.activeTab === "tools"
                                    enabled: root.gcsToolsAvailable()
                                    onClicked: {
                                        root.activeTab = "tools";
                                        root.openGcsTools();
                                    }
                                }

                                SidebarTabButton {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 44
                                    text: "Map"
                                    iconSource: AssetRegistry.icons.lucide_satellite
                                    active: root.activeTab === "map"
                                    onClicked: root.activeTab = "map"
                                }
                            }

                            GridLayout {
                                Layout.fillWidth: true
                                columns: root.dashboardGridColumns
                                rowSpacing: 16
                                columnSpacing: 16

                                Rectangle {
                                    visible: accessManager.can("can_view_fleet") || root.canViewTelemetryData()
                                    Layout.fillWidth: !root.dashboardHasSideCard
                                    Layout.preferredWidth: root.dashboardHasSideCard ? root.assignedAircraftTargetWidth : root.centerContentWidth
                                    Layout.minimumWidth: root.dashboardHasSideCard ? Math.min(root.assignedAircraftTargetWidth, root.centerContentWidth) : 0
                                    Layout.maximumWidth: root.dashboardHasSideCard ? root.assignedAircraftTargetWidth : 99999
                                    Layout.columnSpan: 1
                                    Layout.preferredHeight: root.assignedAircraftHeight
                                    radius: 10
                                    color: "#ffffff"
                                    border.color: root.line
                                    clip: true

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 18
                                        spacing: 12
                                        RowLayout {
                                            Layout.fillWidth: true
                                            Text {
                                                Layout.fillWidth: true
                                                text: "Assigned Aircraft"
                                                color: root.ink
                                                font.pixelSize: 16
                                                font.bold: true
                                            }
                                            Rectangle {
                                                visible: accessManager.can("can_view_fleet")
                                                Layout.preferredWidth: 82
                                                Layout.preferredHeight: 30
                                                radius: 7
                                                color: fleetMouse.containsMouse ? "#f5f0fb" : "#ffffff"
                                                border.color: "#d8ceeb"
                                                Text {
                                                    anchors.centerIn: parent
                                                    text: "View Fleet"
                                                    color: root.purpleDark
                                                    font.pixelSize: 11
                                                    font.bold: true
                                                }
                                                MouseArea {
                                                    id: fleetMouse
                                                    anchors.fill: parent
                                                    hoverEnabled: true
                                                    onClicked: root.showPanel("Fleet Center", root.fleetDetailText())
                                                }
                                            }
                                        }

                                        Text {
                                            visible: root.aircraftItems().length === 0
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            text: "No aircraft are available for this role yet."
                                            color: root.muted
                                            font.pixelSize: 13
                                            wrapMode: Text.WordWrap
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                        }

                                        ScrollView {
                                            id: aircraftScroll
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            visible: root.aircraftItems().length > 0
                                            contentWidth: availableWidth
                                            clip: true
                                            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                                            GridLayout {
                                                width: aircraftScroll.availableWidth
                                                columns: root.aircraftColumns
                                                rowSpacing: 14
                                                columnSpacing: 14
                                                Repeater {
                                                    model: Math.min(2, root.aircraftItems().length)
                                                    delegate: AircraftSummaryCard {
                                                        Layout.fillWidth: true
                                                        Layout.preferredHeight: 284
                                                        imageSource: index === 0 ? AssetRegistry.aircraft.x8_01 : AssetRegistry.aircraft.vtol_03
                                                        aircraftName: root.textValue(root.aircraftItems()[index], ["name", "model", "tail_number", "serial_number", "aircraft_id"], "Assigned aircraft")
                                                        tailNumber: root.textValue(root.aircraftItems()[index], ["tail_number", "serial_number", "aircraft_id", "id"], "--")
                                                        battery: root.numberValue(root.aircraftItems()[index], ["battery_percent", "battery"], root.telemetryConnected() ? telemetryStore.battery : 0)
                                                        linkQuality: root.textValue(root.aircraftItems()[index], ["link_quality", "rc_quality", "telemetry_status", "status"], root.telemetryConnected() ? telemetryStore.rcQuality : "Offline")
                                                        lastSync: root.textValue(root.aircraftItems()[index], ["last_sync", "last_seen", "updated_at"], root.telemetryConnected() ? "Live" : "No recent sync")
                                                        ready: root.textValue(root.aircraftItems()[index], ["status", "readiness", "health_status"], root.telemetryConnected() ? telemetryStore.aircraftReadiness : "pending").toLowerCase().indexOf("ready") >= 0
                                                        onClicked: {
                                                            var aircraft = root.aircraftItems()[index]
                                                            var aircraftId = root.textValue(aircraft, ["id", "aircraft_id", "tail_number", "serial_number"], "")
                                                            if (accessManager.canPerform("aircraft_profile_access") && accessManager.canAccessAircraft(aircraftId)) {
                                                                root.showPanel("Aircraft Details", root.aircraftDetailText(aircraft))
                                                            } else {
                                                                accessManager.recordBlocked("aircraft_profile_access", "Aircraft detail view blocked by local permissions.", { aircraft_id: aircraftId })
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                Rectangle {
                                    visible: false
                                    Layout.fillWidth: true
                                    Layout.columnSpan: 1
                                    Layout.preferredHeight: root.assignedAircraftHeight
                                    radius: 10
                                    color: "#ffffff"
                                    border.color: root.line
                                    clip: true

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 18
                                        spacing: 12

                                        RowLayout {
                                            Layout.fillWidth: true
                                            Text {
                                                Layout.fillWidth: true
                                                text: root.manufacturerName()
                                                color: root.ink
                                                font.pixelSize: 16
                                                font.bold: true
                                                elide: Text.ElideRight
                                            }
                                            Text {
                                                text: root.vehicleProfileItems().length + " profiles"
                                                color: root.muted
                                                font.pixelSize: 11
                                            }
                                        }

                                        GridLayout {
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            columns: 1
                                            rowSpacing: 10
                                            columnSpacing: 10

                                            QuickActionTile {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                                iconSource: AssetRegistry.icons.cube
                                                title: "Manufacturer Workspace"
                                                subtitle: "Profiles, setup, release"
                                                primary: true
                                                enabled: root.hasManufacturerAccess()
                                                onClicked: root.openManufacturerWorkspace()
                                            }
                                        }
                                    }
                                }

                                Rectangle {
                                    visible: root.dashboardActionCount() > 0
                                    Layout.fillWidth: true
                                    Layout.columnSpan: 1
                                    Layout.preferredHeight: root.assignedAircraftHeight
                                    radius: 10
                                    color: "#ffffff"
                                    border.color: root.line
                                    clip: true

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 18
                                        spacing: 14
                                        Text {
                                            text: "Quick Actions"
                                            color: root.ink
                                            font.pixelSize: 14
                                            font.bold: true
                                        }
                                        ScrollView {
                                            id: quickActionScroll
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            contentWidth: availableWidth
                                            clip: true
                                            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                                            GridLayout {
                                                width: quickActionScroll.availableWidth
                                                columns: root.quickActionColumns
                                                rowSpacing: 10
                                                columnSpacing: 10
                                                QuickActionTile {
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: root.quickActionTileHeight
                                                    visible: !root.showLeftSidebar && root.gcsToolsAvailable()
                                                    iconSource: AssetRegistry.icons.lucide_grid_3x3
                                                    title: "GCS Tools"
                                                    subtitle: "Setup, logs, diagnostics"
                                                    primary: true
                                                    enabled: true
                                                    onClicked: root.openGcsTools()
                                                }
                                                QuickActionTile {
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: root.quickActionTileHeight
                                                    visible: !root.showLeftSidebar && root.hasManufacturerAccess()
                                                    iconSource: AssetRegistry.icons.cube
                                                    title: "Manufacturer"
                                                    subtitle: "Profiles and release"
                                                    primary: true
                                                    enabled: root.hasManufacturerAccess()
                                                    onClicked: root.openManufacturerWorkspace()
                                                }
                                                QuickActionTile {
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: root.quickActionTileHeight
                                                    visible: !root.showLeftSidebar && !root.hasManufacturerAccess() && root.canPermission("can_configure_vehicle") && root.canModule("vehicle_configuration")
                                                    iconSource: AssetRegistry.icons.cube
                                                    title: "Vehicle Config"
                                                    subtitle: root.isManufacturer() ? "Bind, calibrate, release" : "Profiles"
                                                    primary: true
                                                    enabled: true
                                                    onClicked: root.openVehicleConfiguration()
                                                }
                                                QuickActionTile {
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: root.quickActionTileHeight
                                                    visible: !root.showLeftSidebar && !root.hasManufacturerAccess() && root.canPermission("can_plan_mission") && root.canModule("mission_planning")
                                                    iconSource: AssetRegistry.icons.plus
                                                    title: root.missionActionTitle()
                                                    subtitle: "Plan route"
                                                    enabled: true
                                                    onClicked: root.openNewMission()
                                                }
                                                QuickActionTile {
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: root.quickActionTileHeight
                                                    visible: root.hasManufacturerAccess()
                                                    iconSource: AssetRegistry.icons.lucide_satellite
                                                    title: "Pilot Mode"
                                                    subtitle: "Manual flight test"
                                                    disabledReason: root.pilotModeDenial()
                                                    enabled: root.canOpenPilotMode()
                                                    onClicked: root.startPilotMode()
                                                }
                                                QuickActionTile {
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: root.quickActionTileHeight
                                                    visible: root.hasManufacturerAccess()
                                                    iconSource: AssetRegistry.icons.plus
                                                    title: "Test Mission Plan"
                                                    subtitle: "Open mission planner"
                                                    disabledReason: root.missionPlannerDenial()
                                                    enabled: root.canOpenMissionPlanner()
                                                    onClicked: root.openNewMission()
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            GridLayout {
                                Layout.fillWidth: true
                                columns: root.summaryGridColumns
                                rowSpacing: 16
                                columnSpacing: 16
                                DashboardListCard {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: root.summaryCardHeight
                                    title: "Recent Missions"
                                    emptyText: "No recent missions"
                                    items: root.recentMissionSummaryItems()
                                    onOpenRequested: root.showPanel("Recent Missions", "Recent missions available: " + root.filteredMissions().length)
                                }
                                DashboardListCard {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: root.summaryCardHeight
                                    title: "Notifications"
                                    emptyText: "No new notifications"
                                    items: root.notificationSummaryItems()
                                    onOpenRequested: root.showPanel("Notifications", "Notifications available: " + root.notificationCount())
                                }
                                SystemStatusCard {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: root.summaryCardHeight
                                    rows: root.systemStatusRows()
                                    onDetailsRequested: root.showPanel("System Status", root.systemDetailText())
                                }
                            }
                        }
                    }

                    GridLayout {
                        visible: !root.showRightSidebar && root.showSecondaryOperations
                        Layout.fillWidth: true
                        columns: root.operationsGridColumns
                        rowSpacing: 16
                        columnSpacing: 16
                        ReadinessCard {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 250
                            readiness: root.readinessPercent()
                            statusText: root.readinessStatusText()
                            onDetailsRequested: {
                                if (!root.canShowPanel("Readiness Details")) {
                                    root.blockPanel("Readiness Details", root.panelAccessAction("Readiness Details"))
                                    return
                                }
                                if (root.hasObject("preflightChecklistManager"))
                                    preflightChecklistManager.runChecklist(true);
                                root.showPanel("Readiness Details", root.readinessDetailText());
                            }
                        }
                        RecentActivityCard {
                            visible: accessManager.can("can_view_vehicle_audit")
                            Layout.fillWidth: true
                            Layout.preferredHeight: 250
                            activities: root.activityItems()
                            onViewAllRequested: root.showPanel("Activity Feed", root.activityDetailText())
                        }
                        PilotAssignmentCard {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 250
                            pilotName: root.profileName()
                            organization: root.organizationName()
                            unitSystem: root.hasObject("preferencesManager") ? preferencesManager.units.toUpperCase() : "METRIC"
                            assignment: root.pilotAssignmentLabel()
                            clearance: root.operatorRoleLabel()
                            lastLogin: root.lastSessionLabel()
                            canPilotMode: root.canPermission("can_fly_manual") && root.canModule("pilot_operations")
                            canChangePilot: root.canPermission("can_assign_pilots") && root.canModule("fleet")
                            onPilotModeRequested: root.startPilotMode()
                            onChangePilotRequested: root.showPanel("Change Pilot", "Pilot assignment modal is ready for organization-approved operator switching.")
                        }
                        WeatherForecastCard {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 245
                            status: root.weatherStatus()
                            temperature: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.temperatureC).toFixed(0) + " C" : "--"
                            condition: root.hasObject("weatherManager") && weatherManager.available ? weatherManager.status : "Weather pending"
                            wind: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.windSpeedMps * 3.6).toFixed(0) + " km/h " + root.weatherDirectionLabel(weatherManager.windDirectionDeg) : "--"
                            visibility: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.visibilityKm).toFixed(1) + " km" : "--"
                            precipitation: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.precipitationProbability).toFixed(0) + "%" : "--"
                            gusts: root.hasObject("weatherManager") && weatherManager.available ? Number(weatherManager.windGustMps * 3.6).toFixed(0) + " km/h" : "--"
                            forecastWindow: root.weatherStatus() === "GOOD_TO_FLY" ? "Good for next 6 hours" : "Review before flight"
                            syncing: root.hasObject("weatherSyncManager") && weatherSyncManager.syncing
                            onRefreshRequested: root.refreshWeather()
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        visible: root.toast.length > 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 22
        width: Math.min(parent.width - 40, toastText.implicitWidth + 42)
        height: 42
        radius: 21
        color: "#211033ee"
        Text {
            id: toastText
            anchors.centerIn: parent
            text: root.toast
            color: "#ffffff"
            font.pixelSize: 12
            font.bold: true
            elide: Text.ElideRight
            width: parent.width - 30
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Rectangle {
        id: detailPanel
        visible: root.modalOpen
        z: 40
        width: root.compact ? Math.min(parent.width - 28, 560) : 410
        height: root.compact ? Math.min(250, parent.height - 130) : Math.min(310, parent.height - 120)
        anchors.right: parent.right
        anchors.rightMargin: root.compact ? 14 : 24
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.compact ? 18 : 24
        radius: 12
        color: "#ffffff"
        border.color: "#d8ceeb"
        border.width: 1
        opacity: root.modalOpen ? 1 : 0

        Rectangle {
            anchors.fill: parent
            anchors.margins: -1
            radius: parent.radius
            color: "transparent"
            border.color: "#ffffff"
            border.width: 1
            opacity: 0.7
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 12

            RowLayout {
                Layout.fillWidth: true

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.fillWidth: true
                        text: root.modalTitle
                        color: root.ink
                        font.pixelSize: 17
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Text {
                        Layout.fillWidth: true
                        text: "Dashboard detail panel"
                        color: root.muted
                        font.pixelSize: 10
                        elide: Text.ElideRight
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    radius: 16
                    color: closeMouse.containsMouse ? "#f2ecfb" : "#ffffff"
                    border.color: root.line
                    Text {
                        anchors.centerIn: parent
                        text: "X"
                        color: root.purple
                        font.pixelSize: 11
                        font.bold: true
                    }
                    MouseArea {
                        id: closeMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: root.modalOpen = false
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: root.line
            }

            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: root.modalBody
                color: root.muted
                font.pixelSize: 13
                wrapMode: Text.WordWrap
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    radius: 8
                    color: "#f7f3fb"
                    border.color: "#d8ceeb"
                    Text {
                        anchors.centerIn: parent
                        text: "Keep Dashboard Visible"
                        color: root.purple
                        font.pixelSize: 11
                        font.bold: true
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 84
                    Layout.preferredHeight: 36
                    radius: 8
                    color: doneMouse.containsMouse ? "#4f0aa6" : "#3b0787"
                    Text {
                        anchors.centerIn: parent
                        text: "Done"
                        color: "#ffffff"
                        font.pixelSize: 11
                        font.bold: true
                    }
                    MouseArea {
                        id: doneMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: root.modalOpen = false
                    }
                }
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 140
            }
        }
    }

    component DashboardListCard: Rectangle {
        id: card
        property string title: ""
        property string emptyText: "No items"
        property var items: []
        signal openRequested()

        radius: 10
        color: cardMouse.containsMouse ? "#fbfaff" : "#ffffff"
        border.color: cardMouse.containsMouse ? "#cfc4eb" : "#e2dceb"
        border.width: 1
        clip: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 10

            Text {
                Layout.fillWidth: true
                text: card.title
                color: root.ink
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
            }

            Repeater {
                model: card.items.slice(0, 4)
                delegate: RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 24
                    spacing: 9

                    Rectangle {
                        Layout.preferredWidth: 7
                        Layout.preferredHeight: 7
                        radius: 4
                        color: modelData.color || root.purple
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0
                        Text {
                            Layout.fillWidth: true
                            text: modelData.label || ""
                            color: root.ink
                            font.pixelSize: 11
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        Text {
                            Layout.fillWidth: true
                            visible: (modelData.detail || "").length > 0
                            text: modelData.detail || ""
                            color: root.muted
                            font.pixelSize: 9
                            elide: Text.ElideRight
                        }
                    }

                    Text {
                        text: modelData.value || ""
                        color: root.muted
                        font.pixelSize: 10
                        font.bold: true
                        elide: Text.ElideRight
                    }
                }
            }

            Text {
                visible: card.items.length === 0
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: card.emptyText
                color: root.muted
                font.pixelSize: 12
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Item { Layout.fillHeight: true }
        }

        MouseArea {
            id: cardMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: card.openRequested()
        }
    }

    Connections {
        target: root.hasObject("preflightChecklistManager") ? preflightChecklistManager : null
        function onChecklistChanged() {
            root.readinessRevision += 1;
        }
    }
}
