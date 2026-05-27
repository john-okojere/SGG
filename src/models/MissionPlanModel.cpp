#include "MissionPlanModel.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QUrl>
#include <QXmlStreamReader>
#include <QUuid>

#include <algorithm>
#include <cmath>

MissionPlanModel::MissionPlanModel(QObject *parent) : QObject(parent)
{
    resetParameters();
}

QString MissionPlanModel::missionId() const { return m_missionId; }
QString MissionPlanModel::name() const { return m_name; }
QString MissionPlanModel::missionType() const { return m_missionType; }
QString MissionPlanModel::selectedAircraftId() const { return m_selectedAircraftId; }
QString MissionPlanModel::syncState() const { return m_syncState; }
QString MissionPlanModel::missionState() const
{
    if (m_executionState == QStringLiteral("executing")) {
        return QStringLiteral("RUNNING");
    }
    if (m_executionState == QStringLiteral("completed")) {
        return QStringLiteral("COMPLETED");
    }
    if (m_executionState == QStringLiteral("failed")) {
        return QStringLiteral("FAILED");
    }
    if (m_uploadState == QStringLiteral("uploading")) {
        return QStringLiteral("UPLOADING");
    }
    if (m_uploadState == QStringLiteral("uploaded")) {
        return QStringLiteral("READY_TO_START");
    }
    if (missionReady() && m_syncState == QStringLiteral("queued")) {
        return QStringLiteral("READY_TO_UPLOAD");
    }
    if (missionReady()) {
        return QStringLiteral("VALIDATED");
    }
    if (!m_waypoints.isEmpty() || !m_polygon.isEmpty() || hasPoi()) {
        return QStringLiteral("PLANNING");
    }
    return m_missionType.isEmpty() ? QStringLiteral("EMPTY") : QStringLiteral("DRAFT");
}
QString MissionPlanModel::uploadState() const { return m_uploadState; }
QString MissionPlanModel::executionState() const { return m_executionState; }
int MissionPlanModel::activeWaypointIndex() const { return m_activeWaypointIndex; }
int MissionPlanModel::progressPercent() const { return m_progressPercent; }
bool MissionPlanModel::createdLocally() const { return m_createdLocally; }
bool MissionPlanModel::dirty() const { return m_dirty; }
bool MissionPlanModel::hasPoi() const { return m_poi.contains(QStringLiteral("latitude")) && m_poi.contains(QStringLiteral("longitude")); }
double MissionPlanModel::speed() const { return m_speed; }
double MissionPlanModel::altitude() const { return m_altitude; }
double MissionPlanModel::captureInterval() const { return m_captureInterval; }
double MissionPlanModel::gsd() const { return m_gsd; }
int MissionPlanModel::frontOverlap() const { return m_frontOverlap; }
int MissionPlanModel::sideOverlap() const { return m_sideOverlap; }
double MissionPlanModel::radius() const { return m_radius; }
double MissionPlanModel::buildingRadius() const { return m_buildingRadius; }
double MissionPlanModel::safeMargin() const { return m_safeMargin; }
double MissionPlanModel::minAltitude() const { return m_minAltitude; }
double MissionPlanModel::maxAltitude() const { return m_maxAltitude; }
double MissionPlanModel::courseAngle() const { return m_courseAngle; }
double MissionPlanModel::margin() const { return m_margin; }
double MissionPlanModel::gimbalPitch() const { return m_gimbalPitch; }
QString MissionPlanModel::cameraModel() const { return m_cameraModel; }
QString MissionPlanModel::shootingAngle() const { return m_shootingAngle; }
QString MissionPlanModel::captureMode() const { return m_captureMode; }
QString MissionPlanModel::flightCourseMode() const { return m_flightCourseMode; }
QString MissionPlanModel::insideMode() const { return m_insideMode; }
QString MissionPlanModel::headingMode() const { return m_headingMode; }
QString MissionPlanModel::aircraftRotation() const { return m_aircraftRotation; }
QString MissionPlanModel::corneringStyle() const { return m_corneringStyle; }
QString MissionPlanModel::finishAction() const { return m_finishAction; }
QString MissionPlanModel::fenceShape() const { return m_fenceShape; }
QString MissionPlanModel::boundaryType() const { return m_boundaryType; }
QString MissionPlanModel::breachAction() const { return m_breachAction; }
QString MissionPlanModel::warningAction() const { return m_warningAction; }
QString MissionPlanModel::flightDirection() const { return m_flightDirection; }
QVariantList MissionPlanModel::waypoints() const { return m_waypoints; }
QVariantMap MissionPlanModel::takeoffPoint() const { return m_takeoffPoint; }
bool MissionPlanModel::hasTakeoffPoint() const
{
    return m_takeoffPoint.contains(QStringLiteral("latitude"))
        && m_takeoffPoint.contains(QStringLiteral("longitude"));
}
double MissionPlanModel::routeDistanceKm() const
{
    const QVariantList route = serializeForMavsdkMission();
    double routeMeters = 0.0;
    for (int i = 1; i < route.size(); ++i) {
        routeMeters += distanceMeters(route.at(i - 1).toMap(), route.at(i).toMap());
    }
    return routeMeters / 1000.0;
}
QVariantList MissionPlanModel::polygon() const { return m_polygon; }
QVariantMap MissionPlanModel::poi() const { return m_poi; }

double MissionPlanModel::primaryLatitude() const
{
    if (!m_waypoints.isEmpty()) {
        return m_waypoints.first().toMap().value(QStringLiteral("latitude")).toDouble();
    }
    if (!m_polygon.isEmpty()) {
        return m_polygon.first().toMap().value(QStringLiteral("latitude")).toDouble();
    }
    if (hasPoi()) {
        return m_poi.value(QStringLiteral("latitude")).toDouble();
    }
    return 0.0;
}

double MissionPlanModel::primaryLongitude() const
{
    if (!m_waypoints.isEmpty()) {
        return m_waypoints.first().toMap().value(QStringLiteral("longitude")).toDouble();
    }
    if (!m_polygon.isEmpty()) {
        return m_polygon.first().toMap().value(QStringLiteral("longitude")).toDouble();
    }
    if (hasPoi()) {
        return m_poi.value(QStringLiteral("longitude")).toDouble();
    }
    return 0.0;
}

double MissionPlanModel::missionAreaHa() const
{
    if (m_polygon.size() < 3) {
        return 0.0;
    }
    const double latMeters = 111320.0;
    const double centerLat = m_polygon.first().toMap().value(QStringLiteral("latitude")).toDouble();
    const double lonMeters = 111320.0 * std::cos(centerLat * M_PI / 180.0);
    double sum = 0.0;
    for (int i = 0; i < m_polygon.size(); ++i) {
        const auto a = m_polygon.at(i).toMap();
        const auto b = m_polygon.at((i + 1) % m_polygon.size()).toMap();
        const double ax = a.value(QStringLiteral("longitude")).toDouble() * lonMeters;
        const double ay = a.value(QStringLiteral("latitude")).toDouble() * latMeters;
        const double bx = b.value(QStringLiteral("longitude")).toDouble() * lonMeters;
        const double by = b.value(QStringLiteral("latitude")).toDouble() * latMeters;
        sum += ax * by - bx * ay;
    }
    return std::abs(sum) * 0.5 / 10000.0;
}

double MissionPlanModel::estimatedBattery() const
{
    const QVariantList route = serializeForMavsdkMission();
    double routeMeters = 0.0;
    for (int i = 1; i < route.size(); ++i) {
        routeMeters += distanceMeters(route.at(i - 1).toMap(), route.at(i).toMap());
    }
    const double areaLoad = missionAreaHa() * (1.0 + (m_frontOverlap + m_sideOverlap) / 160.0);
    const double speedLoad = std::max(0.0, (m_speed - 16.0) / 4.0);
    const double battery = 8.0 + routeMeters / 450.0 + areaLoad * 0.75 + speedLoad;
    return std::clamp(battery, 0.0, 96.0);
}

