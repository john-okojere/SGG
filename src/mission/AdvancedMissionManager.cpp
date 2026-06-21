#include "AdvancedMissionManager.h"

#include "../auth/SessionManager.h"
#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"

#include <mavsdk/plugins/mission_raw/mission_raw.h>

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMetaObject>
#include <QPointer>
#include <QRegularExpression>
#include <QTextStream>
#include <QUrl>

#include <cmath>
#include <thread>
#include <vector>

namespace {
QString missionRawResultString(mavsdk::MissionRaw::Result result)
{
    switch (result) {
    case mavsdk::MissionRaw::Result::Success: return QStringLiteral("Success");
    case mavsdk::MissionRaw::Result::Error: return QStringLiteral("Error");
    case mavsdk::MissionRaw::Result::TooManyMissionItems: return QStringLiteral("Too many mission items");
    case mavsdk::MissionRaw::Result::Busy: return QStringLiteral("Vehicle busy");
    case mavsdk::MissionRaw::Result::Timeout: return QStringLiteral("Timeout");
    case mavsdk::MissionRaw::Result::InvalidArgument: return QStringLiteral("Invalid argument");
    case mavsdk::MissionRaw::Result::Unsupported: return QStringLiteral("Unsupported");
    case mavsdk::MissionRaw::Result::NoMissionAvailable: return QStringLiteral("No mission available");
    case mavsdk::MissionRaw::Result::TransferCancelled: return QStringLiteral("Transfer cancelled");
    case mavsdk::MissionRaw::Result::NoSystem: return QStringLiteral("No system");
    case mavsdk::MissionRaw::Result::Denied: return QStringLiteral("Denied");
    case mavsdk::MissionRaw::Result::MissionTypeNotConsistent: return QStringLiteral("Mission type inconsistent");
    case mavsdk::MissionRaw::Result::InvalidSequence: return QStringLiteral("Invalid sequence");
    case mavsdk::MissionRaw::Result::CurrentInvalid: return QStringLiteral("Current item invalid");
    case mavsdk::MissionRaw::Result::ProtocolError: return QStringLiteral("Protocol error");
    case mavsdk::MissionRaw::Result::IntMessagesNotSupported: return QStringLiteral("MISSION_INT unsupported");
    default: return QStringLiteral("Mission operation failed");
    }
}

int32_t coordinateToRawInt(double value);
double rawIntToCoordinate(const QVariant &value);

QVariantMap itemToMap(const mavsdk::MissionRaw::MissionItem &item)
{
    const double latitude = rawIntToCoordinate(item.x);
    const double longitude = rawIntToCoordinate(item.y);
    return QVariantMap{
        {QStringLiteral("seq"), int(item.seq)},
        {QStringLiteral("frame"), int(item.frame)},
        {QStringLiteral("command"), int(item.command)},
        {QStringLiteral("current"), int(item.current)},
        {QStringLiteral("autocontinue"), int(item.autocontinue)},
        {QStringLiteral("param1"), double(item.param1)},
        {QStringLiteral("param2"), double(item.param2)},
        {QStringLiteral("param3"), double(item.param3)},
        {QStringLiteral("param4"), double(item.param4)},
        {QStringLiteral("x"), int(item.x)},
        {QStringLiteral("y"), int(item.y)},
        {QStringLiteral("latitude"), latitude},
        {QStringLiteral("longitude"), longitude},
        {QStringLiteral("z"), double(item.z)},
        {QStringLiteral("altitude"), double(item.z)},
        {QStringLiteral("missionType"), int(item.mission_type)}
    };
}

mavsdk::MissionRaw::MissionItem mapToItem(const QVariantMap &map)
{
    mavsdk::MissionRaw::MissionItem item{};
    item.seq = uint32_t(map.value(QStringLiteral("seq")).toInt());
    item.frame = uint32_t(map.value(QStringLiteral("frame"), 3).toInt());
    item.command = uint32_t(map.value(QStringLiteral("command"), 16).toInt());
    item.current = uint32_t(map.value(QStringLiteral("current")).toInt());
    item.autocontinue = uint32_t(map.value(QStringLiteral("autocontinue"), 1).toInt());
    item.param1 = float(map.value(QStringLiteral("param1")).toDouble());
    item.param2 = float(map.value(QStringLiteral("param2")).toDouble());
    item.param3 = float(map.value(QStringLiteral("param3")).toDouble());
    item.param4 = float(map.value(QStringLiteral("param4")).toDouble());
    item.x = map.contains(QStringLiteral("latitude"))
        ? coordinateToRawInt(map.value(QStringLiteral("latitude")).toDouble())
        : int32_t(map.value(QStringLiteral("x")).toInt());
    item.y = map.contains(QStringLiteral("longitude"))
        ? coordinateToRawInt(map.value(QStringLiteral("longitude")).toDouble())
        : int32_t(map.value(QStringLiteral("y")).toInt());
    item.z = float(map.value(QStringLiteral("altitude"), map.value(QStringLiteral("z"))).toDouble());
    item.mission_type = uint32_t(map.value(QStringLiteral("missionType")).toInt());
    return item;
}

int32_t coordinateToRawInt(double value)
{
    if (std::abs(value) <= 180.0) {
        return int32_t(std::llround(value * 10000000.0));
    }
    return int32_t(std::llround(value));
}

double rawIntToCoordinate(const QVariant &value)
{
    const double number = value.toDouble();
    if (std::abs(number) > 1000000.0) {
        return number / 10000000.0;
    }
    return number;
}

QVariantMap normalizedRawRow(QVariantMap row)
{
    if (row.contains(QStringLiteral("latitude"))) {
        row[QStringLiteral("x")] = coordinateToRawInt(row.value(QStringLiteral("latitude")).toDouble());
    } else {
        row[QStringLiteral("latitude")] = rawIntToCoordinate(row.value(QStringLiteral("x")));
    }
    if (row.contains(QStringLiteral("longitude"))) {
        row[QStringLiteral("y")] = coordinateToRawInt(row.value(QStringLiteral("longitude")).toDouble());
    } else {
        row[QStringLiteral("longitude")] = rawIntToCoordinate(row.value(QStringLiteral("y")));
    }
    if (row.contains(QStringLiteral("altitude"))) {
        row[QStringLiteral("z")] = row.value(QStringLiteral("altitude")).toDouble();
    } else {
        row[QStringLiteral("altitude")] = row.value(QStringLiteral("z")).toDouble();
    }
    return row;
}

QStringList missionFileTokens(const QString &line)
{
    return line.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
}

QVariantMap commandRow(const QString &name,
                       int command,
                       const QString &category,
                       int frame,
                       const QString &detail,
                       const QStringList &params = {})
{
    QVariantList paramRows;
    for (const QString &param : params) {
        paramRows << param;
    }
    return QVariantMap{{QStringLiteral("name"), name},
                       {QStringLiteral("value"), QStringLiteral("%1 / %2").arg(command).arg(name)},
                       {QStringLiteral("category"), category},
                       {QStringLiteral("command"), command},
                       {QStringLiteral("frame"), frame},
                       {QStringLiteral("detail"), detail},
                       {QStringLiteral("params"), paramRows}};
}

QVariantList commonCommandCatalog()
{
    return QVariantList{
        commandRow(QStringLiteral("NAV_WAYPOINT"), 16, QStringLiteral("NAV"), 3,
                   QStringLiteral("Fly to a latitude/longitude/altitude waypoint."),
                   {QStringLiteral("Hold"), QStringLiteral("Accept radius"), QStringLiteral("Pass radius"), QStringLiteral("Yaw")}),
        commandRow(QStringLiteral("NAV_LOITER_UNLIM"), 17, QStringLiteral("NAV"), 3,
                   QStringLiteral("Loiter indefinitely until another command is issued.")),
        commandRow(QStringLiteral("NAV_LOITER_TURNS"), 18, QStringLiteral("NAV"), 3,
                   QStringLiteral("Loiter for a number of turns."), {QStringLiteral("Turns"), QStringLiteral("Heading required"), QStringLiteral("Radius"), QStringLiteral("Xtrack")}),
        commandRow(QStringLiteral("NAV_LOITER_TIME"), 19, QStringLiteral("NAV"), 3,
                   QStringLiteral("Loiter for a time period."), {QStringLiteral("Seconds"), QStringLiteral("Heading required"), QStringLiteral("Radius"), QStringLiteral("Xtrack")}),
        commandRow(QStringLiteral("NAV_RETURN_TO_LAUNCH"), 20, QStringLiteral("NAV"), 2,
                   QStringLiteral("Return to launch using autopilot RTL behavior.")),
        commandRow(QStringLiteral("NAV_LAND"), 21, QStringLiteral("NAV"), 3,
                   QStringLiteral("Land at the current or specified position."), {QStringLiteral("Abort alt"), QStringLiteral("Precision land"), QStringLiteral("Empty"), QStringLiteral("Yaw")}),
        commandRow(QStringLiteral("NAV_TAKEOFF"), 22, QStringLiteral("NAV"), 3,
                   QStringLiteral("Take off to the commanded altitude where supported."), {QStringLiteral("Pitch"), QStringLiteral("Empty"), QStringLiteral("Empty"), QStringLiteral("Yaw")}),
        commandRow(QStringLiteral("NAV_SPLINE_WAYPOINT"), 82, QStringLiteral("NAV"), 3,
                   QStringLiteral("Spline waypoint for smoother path following.")),
        commandRow(QStringLiteral("NAV_VTOL_TAKEOFF"), 84, QStringLiteral("NAV"), 3,
                   QStringLiteral("VTOL takeoff command where supported.")),
        commandRow(QStringLiteral("NAV_VTOL_LAND"), 85, QStringLiteral("NAV"), 3,
                   QStringLiteral("VTOL landing command where supported.")),
        commandRow(QStringLiteral("CONDITION_DELAY"), 112, QStringLiteral("CONDITION"), 2,
                   QStringLiteral("Delay subsequent mission commands."), {QStringLiteral("Seconds")}),
        commandRow(QStringLiteral("CONDITION_CHANGE_ALT"), 113, QStringLiteral("CONDITION"), 3,
                   QStringLiteral("Wait until the vehicle reaches an altitude."), {QStringLiteral("Rate")}),
        commandRow(QStringLiteral("CONDITION_DISTANCE"), 114, QStringLiteral("CONDITION"), 2,
                   QStringLiteral("Delay until the vehicle is within a distance."), {QStringLiteral("Distance")}),
        commandRow(QStringLiteral("CONDITION_YAW"), 115, QStringLiteral("CONDITION"), 2,
                   QStringLiteral("Point the vehicle heading where supported."), {QStringLiteral("Angle"), QStringLiteral("Rate"), QStringLiteral("Direction"), QStringLiteral("Relative")}),
        commandRow(QStringLiteral("DO_SET_MODE"), 176, QStringLiteral("DO"), 2,
                   QStringLiteral("Set autopilot mode where supported.")),
        commandRow(QStringLiteral("DO_JUMP"), 177, QStringLiteral("DO"), 2,
                   QStringLiteral("Jump to another mission item."), {QStringLiteral("Seq"), QStringLiteral("Repeat")}),
        commandRow(QStringLiteral("DO_CHANGE_SPEED"), 178, QStringLiteral("DO"), 2,
                   QStringLiteral("Change target speed."), {QStringLiteral("Speed type"), QStringLiteral("Speed"), QStringLiteral("Throttle")}),
        commandRow(QStringLiteral("DO_SET_HOME"), 179, QStringLiteral("DO"), 3,
                   QStringLiteral("Set the vehicle home position."), {QStringLiteral("Use current")}),
        commandRow(QStringLiteral("DO_SET_RELAY"), 181, QStringLiteral("DO"), 2,
                   QStringLiteral("Set a relay state where the autopilot supports it."), {QStringLiteral("Relay"), QStringLiteral("State")}),
        commandRow(QStringLiteral("DO_REPEAT_RELAY"), 182, QStringLiteral("DO"), 2,
                   QStringLiteral("Toggle a relay repeatedly."), {QStringLiteral("Relay"), QStringLiteral("Count"), QStringLiteral("Period")}),
        commandRow(QStringLiteral("DO_SET_SERVO"), 183, QStringLiteral("DO"), 2,
                   QStringLiteral("Set a servo channel/PWM value where allowed."), {QStringLiteral("Servo"), QStringLiteral("PWM")}),
        commandRow(QStringLiteral("DO_REPEAT_SERVO"), 184, QStringLiteral("DO"), 2,
                   QStringLiteral("Repeat a servo movement."), {QStringLiteral("Servo"), QStringLiteral("PWM"), QStringLiteral("Count"), QStringLiteral("Period")}),
        commandRow(QStringLiteral("DO_LAND_START"), 189, QStringLiteral("DO"), 2,
                   QStringLiteral("Mark a landing sequence start.")),
        commandRow(QStringLiteral("DO_SET_ROI_LOCATION"), 195, QStringLiteral("DO"), 3,
                   QStringLiteral("Point camera/gimbal at a region of interest location.")),
        commandRow(QStringLiteral("DO_SET_ROI_NONE"), 197, QStringLiteral("DO"), 2,
                   QStringLiteral("Clear active ROI behavior.")),
        commandRow(QStringLiteral("DO_DIGICAM_CONFIGURE"), 202, QStringLiteral("CAMERA"), 2,
                   QStringLiteral("Configure compatible camera payloads.")),
        commandRow(QStringLiteral("DO_DIGICAM_CONTROL"), 203, QStringLiteral("CAMERA"), 2,
                   QStringLiteral("Trigger compatible camera payloads.")),
        commandRow(QStringLiteral("DO_MOUNT_CONFIGURE"), 204, QStringLiteral("GIMBAL"), 2,
                   QStringLiteral("Configure mount/gimbal mode.")),
        commandRow(QStringLiteral("DO_MOUNT_CONTROL"), 205, QStringLiteral("GIMBAL"), 2,
                   QStringLiteral("Set mount/gimbal pitch, roll, yaw."), {QStringLiteral("Pitch"), QStringLiteral("Roll"), QStringLiteral("Yaw")}),
        commandRow(QStringLiteral("DO_SET_CAM_TRIGG_DIST"), 206, QStringLiteral("CAMERA"), 2,
                   QStringLiteral("Trigger camera by distance."), {QStringLiteral("Distance"), QStringLiteral("Shutter")}),
        commandRow(QStringLiteral("DO_FENCE_ENABLE"), 207, QStringLiteral("FENCE"), 2,
                   QStringLiteral("Enable or disable geofence enforcement."), {QStringLiteral("Enable")}),
        commandRow(QStringLiteral("DO_PARACHUTE"), 208, QStringLiteral("SAFETY"), 2,
                   QStringLiteral("Parachute action where supported."), {QStringLiteral("Action")}),
        commandRow(QStringLiteral("DO_MOTOR_TEST"), 209, QStringLiteral("SAFETY"), 2,
                   QStringLiteral("Motor test command where supported.")),
        commandRow(QStringLiteral("DO_GRIPPER"), 211, QStringLiteral("PAYLOAD"), 2,
                   QStringLiteral("Operate gripper/payload release where supported."), {QStringLiteral("Instance"), QStringLiteral("Action")}),
        commandRow(QStringLiteral("DO_SET_CAM_TRIGG_INTERVAL"), 214, QStringLiteral("CAMERA"), 2,
                   QStringLiteral("Trigger camera by time interval."), {QStringLiteral("Interval"), QStringLiteral("Cycles")}),
        commandRow(QStringLiteral("DO_SET_RESUME_REPEAT_DIST"), 215, QStringLiteral("DO"), 2,
                   QStringLiteral("Set resume repeat distance.")),
        commandRow(QStringLiteral("DO_GIMBAL_MANAGER_PITCHYAW"), 1000, QStringLiteral("GIMBAL"), 2,
                   QStringLiteral("Gimbal manager pitch/yaw command."), {QStringLiteral("Pitch"), QStringLiteral("Yaw"), QStringLiteral("Pitch rate"), QStringLiteral("Yaw rate")}),
        commandRow(QStringLiteral("IMAGE_START_CAPTURE"), 2000, QStringLiteral("CAMERA"), 2,
                   QStringLiteral("Start image capture."), {QStringLiteral("Interval"), QStringLiteral("Count"), QStringLiteral("Seq")}),
        commandRow(QStringLiteral("IMAGE_STOP_CAPTURE"), 2001, QStringLiteral("CAMERA"), 2,
                   QStringLiteral("Stop image capture.")),
        commandRow(QStringLiteral("VIDEO_START_CAPTURE"), 2500, QStringLiteral("CAMERA"), 2,
                   QStringLiteral("Start video capture.")),
        commandRow(QStringLiteral("VIDEO_STOP_CAPTURE"), 2501, QStringLiteral("CAMERA"), 2,
                   QStringLiteral("Stop video capture.")),
        commandRow(QStringLiteral("FENCE_RETURN_POINT"), 5000, QStringLiteral("FENCE"), 0,
                   QStringLiteral("Geofence return point.")),
        commandRow(QStringLiteral("FENCE_POLYGON_VERTEX_INCLUSION"), 5001, QStringLiteral("FENCE"), 0,
                   QStringLiteral("Inclusion polygon geofence vertex.")),
        commandRow(QStringLiteral("FENCE_POLYGON_VERTEX_EXCLUSION"), 5002, QStringLiteral("FENCE"), 0,
                   QStringLiteral("Exclusion polygon geofence vertex.")),
        commandRow(QStringLiteral("FENCE_CIRCLE_INCLUSION"), 5003, QStringLiteral("FENCE"), 0,
                   QStringLiteral("Inclusion circle geofence."), {QStringLiteral("Radius")}),
        commandRow(QStringLiteral("FENCE_CIRCLE_EXCLUSION"), 5004, QStringLiteral("FENCE"), 0,
                   QStringLiteral("Exclusion circle geofence."), {QStringLiteral("Radius")}),
        commandRow(QStringLiteral("RALLY_POINT"), 5100, QStringLiteral("RALLY"), 0,
                   QStringLiteral("Rally point for emergency routing."))
    };
}

QVariantList commonFrameCatalog()
{
    return QVariantList{
        QVariantMap{{QStringLiteral("name"), QStringLiteral("GLOBAL")}, {QStringLiteral("frame"), 0}, {QStringLiteral("detail"), QStringLiteral("Absolute altitude global frame.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("LOCAL_NED")}, {QStringLiteral("frame"), 1}, {QStringLiteral("detail"), QStringLiteral("Local north/east/down frame.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("MISSION")}, {QStringLiteral("frame"), 2}, {QStringLiteral("detail"), QStringLiteral("Mission command frame with no position.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("GLOBAL_RELATIVE_ALT")}, {QStringLiteral("frame"), 3}, {QStringLiteral("detail"), QStringLiteral("Relative altitude above home.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("GLOBAL_INT")}, {QStringLiteral("frame"), 5}, {QStringLiteral("detail"), QStringLiteral("Integer lat/lon absolute altitude.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("GLOBAL_RELATIVE_ALT_INT")}, {QStringLiteral("frame"), 6}, {QStringLiteral("detail"), QStringLiteral("Integer lat/lon relative altitude.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("BODY_NED")}, {QStringLiteral("frame"), 8}, {QStringLiteral("detail"), QStringLiteral("Body-relative NED frame.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("GLOBAL_TERRAIN_ALT")}, {QStringLiteral("frame"), 10}, {QStringLiteral("detail"), QStringLiteral("Altitude above terrain.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("GLOBAL_TERRAIN_ALT_INT")}, {QStringLiteral("frame"), 11}, {QStringLiteral("detail"), QStringLiteral("Integer lat/lon terrain altitude.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("BODY_FRD")}, {QStringLiteral("frame"), 12}, {QStringLiteral("detail"), QStringLiteral("Body forward/right/down frame.")}}
    };
}
}

AdvancedMissionManager::AdvancedMissionManager(MavsdkVehicleManager *vehicle,
                                               AccessManager *access,
                                               SessionManager *session,
                                               GcsEventSyncManager *events,
                                               QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_access(access),
      m_session(session),
      m_events(events)
{
}

QVariantList AdvancedMissionManager::missionItems() const { return m_missionItems; }
QVariantList AdvancedMissionManager::geofenceItems() const { return m_geofenceItems; }
QVariantList AdvancedMissionManager::rallyItems() const { return m_rallyItems; }
QVariantList AdvancedMissionManager::commandCatalog() const { return commonCommandCatalog(); }
QVariantList AdvancedMissionManager::frameCatalog() const { return commonFrameCatalog(); }
bool AdvancedMissionManager::useForUpload() const { return m_useForUpload; }
bool AdvancedMissionManager::busy() const { return m_busy; }
QString AdvancedMissionManager::status() const { return m_status; }
QString AdvancedMissionManager::compareStatus() const { return m_compareStatus; }

void AdvancedMissionManager::setUseForUpload(bool useForUpload)
{
    if (m_useForUpload == useForUpload) {
        return;
    }
    m_useForUpload = useForUpload;
    setStatus(useForUpload
                  ? QStringLiteral("Advanced raw MAVLink table will be used for the next aircraft upload.")
                  : QStringLiteral("SkyGrid high-level mission plan will be used for the next aircraft upload."));
    emit missionChanged();
}

bool AdvancedMissionManager::hasUploadableMissionItems() const
{
    return !m_missionItems.isEmpty();
}

bool AdvancedMissionManager::containsTakeoffCommand() const
{
    for (const QVariant &entry : m_missionItems) {
        const int command = entry.toMap().value(QStringLiteral("command")).toInt();
        if (command == 22 || command == 84) {
            return true;
        }
    }
    return false;
}

void AdvancedMissionManager::readMissionFromAircraft()
{
    if (m_busy) {
        return;
    }
    if (!m_access || !m_access->authorizeAction(QStringLiteral("advanced_mission_editor"),
                                                 {},
                                                 QStringLiteral("Advanced mission read blocked by local permissions."))) {
        setStatus(QStringLiteral("Advanced mission read blocked by RBAC."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a flight controller before reading raw mission items."));
        return;
    }
    setBusy(true);
    setStatus(QStringLiteral("Reading raw mission items from aircraft."));
    const auto system = m_vehicle->system();
    QPointer<AdvancedMissionManager> self(this);
    std::thread([self, system]() {
        mavsdk::MissionRaw raw(system);
        const auto [result, items] = raw.download_mission();
        QVariantList rows;
        for (const mavsdk::MissionRaw::MissionItem &item : items) {
            rows << itemToMap(item);
        }
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, rows]() {
            if (!self) {
                return;
            }
            self->m_missionItems = rows;
            self->setStatus(result == mavsdk::MissionRaw::Result::Success
                                ? QStringLiteral("Read %1 raw mission items from aircraft.").arg(rows.size())
                                : QStringLiteral("Mission read unavailable: %1").arg(missionRawResultString(result)));
            self->setBusy(false);
            emit self->missionChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void AdvancedMissionManager::readGeofenceFromAircraft()
{
    if (m_busy) {
        return;
    }
    if (m_access && !m_access->authorizeAction(QStringLiteral("advanced_mission_editor"),
                                                {},
                                                QStringLiteral("Geofence read blocked by local permissions."))) {
        setStatus(QStringLiteral("Geofence read blocked by RBAC."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a flight controller before reading geofence items."));
        return;
    }
    setBusy(true);
    const auto system = m_vehicle->system();
    QPointer<AdvancedMissionManager> self(this);
    std::thread([self, system]() {
        mavsdk::MissionRaw raw(system);
        const auto [result, items] = raw.download_geofence();
        QVariantList rows;
        for (const mavsdk::MissionRaw::MissionItem &item : items) {
            rows << itemToMap(item);
        }
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, rows]() {
            self->m_geofenceItems = rows;
            self->setStatus(result == mavsdk::MissionRaw::Result::Success
                                ? QStringLiteral("Read %1 geofence items from aircraft.").arg(rows.size())
                                : QStringLiteral("Geofence read unavailable: %1").arg(missionRawResultString(result)));
            self->setBusy(false);
            emit self->missionChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void AdvancedMissionManager::readRallyPointsFromAircraft()
{
    if (m_busy) {
        return;
    }
    if (m_access && !m_access->authorizeAction(QStringLiteral("advanced_mission_editor"),
                                                {},
                                                QStringLiteral("Rally point read blocked by local permissions."))) {
        setStatus(QStringLiteral("Rally point read blocked by RBAC."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a flight controller before reading rally points."));
        return;
    }
    setBusy(true);
    const auto system = m_vehicle->system();
    QPointer<AdvancedMissionManager> self(this);
    std::thread([self, system]() {
        mavsdk::MissionRaw raw(system);
        const auto [result, items] = raw.download_rallypoints();
        QVariantList rows;
        for (const mavsdk::MissionRaw::MissionItem &item : items) {
            rows << itemToMap(item);
        }
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, rows]() {
            self->m_rallyItems = rows;
            self->setStatus(result == mavsdk::MissionRaw::Result::Success
                                ? QStringLiteral("Read %1 rally points from aircraft.").arg(rows.size())
                                : QStringLiteral("Rally point read unavailable: %1").arg(missionRawResultString(result)));
            self->setBusy(false);
            emit self->missionChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void AdvancedMissionManager::writeMissionToAircraft()
{
    if (m_busy) {
        return;
    }
    if (!guardWritableMissionOperation(QStringLiteral("Raw mission upload"), QStringLiteral("mission_upload_raw_blocked"))) {
        return;
    }
    std::vector<mavsdk::MissionRaw::MissionItem> items;
    for (const QVariant &entry : m_missionItems) {
        items.push_back(mapToItem(entry.toMap()));
    }
    setBusy(true);
    const auto system = m_vehicle->system();
    QPointer<AdvancedMissionManager> self(this);
    std::thread([self, system, items]() {
        mavsdk::MissionRaw raw(system);
        const mavsdk::MissionRaw::Result result = raw.upload_mission(items);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, count = int(items.size())]() {
            self->setStatus(result == mavsdk::MissionRaw::Result::Success
                                ? QStringLiteral("Uploaded %1 raw mission items to aircraft.").arg(count)
                                : QStringLiteral("Raw mission upload failed: %1").arg(missionRawResultString(result)));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("mission_upload_raw"),
                                            result == mavsdk::MissionRaw::Result::Success ? QStringLiteral("info") : QStringLiteral("warning"),
                                            QStringLiteral("Raw MAVLink mission upload attempted"),
                                            QJsonObject{{QStringLiteral("count"), count},
                                                        {QStringLiteral("result"), missionRawResultString(result)}});
            }
            self->setBusy(false);
            emit self->missionChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void AdvancedMissionManager::writeGeofenceToAircraft()
{
    if (m_busy) {
        return;
    }
    if (!guardWritableMissionOperation(QStringLiteral("Geofence upload"), QStringLiteral("mission_upload_geofence_blocked"))) {
        return;
    }
    std::vector<mavsdk::MissionRaw::MissionItem> items;
    for (const QVariant &entry : m_geofenceItems) {
        items.push_back(mapToItem(entry.toMap()));
    }
    setBusy(true);
    setStatus(QStringLiteral("Uploading %1 geofence items to aircraft.").arg(int(items.size())));
    const auto system = m_vehicle->system();
    QPointer<AdvancedMissionManager> self(this);
    std::thread([self, system, items]() {
        mavsdk::MissionRaw raw(system);
        const mavsdk::MissionRaw::Result result = raw.upload_geofence(items);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, count = int(items.size())]() {
            self->setStatus(result == mavsdk::MissionRaw::Result::Success
                                ? QStringLiteral("Uploaded %1 geofence items to aircraft.").arg(count)
                                : QStringLiteral("Geofence upload failed: %1").arg(missionRawResultString(result)));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("mission_upload_geofence"),
                                            result == mavsdk::MissionRaw::Result::Success ? QStringLiteral("info") : QStringLiteral("warning"),
                                            QStringLiteral("MAVLink geofence upload attempted"),
                                            QJsonObject{{QStringLiteral("count"), count},
                                                        {QStringLiteral("result"), missionRawResultString(result)}});
            }
            self->setBusy(false);
            emit self->missionChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void AdvancedMissionManager::writeRallyPointsToAircraft()
{
    if (m_busy) {
        return;
    }
    if (!guardWritableMissionOperation(QStringLiteral("Rally point upload"), QStringLiteral("mission_upload_rally_blocked"))) {
        return;
    }
    std::vector<mavsdk::MissionRaw::MissionItem> items;
    for (const QVariant &entry : m_rallyItems) {
        items.push_back(mapToItem(entry.toMap()));
    }
    setBusy(true);
    setStatus(QStringLiteral("Uploading %1 rally points to aircraft.").arg(int(items.size())));
    const auto system = m_vehicle->system();
    QPointer<AdvancedMissionManager> self(this);
    std::thread([self, system, items]() {
        mavsdk::MissionRaw raw(system);
        const mavsdk::MissionRaw::Result result = raw.upload_rally_points(items);
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, count = int(items.size())]() {
            self->setStatus(result == mavsdk::MissionRaw::Result::Success
                                ? QStringLiteral("Uploaded %1 rally points to aircraft.").arg(count)
                                : QStringLiteral("Rally point upload failed: %1").arg(missionRawResultString(result)));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("mission_upload_rally_points"),
                                            result == mavsdk::MissionRaw::Result::Success ? QStringLiteral("info") : QStringLiteral("warning"),
                                            QStringLiteral("MAVLink rally point upload attempted"),
                                            QJsonObject{{QStringLiteral("count"), count},
                                                        {QStringLiteral("result"), missionRawResultString(result)}});
            }
            self->setBusy(false);
            emit self->missionChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void AdvancedMissionManager::compareLocalToAircraft()
{
    if (!m_access || !m_access->authorizeAction(QStringLiteral("advanced_mission_editor"),
                                                 {},
                                                 QStringLiteral("Advanced mission compare blocked by local permissions."))) {
        setCompareStatus(QStringLiteral("Advanced mission compare blocked by RBAC."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setCompareStatus(QStringLiteral("Connect a flight controller before comparing missions."));
        return;
    }
    setBusy(true);
    setCompareStatus(QStringLiteral("Downloading aircraft mission for comparison."));
    const QVariantList localRows = m_missionItems;
    const auto system = m_vehicle->system();
    QPointer<AdvancedMissionManager> self(this);
    std::thread([self, system, localRows]() {
        mavsdk::MissionRaw raw(system);
        const auto [result, items] = raw.download_mission();
        QVariantList aircraftRows;
        for (const mavsdk::MissionRaw::MissionItem &item : items) {
            aircraftRows << itemToMap(item);
        }
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, localRows, aircraftRows]() {
            if (!self) {
                return;
            }
            if (result != mavsdk::MissionRaw::Result::Success) {
                self->setCompareStatus(QStringLiteral("Mission compare failed: %1").arg(missionRawResultString(result)));
                self->setBusy(false);
                return;
            }
            const int maxRows = qMax(localRows.size(), aircraftRows.size());
            int differences = 0;
            for (int i = 0; i < maxRows; ++i) {
                const QJsonDocument localDoc = i < localRows.size()
                    ? QJsonDocument(QJsonObject::fromVariantMap(localRows.at(i).toMap()))
                    : QJsonDocument();
                const QJsonDocument aircraftDoc = i < aircraftRows.size()
                    ? QJsonDocument(QJsonObject::fromVariantMap(aircraftRows.at(i).toMap()))
                    : QJsonDocument();
                if (localDoc.toJson(QJsonDocument::Compact) != aircraftDoc.toJson(QJsonDocument::Compact)) {
                    ++differences;
                }
            }
            self->setCompareStatus(QStringLiteral("%1 difference(s): local %2 item(s), aircraft %3 item(s).")
                                       .arg(differences)
                                       .arg(localRows.size())
                                       .arg(aircraftRows.size()));
            self->setBusy(false);
        }, Qt::QueuedConnection);
    }).detach();
}

void AdvancedMissionManager::saveMissionFile(const QString &pathOrUrl)
{
    const QString path = normalizePath(pathOrUrl);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        setStatus(QStringLiteral("Could not save raw mission file."));
        return;
    }
    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == QStringLiteral("plan")) {
        QJsonArray items;
        int doJumpId = 1;
        for (const QVariant &entry : m_missionItems) {
            const QVariantMap row = normalizedRawRow(entry.toMap());
            const double latitude = row.value(QStringLiteral("latitude")).toDouble();
            const double longitude = row.value(QStringLiteral("longitude")).toDouble();
            const double altitude = row.value(QStringLiteral("altitude"), row.value(QStringLiteral("z"))).toDouble();
            items << QJsonObject{
                {QStringLiteral("AMSLAltAboveTerrain"), QJsonValue()},
                {QStringLiteral("Altitude"), altitude},
                {QStringLiteral("AltitudeMode"), 1},
                {QStringLiteral("autoContinue"), row.value(QStringLiteral("autocontinue"), 1).toInt() != 0},
                {QStringLiteral("command"), row.value(QStringLiteral("command"), 16).toInt()},
                {QStringLiteral("doJumpId"), doJumpId++},
                {QStringLiteral("frame"), row.value(QStringLiteral("frame"), 3).toInt()},
                {QStringLiteral("params"), QJsonArray{
                     row.value(QStringLiteral("param1")).toDouble(),
                     row.value(QStringLiteral("param2")).toDouble(),
                     row.value(QStringLiteral("param3")).toDouble(),
                     row.value(QStringLiteral("param4")).toDouble(),
                     latitude,
                     longitude,
                     altitude}},
                {QStringLiteral("type"), QStringLiteral("SimpleItem")}
            };
        }
        QJsonArray rallyPoints;
        for (const QVariant &entry : m_rallyItems) {
            const QVariantMap row = normalizedRawRow(entry.toMap());
            rallyPoints << QJsonArray{
                row.value(QStringLiteral("latitude")).toDouble(),
                row.value(QStringLiteral("longitude")).toDouble(),
                row.value(QStringLiteral("altitude"), row.value(QStringLiteral("z"))).toDouble()
            };
        }
        QJsonArray polygons;
        if (!m_geofenceItems.isEmpty()) {
            QJsonArray polygonPoints;
            for (const QVariant &entry : m_geofenceItems) {
                const QVariantMap row = normalizedRawRow(entry.toMap());
                polygonPoints << QJsonArray{
                    row.value(QStringLiteral("latitude")).toDouble(),
                    row.value(QStringLiteral("longitude")).toDouble()
                };
            }
            polygons << QJsonObject{{QStringLiteral("inclusion"), true},
                                    {QStringLiteral("polygon"), polygonPoints},
                                    {QStringLiteral("version"), 1}};
        }
        const QJsonObject root{
            {QStringLiteral("fileType"), QStringLiteral("Plan")},
            {QStringLiteral("geoFence"), QJsonObject{
                 {QStringLiteral("circles"), QJsonArray{}},
                 {QStringLiteral("polygons"), polygons},
                 {QStringLiteral("version"), 2}}},
            {QStringLiteral("groundStation"), QStringLiteral("SkyGrid GCS")},
            {QStringLiteral("mission"), QJsonObject{
                 {QStringLiteral("cruiseSpeed"), 15},
                 {QStringLiteral("firmwareType"), 12},
                 {QStringLiteral("hoverSpeed"), 5},
                 {QStringLiteral("items"), items},
                 {QStringLiteral("plannedHomePosition"), QJsonArray{0, 0, 0}},
                 {QStringLiteral("vehicleType"), 2},
                 {QStringLiteral("version"), 2}}},
            {QStringLiteral("rallyPoints"), QJsonObject{
                 {QStringLiteral("points"), rallyPoints},
                 {QStringLiteral("version"), 2}}},
            {QStringLiteral("version"), 1}
        };
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        setStatus(QStringLiteral("Saved QGroundControl .plan with %1 mission items.").arg(m_missionItems.size()));
        if (m_events) {
            m_events->recordEvent(QStringLiteral("mission_file_saved"),
                                  QStringLiteral("info"),
                                  QStringLiteral("Advanced mission file saved"),
                                  QJsonObject{{QStringLiteral("format"), QStringLiteral("qgc_plan")},
                                              {QStringLiteral("mission_items"), m_missionItems.size()},
                                              {QStringLiteral("geofence_items"), m_geofenceItems.size()},
                                              {QStringLiteral("rally_items"), m_rallyItems.size()}});
        }
        return;
    }
    if (suffix == QStringLiteral("waypoints") || suffix == QStringLiteral("txt")) {
        QTextStream out(&file);
        out << "QGC WPL 110\n";
        for (const QVariant &entry : m_missionItems) {
            const QVariantMap row = entry.toMap();
            out << row.value(QStringLiteral("seq")).toInt() << '\t'
                << row.value(QStringLiteral("current")).toInt() << '\t'
                << row.value(QStringLiteral("frame"), 3).toInt() << '\t'
                << row.value(QStringLiteral("command"), 16).toInt() << '\t'
                << row.value(QStringLiteral("param1")).toDouble() << '\t'
                << row.value(QStringLiteral("param2")).toDouble() << '\t'
                << row.value(QStringLiteral("param3")).toDouble() << '\t'
                << row.value(QStringLiteral("param4")).toDouble() << '\t'
                << rawIntToCoordinate(row.value(QStringLiteral("x"))) << '\t'
                << rawIntToCoordinate(row.value(QStringLiteral("y"))) << '\t'
                << row.value(QStringLiteral("z")).toDouble() << '\t'
                << row.value(QStringLiteral("autocontinue"), 1).toInt() << '\n';
        }
        setStatus(QStringLiteral("Saved QGC WPL mission file with %1 items.").arg(m_missionItems.size()));
        if (m_events) {
            m_events->recordEvent(QStringLiteral("mission_file_saved"),
                                  QStringLiteral("info"),
                                  QStringLiteral("Advanced mission file saved"),
                                  QJsonObject{{QStringLiteral("format"), QStringLiteral("qgc_wpl_110")},
                                              {QStringLiteral("mission_items"), m_missionItems.size()}});
        }
        return;
    }
    QJsonObject root{{QStringLiteral("mission_items"), QJsonArray::fromVariantList(m_missionItems)},
                     {QStringLiteral("geofence_items"), QJsonArray::fromVariantList(m_geofenceItems)},
                     {QStringLiteral("rally_items"), QJsonArray::fromVariantList(m_rallyItems)}};
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    setStatus(QStringLiteral("Saved raw mission JSON file."));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("mission_file_saved"),
                              QStringLiteral("info"),
                              QStringLiteral("Advanced mission file saved"),
                              QJsonObject{{QStringLiteral("format"), QStringLiteral("skygrid_raw_json")},
                                          {QStringLiteral("mission_items"), m_missionItems.size()},
                                          {QStringLiteral("geofence_items"), m_geofenceItems.size()},
                                          {QStringLiteral("rally_items"), m_rallyItems.size()}});
    }
}

void AdvancedMissionManager::loadMissionFile(const QString &pathOrUrl)
{
    const QString path = normalizePath(pathOrUrl);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        setStatus(QStringLiteral("Could not load raw mission file."));
        return;
    }
    const QByteArray bytes = file.readAll();
    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == QStringLiteral("plan") || bytes.contains("\"fileType\"") && bytes.contains("\"Plan\"")) {
        QJsonParseError parseError{};
        const QJsonObject root = QJsonDocument::fromJson(bytes, &parseError).object();
        if (parseError.error != QJsonParseError::NoError || root.value(QStringLiteral("fileType")).toString() != QStringLiteral("Plan")) {
            setStatus(QStringLiteral("Could not parse .plan file: %1").arg(parseError.errorString()));
            return;
        }
        QVariantList missionRows;
        const QJsonArray items = root.value(QStringLiteral("mission")).toObject().value(QStringLiteral("items")).toArray();
        int ignoredComplexItems = 0;
        for (const QJsonValue &itemValue : items) {
            const QJsonObject item = itemValue.toObject();
            if (item.value(QStringLiteral("type")).toString(QStringLiteral("SimpleItem")) != QStringLiteral("SimpleItem")) {
                ++ignoredComplexItems;
                continue;
            }
            const QJsonArray params = item.value(QStringLiteral("params")).toArray();
            const double latitude = params.size() > 4 ? params.at(4).toDouble() : 0.0;
            const double longitude = params.size() > 5 ? params.at(5).toDouble() : 0.0;
            const double altitude = params.size() > 6 ? params.at(6).toDouble() : item.value(QStringLiteral("Altitude")).toDouble();
            missionRows << normalizedRawRow(QVariantMap{
                {QStringLiteral("seq"), missionRows.size()},
                {QStringLiteral("current"), missionRows.isEmpty() ? 1 : 0},
                {QStringLiteral("frame"), item.value(QStringLiteral("frame")).toInt(3)},
                {QStringLiteral("command"), item.value(QStringLiteral("command")).toInt(16)},
                {QStringLiteral("autocontinue"), item.value(QStringLiteral("autoContinue")).toBool(true) ? 1 : 0},
                {QStringLiteral("param1"), params.size() > 0 ? params.at(0).toDouble() : 0.0},
                {QStringLiteral("param2"), params.size() > 1 ? params.at(1).toDouble() : 0.0},
                {QStringLiteral("param3"), params.size() > 2 ? params.at(2).toDouble() : 0.0},
                {QStringLiteral("param4"), params.size() > 3 ? params.at(3).toDouble() : 0.0},
                {QStringLiteral("latitude"), latitude},
                {QStringLiteral("longitude"), longitude},
                {QStringLiteral("altitude"), altitude},
                {QStringLiteral("missionType"), 0}
            });
        }
        QVariantList rallyRows;
        const QJsonArray rallyPoints = root.value(QStringLiteral("rallyPoints")).toObject().value(QStringLiteral("points")).toArray();
        for (const QJsonValue &pointValue : rallyPoints) {
            const QJsonArray point = pointValue.toArray();
            if (point.size() < 2) {
                continue;
            }
            rallyRows << normalizedRawRow(QVariantMap{
                {QStringLiteral("seq"), rallyRows.size()},
                {QStringLiteral("frame"), 0},
                {QStringLiteral("command"), 5100},
                {QStringLiteral("current"), 0},
                {QStringLiteral("autocontinue"), 1},
                {QStringLiteral("latitude"), point.at(0).toDouble()},
                {QStringLiteral("longitude"), point.at(1).toDouble()},
                {QStringLiteral("altitude"), point.size() > 2 ? point.at(2).toDouble() : 0.0},
                {QStringLiteral("missionType"), 2}
            });
        }
        QVariantList fenceRows;
        const QJsonArray polygons = root.value(QStringLiteral("geoFence")).toObject().value(QStringLiteral("polygons")).toArray();
        for (const QJsonValue &polygonValue : polygons) {
            const QJsonObject polygon = polygonValue.toObject();
            const int command = polygon.value(QStringLiteral("inclusion")).toBool(true) ? 5001 : 5002;
            const QJsonArray points = polygon.value(QStringLiteral("polygon")).toArray();
            for (const QJsonValue &pointValue : points) {
                const QJsonArray point = pointValue.toArray();
                if (point.size() < 2) {
                    continue;
                }
                fenceRows << normalizedRawRow(QVariantMap{
                    {QStringLiteral("seq"), fenceRows.size()},
                    {QStringLiteral("frame"), 0},
                    {QStringLiteral("command"), command},
                    {QStringLiteral("current"), 0},
                    {QStringLiteral("autocontinue"), 1},
                    {QStringLiteral("latitude"), point.at(0).toDouble()},
                    {QStringLiteral("longitude"), point.at(1).toDouble()},
                    {QStringLiteral("altitude"), 0.0},
                    {QStringLiteral("missionType"), 1}
                });
            }
        }
        m_missionItems = missionRows;
        m_geofenceItems = fenceRows;
        m_rallyItems = rallyRows;
        reindexMissionItems();
        setStatus(ignoredComplexItems > 0
                      ? QStringLiteral("Loaded %1 simple .plan mission items; ignored %2 complex item(s).")
                            .arg(m_missionItems.size())
                            .arg(ignoredComplexItems)
                      : QStringLiteral("Loaded %1 .plan mission items.").arg(m_missionItems.size()));
        if (m_events) {
            m_events->recordEvent(QStringLiteral("mission_file_loaded"),
                                  QStringLiteral("info"),
                                  QStringLiteral("Advanced mission file loaded"),
                                  QJsonObject{{QStringLiteral("format"), QStringLiteral("qgc_plan")},
                                              {QStringLiteral("mission_items"), m_missionItems.size()},
                                              {QStringLiteral("geofence_items"), m_geofenceItems.size()},
                                              {QStringLiteral("rally_items"), m_rallyItems.size()},
                                              {QStringLiteral("ignored_complex_items"), ignoredComplexItems}});
        }
        emit missionChanged();
        return;
    }
    if (suffix == QStringLiteral("waypoints") || suffix == QStringLiteral("txt") || bytes.startsWith("QGC WPL")) {
        QVariantList rows;
        const QString text = QString::fromUtf8(bytes);
        const QStringList lines = text.split(QRegularExpression(QStringLiteral("[\\r\\n]+")), Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            if (line.trimmed().startsWith(QStringLiteral("QGC WPL"))) {
                continue;
            }
            const QStringList tokens = missionFileTokens(line.trimmed());
            if (tokens.size() < 12) {
                continue;
            }
            QVariantMap row = normalizedRawRow(QVariantMap{
                {QStringLiteral("seq"), tokens.at(0).toInt()},
                {QStringLiteral("current"), tokens.at(1).toInt()},
                {QStringLiteral("frame"), tokens.at(2).toInt()},
                {QStringLiteral("command"), tokens.at(3).toInt()},
                {QStringLiteral("param1"), tokens.at(4).toDouble()},
                {QStringLiteral("param2"), tokens.at(5).toDouble()},
                {QStringLiteral("param3"), tokens.at(6).toDouble()},
                {QStringLiteral("param4"), tokens.at(7).toDouble()},
                {QStringLiteral("x"), coordinateToRawInt(tokens.at(8).toDouble())},
                {QStringLiteral("y"), coordinateToRawInt(tokens.at(9).toDouble())},
                {QStringLiteral("z"), tokens.at(10).toDouble()},
                {QStringLiteral("autocontinue"), tokens.at(11).toInt()},
                {QStringLiteral("missionType"), 0}
            });
            rows << row;
        }
        m_missionItems = rows;
        reindexMissionItems();
        setStatus(QStringLiteral("Loaded %1 QGC WPL mission items.").arg(m_missionItems.size()));
        emit missionChanged();
        if (m_events) {
            m_events->recordEvent(QStringLiteral("mission_file_loaded"),
                                  QStringLiteral("info"),
                                  QStringLiteral("Advanced mission file loaded"),
                                  QJsonObject{{QStringLiteral("format"), QStringLiteral("qgc_wpl_110")},
                                              {QStringLiteral("mission_items"), m_missionItems.size()}});
        }
        return;
    }
    QJsonParseError parseError{};
    const QJsonObject root = QJsonDocument::fromJson(bytes, &parseError).object();
    if (parseError.error != QJsonParseError::NoError) {
        setStatus(QStringLiteral("Could not parse mission file: %1").arg(parseError.errorString()));
        return;
    }
    m_missionItems.clear();
    for (const QVariant &entry : root.value(QStringLiteral("mission_items")).toArray().toVariantList()) {
        m_missionItems << normalizedRawRow(entry.toMap());
    }
    m_geofenceItems.clear();
    for (const QVariant &entry : root.value(QStringLiteral("geofence_items")).toArray().toVariantList()) {
        m_geofenceItems << normalizedRawRow(entry.toMap());
    }
    m_rallyItems.clear();
    for (const QVariant &entry : root.value(QStringLiteral("rally_items")).toArray().toVariantList()) {
        m_rallyItems << normalizedRawRow(entry.toMap());
    }
    reindexMissionItems();
    setStatus(QStringLiteral("Loaded raw mission JSON file."));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("mission_file_loaded"),
                              QStringLiteral("info"),
                              QStringLiteral("Advanced mission file loaded"),
                              QJsonObject{{QStringLiteral("format"), QStringLiteral("skygrid_raw_json")},
                                          {QStringLiteral("mission_items"), m_missionItems.size()},
                                          {QStringLiteral("geofence_items"), m_geofenceItems.size()},
                                          {QStringLiteral("rally_items"), m_rallyItems.size()}});
    }
    emit missionChanged();
}

void AdvancedMissionManager::addMissionItem(const QVariantMap &item)
{
    QVariantMap row = normalizedRawRow(item);
    row[QStringLiteral("seq")] = m_missionItems.size();
    row[QStringLiteral("current")] = m_missionItems.isEmpty() ? 1 : row.value(QStringLiteral("current")).toInt();
    if (!row.contains(QStringLiteral("command"))) {
        row[QStringLiteral("command")] = 16;
    }
    if (!row.contains(QStringLiteral("frame"))) {
        row[QStringLiteral("frame")] = 3;
    }
    if (!row.contains(QStringLiteral("autocontinue"))) {
        row[QStringLiteral("autocontinue")] = 1;
    }
    if (!row.contains(QStringLiteral("missionType"))) {
        row[QStringLiteral("missionType")] = 0;
    }
    m_missionItems << row;
    reindexMissionItems();
    setStatus(QStringLiteral("Added raw mission item."));
    emit missionChanged();
}

void AdvancedMissionManager::addMissionCommand(int command,
                                               int frame,
                                               double param1,
                                               double param2,
                                               double param3,
                                               double param4,
                                               double latitude,
                                               double longitude,
                                               double altitude,
                                               int missionType)
{
    addMissionItem(QVariantMap{{QStringLiteral("frame"), frame},
                               {QStringLiteral("command"), command},
                               {QStringLiteral("current"), m_missionItems.isEmpty() ? 1 : 0},
                               {QStringLiteral("autocontinue"), 1},
                               {QStringLiteral("param1"), param1},
                               {QStringLiteral("param2"), param2},
                               {QStringLiteral("param3"), param3},
                               {QStringLiteral("param4"), param4},
                               {QStringLiteral("x"), coordinateToRawInt(latitude)},
                               {QStringLiteral("y"), coordinateToRawInt(longitude)},
                               {QStringLiteral("z"), altitude},
                               {QStringLiteral("missionType"), missionType}});
}

void AdvancedMissionManager::buildFromSkyGridRoute(const QVariantList &route,
                                                   const QVariantMap &takeoffPoint,
                                                   const QString &finishAction)
{
    m_missionItems.clear();
    if (!takeoffPoint.isEmpty()
        && takeoffPoint.contains(QStringLiteral("latitude"))
        && takeoffPoint.contains(QStringLiteral("longitude"))) {
        addMissionCommand(22,
                          3,
                          0,
                          0,
                          0,
                          0,
                          takeoffPoint.value(QStringLiteral("latitude")).toDouble(),
                          takeoffPoint.value(QStringLiteral("longitude")).toDouble(),
                          takeoffPoint.value(QStringLiteral("altitude"), 20.0).toDouble(),
                          0);
    }
    for (const QVariant &entry : route) {
        const QVariantMap point = entry.toMap();
        if (!point.contains(QStringLiteral("latitude")) || !point.contains(QStringLiteral("longitude"))) {
            continue;
        }
        addMissionCommand(16,
                          3,
                          point.value(QStringLiteral("hover_seconds"), point.value(QStringLiteral("loiter_seconds"))).toDouble(),
                          0,
                          0,
                          point.value(QStringLiteral("heading")).toDouble(),
                          point.value(QStringLiteral("latitude")).toDouble(),
                          point.value(QStringLiteral("longitude")).toDouble(),
                          point.value(QStringLiteral("altitude"), 30.0).toDouble(),
                          0);
        const QString action = point.value(QStringLiteral("action")).toString();
        const QString cameraMode = point.value(QStringLiteral("camera_mode")).toString();
        if (action == QStringLiteral("Q-loiter") || point.value(QStringLiteral("loiter_seconds")).toDouble() > 0.0) {
            addMissionCommand(19,
                              3,
                              point.value(QStringLiteral("loiter_seconds"), 0.0).toDouble(),
                              0,
                              0,
                              point.value(QStringLiteral("heading")).toDouble(),
                              point.value(QStringLiteral("latitude")).toDouble(),
                              point.value(QStringLiteral("longitude")).toDouble(),
                              point.value(QStringLiteral("altitude"), 30.0).toDouble(),
                              0);
        } else if (action == QStringLiteral("Hover") || point.value(QStringLiteral("hover_seconds")).toDouble() > 0.0) {
            addMissionCommand(112,
                              2,
                              point.value(QStringLiteral("hover_seconds"), 0.0).toDouble(),
                              0,
                              0,
                              0,
                              0,
                              0,
                              0,
                              0);
        }
        if (action == QStringLiteral("Take Photo") || cameraMode == QStringLiteral("Take Photo")) {
            addMissionCommand(203, 2, 0, 0, 0, 0, 0, 0, 0, 0);
        } else if (action == QStringLiteral("Start Recording") || cameraMode == QStringLiteral("Start Recording")) {
            addMissionCommand(2500, 2, 0, 0, 0, 0, 0, 0, 0, 0);
        } else if (action == QStringLiteral("Stop Recording") || cameraMode == QStringLiteral("Stop Recording")) {
            addMissionCommand(2501, 2, 0, 0, 0, 0, 0, 0, 0, 0);
        }
        if (action == QStringLiteral("Return-to-Home")) {
            addMissionCommand(20, 2, 0, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    const QString finish = finishAction.trimmed().toLower();
    if (finish.contains(QStringLiteral("return"))) {
        addMissionCommand(20, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    } else if (finish.contains(QStringLiteral("land"))) {
        addMissionCommand(21, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    } else if (finish.contains(QStringLiteral("hover"))) {
        addMissionCommand(17, 3, 0, 0, 0, 0, 0, 0, route.isEmpty() ? 0 : route.last().toMap().value(QStringLiteral("altitude"), 30.0).toDouble(), 0);
    }
    reindexMissionItems();
    setUseForUpload(true);
    setStatus(QStringLiteral("Built %1 raw MAVLink mission items from SkyGrid mission plan.").arg(m_missionItems.size()));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("mission_raw_built_from_skygrid"),
                              QStringLiteral("info"),
                              QStringLiteral("Raw MAVLink mission table built from SkyGrid mission"),
                              QJsonObject{{QStringLiteral("mission_items"), m_missionItems.size()},
                                          {QStringLiteral("finish_action"), finishAction}});
    }
    emit missionChanged();
}

void AdvancedMissionManager::updateMissionItem(int index, const QVariantMap &changes)
{
    if (index < 0 || index >= m_missionItems.size()) {
        setStatus(QStringLiteral("Mission item update ignored: invalid row."));
        return;
    }
    QVariantMap row = m_missionItems.at(index).toMap();
    for (auto it = changes.cbegin(); it != changes.cend(); ++it) {
        row[it.key()] = it.value();
    }
    m_missionItems[index] = normalizedRawRow(row);
    reindexMissionItems();
    setStatus(QStringLiteral("Updated mission item %1.").arg(index));
    emit missionChanged();
}

void AdvancedMissionManager::duplicateMissionItem(int index)
{
    if (index < 0 || index >= m_missionItems.size()) {
        setStatus(QStringLiteral("Mission item duplicate ignored: invalid row."));
        return;
    }
    QVariantMap row = m_missionItems.at(index).toMap();
    row[QStringLiteral("current")] = 0;
    m_missionItems.insert(index + 1, normalizedRawRow(row));
    reindexMissionItems();
    setStatus(QStringLiteral("Duplicated mission item %1.").arg(index));
    emit missionChanged();
}

void AdvancedMissionManager::moveMissionItem(int fromIndex, int toIndex)
{
    if (fromIndex < 0 || fromIndex >= m_missionItems.size() || toIndex < 0 || toIndex >= m_missionItems.size()) {
        setStatus(QStringLiteral("Mission item move ignored: invalid row."));
        return;
    }
    if (fromIndex == toIndex) {
        return;
    }
    m_missionItems.move(fromIndex, toIndex);
    reindexMissionItems();
    setStatus(QStringLiteral("Moved mission item %1 to %2.").arg(fromIndex).arg(toIndex));
    emit missionChanged();
}

void AdvancedMissionManager::removeMissionItem(int index)
{
    if (index < 0 || index >= m_missionItems.size()) {
        setStatus(QStringLiteral("Mission item remove ignored: invalid row."));
        return;
    }
    m_missionItems.removeAt(index);
    reindexMissionItems();
    setStatus(QStringLiteral("Removed mission item %1.").arg(index));
    emit missionChanged();
}

void AdvancedMissionManager::clearMissionItems()
{
    m_missionItems.clear();
    setStatus(QStringLiteral("Raw mission table cleared."));
    emit missionChanged();
}

void AdvancedMissionManager::addGeofencePoint(double latitude, double longitude, double altitude, int command)
{
    const int safeCommand = command == 5002 || command == 5003 || command == 5004 || command == 5000 ? command : 5001;
    m_geofenceItems << normalizedRawRow(QVariantMap{
        {QStringLiteral("seq"), m_geofenceItems.size()},
        {QStringLiteral("frame"), 0},
        {QStringLiteral("command"), safeCommand},
        {QStringLiteral("current"), 0},
        {QStringLiteral("autocontinue"), 1},
        {QStringLiteral("param1"), 0.0},
        {QStringLiteral("param2"), 0.0},
        {QStringLiteral("param3"), 0.0},
        {QStringLiteral("param4"), 0.0},
        {QStringLiteral("latitude"), latitude},
        {QStringLiteral("longitude"), longitude},
        {QStringLiteral("altitude"), altitude},
        {QStringLiteral("missionType"), 1}
    });
    setStatus(QStringLiteral("Added geofence item."));
    emit missionChanged();
}

void AdvancedMissionManager::updateGeofenceItem(int index, const QVariantMap &changes)
{
    if (index < 0 || index >= m_geofenceItems.size()) {
        setStatus(QStringLiteral("Geofence update ignored: invalid row."));
        return;
    }
    QVariantMap row = m_geofenceItems.at(index).toMap();
    for (auto it = changes.cbegin(); it != changes.cend(); ++it) {
        row[it.key()] = it.value();
    }
    row[QStringLiteral("missionType")] = 1;
    m_geofenceItems[index] = normalizedRawRow(row);
    setStatus(QStringLiteral("Updated geofence item %1.").arg(index));
    emit missionChanged();
}

void AdvancedMissionManager::removeGeofenceItem(int index)
{
    if (index < 0 || index >= m_geofenceItems.size()) {
        setStatus(QStringLiteral("Geofence remove ignored: invalid row."));
        return;
    }
    m_geofenceItems.removeAt(index);
    for (int i = 0; i < m_geofenceItems.size(); ++i) {
        QVariantMap row = m_geofenceItems.at(i).toMap();
        row[QStringLiteral("seq")] = i;
        m_geofenceItems[i] = row;
    }
    setStatus(QStringLiteral("Removed geofence item %1.").arg(index));
    emit missionChanged();
}

void AdvancedMissionManager::clearGeofenceItems()
{
    m_geofenceItems.clear();
    setStatus(QStringLiteral("Geofence table cleared."));
    emit missionChanged();
}

void AdvancedMissionManager::addRallyPoint(double latitude, double longitude, double altitude)
{
    m_rallyItems << normalizedRawRow(QVariantMap{
        {QStringLiteral("seq"), m_rallyItems.size()},
        {QStringLiteral("frame"), 0},
        {QStringLiteral("command"), 5100},
        {QStringLiteral("current"), 0},
        {QStringLiteral("autocontinue"), 1},
        {QStringLiteral("param1"), 0.0},
        {QStringLiteral("param2"), 0.0},
        {QStringLiteral("param3"), 0.0},
        {QStringLiteral("param4"), 0.0},
        {QStringLiteral("latitude"), latitude},
        {QStringLiteral("longitude"), longitude},
        {QStringLiteral("altitude"), altitude},
        {QStringLiteral("missionType"), 2}
    });
    setStatus(QStringLiteral("Added rally point."));
    emit missionChanged();
}

void AdvancedMissionManager::updateRallyItem(int index, const QVariantMap &changes)
{
    if (index < 0 || index >= m_rallyItems.size()) {
        setStatus(QStringLiteral("Rally update ignored: invalid row."));
        return;
    }
    QVariantMap row = m_rallyItems.at(index).toMap();
    for (auto it = changes.cbegin(); it != changes.cend(); ++it) {
        row[it.key()] = it.value();
    }
    row[QStringLiteral("missionType")] = 2;
    m_rallyItems[index] = normalizedRawRow(row);
    setStatus(QStringLiteral("Updated rally point %1.").arg(index));
    emit missionChanged();
}

void AdvancedMissionManager::removeRallyItem(int index)
{
    if (index < 0 || index >= m_rallyItems.size()) {
        setStatus(QStringLiteral("Rally remove ignored: invalid row."));
        return;
    }
    m_rallyItems.removeAt(index);
    for (int i = 0; i < m_rallyItems.size(); ++i) {
        QVariantMap row = m_rallyItems.at(i).toMap();
        row[QStringLiteral("seq")] = i;
        m_rallyItems[i] = row;
    }
    setStatus(QStringLiteral("Removed rally point %1.").arg(index));
    emit missionChanged();
}

void AdvancedMissionManager::clearRallyItems()
{
    m_rallyItems.clear();
    setStatus(QStringLiteral("Rally table cleared."));
    emit missionChanged();
}

QString AdvancedMissionManager::normalizePath(const QString &pathOrUrl) const
{
    const QUrl url(pathOrUrl);
    if (url.isValid() && url.isLocalFile()) {
        return url.toLocalFile();
    }
    QString path = pathOrUrl;
    if (path.startsWith(QStringLiteral("file:///"))) {
        path = QUrl(path).toLocalFile();
    }
    return path.trimmed();
}

bool AdvancedMissionManager::guardWritableMissionOperation(const QString &label, const QString &eventType)
{
    if (!m_access || !m_access->authorizeAction(QStringLiteral("mission_upload"),
                                                 {},
                                                 QStringLiteral("Mission write blocked by local permissions."))) {
        setStatus(QStringLiteral("%1 blocked by RBAC.").arg(label));
        return false;
    }
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("%1 blocked: trusted session required.").arg(label));
        return false;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a flight controller before %1.").arg(label.toLower()));
        return false;
    }
    if (m_vehicle->armed() || m_vehicle->inAir()) {
        setStatus(QStringLiteral("%1 blocked while vehicle is armed or in air.").arg(label));
        if (m_events) {
            m_events->recordEvent(eventType,
                                  QStringLiteral("warning"),
                                  QStringLiteral("MAVLink mission write blocked by safety interlock"),
                                  QJsonObject{{QStringLiteral("operation"), label},
                                              {QStringLiteral("armed"), m_vehicle->armed()},
                                              {QStringLiteral("in_air"), m_vehicle->inAir()}});
        }
        return false;
    }
    return true;
}

void AdvancedMissionManager::reindexMissionItems()
{
    for (int i = 0; i < m_missionItems.size(); ++i) {
        QVariantMap row = m_missionItems.at(i).toMap();
        row[QStringLiteral("seq")] = i;
        row[QStringLiteral("current")] = (i == 0) ? 1 : 0;
        if (!row.contains(QStringLiteral("autocontinue"))) {
            row[QStringLiteral("autocontinue")] = 1;
        }
        if (!row.contains(QStringLiteral("missionType"))) {
            row[QStringLiteral("missionType")] = 0;
        }
        m_missionItems[i] = normalizedRawRow(row);
    }
}

void AdvancedMissionManager::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit missionChanged();
}

void AdvancedMissionManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit missionChanged();
}

void AdvancedMissionManager::setCompareStatus(const QString &status)
{
    if (m_compareStatus == status) {
        return;
    }
    m_compareStatus = status;
    emit missionChanged();
}
