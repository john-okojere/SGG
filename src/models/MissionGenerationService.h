#pragma once

#include <QVariantList>
#include <QVariantMap>
#include <QString>
#include <QVector>

class MissionGenerationService
{
public:
    struct Request {
        QString missionType;
        QVariantList waypoints;
        QVariantMap takeoffPoint;
        QVariantList polygon;
        QVariantMap poi;
        double speed = 8.0;
        double altitude = 80.0;
        double maxAltitude = 120.0;
        double captureInterval = 2.0;
        double gsd = 2.3;
        int frontOverlap = 70;
        int sideOverlap = 70;
        double radius = 80.0;
        double buildingRadius = 30.0;
        double safeMargin = 10.0;
        double courseAngle = 0.0;
        double margin = 0.0;
        double gimbalPitch = -45.0;
        QString captureMode;
        QString flightDirection;
        QString cameraModel;
        bool boundaryOnly = false;
    };

    struct Result {
        QVariantList route;
        QVariantMap estimates;
        QVariantMap cameraPreview;
        QVariantList elevationProfile;
        QVariantList boundaryPreview;
    };

    static Result generate(const Request &request);
    static double distanceMeters(const QVariantMap &a, const QVariantMap &b);

private:
    struct Point {
        double x = 0.0;
        double y = 0.0;
    };

    static QVariantList generateSurveyRoute(const Request &request);
    static QVariantList generateOrbitRoute(const Request &request);
    static QVariantList normalizedWaypointRoute(const Request &request);
    static QVariantList withTakeoffPoint(const Request &request, QVariantList route);
    static QVariantList boundaryPreview(const Request &request);
    static QVariantMap routePoint(double latitude, double longitude, const Request &request, double heading = 0.0);
    static bool pointInPolygon(double x, double y, const QVector<Point> &polygon);
};