QString MissionPlanModel::estimatedTime() const
{
    const QVariantList route = serializeForMavsdkMission();
    double routeMeters = 0.0;
    for (int i = 1; i < route.size(); ++i) {
        routeMeters += distanceMeters(route.at(i - 1).toMap(), route.at(i).toMap());
    }
    if (routeMeters <= 0.1) {
        return QStringLiteral("0m00s");
    }
    const int seconds = static_cast<int>(routeMeters / std::max(1.0, m_speed));
    return QStringLiteral("%1m%2s").arg(seconds / 60).arg(seconds % 60, 2, 10, QLatin1Char('0'));
}

int MissionPlanModel::missionReadiness() const
{
    const auto checks = validationChecks();
    if (checks.isEmpty()) {
        return 0;
    }
    int passed = 0;
    for (const auto &check : checks) {
        if (check.toMap().value(QStringLiteral("passed")).toBool()) {
            ++passed;
        }
    }
    return static_cast<int>(std::round((passed * 100.0) / checks.size()));
}

QVariantList MissionPlanModel::validationWarnings() const
{
    QVariantList warnings;
    for (const auto &check : validationChecks()) {
        const auto map = check.toMap();
        if (!map.value(QStringLiteral("passed")).toBool()) {
            warnings << QVariantMap{{QStringLiteral("severity"), map.value(QStringLiteral("severity")).toString()},
                                    {QStringLiteral("text"), map.value(QStringLiteral("message")).toString()}};
        }
    }
    return warnings;
}

QVariantList MissionPlanModel::validationChecks() const
{
    const bool polygonMission = m_missionType == QStringLiteral("photomap")
        || m_missionType == QStringLiteral("map3dArea")
        || m_missionType == QStringLiteral("towerInspection")
        || m_missionType == QStringLiteral("virtualFence");
    const bool takeoffReady = hasTakeoffPoint();
    const bool geometryComplete = m_missionType == QStringLiteral("waypointRoute")
        ? m_waypoints.size() >= 2
        : (m_missionType == QStringLiteral("map3dPoi") ? hasPoi() : (!polygonMission || m_polygon.size() >= 3));
    const bool overlapCritical = m_missionType == QStringLiteral("photomap")
        || m_missionType == QStringLiteral("map3dArea")
        || m_missionType == QStringLiteral("towerInspection");
    const bool overlapValid = !overlapCritical || (m_frontOverlap >= 55 && m_sideOverlap >= 55);
    const bool altitudeSafe = m_altitude >= 20.0 && m_altitude <= 130.0;
    const bool batteryOk = estimatedBattery() < 80.0;
    const bool fenceValid = m_missionType != QStringLiteral("virtualFence") || (m_polygon.size() >= 3 && m_safeMargin >= 10.0);
    const bool missionSizeOk = missionAreaHa() < 850.0;
    const bool orbitRadiusOk = m_missionType != QStringLiteral("map3dPoi") || (m_radius >= 20.0 && m_radius <= 1000.0 && m_buildingRadius < m_radius);
    const bool uploadable = serializeForMavsdkMission().size() >= 2 || m_missionType == QStringLiteral("virtualFence");

    return {
        QVariantMap{{"label", "Takeoff"}, {"passed", takeoffReady}, {"critical", true}, {"severity", "critical"}, {"message", "Set a required takeoff point before mission upload or execution."}},
        QVariantMap{{"label", "Geometry"}, {"passed", geometryComplete}, {"critical", true}, {"severity", "critical"}, {"message", "Mission geometry is incomplete."}},
        QVariantMap{{"label", "Upload Route"}, {"passed", uploadable}, {"critical", m_missionType != QStringLiteral("virtualFence")}, {"severity", "critical"}, {"message", "Mission cannot yet be converted into uploadable route items."}},
        QVariantMap{{"label", "Battery"}, {"passed", batteryOk}, {"critical", true}, {"severity", "critical"}, {"message", "Estimated battery reserve is insufficient."}},
        QVariantMap{{"label", "Overlap"}, {"passed", overlapValid}, {"critical", overlapCritical}, {"severity", overlapCritical ? "critical" : "info"}, {"message", "Survey overlap is below enterprise capture recommendation."}},
        QVariantMap{{"label", "Altitude"}, {"passed", altitudeSafe}, {"critical", true}, {"severity", "critical"}, {"message", "Altitude is outside the configured safe envelope."}},
        QVariantMap{{"label", "Mission Size"}, {"passed", missionSizeOk}, {"critical", true}, {"severity", "critical"}, {"message", "Mission area is too large for the selected aircraft profile."}},
        QVariantMap{{"label", "Orbit Radius"}, {"passed", orbitRadiusOk}, {"critical", m_missionType == QStringLiteral("map3dPoi")}, {"severity", m_missionType == QStringLiteral("map3dPoi") ? "critical" : "info"}, {"message", "Orbit radius must exceed building radius and remain within aircraft limits."}},
        QVariantMap{{"label", "Geofence"}, {"passed", fenceValid}, {"critical", m_missionType == QStringLiteral("virtualFence")}, {"severity", m_missionType == QStringLiteral("virtualFence") ? "critical" : "warning"}, {"message", "Geofence safe margin is too low."}}
    };
}

bool MissionPlanModel::validationRunning() const { return m_validationRunning; }
bool MissionPlanModel::missionReady() const
{
    for (const auto &check : validationChecks()) {
        const auto map = check.toMap();
        if (map.value(QStringLiteral("critical")).toBool() && !map.value(QStringLiteral("passed")).toBool()) {
            return false;
        }
    }
    return true;
}
QString MissionPlanModel::backendValidationState() const { return m_backendValidationState; }
bool MissionPlanModel::backendSyncReady() const { return m_backendSyncReady; }
bool MissionPlanModel::backendUploadEligible() const { return m_backendUploadEligible; }
bool MissionPlanModel::boundaryOnly() const { return m_boundaryOnly || m_missionType == QStringLiteral("virtualFence"); }
QString MissionPlanModel::importStatus() const { return m_importStatus; }
QString MissionPlanModel::importSummary() const { return m_importSummary; }
QString MissionPlanModel::operationStatus() const { return m_operationStatus; }

void MissionPlanModel::setName(const QString &value)
{
    if (m_name == value) {
        return;
    }
    m_name = value.trimmed().isEmpty() ? QStringLiteral("Untitled Mission") : value.trimmed();
    markDirty();
}

void MissionPlanModel::setMissionType(const QString &value)
{
    if (m_missionType == value) {
        return;
    }
    m_missionType = value;
    markDirty();
}

void MissionPlanModel::setSpeed(double value)
{
    m_speed = std::clamp(value, 1.0, 35.0);
    markDirty();
}

void MissionPlanModel::setAltitude(double value)
{
    m_altitude = std::clamp(value, 10.0, 160.0);
    markDirty();
}

void MissionPlanModel::setCaptureInterval(double value)
{
    m_captureInterval = std::clamp(value, 0.5, 10.0);
    markDirty();
}

void MissionPlanModel::setGsd(double value)
{
    m_gsd = std::clamp(value, 0.5, 12.0);
    markDirty();
}

void MissionPlanModel::setFrontOverlap(int value)
{
    m_frontOverlap = std::clamp(value, 10, 95);
    markDirty();
}

void MissionPlanModel::setSideOverlap(int value)
{
    m_sideOverlap = std::clamp(value, 10, 95);
    markDirty();
}

void MissionPlanModel::setRadius(double value)
{
    m_radius = std::clamp(value, 20.0, 1200.0);
    markDirty();
}

void MissionPlanModel::setBuildingRadius(double value)
{
    m_buildingRadius = std::clamp(value, 5.0, 600.0);
    markDirty();
}

void MissionPlanModel::setSafeMargin(double value)
{
    m_safeMargin = std::clamp(value, 1.0, 100.0);
    markDirty();
}

