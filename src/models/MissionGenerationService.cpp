#include "MissionGenerationService.h"

#include <QtMath>

#include <algorithm>
#include <cmath>

namespace {
constexpr double earthRadiusMeters = 6371000.0;

double valueOf(const QVariantMap &map, const QString &key, double fallback = 0.0)
{
    return map.value(key, fallback).toDouble();
}

bool hasCoordinate(const QVariantMap &map)
{
    return map.contains(QStringLiteral("latitude")) && map.contains(QStringLiteral("longitude"));
}

QString normalizedDirection(const QString &direction)
{
    return direction.trimmed().toLower();
}
}

MissionGenerationService::Result MissionGenerationService::generate(const Request &request)
{
    Result result;
    if (request.boundaryOnly || request.missionType == QStringLiteral("virtualFence")) {
        result.boundaryPreview = boundaryPreview(request);
    } else if (request.missionType == QStringLiteral("waypointRoute")) {
        result.route = normalizedWaypointRoute(request);
    } else if (request.missionType == QStringLiteral("map3dPoi")) {
        result.route = generateOrbitRoute(request);
    } else if (request.missionType == QStringLiteral("photomap")
               || request.missionType == QStringLiteral("map3dArea")
               || request.missionType == QStringLiteral("towerInspection")) {
        result.route = generateSurveyRoute(request);
    }

    result.route = withTakeoffPoint(request, result.route);

    double routeMeters = 0.0;
    for (int i = 1; i < result.route.size(); ++i) {
        routeMeters += distanceMeters(result.route.at(i - 1).toMap(), result.route.at(i).toMap());
    }
    const double captureFootprintLong = std::max(1.0, request.altitude * 1.35);
    const double captureFootprintWide = std::max(1.0, request.altitude * 1.80);
    const double laneSpacing = captureFootprintWide * (1.0 - std::clamp(request.sideOverlap / 100.0, 0.05, 0.95));
    const double shotSpacing = captureFootprintLong * (1.0 - std::clamp(request.frontOverlap / 100.0, 0.05, 0.95));
    const int estimatedPhotos = shotSpacing > 0.1 ? static_cast<int>(std::ceil(routeMeters / shotSpacing)) : 0;
    const int durationSeconds = routeMeters > 0.1 ? static_cast<int>(std::ceil(routeMeters / std::max(1.0, request.speed))) : 0;
    const double areaLoad = request.polygon.size() * 0.4 + request.frontOverlap * 0.04 + request.sideOverlap * 0.04;
    const double batteryEstimate = std::clamp(8.0 + routeMeters / 450.0 + areaLoad, 0.0, 96.0);

    result.estimates = QVariantMap{
        {QStringLiteral("distance_m"), routeMeters},
        {QStringLiteral("distance_km"), routeMeters / 1000.0},
        {QStringLiteral("duration_seconds"), durationSeconds},
        {QStringLiteral("duration_minutes"), static_cast<int>(std::ceil(durationSeconds / 60.0))},
        {QStringLiteral("battery_percent"), batteryEstimate},
        {QStringLiteral("route_items"), result.route.size()},
        {QStringLiteral("lane_spacing_m"), laneSpacing},
        {QStringLiteral("shot_spacing_m"), shotSpacing},
        {QStringLiteral("estimated_photos"), estimatedPhotos},
        {QStringLiteral("generated_from"), request.missionType}
    };
    result.cameraPreview = QVariantMap{
        {QStringLiteral("camera_model"), request.cameraModel},
        {QStringLiteral("capture_mode"), request.captureMode},
        {QStringLiteral("gimbal_pitch"), request.gimbalPitch},
        {QStringLiteral("footprint_width_m"), captureFootprintWide},
        {QStringLiteral("footprint_length_m"), captureFootprintLong},
        {QStringLiteral("front_overlap"), request.frontOverlap},
        {QStringLiteral("side_overlap"), request.sideOverlap},
        {QStringLiteral("flight_direction"), request.flightDirection}
    };

    double cumulative = 0.0;
    for (int i = 0; i < result.route.size(); ++i) {
        const QVariantMap point = result.route.at(i).toMap();
        if (i > 0) {
            cumulative += distanceMeters(result.route.at(i - 1).toMap(), point);
        }
        result.elevationProfile << QVariantMap{
            {QStringLiteral("index"), i},
            {QStringLiteral("distance_m"), cumulative},
            {QStringLiteral("altitude"), valueOf(point, QStringLiteral("altitude"), request.altitude)}
        };
    }
    return result;
}

