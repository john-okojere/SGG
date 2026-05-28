#pragma once

#include <QObject>
#include <QJsonValue>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

class MissionPlanModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString missionId READ missionId NOTIFY planChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY planChanged)
    Q_PROPERTY(QString missionType READ missionType WRITE setMissionType NOTIFY planChanged)
    Q_PROPERTY(QString selectedAircraftId READ selectedAircraftId NOTIFY planChanged)
    Q_PROPERTY(QString syncState READ syncState NOTIFY planChanged)
    Q_PROPERTY(QString missionState READ missionState NOTIFY planChanged)
    Q_PROPERTY(QString uploadState READ uploadState NOTIFY planChanged)
    Q_PROPERTY(QString executionState READ executionState NOTIFY planChanged)
    Q_PROPERTY(int activeWaypointIndex READ activeWaypointIndex NOTIFY planChanged)
    Q_PROPERTY(int progressPercent READ progressPercent NOTIFY planChanged)
    Q_PROPERTY(bool createdLocally READ createdLocally NOTIFY planChanged)
    Q_PROPERTY(bool dirty READ dirty NOTIFY planChanged)
    Q_PROPERTY(bool hasPoi READ hasPoi NOTIFY planChanged)
    Q_PROPERTY(double speed READ speed WRITE setSpeed NOTIFY planChanged)
    Q_PROPERTY(double altitude READ altitude WRITE setAltitude NOTIFY planChanged)
    Q_PROPERTY(double captureInterval READ captureInterval WRITE setCaptureInterval NOTIFY planChanged)
    Q_PROPERTY(double gsd READ gsd WRITE setGsd NOTIFY planChanged)
    Q_PROPERTY(int frontOverlap READ frontOverlap WRITE setFrontOverlap NOTIFY planChanged)
    Q_PROPERTY(int sideOverlap READ sideOverlap WRITE setSideOverlap NOTIFY planChanged)
    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY planChanged)
    Q_PROPERTY(double buildingRadius READ buildingRadius WRITE setBuildingRadius NOTIFY planChanged)
    Q_PROPERTY(double safeMargin READ safeMargin WRITE setSafeMargin NOTIFY planChanged)
    Q_PROPERTY(double minAltitude READ minAltitude WRITE setMinAltitude NOTIFY planChanged)
    Q_PROPERTY(double maxAltitude READ maxAltitude WRITE setMaxAltitude NOTIFY planChanged)
    Q_PROPERTY(double courseAngle READ courseAngle WRITE setCourseAngle NOTIFY planChanged)
    Q_PROPERTY(double margin READ margin WRITE setMargin NOTIFY planChanged)
    Q_PROPERTY(double gimbalPitch READ gimbalPitch WRITE setGimbalPitch NOTIFY planChanged)
    Q_PROPERTY(QString cameraModel READ cameraModel WRITE setCameraModel NOTIFY planChanged)
    Q_PROPERTY(QString shootingAngle READ shootingAngle WRITE setShootingAngle NOTIFY planChanged)
    Q_PROPERTY(QString captureMode READ captureMode WRITE setCaptureMode NOTIFY planChanged)
    Q_PROPERTY(QString flightCourseMode READ flightCourseMode WRITE setFlightCourseMode NOTIFY planChanged)
    Q_PROPERTY(QString insideMode READ insideMode WRITE setInsideMode NOTIFY planChanged)
    Q_PROPERTY(QString headingMode READ headingMode WRITE setHeadingMode NOTIFY planChanged)
    Q_PROPERTY(QString aircraftRotation READ aircraftRotation WRITE setAircraftRotation NOTIFY planChanged)
    Q_PROPERTY(QString corneringStyle READ corneringStyle WRITE setCorneringStyle NOTIFY planChanged)
    Q_PROPERTY(QString finishAction READ finishAction WRITE setFinishAction NOTIFY planChanged)
    Q_PROPERTY(QString fenceShape READ fenceShape WRITE setFenceShape NOTIFY planChanged)
    Q_PROPERTY(QString boundaryType READ boundaryType WRITE setBoundaryType NOTIFY planChanged)
    Q_PROPERTY(QString breachAction READ breachAction WRITE setBreachAction NOTIFY planChanged)
    Q_PROPERTY(QString warningAction READ warningAction WRITE setWarningAction NOTIFY planChanged)
    Q_PROPERTY(QString flightDirection READ flightDirection WRITE setFlightDirection NOTIFY planChanged)
    Q_PROPERTY(QVariantList waypoints READ waypoints NOTIFY planChanged)
    Q_PROPERTY(QVariantMap takeoffPoint READ takeoffPoint NOTIFY planChanged)
    Q_PROPERTY(bool hasTakeoffPoint READ hasTakeoffPoint NOTIFY planChanged)
    Q_PROPERTY(double routeDistanceKm READ routeDistanceKm NOTIFY planChanged)
    Q_PROPERTY(QVariantList polygon READ polygon NOTIFY planChanged)
    Q_PROPERTY(QVariantMap poi READ poi NOTIFY planChanged)
    Q_PROPERTY(double primaryLatitude READ primaryLatitude WRITE setPrimaryLatitude NOTIFY planChanged)
    Q_PROPERTY(double primaryLongitude READ primaryLongitude WRITE setPrimaryLongitude NOTIFY planChanged)
    Q_PROPERTY(double missionAreaHa READ missionAreaHa NOTIFY planChanged)
    Q_PROPERTY(double estimatedBattery READ estimatedBattery NOTIFY planChanged)
    Q_PROPERTY(QString estimatedTime READ estimatedTime NOTIFY planChanged)
    Q_PROPERTY(int missionReadiness READ missionReadiness NOTIFY validationChanged)
    Q_PROPERTY(QVariantList validationWarnings READ validationWarnings NOTIFY planChanged)
    Q_PROPERTY(QVariantList validationChecks READ validationChecks NOTIFY validationChanged)
    Q_PROPERTY(bool validationRunning READ validationRunning NOTIFY validationChanged)
    Q_PROPERTY(bool missionReady READ missionReady NOTIFY validationChanged)
    Q_PROPERTY(QString backendValidationState READ backendValidationState NOTIFY validationChanged)
    Q_PROPERTY(bool backendSyncReady READ backendSyncReady NOTIFY validationChanged)
    Q_PROPERTY(bool backendUploadEligible READ backendUploadEligible NOTIFY validationChanged)
    Q_PROPERTY(bool boundaryOnly READ boundaryOnly NOTIFY validationChanged)
    Q_PROPERTY(QString importStatus READ importStatus NOTIFY planChanged)
    Q_PROPERTY(QString importSummary READ importSummary NOTIFY planChanged)
    Q_PROPERTY(QString operationStatus READ operationStatus NOTIFY operationStatusChanged)
    Q_PROPERTY(QVariantList generatedRoute READ generatedRoute NOTIFY geometryChanged)
    Q_PROPERTY(QVariantMap routeEstimates READ routeEstimates NOTIFY geometryChanged)
    Q_PROPERTY(QVariantMap cameraPreview READ cameraPreview NOTIFY geometryChanged)
    Q_PROPERTY(QVariantList elevationProfile READ elevationProfile NOTIFY geometryChanged)
    Q_PROPERTY(QVariantList boundaryPreview READ boundaryPreview NOTIFY geometryChanged)
    Q_PROPERTY(int generationRevision READ generationRevision NOTIFY geometryChanged)