void MissionPlanModel::setMinAltitude(double value)
{
    m_minAltitude = std::clamp(value, 0.0, 500.0);
    if (m_maxAltitude < m_minAltitude) {
        m_maxAltitude = m_minAltitude;
    }
    markDirty();
}

void MissionPlanModel::setMaxAltitude(double value)
{
    m_maxAltitude = std::clamp(value, 0.0, 500.0);
    if (m_minAltitude > m_maxAltitude) {
        m_minAltitude = m_maxAltitude;
    }
    markDirty();
}

void MissionPlanModel::setCourseAngle(double value)
{
    m_courseAngle = std::clamp(value, 0.0, 360.0);
    markDirty();
}

void MissionPlanModel::setMargin(double value)
{
    m_margin = std::clamp(value, 0.0, 250.0);
    markDirty();
}

void MissionPlanModel::setGimbalPitch(double value)
{
    m_gimbalPitch = std::clamp(value, -90.0, 45.0);
    markDirty();
}

static QString normalizedOption(const QString &value, const QString &fallback)
{
    const QString trimmed = value.trimmed();
    return trimmed.isEmpty() ? fallback : trimmed;
}

void MissionPlanModel::setCameraModel(const QString &value) { m_cameraModel = normalizedOption(value, m_cameraModel); markDirty(); }
void MissionPlanModel::setShootingAngle(const QString &value) { m_shootingAngle = normalizedOption(value, m_shootingAngle); markDirty(); }
void MissionPlanModel::setCaptureMode(const QString &value) { m_captureMode = normalizedOption(value, m_captureMode); markDirty(); }
void MissionPlanModel::setFlightCourseMode(const QString &value) { m_flightCourseMode = normalizedOption(value, m_flightCourseMode); markDirty(); }
void MissionPlanModel::setInsideMode(const QString &value) { m_insideMode = normalizedOption(value, m_insideMode); markDirty(); }
void MissionPlanModel::setHeadingMode(const QString &value) { m_headingMode = normalizedOption(value, m_headingMode); markDirty(); }
void MissionPlanModel::setAircraftRotation(const QString &value) { m_aircraftRotation = normalizedOption(value, m_aircraftRotation); markDirty(); }
void MissionPlanModel::setCorneringStyle(const QString &value) { m_corneringStyle = normalizedOption(value, m_corneringStyle); markDirty(); }
void MissionPlanModel::setFinishAction(const QString &value) { m_finishAction = normalizedOption(value, m_finishAction); markDirty(); }
void MissionPlanModel::setFenceShape(const QString &value) { m_fenceShape = normalizedOption(value, m_fenceShape); markDirty(); }
void MissionPlanModel::setBoundaryType(const QString &value) { m_boundaryType = normalizedOption(value, m_boundaryType); markDirty(); }
void MissionPlanModel::setBreachAction(const QString &value) { m_breachAction = normalizedOption(value, m_breachAction); markDirty(); }
void MissionPlanModel::setWarningAction(const QString &value) { m_warningAction = normalizedOption(value, m_warningAction); markDirty(); }
void MissionPlanModel::setFlightDirection(const QString &value) { m_flightDirection = normalizedOption(value, m_flightDirection); markDirty(); }

void MissionPlanModel::setPrimaryLatitude(double value)
{
    setPrimaryCoordinate(std::clamp(value, -90.0, 90.0), primaryLongitude());
}

void MissionPlanModel::setPrimaryLongitude(double value)
{
    setPrimaryCoordinate(primaryLatitude(), std::clamp(value, -180.0, 180.0));
}

void MissionPlanModel::createDraft(const QString &missionType, const QString &aircraftId)
{
    resetParameters();
    m_missionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_missionType = missionType;
    m_selectedAircraftId = aircraftId;
    m_name = QStringLiteral("Untitled %1").arg(missionType);
    m_syncState = QStringLiteral("local");
    m_uploadState = QStringLiteral("not_uploaded");
    m_executionState = QStringLiteral("idle");
    m_activeWaypointIndex = -1;
    m_progressPercent = 0;
    m_createdLocally = true;
    m_dirty = true;
    m_backendValidationState = QStringLiteral("not_validated");
    m_backendSyncReady = false;
    m_backendUploadEligible = false;
    m_boundaryOnly = missionType == QStringLiteral("virtualFence");
    m_takeoffPoint.clear();
    m_waypoints.clear();
    m_polygon.clear();
    m_poi.clear();
    setOperationStatus(QStringLiteral("New mission draft created"));
    emit planChanged();
    emit validationChanged();
}

void MissionPlanModel::loadMission(const QVariantMap &mission)
{
    resetParameters();
    m_missionId = mission.value(QStringLiteral("id"), mission.value(QStringLiteral("local_id"))).toString();
    m_name = mission.value(QStringLiteral("name"), QStringLiteral("Untitled Mission")).toString();
    m_missionType = mission.value(QStringLiteral("mission_type"), mission.value(QStringLiteral("type"))).toString();
    m_selectedAircraftId = mission.value(QStringLiteral("assigned_aircraft")).toString();
    m_syncState = QStringLiteral("synced");
    m_uploadState = mission.value(QStringLiteral("upload_state"), QStringLiteral("not_uploaded")).toString();
    m_executionState = mission.value(QStringLiteral("execution_state"), QStringLiteral("idle")).toString();
    m_activeWaypointIndex = mission.value(QStringLiteral("active_waypoint"), -1).toInt();
    m_progressPercent = mission.value(QStringLiteral("progress_percent"), 0).toInt();
    m_createdLocally = false;
    m_dirty = false;
    m_backendValidationState = mission.value(QStringLiteral("validation_state"), QStringLiteral("not_validated")).toString();
    m_backendSyncReady = mission.value(QStringLiteral("sync_ready"), false).toBool();
    m_backendUploadEligible = m_backendSyncReady && m_backendValidationState != QStringLiteral("blocked") && m_missionType != QStringLiteral("virtualFence");
    m_boundaryOnly = m_missionType == QStringLiteral("virtualFence");

    const QVariantMap geometry = mission.value(QStringLiteral("geometry")).toMap();
    m_waypoints = geometry.value(QStringLiteral("waypoints")).toList();
    m_takeoffPoint = geometry.value(QStringLiteral("takeoff"), geometry.value(QStringLiteral("takeoff_point"))).toMap();
    m_polygon = geometry.value(QStringLiteral("polygon")).toList();
    m_poi = geometry.value(QStringLiteral("poi")).toMap();

    const QVariantMap commands = mission.value(QStringLiteral("commands")).toMap();
    if (!commands.isEmpty() && !m_takeoffPoint.contains(QStringLiteral("latitude"))) {
        m_takeoffPoint = commands.value(QStringLiteral("takeoff")).toMap();
    }

    const QVariantMap parameters = mission.value(QStringLiteral("parameters")).toMap();
    m_speed = parameters.value(QStringLiteral("speed"), m_speed).toDouble();
    m_altitude = parameters.value(QStringLiteral("altitude"), m_altitude).toDouble();
    m_captureInterval = parameters.value(QStringLiteral("capture_interval"), m_captureInterval).toDouble();
    m_frontOverlap = parameters.value(QStringLiteral("front_overlap"), m_frontOverlap).toInt();
    m_sideOverlap = parameters.value(QStringLiteral("side_overlap"), m_sideOverlap).toInt();
    m_radius = parameters.value(QStringLiteral("radius"), m_radius).toDouble();
    m_buildingRadius = parameters.value(QStringLiteral("building_radius"), m_buildingRadius).toDouble();
    m_safeMargin = parameters.value(QStringLiteral("safe_margin"), m_safeMargin).toDouble();
    m_minAltitude = parameters.value(QStringLiteral("min_altitude"), m_minAltitude).toDouble();
    m_maxAltitude = parameters.value(QStringLiteral("max_altitude"), m_maxAltitude).toDouble();
    m_courseAngle = parameters.value(QStringLiteral("course_angle"), m_courseAngle).toDouble();
    m_margin = parameters.value(QStringLiteral("margin"), m_margin).toDouble();
    m_gimbalPitch = parameters.value(QStringLiteral("gimbal_pitch"), m_gimbalPitch).toDouble();
    m_cameraModel = parameters.value(QStringLiteral("camera_model"), m_cameraModel).toString();
    m_shootingAngle = parameters.value(QStringLiteral("shooting_angle"), m_shootingAngle).toString();
    m_captureMode = parameters.value(QStringLiteral("capture_mode"), m_captureMode).toString();
    m_flightCourseMode = parameters.value(QStringLiteral("flight_course_mode"), m_flightCourseMode).toString();
    m_insideMode = parameters.value(QStringLiteral("inside_mode"), m_insideMode).toString();
    m_headingMode = parameters.value(QStringLiteral("heading_mode"), m_headingMode).toString();
    m_aircraftRotation = parameters.value(QStringLiteral("aircraft_rotation"), m_aircraftRotation).toString();
    m_corneringStyle = parameters.value(QStringLiteral("cornering_style"), m_corneringStyle).toString();
    m_finishAction = parameters.value(QStringLiteral("finish_action"), m_finishAction).toString();
    m_fenceShape = parameters.value(QStringLiteral("fence_shape"), m_fenceShape).toString();
    m_boundaryType = parameters.value(QStringLiteral("boundary_type"), m_boundaryType).toString();
    m_breachAction = parameters.value(QStringLiteral("breach_action"), m_breachAction).toString();
    m_warningAction = parameters.value(QStringLiteral("warning_action"), m_warningAction).toString();
    m_flightDirection = parameters.value(QStringLiteral("flight_direction"), m_flightDirection).toString();

    setOperationStatus(QStringLiteral("Mission loaded"));
    emit planChanged();
    emit validationChanged();
}