QVariantList MissionGenerationService::normalizedWaypointRoute(const Request &request)
{
    QVariantList route;
    for (const QVariant &value : request.waypoints) {
        QVariantMap point = value.toMap();
        if (!hasCoordinate(point)) {
            continue;
        }
        point[QStringLiteral("altitude")] = std::min(valueOf(point, QStringLiteral("altitude"), request.altitude), request.maxAltitude);
        point[QStringLiteral("speed")] = valueOf(point, QStringLiteral("speed"), request.speed);
        point[QStringLiteral("gimbal_pitch")] = valueOf(point, QStringLiteral("gimbal_pitch"), request.gimbalPitch);
        route << point;
    }
    if (normalizedDirection(request.flightDirection).contains(QStringLiteral("counter"))) {
        std::reverse(route.begin(), route.end());
    }
    return route;
}

QVariantList MissionGenerationService::generateSurveyRoute(const Request &request)
{
    QVariantList route;
    if (request.polygon.size() < 3) {
        return route;
    }

    double originLat = 0.0;
    double originLon = 0.0;
    for (const QVariant &value : request.polygon) {
        const QVariantMap point = value.toMap();
        originLat += valueOf(point, QStringLiteral("latitude"));
        originLon += valueOf(point, QStringLiteral("longitude"));
    }
    originLat /= request.polygon.size();
    originLon /= request.polygon.size();
    const double lonScale = 111320.0 * std::cos(qDegreesToRadians(originLat));
    const double latScale = 111320.0;
    const double angle = qDegreesToRadians(request.courseAngle);
    const double ca = std::cos(angle);
    const double sa = std::sin(angle);

    QVector<Point> rotated;
    double minX = 1e12;
    double maxX = -1e12;
    double minY = 1e12;
    double maxY = -1e12;
    for (const QVariant &value : request.polygon) {
        const QVariantMap point = value.toMap();
        const double east = (valueOf(point, QStringLiteral("longitude")) - originLon) * lonScale;
        const double north = (valueOf(point, QStringLiteral("latitude")) - originLat) * latScale;
        const double x = east * ca + north * sa;
        const double y = -east * sa + north * ca;
        rotated << Point{x, y};
        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }

    const double footprintWidth = std::max(1.0, request.altitude * 1.80);
    const double laneSpacing = std::clamp(footprintWidth * (1.0 - request.sideOverlap / 100.0), 6.0, 180.0);
    const double sampleStep = std::clamp(laneSpacing / 2.0, 4.0, 45.0);
    int laneIndex = 0;
    for (double y = minY; y <= maxY + laneSpacing * 0.25; y += laneSpacing) {
        bool inSegment = false;
        double startX = minX;
        for (double x = minX; x <= maxX + sampleStep; x += sampleStep) {
            const bool inside = pointInPolygon(x, y, rotated);
            if (inside && !inSegment) {
                startX = x;
                inSegment = true;
            }
            const bool shouldClose = (!inside || x >= maxX) && inSegment;
            if (!shouldClose) {
                continue;
            }
            const double endX = inside ? std::min(x, maxX) : x - sampleStep;
            if (endX > startX + 1.0) {
                const bool reverseLane = laneIndex % 2 == 1;
                const auto appendPoint = [&](double rx, double ry) {
                    const double east = rx * ca - ry * sa;
                    const double north = rx * sa + ry * ca;
                    const double lat = originLat + north / latScale;
                    const double lon = originLon + east / lonScale;
                    const double heading = std::fmod(request.courseAngle + (reverseLane ? 270.0 : 90.0), 360.0);
                    route << routePoint(lat, lon, request, heading);
                };
                appendPoint(reverseLane ? endX : startX, y);
                appendPoint(reverseLane ? startX : endX, y);
                ++laneIndex;
            }
            inSegment = false;
        }
    }
    if (normalizedDirection(request.flightDirection).contains(QStringLiteral("counter"))) {
        std::reverse(route.begin(), route.end());
    }
    return route;
}

