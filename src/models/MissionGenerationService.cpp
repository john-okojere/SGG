#include "MissionGenerationService.h"

#include <QtMath>

#include <algorithm>
#include <cmath>
#include <limits>

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

double normalizeAngle(double degrees)
{
    double angle = std::fmod(degrees, 360.0);
    if (angle < 0.0) {
        angle += 360.0;
    }
    return angle;
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
    } else if (request.missionType == QStringLiteral("photomap")) {
        result.route = generatePhotomapRoute(request);
    } else if (request.missionType == QStringLiteral("map3dArea")
               || request.missionType == QStringLiteral("towerInspection")) {
        result.route = generateSurveyRoute(request);
    }

    result.route = withTakeoffPoint(request, result.route);

    double routeMeters = 0.0;
    for (int i = 1; i < result.route.size(); ++i) {
        routeMeters += distanceMeters(result.route.at(i - 1).toMap(), result.route.at(i).toMap());
    }
    const Footprint footprint = cameraFootprint(request);
    const double laneSpacing = std::clamp(footprint.widthM * (1.0 - std::clamp(request.sideOverlap / 100.0, 0.05, 0.95)), 1.5, 250.0);
    const double shotSpacing = std::clamp(footprint.lengthM * (1.0 - std::clamp(request.frontOverlap / 100.0, 0.05, 0.95)), 1.5, 250.0);
    int estimatedPhotos = 0;
    for (const QVariant &value : result.route) {
        const QVariantMap point = value.toMap();
        if (point.value(QStringLiteral("capture_point"), false).toBool()) {
            ++estimatedPhotos;
        }
    }
    if (estimatedPhotos == 0) {
        estimatedPhotos = shotSpacing > 0.1 ? static_cast<int>(std::ceil(routeMeters / shotSpacing)) : 0;
    }
    const int durationSeconds = routeMeters > 0.1 ? static_cast<int>(std::ceil(routeMeters / std::max(1.0, request.speed))) : 0;
    const double areaLoad = request.polygon.size() * 0.4 + request.frontOverlap * 0.04 + request.sideOverlap * 0.04 + estimatedPhotos * 0.006;
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
        {QStringLiteral("gsd_cm_px"), footprint.gsdCmPx},
        {QStringLiteral("generated_from"), request.missionType}
    };
    result.cameraPreview = QVariantMap{
        {QStringLiteral("camera_model"), request.cameraModel},
        {QStringLiteral("capture_mode"), request.captureMode},
        {QStringLiteral("gimbal_pitch"), request.gimbalPitch},
        {QStringLiteral("footprint_width_m"), footprint.widthM},
        {QStringLiteral("footprint_length_m"), footprint.lengthM},
        {QStringLiteral("gsd_cm_px"), footprint.gsdCmPx},
        {QStringLiteral("front_overlap"), request.frontOverlap},
        {QStringLiteral("side_overlap"), request.sideOverlap},
        {QStringLiteral("course_angle"), request.courseAngle},
        {QStringLiteral("margin_m"), request.margin},
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

MissionGenerationService::CameraProfile MissionGenerationService::cameraProfile(const QString &cameraModel)
{
    const QString normalized = cameraModel.trimmed().toLower();
    if (normalized.contains(QStringLiteral("mavic 3e"))) {
        return CameraProfile{17.3, 13.0, 12.29, 5280.0};
    }
    if (normalized.contains(QStringLiteral("x5s 70"))) {
        return CameraProfile{17.3, 13.0, 70.0, 5280.0};
    }
    if (normalized.contains(QStringLiteral("x5s"))) {
        return CameraProfile{17.3, 13.0, 15.0, 5280.0};
    }
    return CameraProfile{13.2, 8.8, 8.8, 5472.0};
}

MissionGenerationService::Footprint MissionGenerationService::cameraFootprint(const Request &request)
{
    const CameraProfile profile = cameraProfile(request.cameraModel);
    const double altitude = std::max(1.0, std::min(request.altitude, request.maxAltitude));
    const double width = altitude * profile.sensorWidthMm / std::max(1.0, profile.focalLengthMm);
    const double length = altitude * profile.sensorHeightMm / std::max(1.0, profile.focalLengthMm);
    const double gsd = (width / std::max(1.0, profile.imageWidthPx)) * 100.0;
    return Footprint{std::max(1.0, width), std::max(1.0, length), std::clamp(gsd, 0.1, 50.0)};
}

QVariantList MissionGenerationService::generatePhotomapRoute(const Request &request)
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

    const double latScale = 111320.0;
    const double lonScale = std::max(1.0, 111320.0 * std::cos(qDegreesToRadians(originLat)));
    const double angle = qDegreesToRadians(request.courseAngle);
    const double ca = std::cos(angle);
    const double sa = std::sin(angle);

    QVector<Point> rotated;
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (const QVariant &value : request.polygon) {
        const QVariantMap point = value.toMap();
        const double east = (valueOf(point, QStringLiteral("longitude")) - originLon) * lonScale;
        const double north = (valueOf(point, QStringLiteral("latitude")) - originLat) * latScale;
        const double x = east * ca + north * sa;
        const double y = -east * sa + north * ca;
        rotated << Point{x, y};
    }

    const Footprint footprint = cameraFootprint(request);
    const double laneSpacing = std::clamp(footprint.widthM * (1.0 - std::clamp(request.sideOverlap / 100.0, 0.05, 0.95)), 1.5, 250.0);
    const double shotSpacing = std::clamp(footprint.lengthM * (1.0 - std::clamp(request.frontOverlap / 100.0, 0.05, 0.95)), 1.5, 250.0);

    Point center;
    for (const Point &point : rotated) {
        center.x += point.x;
        center.y += point.y;
    }
    const double vertexCount = static_cast<double>(std::max<qsizetype>(1, rotated.size()));
    center.x /= vertexCount;
    center.y /= vertexCount;

    auto workingPolygonForMargin = [&](double requestedMargin) {
        QVector<Point> working;
        double smallestRadius = std::numeric_limits<double>::max();
        for (const Point &point : rotated) {
            smallestRadius = std::min(smallestRadius, std::hypot(point.x - center.x, point.y - center.y));
        }
        const double effectiveMargin = std::clamp(requestedMargin, 0.0, std::max(0.0, smallestRadius - laneSpacing * 0.5));
        working.reserve(rotated.size());
        for (const Point &point : rotated) {
            const double dx = point.x - center.x;
            const double dy = point.y - center.y;
            const double length = std::hypot(dx, dy);
            if (effectiveMargin <= 0.001 || length <= 0.001) {
                working << point;
                continue;
            }
            const double scale = std::max(0.05, (length - effectiveMargin) / length);
            working << Point{center.x + dx * scale, center.y + dy * scale};
        }
        return working;
    };

    QVector<Point> working = workingPolygonForMargin(request.margin);
    for (const Point &point : working) {
        minX = std::min(minX, point.x);
        maxX = std::max(maxX, point.x);
        minY = std::min(minY, point.y);
        maxY = std::max(maxY, point.y);
    }
    if (maxX <= minX || maxY <= minY) {
        return route;
    }

    const auto appendPoint = [&](double rx, double ry, double heading, int laneIndex, int captureIndex) {
        const double east = rx * ca - ry * sa;
        const double north = rx * sa + ry * ca;
        const double lat = originLat + north / latScale;
        const double lon = originLon + east / lonScale;
        QVariantMap point = routePoint(lat, lon, request, heading);
        point[QStringLiteral("lane_index")] = laneIndex;
        point[QStringLiteral("capture_index")] = captureIndex;
        point[QStringLiteral("capture_point")] = true;
        point[QStringLiteral("gsd_cm_px")] = footprint.gsdCmPx;
        point[QStringLiteral("footprint_width_m")] = footprint.widthM;
        point[QStringLiteral("footprint_length_m")] = footprint.lengthM;
        route << point;
    };

    auto clippedSegmentsAtY = [](double y, const QVector<Point> &polygon) {
        QVector<QPair<double, double>> segments;
        QVector<double> intersections;
        for (int i = 0; i < polygon.size(); ++i) {
            const Point a = polygon.at(i);
            const Point b = polygon.at((i + 1) % polygon.size());
            if (std::abs(a.y - b.y) < 0.000001) {
                continue;
            }
            const bool crosses = (a.y <= y && b.y > y) || (b.y <= y && a.y > y);
            if (!crosses) {
                continue;
            }
            const double t = (y - a.y) / (b.y - a.y);
            intersections << (a.x + t * (b.x - a.x));
        }
        std::sort(intersections.begin(), intersections.end());
        for (int i = 0; i + 1 < intersections.size(); i += 2) {
            const double startX = intersections.at(i);
            const double endX = intersections.at(i + 1);
            if (endX - startX > 1.0) {
                segments << qMakePair(startX, endX);
            }
        }
        return segments;
    };

    QVector<double> scanYValues;
    const double epsilon = std::max(0.05, std::min(0.5, laneSpacing * 0.02));
    const double firstY = minY + epsilon;
    const double lastY = maxY - epsilon;
    if (lastY <= firstY) {
        scanYValues << (minY + maxY) * 0.5;
    } else {
        for (double y = firstY; y <= lastY + epsilon; y += laneSpacing) {
            scanYValues << std::min(y, lastY);
        }
        if (scanYValues.isEmpty() || std::abs(scanYValues.last() - lastY) > laneSpacing * 0.35) {
            scanYValues << lastY;
        }
    }

    int laneIndex = 0;
    for (double y : scanYValues) {
        const QVector<QPair<double, double>> segments = clippedSegmentsAtY(y, working);
        for (const auto &segment : segments) {
            const double startX = segment.first;
            const double endX = segment.second;
            const bool reverseLane = laneIndex % 2 == 1;
            const double heading = normalizeAngle(request.courseAngle + (reverseLane ? 270.0 : 90.0));
            const double laneLength = endX - startX;
            const int intervals = std::max(1, static_cast<int>(std::ceil(laneLength / shotSpacing)));
            for (int i = 0; i <= intervals; ++i) {
                const double t = intervals == 0 ? 0.0 : static_cast<double>(i) / intervals;
                const double xForward = startX + laneLength * t;
                const double x = reverseLane ? endX - laneLength * t : xForward;
                appendPoint(x, y, heading, laneIndex, i);
            }
            ++laneIndex;
        }
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