QVariantMap MissionPlanModel::serializeForBackend() const
{
    const QVariantList route = serializeForMavsdkMission();
    double routeMeters = 0.0;
    for (int i = 1; i < route.size(); ++i) {
        routeMeters += distanceMeters(route.at(i - 1).toMap(), route.at(i).toMap());
    }
    const double estimatedDistanceKm = std::round((routeMeters / 1000.0) * 100.0) / 100.0;
    const int durationMinutes = routeMeters > 0.0 ? static_cast<int>(std::ceil((routeMeters / std::max(1.0, m_speed)) / 60.0)) : 0;
    return {
        {QStringLiteral("id"), m_createdLocally ? QVariant() : m_missionId},
        {QStringLiteral("local_id"), m_missionId},
        {QStringLiteral("name"), m_name},
        {QStringLiteral("mission_type"), m_missionType},
        {QStringLiteral("geometry"), QVariantMap{
            {QStringLiteral("takeoff"), m_takeoffPoint},
            {QStringLiteral("waypoints"), m_waypoints},
            {QStringLiteral("polygon"), m_polygon},
            {QStringLiteral("poi"), m_poi}
        }},
        {QStringLiteral("commands"), QVariantMap{
            {QStringLiteral("takeoff"), m_takeoffPoint},
            {QStringLiteral("waypoints"), m_waypoints},
            {QStringLiteral("finish_action"), m_finishAction}
        }},
        {QStringLiteral("parameters"), QVariantMap{
            {QStringLiteral("speed"), m_speed},
            {QStringLiteral("altitude"), m_altitude},
            {QStringLiteral("capture_interval"), m_captureInterval},
            {QStringLiteral("gsd"), m_gsd},
            {QStringLiteral("front_overlap"), m_frontOverlap},
            {QStringLiteral("side_overlap"), m_sideOverlap},
            {QStringLiteral("radius"), m_radius},
            {QStringLiteral("building_radius"), m_buildingRadius},
            {QStringLiteral("safe_margin"), m_safeMargin},
            {QStringLiteral("min_altitude"), m_minAltitude},
            {QStringLiteral("max_altitude"), m_maxAltitude},
            {QStringLiteral("course_angle"), m_courseAngle},
            {QStringLiteral("margin"), m_margin},
            {QStringLiteral("gimbal_pitch"), m_gimbalPitch},
            {QStringLiteral("camera_model"), m_cameraModel},
            {QStringLiteral("shooting_angle"), m_shootingAngle},
            {QStringLiteral("capture_mode"), m_captureMode},
            {QStringLiteral("flight_course_mode"), m_flightCourseMode},
            {QStringLiteral("inside_mode"), m_insideMode},
            {QStringLiteral("heading_mode"), m_headingMode},
            {QStringLiteral("aircraft_rotation"), m_aircraftRotation},
            {QStringLiteral("cornering_style"), m_corneringStyle},
            {QStringLiteral("finish_action"), m_finishAction},
            {QStringLiteral("fence_shape"), m_fenceShape},
            {QStringLiteral("boundary_type"), m_boundaryType},
            {QStringLiteral("breach_action"), m_breachAction},
            {QStringLiteral("warning_action"), m_warningAction},
            {QStringLiteral("flight_direction"), m_flightDirection}
        }},
        {QStringLiteral("overlays"), route},
        {QStringLiteral("estimated_distance_km"), estimatedDistanceKm},
        {QStringLiteral("estimated_duration_minutes"), durationMinutes},
        {QStringLiteral("status"), QStringLiteral("draft")},
        {QStringLiteral("validation_state"), missionReady() ? QStringLiteral("valid") : QStringLiteral("blocked")},
        {QStringLiteral("validation_issues"), validationWarnings()},
        {QStringLiteral("sync_ready"), missionReady()},
        {QStringLiteral("upload_state"), m_uploadState},
        {QStringLiteral("execution_state"), m_executionState},
        {QStringLiteral("active_waypoint"), m_activeWaypointIndex},
        {QStringLiteral("progress_percent"), m_progressPercent}
    };
}

QVariantList MissionPlanModel::serializeForMavsdkMission() const
{
    QVariantList route;
    if (m_missionType == QStringLiteral("waypointRoute")) {
        route = m_waypoints;
    } else if (m_missionType == QStringLiteral("map3dPoi")) {
        route = generatedOrbitRoute();
    } else if (m_missionType == QStringLiteral("photomap")
               || m_missionType == QStringLiteral("map3dArea")
               || m_missionType == QStringLiteral("towerInspection")) {
        route = generatedSurveyRoute();
    }

    if (route.isEmpty() || !hasTakeoffPoint() || boundaryOnly()) {
        return route;
    }

    QVariantMap takeoff = m_takeoffPoint;
    takeoff[QStringLiteral("altitude")] = takeoff.value(QStringLiteral("altitude"), m_altitude);
    takeoff[QStringLiteral("speed")] = takeoff.value(QStringLiteral("speed"), m_speed);
    takeoff[QStringLiteral("heading")] = takeoff.value(QStringLiteral("heading"), 0.0);
    takeoff[QStringLiteral("gimbal_pitch")] = takeoff.value(QStringLiteral("gimbal_pitch"), m_gimbalPitch);
    takeoff[QStringLiteral("action")] = QStringLiteral("Takeoff");

    const QVariantMap first = route.first().toMap();
    const double dLat = std::abs(first.value(QStringLiteral("latitude")).toDouble() - takeoff.value(QStringLiteral("latitude")).toDouble());
    const double dLon = std::abs(first.value(QStringLiteral("longitude")).toDouble() - takeoff.value(QStringLiteral("longitude")).toDouble());
    if (dLat > 0.0000005 || dLon > 0.0000005) {
        route.prepend(takeoff);
    }
    return route;
}

