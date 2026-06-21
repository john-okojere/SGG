#include "GcsToolCatalogManager.h"

#include "../security/AccessManager.h"

#include <QSet>

GcsToolCatalogManager::GcsToolCatalogManager(AccessManager *access, QObject *parent)
    : QObject(parent), m_access(access)
{
    if (m_access) {
        connect(m_access, &AccessManager::accessChanged, this, &GcsToolCatalogManager::toolsChanged);
    }
}

QVariantList GcsToolCatalogManager::tools() const
{
    QVariantList rows;
    if (!catalogAvailable()) {
        return rows;
    }
    for (const ToolDefinition &tool : definitions()) {
        if (m_access && m_access->canPerform(tool.action)) {
            rows << toMap(tool);
        }
    }
    return rows;
}

QVariantList GcsToolCatalogManager::sections() const
{
    QVariantList rows;
    if (!catalogAvailable()) {
        return rows;
    }
    QSet<QString> seen;
    for (const ToolDefinition &tool : definitions()) {
        if (seen.contains(tool.section) || !m_access || !m_access->canPerform(tool.action)) {
            continue;
        }
        seen.insert(tool.section);
        rows << QVariantMap{
            {QStringLiteral("name"), tool.section},
            {QStringLiteral("available"), true}
        };
    }
    return rows;
}

int GcsToolCatalogManager::availableCount() const
{
    if (!catalogAvailable()) {
        return 0;
    }
    int count = 0;
    for (const ToolDefinition &tool : definitions()) {
        if (m_access && m_access->canPerform(tool.action)) {
            ++count;
        }
    }
    return count;
}

QVariantMap GcsToolCatalogManager::toolForKey(const QString &key) const
{
    const QString normalized = key.trimmed();
    for (const ToolDefinition &tool : definitions()) {
        if (tool.key == normalized) {
            return toMap(tool);
        }
    }
    return {};
}

QVariantList GcsToolCatalogManager::toolsForSection(const QString &section) const
{
    const QString normalized = section.trimmed();
    QVariantList rows;
    if (!catalogAvailable()) {
        return rows;
    }
    for (const ToolDefinition &tool : definitions()) {
        if (tool.section == normalized && m_access && m_access->canPerform(tool.action)) {
            rows << toMap(tool);
        }
    }
    return rows;
}

QVariantList GcsToolCatalogManager::hiddenTools() const
{
    QVariantList rows;
    if (!catalogAvailable()) {
        return rows;
    }
    for (const ToolDefinition &tool : definitions()) {
        if (!m_access || m_access->canPerform(tool.action)) {
            continue;
        }
        QVariantMap row = toMap(tool);
        row[QStringLiteral("reason")] = m_access->denialReasonForAction(tool.action);
        rows << row;
    }
    return rows;
}

bool GcsToolCatalogManager::canOpenTool(const QString &key) const
{
    const QString action = actionForTool(key);
    return catalogAvailable() && !action.isEmpty() && m_access && m_access->canPerform(action);
}

QString GcsToolCatalogManager::actionForTool(const QString &key) const
{
    const QString normalized = key.trimmed();
    for (const ToolDefinition &tool : definitions()) {
        if (tool.key == normalized) {
            return tool.action;
        }
    }
    return {};
}

QVariantMap GcsToolCatalogManager::toMap(const ToolDefinition &tool) const
{
    const bool available = m_access && m_access->canPerform(tool.action);
    const bool accessLoaded = m_access && m_access->accessLoaded();
    return QVariantMap{
        {QStringLiteral("key"), tool.key},
        {QStringLiteral("section"), tool.section},
        {QStringLiteral("title"), tool.title},
        {QStringLiteral("description"), tool.description},
        {QStringLiteral("iconText"), tool.iconText},
        {QStringLiteral("action"), tool.action},
        {QStringLiteral("route"), tool.route},
        {QStringLiteral("available"), available},
        {QStringLiteral("restricted"), accessLoaded && !available},
        {QStringLiteral("statusText"), available ? QStringLiteral("Available")
                                                  : (accessLoaded ? QStringLiteral("Restricted")
                                                                  : QStringLiteral("Login required"))}
    };
}