public:
    explicit MissionPlanModel(QObject *parent = nullptr);

    QString missionId() const;
    QString name() const;
    QString missionType() const;
    QString selectedAircraftId() const;
    QString syncState() const;
    QString missionState() const;
    QString uploadState() const;
    QString executionState() const;
    int activeWaypointIndex() const;
    int progressPercent() const;
    bool createdLocally() const;
    bool dirty() const;
    bool hasPoi() const;
    double speed() const;
    double altitude() const;
    double captureInterval() const;
    double gsd() const;
    int frontOverlap() const;
    int sideOverlap() const;
    double radius() const;
    double buildingRadius() const;
    double safeMargin() const;
    double minAltitude() const;
    double maxAltitude() const;
    double courseAngle() const;
    double margin() const;
    double gimbalPitch() const;
    QString cameraModel() const;
    QString shootingAngle() const;
    QString captureMode() const;
    QString flightCourseMode() const;
    QString insideMode() const;
    QString headingMode() const;
    QString aircraftRotation() const;
    QString corneringStyle() const;
    QString finishAction() const;
    QString fenceShape() const;
    QString boundaryType() const;
    QString breachAction() const;
    QString warningAction() const;
    QString flightDirection() const;
    QVariantList waypoints() const;
    QVariantMap takeoffPoint() const;
    bool hasTakeoffPoint() const;
    double routeDistanceKm() const;
    QVariantList polygon() const;
    QVariantMap poi() const;
    double primaryLatitude() const;
    double primaryLongitude() const;
    double missionAreaHa() const;
    double estimatedBattery() const;
    QString estimatedTime() const;
    int missionReadiness() const;
    QVariantList validationWarnings() const;
    QVariantList validationChecks() const;
    bool validationRunning() const;
    bool missionReady() const;
    QString backendValidationState() const;
    bool backendSyncReady() const;
    bool backendUploadEligible() const;
    bool boundaryOnly() const;
    QString importStatus() const;
    QString importSummary() const;
    QString operationStatus() const;
    QVariantList generatedRoute() const;
    QVariantMap routeEstimates() const;
    QVariantMap cameraPreview() const;
    QVariantList elevationProfile() const;
    QVariantList boundaryPreview() const;
    int generationRevision() const;

    void setName(const QString &value);
    void setMissionType(const QString &value);
    void setSpeed(double value);
    void setAltitude(double value);
    void setCaptureInterval(double value);
    void setGsd(double value);
    void setFrontOverlap(int value);
    void setSideOverlap(int value);
    void setRadius(double value);
    void setBuildingRadius(double value);
    void setSafeMargin(double value);
    void setMinAltitude(double value);
    void setMaxAltitude(double value);
    void setCourseAngle(double value);
    void setMargin(double value);
    void setGimbalPitch(double value);
    void setCameraModel(const QString &value);
    void setShootingAngle(const QString &value);
    void setCaptureMode(const QString &value);
    void setFlightCourseMode(const QString &value);
    void setInsideMode(const QString &value);
    void setHeadingMode(const QString &value);
    void setAircraftRotation(const QString &value);
    void setCorneringStyle(const QString &value);
    void setFinishAction(const QString &value);
    void setFenceShape(const QString &value);
    void setBoundaryType(const QString &value);
    void setBreachAction(const QString &value);
    void setWarningAction(const QString &value);
    void setFlightDirection(const QString &value);
    void setPrimaryLatitude(double value);
    void setPrimaryLongitude(double value);

    Q_INVOKABLE void createDraft(const QString &missionType, const QString &aircraftId);
    Q_INVOKABLE void loadMission(const QVariantMap &mission);
    Q_INVOKABLE QVariantMap serializeForBackend() const;
    Q_INVOKABLE QVariantList serializeForMavsdkMission() const;
    Q_INVOKABLE bool validateForUpload(bool aircraftConnected, bool aircraftReady);
    Q_INVOKABLE void markUploading();
    Q_INVOKABLE void markUploaded(const QString &result);
    Q_INVOKABLE void markUploadFailed(const QString &reason);
    Q_INVOKABLE void markExecuting();
    Q_INVOKABLE void markCompleted(bool success, const QString &reason);
    Q_INVOKABLE void setExecutionProgress(int activeWaypoint, int progressPercent);
    Q_INVOKABLE void addWaypoint(double latitude, double longitude);
    Q_INVOKABLE void setTakeoffPoint(double latitude, double longitude);
    Q_INVOKABLE void clearTakeoffPoint();
    Q_INVOKABLE void addPolygonVertex(double latitude, double longitude);
    Q_INVOKABLE void insertPolygonVertex(int afterIndex, double latitude, double longitude);
    Q_INVOKABLE void movePolygonVertex(int index, double latitude, double longitude);
    Q_INVOKABLE void deletePolygonVertex(int index);
    Q_INVOKABLE void moveWaypoint(int index, double latitude, double longitude);
    Q_INVOKABLE void setWaypointAltitude(int index, double altitude);
    Q_INVOKABLE void setWaypointSpeed(int index, double speed);
    Q_INVOKABLE void setWaypointHeading(int index, double heading);
    Q_INVOKABLE void setWaypointGimbalPitch(int index, double pitch);
    Q_INVOKABLE void setWaypointAction(int index, const QString &action);
    Q_INVOKABLE void setWaypointCameraMode(int index, const QString &cameraMode);
    Q_INVOKABLE void setWaypointLoiterSeconds(int index, double seconds);
    Q_INVOKABLE void setWaypointHoverSeconds(int index, double seconds);
    Q_INVOKABLE void deleteWaypoint(int index);
    Q_INVOKABLE void moveWaypointOrder(int fromIndex, int toIndex);
    Q_INVOKABLE QVariantMap waypointAt(int index) const;
    Q_INVOKABLE void setPoi(double latitude, double longitude);
    Q_INVOKABLE void setPoiRadius(double radius);
    Q_INVOKABLE void setPrimaryCoordinate(double latitude, double longitude);
    Q_INVOKABLE void undoLastGeometry();
    Q_INVOKABLE void clearDraftGeometry();
    Q_INVOKABLE void validateMission();
    Q_INVOKABLE void applyBackendValidation(const QVariantMap &validation);
    Q_INVOKABLE void saveMission();
    Q_INVOKABLE void saveLocal();
    Q_INVOKABLE void resetGeometry();
    Q_INVOKABLE QString chooseImportFile() const;
    Q_INVOKABLE bool importMissionFile(const QString &fileUrlOrPath);