bool MissionPlanModel::validateForUpload(bool aircraftConnected, bool aircraftReady)
{
    validateMission();
    const bool ok = missionReady()
        && m_backendSyncReady
        && m_backendUploadEligible
        && hasTakeoffPoint()
        && !m_createdLocally
        && m_missionType != QStringLiteral("virtualFence")
        && aircraftConnected
        && aircraftReady
        && serializeForMavsdkMission().size() >= 2;
    if (!aircraftConnected) {
        setOperationStatus(QStringLiteral("Upload blocked: no connected aircraft"));
    } else if (!aircraftReady) {
        setOperationStatus(QStringLiteral("Upload blocked: aircraft preflight not ready"));
    } else if (!hasTakeoffPoint()) {
        setOperationStatus(QStringLiteral("Upload blocked: set a takeoff point first"));
    } else if (m_createdLocally || !m_backendSyncReady) {
        setOperationStatus(QStringLiteral("Upload blocked: save and sync mission with Control Center first"));
    } else if (m_missionType == QStringLiteral("virtualFence")) {
        setOperationStatus(QStringLiteral("Upload blocked: virtual fences sync as Control Center boundaries"));
    } else if (!m_backendUploadEligible) {
        setOperationStatus(QStringLiteral("Upload blocked: Control Center validation is required"));
    }
    return ok;
}

void MissionPlanModel::markUploading()
{
    m_uploadState = QStringLiteral("uploading");
    m_executionState = QStringLiteral("ready");
    setOperationStatus(QStringLiteral("Uploading mission to aircraft"));
    emit planChanged();
    emit validationChanged();
}

void MissionPlanModel::markUploaded(const QString &result)
{
    m_uploadState = QStringLiteral("uploaded");
    m_executionState = QStringLiteral("ready");
    m_dirty = true;
    setOperationStatus(result.isEmpty() ? QStringLiteral("Mission uploaded to aircraft") : result);
    emit planChanged();
    emit validationChanged();
}

void MissionPlanModel::markUploadFailed(const QString &reason)
{
    m_uploadState = QStringLiteral("failed");
    m_executionState = QStringLiteral("failed");
    setOperationStatus(reason.isEmpty() ? QStringLiteral("Mission upload failed") : reason);
    emit planChanged();
    emit validationChanged();
}

void MissionPlanModel::markExecuting()
{
    m_executionState = QStringLiteral("executing");
    setOperationStatus(QStringLiteral("Mission executing"));
    emit planChanged();
}

void MissionPlanModel::markCompleted(bool success, const QString &reason)
{
    m_executionState = success ? QStringLiteral("completed") : QStringLiteral("failed");
    m_progressPercent = success ? 100 : m_progressPercent;
    setOperationStatus(success ? QStringLiteral("Mission completed") : QStringLiteral("Mission failed: %1").arg(reason));
    emit planChanged();
}

void MissionPlanModel::setExecutionProgress(int activeWaypoint, int progressPercent)
{
    m_activeWaypointIndex = activeWaypoint;
    m_progressPercent = std::clamp(progressPercent, 0, 100);
    emit planChanged();
}

void MissionPlanModel::addWaypoint(double latitude, double longitude)
{
    m_waypoints << QVariantMap{{"latitude", latitude},
                               {"longitude", longitude},
                               {"altitude", m_altitude},
                               {"speed", m_speed},
                               {"heading", 0.0},
                               {"gimbal_pitch", m_gimbalPitch},
                               {"action", QStringLiteral("No Action")},
                               {"camera_mode", m_captureMode},
                               {"loiter_seconds", 0.0},
                               {"hover_seconds", 0.0},
                               {"is_return_home", false}};
    setOperationStatus(QStringLiteral("Waypoint %1 placed").arg(m_waypoints.size()));
    markDirty();
}

void MissionPlanModel::setTakeoffPoint(double latitude, double longitude)
{
    m_takeoffPoint = QVariantMap{
        {QStringLiteral("latitude"), std::clamp(latitude, -90.0, 90.0)},
        {QStringLiteral("longitude"), std::clamp(longitude, -180.0, 180.0)},
        {QStringLiteral("altitude"), m_altitude},
        {QStringLiteral("speed"), m_speed},
        {QStringLiteral("action"), QStringLiteral("Takeoff")}
    };
    setOperationStatus(QStringLiteral("Takeoff point set"));
    markDirty();
}

void MissionPlanModel::clearTakeoffPoint()
{
    if (m_takeoffPoint.isEmpty()) {
        return;
    }
    m_takeoffPoint.clear();
    setOperationStatus(QStringLiteral("Takeoff point cleared"));
    markDirty();
}

void MissionPlanModel::addPolygonVertex(double latitude, double longitude)
{
    m_polygon << QVariantMap{{"latitude", latitude}, {"longitude", longitude}};
    setOperationStatus(QStringLiteral("Boundary vertex %1 placed").arg(m_polygon.size()));
    markDirty();
}

void MissionPlanModel::movePolygonVertex(int index, double latitude, double longitude)
{
    if (index < 0 || index >= m_polygon.size()) {
        return;
    }
    auto point = m_polygon.at(index).toMap();
    point["latitude"] = latitude;
    point["longitude"] = longitude;
    m_polygon[index] = point;
    setOperationStatus(QStringLiteral("Boundary adjusted"));
    markDirty();
}

void MissionPlanModel::moveWaypoint(int index, double latitude, double longitude)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    auto point = m_waypoints.at(index).toMap();
    point["latitude"] = latitude;
    point["longitude"] = longitude;
    m_waypoints[index] = point;
    setOperationStatus(QStringLiteral("Waypoint %1 adjusted").arg(index + 1));
    markDirty();
}

void MissionPlanModel::setWaypointAltitude(int index, double altitude)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    auto point = m_waypoints.at(index).toMap();
    point["altitude"] = std::clamp(altitude, 10.0, 160.0);
    m_waypoints[index] = point;
    markDirty();
}

void MissionPlanModel::setWaypointSpeed(int index, double speed)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    auto point = m_waypoints.at(index).toMap();
    point["speed"] = std::clamp(speed, 1.0, 35.0);
    m_waypoints[index] = point;
    markDirty();
}

void MissionPlanModel::setWaypointHeading(int index, double heading)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    auto point = m_waypoints.at(index).toMap();
    point["heading"] = std::fmod(std::max(0.0, heading), 360.0);
    m_waypoints[index] = point;
    markDirty();
}

void MissionPlanModel::setWaypointGimbalPitch(int index, double pitch)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    auto point = m_waypoints.at(index).toMap();
    point["gimbal_pitch"] = std::clamp(pitch, -90.0, 20.0);
    m_waypoints[index] = point;
    markDirty();
}

void MissionPlanModel::setWaypointAction(int index, const QString &action)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    auto point = m_waypoints.at(index).toMap();
    const QString normalized = normalizedOption(action, QStringLiteral("No Action"));
    point["action"] = normalized;
    point["is_return_home"] = normalized == QStringLiteral("Return-to-Home");
    m_waypoints[index] = point;
    markDirty();
}

void MissionPlanModel::setWaypointCameraMode(int index, const QString &cameraMode)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    auto point = m_waypoints.at(index).toMap();
    point["camera_mode"] = normalizedOption(cameraMode, m_captureMode);
    m_waypoints[index] = point;
    markDirty();
}

void MissionPlanModel::setWaypointLoiterSeconds(int index, double seconds)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    auto point = m_waypoints.at(index).toMap();
    point["loiter_seconds"] = std::clamp(seconds, 0.0, 600.0);
    m_waypoints[index] = point;
    markDirty();
}

void MissionPlanModel::setWaypointHoverSeconds(int index, double seconds)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    auto point = m_waypoints.at(index).toMap();
    point["hover_seconds"] = std::clamp(seconds, 0.0, 600.0);
    m_waypoints[index] = point;
    markDirty();
}

void MissionPlanModel::deleteWaypoint(int index)
{
    if (index < 0 || index >= m_waypoints.size()) {
        return;
    }
    m_waypoints.removeAt(index);
    setOperationStatus(QStringLiteral("Waypoint deleted"));
    markDirty();
}

void MissionPlanModel::moveWaypointOrder(int fromIndex, int toIndex)
{
    if (fromIndex < 0 || fromIndex >= m_waypoints.size() || toIndex < 0 || toIndex >= m_waypoints.size() || fromIndex == toIndex) {
        return;
    }
    m_waypoints.move(fromIndex, toIndex);
    setOperationStatus(QStringLiteral("Waypoint order updated"));
    markDirty();
}