QVariantList MissionGenerationService::generateOrbitRoute(const Request &request)
{
    QVariantList route;
    if (!hasCoordinate(request.poi)) {
        return route;
    }
    const double centerLat = valueOf(request.poi, QStringLiteral("latitude"));
    const double centerLon = valueOf(request.poi, QStringLiteral("longitude"));
    const double latMeters = 111320.0;
    const double lonMeters = 111320.0 * std::cos(qDegreesToRadians(centerLat));
    const bool counter = normalizedDirection(request.flightDirection).contains(QStringLiteral("counter"));
    const int points = 24;
    for (int i = 0; i <= points; ++i) {
        const double t = counter ? (points - i) : i;
        const double angle = (2.0 * M_PI * t) / points;
        route << routePoint(centerLat + std::sin(angle) * request.radius / latMeters,
                            centerLon + std::cos(angle) * request.radius / lonMeters,
                            request,
                            std::fmod(qRadiansToDegrees(angle) + 180.0, 360.0));
    }
    return route;
}

QVariantList MissionGenerationService::withTakeoffPoint(const Request &request, QVariantList route)
{
    if (route.isEmpty() || request.boundaryOnly || !hasCoordinate(request.takeoffPoint)) {
        return route;
    }
    QVariantMap takeoff = request.takeoffPoint;
    takeoff[QStringLiteral("altitude")] = std::min(valueOf(takeoff, QStringLiteral("altitude"), request.altitude), request.maxAltitude);
    takeoff[QStringLiteral("speed")] = valueOf(takeoff, QStringLiteral("speed"), request.speed);
    takeoff[QStringLiteral("gimbal_pitch")] = valueOf(takeoff, QStringLiteral("gimbal_pitch"), request.gimbalPitch);
    takeoff[QStringLiteral("action")] = QStringLiteral("Takeoff");
    const QVariantMap first = route.first().toMap();
    if (distanceMeters(takeoff, first) > 0.2) {
        route.prepend(takeoff);
    }
    return route;
}

QVariantList MissionGenerationService::boundaryPreview(const Request &request)
{
    if (!hasCoordinate(request.poi) || request.safeMargin <= 0.0) {
        return request.polygon;
    }
    QVariantList route;
    const double centerLat = valueOf(request.poi, QStringLiteral("latitude"));
    const double centerLon = valueOf(request.poi, QStringLiteral("longitude"));
    const double latMeters = 111320.0;
    const double lonMeters = 111320.0 * std::cos(qDegreesToRadians(centerLat));
    for (int i = 0; i < 24; ++i) {
        const double angle = (2.0 * M_PI * i) / 24.0;
        route << QVariantMap{
            {QStringLiteral("latitude"), centerLat + std::sin(angle) * request.safeMargin / latMeters},
            {QStringLiteral("longitude"), centerLon + std::cos(angle) * request.safeMargin / lonMeters}
        };
    }
    return route;
}

QVariantMap MissionGenerationService::routePoint(double latitude, double longitude, const Request &request, double heading)
{
    return QVariantMap{
        {QStringLiteral("latitude"), latitude},
        {QStringLiteral("longitude"), longitude},
        {QStringLiteral("altitude"), std::min(request.altitude, request.maxAltitude)},
        {QStringLiteral("speed"), request.speed},
        {QStringLiteral("heading"), heading},
        {QStringLiteral("gimbal_pitch"), request.gimbalPitch},
        {QStringLiteral("camera_mode"), request.captureMode},
        {QStringLiteral("action"), QStringLiteral("No Action")}
    };
}

bool MissionGenerationService::pointInPolygon(double x, double y, const QVector<Point> &polygon)
{
    bool inside = false;
    for (int i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        const Point &pi = polygon.at(i);
        const Point &pj = polygon.at(j);
        const double denom = (pj.y - pi.y) == 0.0 ? 0.000001 : (pj.y - pi.y);
        const bool intersects = ((pi.y > y) != (pj.y > y)) && (x < (pj.x - pi.x) * (y - pi.y) / denom + pi.x);
        if (intersects) {
            inside = !inside;
        }
    }
    return inside;
}

double MissionGenerationService::distanceMeters(const QVariantMap &a, const QVariantMap &b)
{
    const double lat1 = valueOf(a, QStringLiteral("latitude")) * M_PI / 180.0;
    const double lat2 = valueOf(b, QStringLiteral("latitude")) * M_PI / 180.0;
    const double dLat = lat2 - lat1;
    const double dLon = (valueOf(b, QStringLiteral("longitude")) - valueOf(a, QStringLiteral("longitude"))) * M_PI / 180.0;
    const double h = std::sin(dLat / 2.0) * std::sin(dLat / 2.0)
        + std::cos(lat1) * std::cos(lat2) * std::sin(dLon / 2.0) * std::sin(dLon / 2.0);
    return earthRadiusMeters * 2.0 * std::atan2(std::sqrt(h), std::sqrt(1.0 - h));
}