signals:
    void planChanged();
    void validationChanged();
    void operationStatusChanged();
    void geometryChanged();

private:
    void resetParameters();
    void markDirty();
    void scheduleGeneration();
    void regenerateMission();
    QVariantList generatedSurveyRoute() const;
    QVariantList generatedOrbitRoute() const;
    QVariantList clippedLaneRoute(const QVariantList &polygon) const;
    bool importGeoJson(const QByteArray &data);
    bool importKml(const QByteArray &data);
    void applyImportedGeometry(const QVariantList &waypoints, const QVariantList &polygon, const QVariantMap &poi, const QString &sourceLabel);
    static QVariantMap waypointFromCoordinate(double latitude, double longitude, double altitude, double speed);
    static bool coordinateFromGeoJson(const QJsonValue &value, double *latitude, double *longitude, double *altitude);
    static double distanceMeters(const QVariantMap &a, const QVariantMap &b);
    static bool pointInPolygon(double latitude, double longitude, const QVariantList &polygon);
    void setOperationStatus(const QString &status);

    QString m_missionId;
    QString m_name = "Untitled Mission";
    QString m_missionType;
    QString m_selectedAircraftId;
    QString m_syncState = "local";
    QString m_uploadState = "not_uploaded";
    QString m_executionState = "idle";
    int m_activeWaypointIndex = -1;
    int m_progressPercent = 0;
    bool m_createdLocally = true;
    bool m_dirty = false;
    double m_speed = 120.0;
    double m_altitude = 120.1;
    double m_captureInterval = 2.0;
    double m_gsd = 2.3;
    int m_frontOverlap = 70;
    int m_sideOverlap = 70;
    double m_radius = 654.1;
    double m_buildingRadius = 135.2;
    double m_safeMargin = 10.0;
    double m_minAltitude = 101.2;
    double m_maxAltitude = 120.1;
    double m_courseAngle = 70.0;
    double m_margin = 70.0;
    double m_gimbalPitch = 45.0;
    QString m_cameraModel = "Phantom 4 Pro Camera";
    QString m_shootingAngle = "Parallel To Main Path";
    QString m_captureMode = "Capture at Equal Dist. Interval";
    QString m_flightCourseMode = "Inside Mode";
    QString m_insideMode = "Parallel";
    QString m_headingMode = "Define Per Point";
    QString m_aircraftRotation = "Auto";
    QString m_corneringStyle = "Curved";
    QString m_finishAction = "Return to Home";
    QString m_fenceShape = "Polygon";
    QString m_boundaryType = "Inclusion";
    QString m_breachAction = "Return to Home";
    QString m_warningAction = "Alert Pilot";
    QString m_flightDirection = "Clockwise";
    QVariantList m_waypoints;
    QVariantMap m_takeoffPoint;
    QVariantList m_polygon;
    QVariantMap m_poi;
    bool m_validationRunning = false;
    QString m_backendValidationState = "not_validated";
    bool m_backendSyncReady = false;
    bool m_backendUploadEligible = false;
    bool m_boundaryOnly = false;
    QString m_importStatus = "No imported file";
    QString m_importSummary;
    QString m_operationStatus = "Draft mission ready";
    QVariantList m_generatedRoute;
    QVariantMap m_routeEstimates;
    QVariantMap m_cameraPreview;
    QVariantList m_elevationProfile;
    QVariantList m_boundaryPreview;
    int m_generationRevision = 0;
    QTimer m_generationTimer;
};