QVariantMap MissionPlanModel::waypointAt(int index) const
{
    if (index < 0 || index >= m_waypoints.size()) {
        return {};
    }
    return m_waypoints.at(index).toMap();
}

void MissionPlanModel::setPoi(double latitude, double longitude)
{
    m_poi["latitude"] = latitude;
    m_poi["longitude"] = longitude;
    setOperationStatus(QStringLiteral("POI positioned"));
    markDirty();
}

void MissionPlanModel::setPoiRadius(double radius)
{
    setRadius(radius);
}

void MissionPlanModel::setPrimaryCoordinate(double latitude, double longitude)
{
    latitude = std::clamp(latitude, -90.0, 90.0);
    longitude = std::clamp(longitude, -180.0, 180.0);
    if (!m_waypoints.isEmpty()) {
        moveWaypoint(0, latitude, longitude);
        return;
    }
    if (!m_polygon.isEmpty()) {
        movePolygonVertex(0, latitude, longitude);
        return;
    }
    if (hasPoi() || m_missionType == QStringLiteral("map3dPoi")) {
        setPoi(latitude, longitude);
        return;
    }
    if (m_missionType == QStringLiteral("waypointRoute")) {
        addWaypoint(latitude, longitude);
    } else {
        addPolygonVertex(latitude, longitude);
    }
}

void MissionPlanModel::undoLastGeometry()
{
    if (m_missionType == QStringLiteral("waypointRoute") && !m_waypoints.isEmpty()) {
        m_waypoints.removeLast();
        setOperationStatus(QStringLiteral("Last waypoint removed"));
        markDirty();
        return;
    }
    if ((m_missionType == QStringLiteral("photomap")
         || m_missionType == QStringLiteral("map3dArea")
         || m_missionType == QStringLiteral("towerInspection")
         || m_missionType == QStringLiteral("virtualFence")) && !m_polygon.isEmpty()) {
        m_polygon.removeLast();
        setOperationStatus(QStringLiteral("Last boundary vertex removed"));
        markDirty();
        return;
    }
    if (m_missionType == QStringLiteral("map3dPoi") && !m_poi.isEmpty()) {
        m_poi.clear();
        setOperationStatus(QStringLiteral("POI removed"));
        markDirty();
    }
}

void MissionPlanModel::clearDraftGeometry()
{
    m_waypoints.clear();
    m_takeoffPoint.clear();
    m_polygon.clear();
    m_poi.clear();
    setOperationStatus(QStringLiteral("Mission geometry cleared"));
    markDirty();
}

void MissionPlanModel::validateMission()
{
    m_validationRunning = true;
    setOperationStatus(QStringLiteral("Running mission validation"));
    emit validationChanged();
    m_validationRunning = false;
    if (!missionReady()) {
        m_executionState = QStringLiteral("validation_failed");
    } else if (m_executionState == QStringLiteral("validation_failed") || m_executionState == QStringLiteral("idle")) {
        m_executionState = QStringLiteral("validated");
    }
    setOperationStatus(missionReady() ? QStringLiteral("Mission validated and ready") : QStringLiteral("Validation requires operator review"));
    emit planChanged();
    emit validationChanged();
}

void MissionPlanModel::applyBackendValidation(const QVariantMap &validation)
{
    m_backendValidationState = validation.value(QStringLiteral("validation_state"), QStringLiteral("not_validated")).toString();
    m_backendSyncReady = validation.value(QStringLiteral("sync_ready"), false).toBool();
    m_backendUploadEligible = validation.value(QStringLiteral("upload_eligible"), false).toBool();
    m_boundaryOnly = validation.value(QStringLiteral("boundary_only"), m_missionType == QStringLiteral("virtualFence")).toBool();

    if (m_boundaryOnly) {
        setOperationStatus(QStringLiteral("Control Center validated boundary-only safety mission"));
    } else if (m_backendUploadEligible) {
        setOperationStatus(QStringLiteral("Control Center validation passed; upload eligible"));
    } else if (m_backendValidationState == QStringLiteral("blocked")) {
        setOperationStatus(QStringLiteral("Control Center validation blocked this mission"));
    } else {
        setOperationStatus(QStringLiteral("Control Center validation requires review"));
    }
    emit planChanged();
    emit validationChanged();
}

void MissionPlanModel::saveMission()
{
    saveLocal();
}

void MissionPlanModel::saveLocal()
{
    m_syncState = QStringLiteral("queued");
    m_dirty = false;
    setOperationStatus(QStringLiteral("Mission saved locally and queued for sync"));
    emit planChanged();
}

void MissionPlanModel::resetGeometry()
{
    clearDraftGeometry();
}

QString MissionPlanModel::chooseImportFile() const
{
    return QFileDialog::getOpenFileName(nullptr,
                                        QStringLiteral("Import KML or GeoJSON"),
                                        QDir::homePath(),
                                        QStringLiteral("Mission geometry (*.kml *.geojson *.json);;KML files (*.kml);;GeoJSON files (*.geojson *.json)"));
}

bool MissionPlanModel::importMissionFile(const QString &fileUrlOrPath)
{
    const QUrl url(fileUrlOrPath);
    const QString path = url.isLocalFile() ? url.toLocalFile() : fileUrlOrPath;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        m_importStatus = QStringLiteral("Import failed: cannot open file");
        setOperationStatus(m_importStatus);
        emit planChanged();
        return false;
    }

    const QByteArray data = file.readAll();
    const QString suffix = QFileInfo(path).suffix().toLower();
    const bool imported = suffix == QStringLiteral("kml")
        ? importKml(data)
        : (suffix == QStringLiteral("geojson") || suffix == QStringLiteral("json") ? importGeoJson(data) : false);

    if (!imported && suffix != QStringLiteral("kml") && suffix != QStringLiteral("geojson") && suffix != QStringLiteral("json")) {
        m_importStatus = QStringLiteral("Import failed: use KML, GeoJSON, or JSON");
        setOperationStatus(m_importStatus);
        emit planChanged();
    }
    return imported;
}

void MissionPlanModel::resetParameters()
{
    m_speed = 8.0;
    m_altitude = 80.0;
    m_captureInterval = 2.0;
    m_gsd = 2.3;
    m_frontOverlap = 70;
    m_sideOverlap = 70;
    m_radius = 80.0;
    m_buildingRadius = 30.0;
    m_safeMargin = 10.0;
    m_minAltitude = 101.2;
    m_maxAltitude = 120.1;
    m_courseAngle = 70.0;
    m_margin = 70.0;
    m_gimbalPitch = 45.0;
    m_cameraModel = QStringLiteral("Phantom 4 Pro Camera");
    m_shootingAngle = QStringLiteral("Parallel To Main Path");
    m_captureMode = QStringLiteral("Capture at Equal Dist. Interval");
    m_flightCourseMode = QStringLiteral("Inside Mode");
    m_insideMode = QStringLiteral("Parallel");
    m_headingMode = QStringLiteral("Define Per Point");
    m_aircraftRotation = QStringLiteral("Auto");
    m_corneringStyle = QStringLiteral("Curved");
    m_finishAction = QStringLiteral("Return to Home");
    m_fenceShape = QStringLiteral("Polygon");
    m_boundaryType = QStringLiteral("Inclusion");
    m_breachAction = QStringLiteral("Return to Home");
    m_warningAction = QStringLiteral("Alert Pilot");
    m_flightDirection = QStringLiteral("Clockwise");
}

void MissionPlanModel::markDirty()
{
    m_dirty = true;
    m_uploadState = QStringLiteral("not_uploaded");
    m_backendValidationState = QStringLiteral("not_validated");
    m_backendSyncReady = false;
    m_backendUploadEligible = false;
    m_boundaryOnly = m_missionType == QStringLiteral("virtualFence");
    if (m_executionState != QStringLiteral("executing")) {
        m_executionState = QStringLiteral("planning");
    }
    emit planChanged();
    emit validationChanged();
}