bool GcsToolCatalogManager::catalogAvailable() const
{
    return m_access && m_access->canPerform(QStringLiteral("gcs_tools"));
}

QList<GcsToolCatalogManager::ToolDefinition> GcsToolCatalogManager::definitions()
{
    return {
        {QStringLiteral("dashboard"), QStringLiteral("Workflow"), QStringLiteral("Dashboard"),
         QStringLiteral("Mission readiness, session state, and GCS health."), QStringLiteral("DB"), QStringLiteral("gcs_tools"), QStringLiteral("dashboard")},
        {QStringLiteral("flightData"), QStringLiteral("Workflow"), QStringLiteral("Flight Data"),
         QStringLiteral("Live telemetry, HUD, GPS, power, RC, and vehicle messages."), QStringLiteral("FD"), QStringLiteral("flight_data"), QStringLiteral("flight-data")},
        {QStringLiteral("missionPlanner"), QStringLiteral("Workflow"), QStringLiteral("Mission Planner"),
         QStringLiteral("Advanced mission command, geofence, and rally tools."), QStringLiteral("MP"), QStringLiteral("advanced_mission_editor"), QStringLiteral("mission-planner")},
        {QStringLiteral("multiVehicle"), QStringLiteral("Workflow"), QStringLiteral("Multi-Vehicle"),
         QStringLiteral("Connection profiles and active vehicle readiness."), QStringLiteral("MV"), QStringLiteral("multi_vehicle"), QStringLiteral("multi-vehicle")},

        {QStringLiteral("installFirmware"), QStringLiteral("Firmware"), QStringLiteral("Install Firmware"),
         QStringLiteral("PX4, ArduPilot, SkyGrid, and custom package validation and flashing adapter."), QStringLiteral("FW"), QStringLiteral("firmware_manager"), QStringLiteral("install-firmware")},
        {QStringLiteral("installFirmwareLegacy"), QStringLiteral("Firmware"), QStringLiteral("Install Firmware Legacy"),
         QStringLiteral("Legacy package selection and unsupported flashing status for older boards."), QStringLiteral("FL"), QStringLiteral("firmware_flash"), QStringLiteral("install-firmware-legacy")},

        {QStringLiteral("connect"), QStringLiteral("Setup"), QStringLiteral("Connect"),
         QStringLiteral("COM/USB serial, baud, UDP, TCP, and heartbeat status."), QStringLiteral("CN"), QStringLiteral("connect"), QStringLiteral("connect")},
        {QStringLiteral("initialSetup"), QStringLiteral("Setup"), QStringLiteral("Initial Setup"),
         QStringLiteral("Calibration cards, sensor status, setup sequence, and readiness."), QStringLiteral("IS"), QStringLiteral("initial_setup"), QStringLiteral("initial-setup")},
        {QStringLiteral("parametersTuning"), QStringLiteral("Setup"), QStringLiteral("Parameters / Tuning"),
         QStringLiteral("Metadata-backed dropdowns, switches, steppers, compare, and safe writes."), QStringLiteral("PT"), QStringLiteral("vehicle_tuning"), QStringLiteral("parameters-tuning")},
        {QStringLiteral("flightModes"), QStringLiteral("Setup"), QStringLiteral("Flight Modes"),
         QStringLiteral("Selectable flight-mode parameters for PX4 and ArduPilot."), QStringLiteral("FM"), QStringLiteral("vehicle_tuning"), QStringLiteral("flight-modes")},
        {QStringLiteral("rcRadio"), QStringLiteral("Setup"), QStringLiteral("RC / Radio"),
         QStringLiteral("RC mapping, calibration values, and radio input status."), QStringLiteral("RC"), QStringLiteral("initial_setup"), QStringLiteral("rc-radio")},
        {QStringLiteral("escSetup"), QStringLiteral("Setup"), QStringLiteral("ESC"),
         QStringLiteral("ESC/motor parameter setup and unsupported calibration state."), QStringLiteral("ES"), QStringLiteral("initial_setup"), QStringLiteral("esc")},
        {QStringLiteral("servoSetup"), QStringLiteral("Setup"), QStringLiteral("Servo"),
         QStringLiteral("Servo output function and PWM setup."), QStringLiteral("SV"), QStringLiteral("initial_setup"), QStringLiteral("servo")},
        {QStringLiteral("batterySetup"), QStringLiteral("Setup"), QStringLiteral("Battery"),
         QStringLiteral("Battery monitor type, capacity, cells, and failsafe thresholds."), QStringLiteral("BT"), QStringLiteral("initial_setup"), QStringLiteral("battery")},
        {QStringLiteral("failsafeSetup"), QStringLiteral("Setup"), QStringLiteral("Failsafe"),
         QStringLiteral("RC, GCS, battery, and link failsafe parameters."), QStringLiteral("FS"), QStringLiteral("initial_setup"), QStringLiteral("failsafe")},
        {QStringLiteral("airframeSetup"), QStringLiteral("Setup"), QStringLiteral("Airframe"),
         QStringLiteral("Frame class/type, PX4 autostart, MAV type, and setup status."), QStringLiteral("AF"), QStringLiteral("initial_setup"), QStringLiteral("airframe")},

        {QStringLiteral("rtkGpsInject"), QStringLiteral("Optional Hardware"), QStringLiteral("RTK/GPS Inject"),
         QStringLiteral("NTRIP/manual RTCM source and MAVLink GPS_RTCM_DATA injection state."), QStringLiteral("RT"), QStringLiteral("optional_hardware"), QStringLiteral("rtk-gps-inject")},
        {QStringLiteral("sikRadio"), QStringLiteral("Optional Hardware"), QStringLiteral("SiK Radio"),
         QStringLiteral("Serial detection and AT-command configuration readiness."), QStringLiteral("SK"), QStringLiteral("optional_hardware"), QStringLiteral("sik-radio")},
        {QStringLiteral("droneCan"), QStringLiteral("Optional Hardware"), QStringLiteral("DroneCAN/UAVCAN"),
         QStringLiteral("CAN node list, health, and diagnostics adapter state."), QStringLiteral("DC"), QStringLiteral("optional_hardware"), QStringLiteral("dronecan")},
        {QStringLiteral("joystick"), QStringLiteral("Optional Hardware"), QStringLiteral("Joystick"),
         QStringLiteral("USB controller calibration, mapping, deadzone, and manual-control binding."), QStringLiteral("JS"), QStringLiteral("optional_hardware"), QStringLiteral("joystick")},
        {QStringLiteral("px4flow"), QStringLiteral("Optional Hardware"), QStringLiteral("PX4Flow"),
         QStringLiteral("Optical-flow status and parameter-backed setup."), QStringLiteral("OF"), QStringLiteral("optional_hardware"), QStringLiteral("px4flow")},
        {QStringLiteral("bluetoothSetup"), QStringLiteral("Optional Hardware"), QStringLiteral("Bluetooth Setup"),
         QStringLiteral("Bluetooth scan/connect capability state."), QStringLiteral("BT"), QStringLiteral("optional_hardware"), QStringLiteral("bluetooth-setup")},
        {QStringLiteral("antennaTracker"), QStringLiteral("Optional Hardware"), QStringLiteral("Antenna Tracker"),
         QStringLiteral("Tracker connection profile and target/pointing status."), QStringLiteral("AT"), QStringLiteral("optional_hardware"), QStringLiteral("antenna-tracker")},

        {QStringLiteral("payloadCamera"), QStringLiteral("Payload"), QStringLiteral("Camera"),
         QStringLiteral("MAVLink camera trigger and payload profile status."), QStringLiteral("CM"), QStringLiteral("payload_configuration"), QStringLiteral("payload-camera")},
        {QStringLiteral("payloadGimbal"), QStringLiteral("Payload"), QStringLiteral("Gimbal"),
         QStringLiteral("Gimbal pitch/yaw/mode commands where supported."), QStringLiteral("GB"), QStringLiteral("payload_configuration"), QStringLiteral("payload-gimbal")},
        {QStringLiteral("payloadVideo"), QStringLiteral("Payload"), QStringLiteral("RTSP/H264 Video"),
         QStringLiteral("In-app RTSP/H264 preview with credentials, latency mode, and status."), QStringLiteral("VD"), QStringLiteral("video_stream"), QStringLiteral("payload-video")},
        {QStringLiteral("geotagging"), QStringLiteral("Payload"), QStringLiteral("Geotagging"),
         QStringLiteral("Photo log and GPS-time correlation workflow placeholder."), QStringLiteral("GT"), QStringLiteral("video_payload_configuration"), QStringLiteral("geotagging")},
        {QStringLiteral("mappingOverlap"), QStringLiteral("Payload"), QStringLiteral("Mapping Overlap"),
         QStringLiteral("Camera FOV, altitude, sidelap/frontlap overlap calculator."), QStringLiteral("MO"), QStringLiteral("video_payload_configuration"), QStringLiteral("mapping-overlap")},

        {QStringLiteral("mavlinkInspector"), QStringLiteral("Advanced"), QStringLiteral("MAVLink Inspector"),
         QStringLiteral("Message inspection and stream diagnostics."), QStringLiteral("MI"), QStringLiteral("advanced_mavlink"), QStringLiteral("mavlink-inspector")},
        {QStringLiteral("messageSender"), QStringLiteral("Advanced"), QStringLiteral("Message Sender"),
         QStringLiteral("Permission-gated MAVLink message sender shell."), QStringLiteral("MS"), QStringLiteral("advanced_mavlink"), QStringLiteral("message-sender")},
        {QStringLiteral("terminal"), QStringLiteral("Advanced"), QStringLiteral("Terminal"),
         QStringLiteral("SERIAL_CONTROL shell where autopilot supports it."), QStringLiteral("TM"), QStringLiteral("terminal"), QStringLiteral("terminal")},
        {QStringLiteral("mavftp"), QStringLiteral("Advanced"), QStringLiteral("MAVFTP"),
         QStringLiteral("Onboard filesystem browser adapter state."), QStringLiteral("MF"), QStringLiteral("advanced_mavlink"), QStringLiteral("mavftp")},
        {QStringLiteral("signing"), QStringLiteral("Advanced"), QStringLiteral("Signing"),
         QStringLiteral("MAVLink signing status and configuration state."), QStringLiteral("SG"), QStringLiteral("advanced_mavlink"), QStringLiteral("signing")},
        {QStringLiteral("serialPassthrough"), QStringLiteral("Advanced"), QStringLiteral("Serial Passthrough"),
         QStringLiteral("Serial passthrough capability state."), QStringLiteral("SP"), QStringLiteral("advanced_mavlink"), QStringLiteral("serial-passthrough")},
        {QStringLiteral("boardInfo"), QStringLiteral("Advanced"), QStringLiteral("Board Info"),
         QStringLiteral("Detected board, stack, system id, connection, and capabilities."), QStringLiteral("BI"), QStringLiteral("advanced_mavlink"), QStringLiteral("board-info")},

        {QStringLiteral("logsAnalysis"), QStringLiteral("Analysis"), QStringLiteral("Logs / Analysis"),
         QStringLiteral("Onboard logs, local replay, warnings, graph, KML/GPX placeholders."), QStringLiteral("LG"), QStringLiteral("logs_analysis"), QStringLiteral("logs-analysis")},
        {QStringLiteral("simulation"), QStringLiteral("Analysis"), QStringLiteral("Simulation"),
         QStringLiteral("PX4, ArduPilot, Gazebo, UDP/TCP SITL endpoint management."), QStringLiteral("SIM"), QStringLiteral("simulation"), QStringLiteral("simulation")},

        {QStringLiteral("commandCenterSync"), QStringLiteral("System"), QStringLiteral("Command Center Sync"),
         QStringLiteral("Session, RBAC, workspace, device trust, and audit synchronization."), QStringLiteral("CC"), QStringLiteral("command_center_sync"), QStringLiteral("command-center-sync")}
    };
}