QVariantMap MissionPlanModel::waypointFromCoordinate(double latitude, double longitude, double altitude, double speed)
{
    return QVariantMap{
        {QStringLiteral("latitude"), latitude},
        {QStringLiteral("longitude"), longitude},
        {QStringLiteral("altitude"), altitude},
        {QStringLiteral("speed"), speed},
        {QStringLiteral("heading"), 0.0},
        {QStringLiteral("gimbal_pitch"), 45.0},
        {QStringLiteral("action"), QStringLiteral("No Action")},
        {QStringLiteral("camera_mode"), QStringLiteral("Capture at Equal Dist. Interval")},
        {QStringLiteral("loiter_seconds"), 0.0},
        {QStringLiteral("hover_seconds"), 0.0},
        {QStringLiteral("is_return_home"), false}
    };
}

bool MissionPlanModel::coordinateFromGeoJson(const QJsonValue &value, double *latitude, double *longitude, double *altitude)
{
    if (!value.isArray()) {
        return false;
    }
    const QJsonArray coordinate = value.toArray();
    if (coordinate.size() < 2) {
        return false;
    }
    *longitude = coordinate.at(0).toDouble();
    *latitude = coordinate.at(1).toDouble();
    *altitude = coordinate.size() > 2 ? coordinate.at(2).toDouble() : 0.0;
    return std::abs(*latitude) <= 90.0 && std::abs(*longitude) <= 180.0;
}

bool MissionPlanModel::importGeoJson(const QByteArray &data)
{
    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        m_importStatus = QStringLiteral("Import failed: invalid GeoJSON");
        setOperationStatus(m_importStatus);
        emit planChanged();
        return false;
    }

    QVariantList importedWaypoints;
    QVariantList importedPolygon;
    QVariantMap importedPoi;

    const auto ingestGeometry = [&](const QJsonObject &geometry) {
        const QString type = geometry.value(QStringLiteral("type")).toString();
        const QJsonValue coordinates = geometry.value(QStringLiteral("coordinates"));
        double lat = 0.0;
        double lon = 0.0;
        double alt = m_altitude;

        if (type == QStringLiteral("Point")) {
            if (coordinateFromGeoJson(coordinates, &lat, &lon, &alt)) {
                importedPoi = QVariantMap{{QStringLiteral("latitude"), lat}, {QStringLiteral("longitude"), lon}};
                importedWaypoints << waypointFromCoordinate(lat, lon, alt > 0 ? alt : m_altitude, m_speed);
            }
        } else if (type == QStringLiteral("MultiPoint") || type == QStringLiteral("LineString")) {
            const QJsonArray points = coordinates.toArray();
            for (const QJsonValue &pointValue : points) {
                if (coordinateFromGeoJson(pointValue, &lat, &lon, &alt)) {
                    importedWaypoints << waypointFromCoordinate(lat, lon, alt > 0 ? alt : m_altitude, m_speed);
                }
            }
        } else if (type == QStringLiteral("Polygon")) {
            const QJsonArray rings = coordinates.toArray();
            const QJsonArray outerRing = rings.isEmpty() ? QJsonArray{} : rings.first().toArray();
            for (const QJsonValue &pointValue : outerRing) {
                if (coordinateFromGeoJson(pointValue, &lat, &lon, &alt)) {
                    importedPolygon << QVariantMap{{QStringLiteral("latitude"), lat}, {QStringLiteral("longitude"), lon}};
                }
            }
            if (importedPolygon.size() > 1 && importedPolygon.first().toMap() == importedPolygon.last().toMap()) {
                importedPolygon.removeLast();
            }
        } else if (type == QStringLiteral("MultiPolygon")) {
            const QJsonArray polygons = coordinates.toArray();
            if (!polygons.isEmpty()) {
                const QJsonArray rings = polygons.first().toArray();
                const QJsonArray outerRing = rings.isEmpty() ? QJsonArray{} : rings.first().toArray();
                for (const QJsonValue &pointValue : outerRing) {
                    if (coordinateFromGeoJson(pointValue, &lat, &lon, &alt)) {
                        importedPolygon << QVariantMap{{QStringLiteral("latitude"), lat}, {QStringLiteral("longitude"), lon}};
                    }
                }
            }
        }
    };

    const QJsonObject root = document.object();
    if (root.value(QStringLiteral("type")).toString() == QStringLiteral("FeatureCollection")) {
        for (const QJsonValue &featureValue : root.value(QStringLiteral("features")).toArray()) {
            ingestGeometry(featureValue.toObject().value(QStringLiteral("geometry")).toObject());
        }
    } else if (root.value(QStringLiteral("type")).toString() == QStringLiteral("Feature")) {
        ingestGeometry(root.value(QStringLiteral("geometry")).toObject());
    } else {
        ingestGeometry(root);
    }

    applyImportedGeometry(importedWaypoints, importedPolygon, importedPoi, QStringLiteral("GeoJSON"));
    return !importedWaypoints.isEmpty() || !importedPolygon.isEmpty() || !importedPoi.isEmpty();
}

bool MissionPlanModel::importKml(const QByteArray &data)
{
    QXmlStreamReader xml(data);
    QStringList elementStack;
    QVariantList importedWaypoints;
    QVariantList importedPolygon;
    QVariantMap importedPoi;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            elementStack << xml.name().toString();
            if (xml.name() != QLatin1String("coordinates")) {
                continue;
            }

            const QString text = xml.readElementText().trimmed();
            const bool inPolygon = elementStack.contains(QStringLiteral("Polygon")) || elementStack.contains(QStringLiteral("LinearRing"));
            const bool inPoint = elementStack.contains(QStringLiteral("Point"));
            const QStringList coordinateTokens = text.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            QVariantList parsed;
            for (const QString &token : coordinateTokens) {
                const QStringList parts = token.split(QLatin1Char(','));
                if (parts.size() < 2) {
                    continue;
                }
                const double lon = parts.at(0).toDouble();
                const double lat = parts.at(1).toDouble();
                const double alt = parts.size() > 2 ? parts.at(2).toDouble() : m_altitude;
                if (std::abs(lat) <= 90.0 && std::abs(lon) <= 180.0) {
                    parsed << waypointFromCoordinate(lat, lon, alt > 0 ? alt : m_altitude, m_speed);
                }
            }

            if (inPolygon) {
                for (const QVariant &point : parsed) {
                    const QVariantMap map = point.toMap();
                    importedPolygon << QVariantMap{{QStringLiteral("latitude"), map.value(QStringLiteral("latitude"))},
                                                   {QStringLiteral("longitude"), map.value(QStringLiteral("longitude"))}};
                }
                if (importedPolygon.size() > 1 && importedPolygon.first().toMap() == importedPolygon.last().toMap()) {
                    importedPolygon.removeLast();
                }
            } else if (inPoint && !parsed.isEmpty()) {
                const QVariantMap map = parsed.first().toMap();
                importedPoi = QVariantMap{{QStringLiteral("latitude"), map.value(QStringLiteral("latitude"))},
                                          {QStringLiteral("longitude"), map.value(QStringLiteral("longitude"))}};
                importedWaypoints << map;
            } else {
                for (const QVariant &point : parsed) {
                    importedWaypoints << point;
                }
            }
            if (!elementStack.isEmpty()) {
                elementStack.removeLast();
            }
        } else if (xml.isEndElement() && !elementStack.isEmpty()) {
            elementStack.removeLast();
        }
    }

    if (xml.hasError()) {
        m_importStatus = QStringLiteral("Import failed: invalid KML");
        setOperationStatus(m_importStatus);
        emit planChanged();
        return false;
    }

    applyImportedGeometry(importedWaypoints, importedPolygon, importedPoi, QStringLiteral("KML"));
    return !importedWaypoints.isEmpty() || !importedPolygon.isEmpty() || !importedPoi.isEmpty();
}

void MissionPlanModel::applyImportedGeometry(const QVariantList &waypoints, const QVariantList &polygon, const QVariantMap &poi, const QString &sourceLabel)
{
    if (waypoints.isEmpty() && polygon.isEmpty() && poi.isEmpty()) {
        m_importStatus = QStringLiteral("Import failed: no mission geometry found");
        setOperationStatus(m_importStatus);
        emit planChanged();
        return;
    }

    if (m_missionId.isEmpty()) {
        m_missionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        m_createdLocally = true;
    }
    QString inferredType;
    if (!polygon.isEmpty()) {
        inferredType = QStringLiteral("photomap");
    } else if (!poi.isEmpty() && waypoints.size() <= 1) {
        inferredType = QStringLiteral("map3dPoi");
    } else {
        inferredType = QStringLiteral("waypointRoute");
    }

    const bool currentAcceptsPolygon = m_missionType == QStringLiteral("photomap")
        || m_missionType == QStringLiteral("map3dArea")
        || m_missionType == QStringLiteral("towerInspection")
        || m_missionType == QStringLiteral("virtualFence");
    const bool currentAcceptsRoute = m_missionType == QStringLiteral("waypointRoute");
    const bool currentAcceptsPoi = m_missionType == QStringLiteral("map3dPoi");
    const bool currentCompatible = (!polygon.isEmpty() && currentAcceptsPolygon)
        || (!waypoints.isEmpty() && polygon.isEmpty() && poi.isEmpty() && currentAcceptsRoute)
        || (!poi.isEmpty() && currentAcceptsPoi);
    if (m_missionType.isEmpty() || !currentCompatible) {
        m_missionType = inferredType;
    }

    if (!waypoints.isEmpty()) {
        m_waypoints = waypoints;
    }
    if (!polygon.isEmpty()) {
        m_polygon = polygon;
    }
    if (!poi.isEmpty()) {
        m_poi = poi;
    }

    m_importSummary = QStringLiteral("%1 import: %2 waypoint(s), %3 boundary point(s), %4 POI")
                          .arg(sourceLabel)
                          .arg(m_waypoints.size())
                          .arg(m_polygon.size())
                          .arg(hasPoi() ? 1 : 0);
    m_importStatus = QStringLiteral("%1 imported and ready for preview").arg(sourceLabel);
    setOperationStatus(m_importSummary);
    markDirty();
}

QVariantList MissionPlanModel::generatedSurveyRoute() const
{
    if (m_polygon.size() < 3) {
        return {};
    }
    return clippedLaneRoute(m_polygon);
}

QVariantList MissionPlanModel::clippedLaneRoute(const QVariantList &polygon) const
{
    QVariantList route;
    double minLat = 90.0;
    double maxLat = -90.0;
    double minLon = 180.0;
    double maxLon = -180.0;
    for (const auto &pointValue : polygon) {
        const QVariantMap point = pointValue.toMap();
        const double lat = point.value(QStringLiteral("latitude")).toDouble();
        const double lon = point.value(QStringLiteral("longitude")).toDouble();
        minLat = std::min(minLat, lat);
        maxLat = std::max(maxLat, lat);
        minLon = std::min(minLon, lon);
        maxLon = std::max(maxLon, lon);
    }
    const int lanes = std::clamp(3 + (100 - m_sideOverlap) / 6, 3, 14);
    for (int i = 0; i < lanes; ++i) {
        const double t = lanes == 1 ? 0.5 : static_cast<double>(i) / (lanes - 1);
        const double lat = minLat + (maxLat - minLat) * t;
        const int samples = 40;
        bool inSegment = false;
        double segmentStartLon = minLon;
        QVariantList segments;
        for (int s = 0; s <= samples; ++s) {
            const double u = static_cast<double>(s) / samples;
            const double lon = minLon + (maxLon - minLon) * u;
            const bool inside = pointInPolygon(lat, lon, polygon);
            if (inside && !inSegment) {
                segmentStartLon = lon;
            }
            if ((!inside || s == samples) && inSegment) {
                const double endLon = inside ? lon : minLon + (maxLon - minLon) * static_cast<double>(std::max(0, s - 1)) / samples;
                segments << QVariantMap{{QStringLiteral("start"), segmentStartLon}, {QStringLiteral("end"), endLon}};
            }
            inSegment = inside;
        }
        for (const QVariant &segmentValue : segments) {
            const QVariantMap segment = segmentValue.toMap();
            const double startLon = segment.value(QStringLiteral("start")).toDouble();
            const double endLon = segment.value(QStringLiteral("end")).toDouble();
            if (std::abs(endLon - startLon) < 0.000001) {
                continue;
            }
            const bool reverse = (route.size() / 2) % 2 == 1;
            route << QVariantMap{{"latitude", lat}, {"longitude", reverse ? endLon : startLon}, {"altitude", m_altitude}, {"speed", m_speed}, {"heading", 0.0}};
            route << QVariantMap{{"latitude", lat}, {"longitude", reverse ? startLon : endLon}, {"altitude", m_altitude}, {"speed", m_speed}, {"heading", 0.0}};
        }
    }
    return route;
}

bool MissionPlanModel::pointInPolygon(double latitude, double longitude, const QVariantList &polygon)
{
    bool inside = false;
    for (int i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        const QVariantMap pi = polygon.at(i).toMap();
        const QVariantMap pj = polygon.at(j).toMap();
        const double yi = pi.value(QStringLiteral("latitude")).toDouble();
        const double yj = pj.value(QStringLiteral("latitude")).toDouble();
        const double xi = pi.value(QStringLiteral("longitude")).toDouble();
        const double xj = pj.value(QStringLiteral("longitude")).toDouble();
        const double denom = (yj - yi) == 0.0 ? 0.0000001 : (yj - yi);
        const bool intersects = ((yi > latitude) != (yj > latitude))
            && (longitude < (xj - xi) * (latitude - yi) / denom + xi);
        if (intersects) {
            inside = !inside;
        }
    }
    return inside;
}

QVariantList MissionPlanModel::generatedOrbitRoute() const
{
    QVariantList route;
    if (!hasPoi()) {
        return route;
    }
    const double centerLat = m_poi.value(QStringLiteral("latitude")).toDouble();
    const double centerLon = m_poi.value(QStringLiteral("longitude")).toDouble();
    const double latMeters = 111320.0;
    const double lonMeters = 111320.0 * std::cos(centerLat * M_PI / 180.0);
    const int points = 16;
    for (int i = 0; i <= points; ++i) {
        const double angle = (2.0 * M_PI * i) / points;
        route << QVariantMap{
            {QStringLiteral("latitude"), centerLat + std::sin(angle) * m_radius / latMeters},
            {QStringLiteral("longitude"), centerLon + std::cos(angle) * m_radius / lonMeters},
            {QStringLiteral("altitude"), m_altitude},
            {QStringLiteral("speed"), m_speed},
            {QStringLiteral("heading"), std::fmod(angle * 180.0 / M_PI + 180.0, 360.0)}
        };
    }
    return route;
}

double MissionPlanModel::distanceMeters(const QVariantMap &a, const QVariantMap &b)
{
    const double lat1 = a.value(QStringLiteral("latitude")).toDouble() * M_PI / 180.0;
    const double lat2 = b.value(QStringLiteral("latitude")).toDouble() * M_PI / 180.0;
    const double dLat = lat2 - lat1;
    const double dLon = (b.value(QStringLiteral("longitude")).toDouble() - a.value(QStringLiteral("longitude")).toDouble()) * M_PI / 180.0;
    const double h = std::sin(dLat / 2.0) * std::sin(dLat / 2.0)
        + std::cos(lat1) * std::cos(lat2) * std::sin(dLon / 2.0) * std::sin(dLon / 2.0);
    return 6371000.0 * 2.0 * std::atan2(std::sqrt(h), std::sqrt(1.0 - h));
}

void MissionPlanModel::setOperationStatus(const QString &status)
{
    if (m_operationStatus == status) {
        return;
    }
    m_operationStatus = status;
    emit operationStatusChanged();
}
