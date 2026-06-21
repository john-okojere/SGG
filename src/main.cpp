#include <QApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QColor>
#include <QDebug>
#include <QDir>
#include <QProcessEnvironment>
#include <QTimer>
#include <QUrl>
#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QHash>
#include <QImage>
#include <QIcon>
#include <QPalette>
#include <QQuickImageProvider>
#include <QSettings>
#include <QSize>

#include <cairo.h>
#ifdef signals
#define SKYGRID_RESTORE_QT_SIGNALS_KEYWORD
#undef signals
#endif
#include <librsvg/rsvg.h>
#ifdef SKYGRID_RESTORE_QT_SIGNALS_KEYWORD
#define signals Q_SIGNALS
#endif

#include "auth/AuthManager.h"
#include "auth/SessionManager.h"
#include "auth/TokenManager.h"
#include "access/ModuleAccessManager.h"
#include "access/PermissionManager.h"
#include "access/PermissionGuard.h"
#include "access/RoleAccessManager.h"
#include "app/AppStartupManager.h"
#include "autopilot/AutopilotToolsFacade.h"
#include "cache/LocalSyncCache.h"
#include "controllers/AppState.h"
#include "controllers/MapState.h"
#include "controllers/MapProviderController.h"
#include "controllers/MissionStore.h"
#include "controllers/ThemeController.h"
#include "flight/EventLogManager.h"
#include "flight/FlightDataManager.h"
#include "flight/FlightStatsManager.h"
#include "flight/PostMissionSummaryManager.h"
#include "flight/PreflightChecklistManager.h"
#include "flight/ManualControlManager.h"
#include "flight/UsbControllerManager.h"
#include "firmware/FirmwareUpdateManager.h"
#include "hardware/OptionalHardwareManager.h"
#include "help/HelpCenterManager.h"
#include "logs/LogAnalysisManager.h"
#include "manufacturer/ManufacturerVehicleManager.h"
#include "map/TileCacheManager.h"
#include "mission/AdvancedMissionManager.h"
#include "network/ApiClient.h"
#include "parameters/ParameterManager.h"
#include "parameters/ParameterMetadataManager.h"
#include "payload/PayloadManager.h"
#include "payload/VideoStreamManager.h"
#include "network/WebSocketClient.h"
#include "preferences/PreferencesManager.h"
#include "profile/OperatorStateManager.h"
#include "profile/ProfileManager.h"
#include "readiness/ProductionReadinessManager.h"
#include "security/AccessManager.h"
#include "security/BackendTrustManager.h"
#include "security/DeviceManager.h"
#include "security/SecureStorage.h"
#include "setup/InitialSetupManager.h"
#include "sync/FlightSessionSyncManager.h"
#include "sync/GcsEventSyncManager.h"
#include "sync/MissionPreviewManager.h"
#include "sync/MissionSyncManager.h"
#include "sync/PreferencesSyncManager.h"
#include "sync/ProfileSyncManager.h"
#include "sync/TelemetrySyncManager.h"
#include "sync/PilotActionSyncManager.h"
#include "sync/WeatherSyncManager.h"
#include "simulation/ProtocolTestManager.h"
#include "simulation/SimulationManager.h"
#include "telemetry/WeatherManager.h"
#include "telemetry/WindCheckManager.h"
#include "telemetry/WindTelemetryManager.h"
#include "vehicle/MavsdkVehicleManager.h"
#include "vehicle/HomePositionManager.h"
#include "vehicle/MissionExecutionManager.h"
#include "vehicle/MissionUploadManager.h"
#include "vehicle/MultiVehicleManager.h"
#include "vehicle/RcCalibrationManager.h"
#include "vehicle/VehicleActionManager.h"
#include "vehicle/VehicleConfigManager.h"
#include "vehicle/VehicleProfileManager.h"
#include "vehicle/VehicleTelemetryModel.h"
#include "tools/GcsToolCatalogManager.h"

class SvgIconProvider final : public QQuickImageProvider
{
public:
    SvgIconProvider()
        : QQuickImageProvider(QQuickImageProvider::Image)
    {
    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        const QString cleanId = id.section(QLatin1Char('?'), 0, 0);
        const QString cacheKey = QStringLiteral("%1@%2x%3")
            .arg(cleanId)
            .arg(requestedSize.width())
            .arg(requestedSize.height());
        const auto cached = m_cache.constFind(cacheKey);
        if (cached != m_cache.constEnd()) {
            if (size) {
                *size = cached->size();
            }
            return *cached;
        }
        const QStringList candidates{
            QStringLiteral(":/qt/qml/SkyGrid/assets/icons/%1").arg(cleanId),
            QStringLiteral(":/assets/icons/%1").arg(cleanId),
            QStringLiteral(":/qt/qml/SkyGrid/assets/%1").arg(cleanId),
            QStringLiteral(":/assets/%1").arg(cleanId)
        };

        QByteArray svgData;
        for (const QString &path : candidates) {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly)) {
                svgData = file.readAll();
                break;
            }
        }

        if (svgData.isEmpty()) {
            QImage fallback(24, 24, QImage::Format_ARGB32_Premultiplied);
            fallback.fill(Qt::transparent);
            if (size) {
                *size = fallback.size();
            }
            return fallback;
        }

        GError *error = nullptr;
        RsvgHandle *handle = rsvg_handle_new_from_data(reinterpret_cast<const guint8 *>(svgData.constData()),
                                                       static_cast<gsize>(svgData.size()),
                                                       &error);
        if (!handle) {
            if (error) {
                g_error_free(error);
            }
            QImage fallback(24, 24, QImage::Format_ARGB32_Premultiplied);
            fallback.fill(Qt::transparent);
            if (size) {
                *size = fallback.size();
            }
            return fallback;
        }

        RsvgDimensionData dimensions{};
        rsvg_handle_get_dimensions(handle, &dimensions);
        const int naturalWidth = qMax(1, dimensions.width);
        const int naturalHeight = qMax(1, dimensions.height);
        const int targetWidth = requestedSize.width() > 0 ? requestedSize.width() : naturalWidth;
        const int targetHeight = requestedSize.height() > 0 ? requestedSize.height() : naturalHeight;

        QImage image(targetWidth, targetHeight, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);

        cairo_surface_t *surface = cairo_image_surface_create_for_data(image.bits(),
                                                                       CAIRO_FORMAT_ARGB32,
                                                                       image.width(),
                                                                       image.height(),
                                                                       image.bytesPerLine());
        cairo_t *cr = cairo_create(surface);
        cairo_scale(cr,
                    static_cast<double>(targetWidth) / static_cast<double>(naturalWidth),
                    static_cast<double>(targetHeight) / static_cast<double>(naturalHeight));
        rsvg_handle_render_cairo(handle, cr);
        cairo_destroy(cr);
        cairo_surface_flush(surface);
        cairo_surface_destroy(surface);
        g_object_unref(handle);

        if (size) {
            *size = image.size();
        }
        m_cache.insert(cacheKey, image);
        return image;
    }

private:
    QHash<QString, QImage> m_cache;
};

namespace {
void applyPortableConfigDefaults()
{
    const QString configPath = QCoreApplication::applicationDirPath() + QStringLiteral("/SkyGridGCS.ini");
    if (!QFile::exists(configPath)) {
        return;
    }

    QSettings settings(configPath, QSettings::IniFormat);
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QHash<QString, QStringList> keys{
        {QStringLiteral("SKYGRID_API_BASE_URL"),
         {QStringLiteral("SKYGRID_API_BASE_URL"),
          QStringLiteral("backend/api_base_url"),
          QStringLiteral("SkyGrid/SKYGRID_API_BASE_URL")}},
        {QStringLiteral("SKYGRID_WS_BASE_URL"),
         {QStringLiteral("SKYGRID_WS_BASE_URL"),
          QStringLiteral("backend/ws_base_url"),
          QStringLiteral("SkyGrid/SKYGRID_WS_BASE_URL")}},
        {QStringLiteral("SKYGRID_BACKEND_URL"),
         {QStringLiteral("SKYGRID_BACKEND_URL"),
          QStringLiteral("backend/backend_url"),
          QStringLiteral("SkyGrid/SKYGRID_BACKEND_URL")}},
        {QStringLiteral("SKYGRID_MAVSDK_URLS"),
         {QStringLiteral("SKYGRID_MAVSDK_URLS"),
          QStringLiteral("SkyGrid/SKYGRID_MAVSDK_URLS")}},
        {QStringLiteral("SKYGRID_MAVSDK_ALLOW_MULTIPLE_URLS"),
         {QStringLiteral("SKYGRID_MAVSDK_ALLOW_MULTIPLE_URLS"),
          QStringLiteral("SkyGrid/SKYGRID_MAVSDK_ALLOW_MULTIPLE_URLS")}},
        {QStringLiteral("SKYGRID_PERFORMANCE_MODE"),
         {QStringLiteral("SKYGRID_PERFORMANCE_MODE"),
          QStringLiteral("SkyGrid/SKYGRID_PERFORMANCE_MODE")}}
    };

    for (auto it = keys.constBegin(); it != keys.constEnd(); ++it) {
        const QString &key = it.key();
        if (env.contains(key)) {
            continue;
        }
        QVariant value;
        for (const QString &candidate : it.value()) {
            value = settings.value(candidate);
            if (value.isValid()) {
                break;
            }
        }
        const QString text = value.toString().trimmed();
        if (!text.isEmpty()) {
            qputenv(key.toUtf8(), text.toUtf8());
        }
    }
}
}

int main(int argc, char *argv[])
{
#ifdef SKYGRID_DEV_BUILD
    qputenv("DEV_BUILD", "true");
    qputenv("SKYGRID_VERBOSE_COMMS", "true");
#endif
#ifdef SKYGRID_HAS_ARCGIS
    qputenv("QSG_RENDER_LOOP", "basic");
#endif

    QApplication app(argc, argv);
    applyPortableConfigDefaults();
    QApplication::setOrganizationName("SkyGrid");
    QApplication::setApplicationName("SkyGrid GCS");
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/qt/qml/SkyGrid/assets/app_icon.png")));
    QQuickStyle::setStyle("Basic");
    QPalette palette = app.palette();
    palette.setColor(QPalette::Base, QColor(QStringLiteral("#ffffff")));
    palette.setColor(QPalette::AlternateBase, QColor(QStringLiteral("#f7f3fb")));
    palette.setColor(QPalette::Text, QColor(QStringLiteral("#050505")));
    palette.setColor(QPalette::Button, QColor(QStringLiteral("#ffffff")));
    palette.setColor(QPalette::ButtonText, QColor(QStringLiteral("#050505")));
    palette.setColor(QPalette::Highlight, QColor(QStringLiteral("#5f16c5")));
    palette.setColor(QPalette::HighlightedText, QColor(QStringLiteral("#ffffff")));
    palette.setColor(QPalette::PlaceholderText, QColor(QStringLiteral("#6f687a")));
    app.setPalette(palette);

    AppState appState;
    MissionStore missionStore;
    VehicleTelemetryModel telemetry;
    MapState mapState;
    MapProviderController mapProvider;
    TileCacheManager tileCacheManager;
    ThemeController theme;
    SecureStorage secureStorage;
    TokenManager tokenManager(&secureStorage);
    DeviceManager deviceManager(&secureStorage);
    BackendTrustManager backendTrust;
    ApiClient apiClient(&backendTrust, &tokenManager);
    RoleAccessManager roleAccessManager;
    PermissionManager permissionManager;
    ModuleAccessManager moduleAccessManager;
    AppStartupManager appStartupManager;
    AuthManager authManager(&apiClient, &tokenManager, &deviceManager);
    SessionManager sessionManager(&apiClient, &tokenManager);
    PermissionGuard permissionGuard(&permissionManager, &moduleAccessManager, &sessionManager);
    WebSocketClient webSocketClient(&backendTrust, &tokenManager);
    LocalSyncCache localSyncCache;
    ProfileManager profileManager(&localSyncCache);
    OperatorStateManager operatorStateManager(&localSyncCache);
    PreferencesManager preferencesManager(&localSyncCache);
    GcsEventSyncManager gcsEventSyncManager(&apiClient, &sessionManager, &localSyncCache);
    AccessManager accessManager(&localSyncCache);
    accessManager.setEventSyncManager(&gcsEventSyncManager);
    gcsEventSyncManager.setAccessManager(&accessManager);
    appState.setAccessManager(&accessManager);
    HelpCenterManager helpCenterManager;
    EventLogManager eventLogManager(&gcsEventSyncManager, &localSyncCache, &accessManager);
    WeatherManager weatherManager;
    WindTelemetryManager windTelemetryManager;
    WindCheckManager windCheckManager(&weatherManager, &windTelemetryManager);
    WeatherSyncManager weatherSyncManager(&apiClient, &sessionManager, &weatherManager, &localSyncCache);
    PreflightChecklistManager preflightChecklistManager(&sessionManager,
                                                        &apiClient,
                                                        &telemetry,
                                                        missionStore.plan(),
                                                        &appState,
                                                        &weatherManager,
                                                        &windCheckManager,
                                                        &gcsEventSyncManager);
    MissionSyncManager missionSyncManager(&apiClient, &sessionManager, &localSyncCache, missionStore.plan(), &permissionManager, &accessManager, &gcsEventSyncManager);
    ManufacturerVehicleManager manufacturerVehicleManager(&apiClient, &sessionManager, &accessManager, &gcsEventSyncManager);
    MissionPreviewManager missionPreviewManager(&apiClient, &sessionManager);
    FlightSessionSyncManager flightSessionSyncManager(&apiClient,
                                                      &sessionManager,
                                                      &missionSyncManager,
                                                      missionStore.plan(),
                                                      &appState,
                                                      &telemetry,
                                                      &gcsEventSyncManager,
                                                      &localSyncCache,
                                                      &accessManager);
    PilotActionSyncManager pilotActionSyncManager(&flightSessionSyncManager, &gcsEventSyncManager, &localSyncCache, &accessManager);
    ProfileSyncManager profileSyncManager(&apiClient, &sessionManager, &profileManager);
    PreferencesSyncManager preferencesSyncManager(&apiClient, &sessionManager, &preferencesManager);
    GcsToolCatalogManager gcsToolCatalog(&accessManager);
    MavsdkVehicleManager vehicleManager(&telemetry, &accessManager);
    AutopilotToolsFacade autopilotToolsFacade(&vehicleManager, &accessManager, &gcsEventSyncManager);
    FirmwareUpdateManager firmwareUpdateManager(&vehicleManager,
                                                &sessionManager,
                                                &permissionManager,
                                                &autopilotToolsFacade,
                                                &gcsEventSyncManager);
    VehicleProfileManager vehicleProfileManager(&apiClient, &sessionManager, &permissionManager);
    VehicleConfigManager vehicleConfigManager(&vehicleManager,
                                              &vehicleProfileManager,
                                              &apiClient,
                                              &sessionManager,
                                              &permissionManager,
                                              &gcsEventSyncManager);
    RcCalibrationManager rcCalibrationManager(&vehicleManager,
                                              &sessionManager,
                                              &permissionManager,
                                              &gcsEventSyncManager);
    HomePositionManager homePositionManager(&telemetry);
    AdvancedMissionManager advancedMissionManager(&vehicleManager, &accessManager, &sessionManager, &gcsEventSyncManager);
    MissionUploadManager missionUploadManager(&vehicleManager,
                                              &telemetry,
                                              missionStore.plan(),
                                              &apiClient,
                                              &sessionManager,
                                              &permissionManager,
                                              &preflightChecklistManager,
                                              &accessManager,
                                              &advancedMissionManager,
                                              &gcsEventSyncManager);
    MissionExecutionManager missionExecutionManager(&vehicleManager,
                                                    missionStore.plan(),
                                                    &apiClient,
                                                    &sessionManager,
                                                    &permissionManager,
                                                    &flightSessionSyncManager,
                                                    &preflightChecklistManager,
                                                    &accessManager,
                                                    &advancedMissionManager,
                                                    &gcsEventSyncManager);
    VehicleActionManager vehicleActionManager(&vehicleManager,
                                              &sessionManager,
                                              &permissionManager,
                                              &pilotActionSyncManager,
                                              &preflightChecklistManager,
                                              &accessManager,
                                              &gcsEventSyncManager);
    ManualControlManager manualControlManager(&vehicleManager, &sessionManager, &permissionManager, &accessManager, &gcsEventSyncManager);
    ParameterMetadataManager parameterMetadataManager;
    ParameterManager parameterManager(&vehicleManager,
                                      &parameterMetadataManager,
                                      &accessManager,
                                      &sessionManager,
                                      &missionExecutionManager,
                                      &manualControlManager,
                                      &gcsEventSyncManager);
    FlightDataManager flightDataManager(&telemetry, &vehicleManager, &accessManager, &gcsEventSyncManager);
    LogAnalysisManager logAnalysisManager(&vehicleManager,
                                          &accessManager,
                                          &sessionManager,
                                          &gcsEventSyncManager,
                                          &eventLogManager);
    SimulationManager simulationManager(&vehicleConfigManager, &vehicleManager, &accessManager, &gcsEventSyncManager);
    InitialSetupManager initialSetupManager(&vehicleManager, &accessManager, &sessionManager, &gcsEventSyncManager);
    VideoStreamManager videoStreamManager(&accessManager, &gcsEventSyncManager);
    PayloadManager payloadManager(&telemetry, &vehicleManager, &videoStreamManager, &accessManager, &gcsEventSyncManager);
    OptionalHardwareManager optionalHardwareManager(&vehicleManager, &accessManager, &gcsEventSyncManager);
    MultiVehicleManager multiVehicleManager(&vehicleManager, &telemetry, &accessManager, &gcsEventSyncManager);
    ProtocolTestManager protocolTestManager(&accessManager, &gcsEventSyncManager);
    ProductionReadinessManager productionReadinessManager(&vehicleManager,
                                                          &parameterManager,
                                                          &advancedMissionManager,
                                                          &firmwareUpdateManager,
                                                          &logAnalysisManager,
                                                          &videoStreamManager,
                                                          &sessionManager,
                                                          &accessManager,
                                                          &gcsEventSyncManager);
    UsbControllerManager usbControllerManager(&manualControlManager);
    FlightStatsManager flightStatsManager(&telemetry,
                                          &flightSessionSyncManager,
                                          &appState,
                                          missionStore.plan(),
                                          &gcsEventSyncManager,
                                          &localSyncCache,
                                          &accessManager);
    TelemetrySyncManager telemetrySyncManager(&apiClient,
                                              &sessionManager,
                                              &missionSyncManager,
                                              &telemetry,
                                              missionStore.plan(),
                                              &appState,
                                              &manualControlManager,
                                              &windTelemetryManager,
                                              &flightStatsManager,
                                              &homePositionManager,
                                              &flightSessionSyncManager,
                                              &webSocketClient,
                                              &permissionManager,
                                              &localSyncCache,
                                              &accessManager);
    PostMissionSummaryManager postMissionSummaryManager(&apiClient,
                                                        &sessionManager,
                                                        &localSyncCache,
                                                        missionStore.plan(),
                                                        &flightStatsManager,
                                                        &flightSessionSyncManager,
                                                        &telemetrySyncManager,
                                                        &missionSyncManager,
                                                        &profileManager,
                                                        &telemetry,
                                                        &accessManager);
    QObject::connect(&missionExecutionManager,
                     &MissionExecutionManager::missionFinished,
                     &postMissionSummaryManager,
                     &PostMissionSummaryManager::handleMissionFinished);
    QObject::connect(&postMissionSummaryManager,
                     &PostMissionSummaryManager::returnToDashboardRequested,
                     &appState,
                     &AppState::goHome);
    QObject::connect(&postMissionSummaryManager,
                     &PostMissionSummaryManager::startNewMissionRequested,
                     &appState,
                     &AppState::openMissionSelector);

    QQmlApplicationEngine engine;
    engine.addImageProvider(QStringLiteral("svgicon"), new SvgIconProvider);
#ifdef SKYGRID_HAS_ARCGIS_QML
    const QString arcGisRuntimePath = QProcessEnvironment::systemEnvironment()
                                          .value(QStringLiteral("ARCGIS_RUNTIME_QT_PATH"))
                                          .trimmed();
    const QStringList arcGisImportCandidates{
        arcGisRuntimePath + QStringLiteral("/qml"),
        arcGisRuntimePath + QStringLiteral("/sdk/qml"),
        arcGisRuntimePath + QStringLiteral("/Resources/qml")
    };
    for (const QString &importPath : arcGisImportCandidates) {
        if (QDir(importPath + QStringLiteral("/Esri/ArcGISRuntime")).exists()) {
            engine.addImportPath(importPath);
            break;
        }
    }
#endif

    const QString assetBase = QStringLiteral("qrc:/qt/qml/SkyGrid/assets/");
    const QString performanceMode = QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("SKYGRID_PERFORMANCE_MODE"), QStringLiteral("balanced"))
        .toLower();
    const bool rbacDiagnosticsEnabled = QProcessEnvironment::systemEnvironment()
                                            .value(QStringLiteral("SKYGRID_RBAC_DEBUG")) == QStringLiteral("true")
        || QProcessEnvironment::systemEnvironment().value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true");
    const auto asset = [&assetBase](const QString &fileName) {
        return assetBase + fileName;
    };
    const auto icon = [&assetBase](const QString &fileName) {
        return assetBase + QStringLiteral("icons/") + fileName;
    };
    const auto svg = [](const QString &fileName) {
        return QStringLiteral("image://svgicon/") + fileName;
    };
    QVariantMap logos{
        {QStringLiteral("app_icon"), asset(QStringLiteral("app_icon.png"))},
        {QStringLiteral("full_logo"), svg(QStringLiteral("full_logo.svg"))},
        {QStringLiteral("logo"), asset(QStringLiteral("logo.png"))},
        {QStringLiteral("icon"), asset(QStringLiteral("icon.png"))}
    };
    QVariantMap dashboardAssets{
        {QStringLiteral("mission_operations_center_bg"), asset(QStringLiteral("dashboard/mission_operations_center_bg.png"))}
    };
    QVariantMap aircraftAssets{
        {QStringLiteral("x8_01"), asset(QStringLiteral("aircraft/SkyGrid_X8-01.png"))},
        {QStringLiteral("vtol_03"), asset(QStringLiteral("aircraft/SkyGrid_VTOL-03.png"))}
    };
    QVariantMap weatherAssets{
        {QStringLiteral("widgets"), asset(QStringLiteral("weather/weather_widgets.png"))},
        {QStringLiteral("wind_compass_arrow"), asset(QStringLiteral("weather/weather_widgets.png"))},
        {QStringLiteral("badge_good"), asset(QStringLiteral("weather/weather_widgets.png"))},
        {QStringLiteral("badge_caution"), asset(QStringLiteral("weather/weather_widgets.png"))},
        {QStringLiteral("badge_dnf"), asset(QStringLiteral("weather/weather_widgets.png"))}
    };
    QVariantMap sidebarAssets{
        {QStringLiteral("widgets"), asset(QStringLiteral("sidebar/sidebar_badges.png"))},
        {QStringLiteral("tab_mission"), asset(QStringLiteral("sidebar/sidebar_badges.png"))},
        {QStringLiteral("tab_material"), asset(QStringLiteral("sidebar/sidebar_badges.png"))},
        {QStringLiteral("tab_map"), asset(QStringLiteral("sidebar/sidebar_badges.png"))}
    };
    QVariantMap missionBadgeAssets{
        {QStringLiteral("widgets"), asset(QStringLiteral("mission_badges/mission_badges.png"))},
        {QStringLiteral("surveillance"), asset(QStringLiteral("mission_badges/mission_badges.png"))},
        {QStringLiteral("logistics"), asset(QStringLiteral("mission_badges/mission_badges.png"))},
        {QStringLiteral("recon"), asset(QStringLiteral("mission_badges/mission_badges.png"))},
        {QStringLiteral("training"), asset(QStringLiteral("mission_badges/mission_badges.png"))}
    };
    QVariantMap icons{
        {QStringLiteral("arrow"), icon(QStringLiteral("arrow.svg"))},
        {QStringLiteral("boxicons_battery_3"), icon(QStringLiteral("boxicons_battery-3.svg"))},
        {QStringLiteral("boxicons_camera"), icon(QStringLiteral("boxicons_camera.svg"))},
        {QStringLiteral("boxicons_cursor_pointer"), icon(QStringLiteral("boxicons_cursor-pointer.svg"))},
        {QStringLiteral("boxicons_filter"), icon(QStringLiteral("boxicons_filter.svg"))},
        {QStringLiteral("boxicons_search_big"), icon(QStringLiteral("boxicons_search-big.svg"))},
        {QStringLiteral("boxicons_git_branch"), icon(QStringLiteral("boxicons_git-branch.svg"))},
        {QStringLiteral("boxicons_play"), icon(QStringLiteral("boxicons_play.svg"))},
        {QStringLiteral("boxicons_save"), icon(QStringLiteral("boxicons_save.svg"))},
        {QStringLiteral("boxicons_trash"), icon(QStringLiteral("boxicons_trash.svg"))},
        {QStringLiteral("boxicons_undo"), icon(QStringLiteral("boxicons_undo.svg"))},
        {QStringLiteral("boxicons_wifi"), icon(QStringLiteral("boxicons_wifi.svg"))},
        {QStringLiteral("boxicons_wind_filled"), icon(QStringLiteral("boxicons_wind-filled.svg"))},
        {QStringLiteral("cloud"), icon(QStringLiteral("cloud.svg"))},
        {QStringLiteral("compass_border"), icon(QStringLiteral("compass_border.svg"))},
        {QStringLiteral("cube"), icon(QStringLiteral("cube.svg"))},
        {QStringLiteral("divider_lines"), icon(QStringLiteral("divider_lines.svg"))},
        {QStringLiteral("edit"), icon(QStringLiteral("edit.svg"))},
        {QStringLiteral("iconoir_arrow_up_2"), icon(QStringLiteral("iconamoon_arrow-up-2.svg"))},
        {QStringLiteral("iconamoon_arrow_up_2"), icon(QStringLiteral("iconamoon_arrow-up-2.svg"))},
        {QStringLiteral("iconoir_cancel"), icon(QStringLiteral("iconoir_cancel.svg"))},
        {QStringLiteral("lucide_building_2"), icon(QStringLiteral("lucide_building-2.svg"))},
        {QStringLiteral("lucide_grid_3x3"), icon(QStringLiteral("lucide_grid-3x3.svg"))},
        {QStringLiteral("lucide_mountain"), icon(QStringLiteral("lucide_mountain.svg"))},
        {QStringLiteral("lucide_route"), icon(QStringLiteral("lucide_route.svg"))},
        {QStringLiteral("lucide_satellite"), icon(QStringLiteral("lucide_satellite.svg"))},
        {QStringLiteral("lucide_waypoints"), icon(QStringLiteral("lucide_waypoints.svg"))},
        {QStringLiteral("mdi_person_outline"), icon(QStringLiteral("mdi_person-outline.svg"))},
        {QStringLiteral("mdi_check_circle"), icon(QStringLiteral("mdi_check-circle.svg"))},
        {QStringLiteral("mdi_home_outline"), icon(QStringLiteral("mdi_home-outline.svg"))},
        {QStringLiteral("mingcute_compass_line"), icon(QStringLiteral("mingcute_compass-line.svg"))},
        {QStringLiteral("pin_location"), icon(QStringLiteral("pin_location.svg"))},
        {QStringLiteral("plane"), icon(QStringLiteral("plane.svg"))},
        {QStringLiteral("plus"), icon(QStringLiteral("plus.svg"))},
        {QStringLiteral("vector"), icon(QStringLiteral("Vector.svg"))},
        {QStringLiteral("Vector"), icon(QStringLiteral("Vector.svg"))},
        {QStringLiteral("waypoint"), icon(QStringLiteral("waypioint.svg"))},
        {QStringLiteral("waypioint"), icon(QStringLiteral("waypioint.svg"))}
    };
    QVariantMap assetRegistry{
        {QStringLiteral("logos"), logos},
        {QStringLiteral("dashboard"), dashboardAssets},
        {QStringLiteral("aircraft"), aircraftAssets},
        {QStringLiteral("weather"), weatherAssets},
        {QStringLiteral("sidebar"), sidebarAssets},
        {QStringLiteral("mission_badges"), missionBadgeAssets},
        {QStringLiteral("icons"), icons}
    };
    engine.rootContext()->setContextProperty("AssetRegistry", assetRegistry);
    engine.rootContext()->setContextProperty("performanceMode", performanceMode);
    engine.rootContext()->setContextProperty("rbacDiagnosticsEnabled", rbacDiagnosticsEnabled);
    engine.rootContext()->setContextProperty("Theme", &theme);
    engine.rootContext()->setContextProperty("appState", &appState);
    engine.rootContext()->setContextProperty("roleAccessManager", &roleAccessManager);
    engine.rootContext()->setContextProperty("permissionManager", &permissionManager);
    engine.rootContext()->setContextProperty("moduleAccessManager", &moduleAccessManager);
    engine.rootContext()->setContextProperty("appStartupManager", &appStartupManager);
    engine.rootContext()->setContextProperty("permissionGuard", &permissionGuard);
    engine.rootContext()->setContextProperty("authManager", &authManager);
    engine.rootContext()->setContextProperty("accessManager", &accessManager);
    engine.rootContext()->setContextProperty("helpCenterManager", &helpCenterManager);
    engine.rootContext()->setContextProperty("gcsToolCatalog", &gcsToolCatalog);
    engine.rootContext()->setContextProperty("autopilotToolsFacade", &autopilotToolsFacade);
    engine.rootContext()->setContextProperty("parameterManager", &parameterManager);
    engine.rootContext()->setContextProperty("parameterMetadataManager", &parameterMetadataManager);
    engine.rootContext()->setContextProperty("flightDataManager", &flightDataManager);
    engine.rootContext()->setContextProperty("logAnalysisManager", &logAnalysisManager);
    engine.rootContext()->setContextProperty("simulationManager", &simulationManager);
    engine.rootContext()->setContextProperty("protocolTestManager", &protocolTestManager);
    engine.rootContext()->setContextProperty("advancedMissionManager", &advancedMissionManager);
    engine.rootContext()->setContextProperty("initialSetupManager", &initialSetupManager);
    engine.rootContext()->setContextProperty("payloadManager", &payloadManager);
    engine.rootContext()->setContextProperty("videoStreamManager", &videoStreamManager);
    engine.rootContext()->setContextProperty("optionalHardwareManager", &optionalHardwareManager);
    engine.rootContext()->setContextProperty("multiVehicleManager", &multiVehicleManager);
    engine.rootContext()->setContextProperty("productionReadinessManager", &productionReadinessManager);
    engine.rootContext()->setContextProperty("backendTrustManager", &backendTrust);
    engine.rootContext()->setContextProperty("deviceManager", &deviceManager);
    engine.rootContext()->setContextProperty("localSyncCache", &localSyncCache);
    engine.rootContext()->setContextProperty("missionSyncManager", &missionSyncManager);
    engine.rootContext()->setContextProperty("manufacturerVehicleManager", &manufacturerVehicleManager);
    engine.rootContext()->setContextProperty("missionPreviewManager", &missionPreviewManager);
    engine.rootContext()->setContextProperty("missionUploadManager", &missionUploadManager);
    engine.rootContext()->setContextProperty("missionExecutionManager", &missionExecutionManager);
    engine.rootContext()->setContextProperty("vehicleActionManager", &vehicleActionManager);
    engine.rootContext()->setContextProperty("manualControlManager", &manualControlManager);
    engine.rootContext()->setContextProperty("usbControllerManager", &usbControllerManager);
    engine.rootContext()->setContextProperty("gcsEventSyncManager", &gcsEventSyncManager);
    engine.rootContext()->setContextProperty("eventLogManager", &eventLogManager);
    engine.rootContext()->setContextProperty("pilotActionSyncManager", &pilotActionSyncManager);
    engine.rootContext()->setContextProperty("weatherManager", &weatherManager);
    engine.rootContext()->setContextProperty("windCheckManager", &windCheckManager);
    engine.rootContext()->setContextProperty("preflightChecklistManager", &preflightChecklistManager);
    engine.rootContext()->setContextProperty("weatherSyncManager", &weatherSyncManager);
    engine.rootContext()->setContextProperty("windTelemetryManager", &windTelemetryManager);
    engine.rootContext()->setContextProperty("flightStatsManager", &flightStatsManager);
    engine.rootContext()->setContextProperty("postMissionSummaryManager", &postMissionSummaryManager);
    engine.rootContext()->setContextProperty("flightSessionSyncManager", &flightSessionSyncManager);
    engine.rootContext()->setContextProperty("missionStore", &missionStore);
    engine.rootContext()->setContextProperty("operatorStateManager", &operatorStateManager);
    engine.rootContext()->setContextProperty("preferencesManager", &preferencesManager);
    engine.rootContext()->setContextProperty("preferencesSyncManager", &preferencesSyncManager);
    engine.rootContext()->setContextProperty("profileManager", &profileManager);
    engine.rootContext()->setContextProperty("profileSyncManager", &profileSyncManager);
    engine.rootContext()->setContextProperty("sessionManager", &sessionManager);
    engine.rootContext()->setContextProperty("telemetryStore", &telemetry);
    engine.rootContext()->setContextProperty("telemetrySyncManager", &telemetrySyncManager);
    engine.rootContext()->setContextProperty("webSocketClient", &webSocketClient);
    engine.rootContext()->setContextProperty("mapState", &mapState);
    engine.rootContext()->setContextProperty("mapProvider", &mapProvider);
    engine.rootContext()->setContextProperty("tileCacheManager", &tileCacheManager);
    engine.rootContext()->setContextProperty("vehicleManager", &vehicleManager);
    engine.rootContext()->setContextProperty("firmwareUpdateManager", &firmwareUpdateManager);
    engine.rootContext()->setContextProperty("vehicleProfileManager", &vehicleProfileManager);
    engine.rootContext()->setContextProperty("vehicleConfigManager", &vehicleConfigManager);
    engine.rootContext()->setContextProperty("rcCalibrationManager", &rcCalibrationManager);
    engine.rootContext()->setContextProperty("homePositionManager", &homePositionManager);

    QObject::connect(&appState, &AppState::missionStarted, &missionStore, &MissionStore::startDraft);
    QObject::connect(&missionSyncManager, &MissionSyncManager::missionOpened, &appState, &AppState::openExistingMission);
    QObject::connect(&missionStore, &MissionStore::overlayInputsChanged, &mapState, &MapState::refreshOverlay);
    if (rbacDiagnosticsEnabled) {
        const auto logNavigationState = [&]() {
            qInfo().noquote()
                << "[RBAC] navigation"
                << "currentScreen=" << appState.currentScreen()
                << "operationalMode=" << appState.operationalMode()
                << "defaultWorkspace=" << moduleAccessManager.defaultWorkspace();
        };
        QObject::connect(&appState, &AppState::navigationChanged, &appState, logNavigationState);
        QObject::connect(&appState, &AppState::operationalModeChanged, &appState, logNavigationState);
    }
    QObject::connect(&authManager, &AuthManager::loginSucceeded, &sessionManager, &SessionManager::validateSession);
    QObject::connect(&authManager, &AuthManager::loginSucceeded, &appStartupManager, &AppStartupManager::checkingSession);
    QObject::connect(&authManager, &AuthManager::operationsBlocked, &sessionManager, &SessionManager::blockOperations);
    QObject::connect(&authManager, &AuthManager::operationsBlocked, &appStartupManager, &AppStartupManager::deviceBlocked);
    QObject::connect(&sessionManager, &SessionManager::forceLogout, &authManager, &AuthManager::logout);
    QObject::connect(&authManager, &AuthManager::loginSessionRecorded, &eventLogManager, [&](qint64 durationSeconds) {
        eventLogManager.logEvent(QStringLiteral("gcs_login_session_duration"),
                                 QStringLiteral("info"),
                                 QStringLiteral("GCS login session duration recorded."),
                                 QJsonObject{
                                     {QStringLiteral("duration_seconds"), static_cast<double>(durationSeconds)},
                                     {QStringLiteral("reason"), QStringLiteral("logout_or_app_close")}
                                 });
    });
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &authManager, [&]() {
        authManager.finishLoginSession();
        tokenManager.clearRuntimeSession();
    });
    QObject::connect(&authManager, &AuthManager::operationsBlocked, &roleAccessManager, &RoleAccessManager::reset);
    QObject::connect(&authManager, &AuthManager::operationsBlocked, &permissionManager, &PermissionManager::reset);
    QObject::connect(&authManager, &AuthManager::operationsBlocked, &moduleAccessManager, &ModuleAccessManager::reset);
    QObject::connect(&authManager, &AuthManager::operationsBlocked, &accessManager, &AccessManager::clearAccess);
    QObject::connect(&sessionManager, &SessionManager::forceLogout, &accessManager, &AccessManager::clearAccess);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &roleAccessManager, &RoleAccessManager::applyBootstrap);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &permissionManager, &PermissionManager::applyBootstrap);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &moduleAccessManager, &ModuleAccessManager::applyBootstrap);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &accessManager, &AccessManager::applyBootstrap);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &profileSyncManager, &ProfileSyncManager::applyBootstrap);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &preferencesSyncManager, &PreferencesSyncManager::applyBootstrap);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &operatorStateManager, &OperatorStateManager::applyBootstrap);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &flightSessionSyncManager, &FlightSessionSyncManager::applyBootstrap);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &vehicleProfileManager, &VehicleProfileManager::applyBootstrap);
    QObject::connect(&missionSyncManager, &MissionSyncManager::bootstrapReceived, &appStartupManager, [&]() {
        appStartupManager.accessReady(moduleAccessManager.defaultWorkspace());
    });
    QObject::connect(&missionSyncManager, &MissionSyncManager::syncChanged, &appStartupManager, [&]() {
        if (missionSyncManager.syncing()) {
            appStartupManager.backgroundSyncStarted();
        } else if (appStartupManager.startupComplete()) {
            appStartupManager.backgroundSyncFinished(missionSyncManager.status());
        }
    });
    const auto enforceWorkspaceAccess = [&]() {
        const QString current = appState.currentScreen();
        if ((current == QStringLiteral("home") || current == QStringLiteral("missionSelector"))
            && moduleAccessManager.defaultWorkspace() != QStringLiteral("home")) {
            appState.openDefaultWorkspace(moduleAccessManager.defaultWorkspace());
            return;
        }
        if (current == QStringLiteral("vehicleConfiguration")
            && !moduleAccessManager.vehicleConfigurationWorkspaceAllowed()) {
            appState.openDefaultWorkspace(moduleAccessManager.defaultWorkspace());
            return;
        }
        if (current == QStringLiteral("manufacturerTestFlight")
            && !moduleAccessManager.manufacturerTestFlightWorkspaceAllowed()) {
            appState.openDefaultWorkspace(moduleAccessManager.defaultWorkspace());
            return;
        }
        if (current == QStringLiteral("planner")
            && appState.operationalMode() == QStringLiteral("pilot")
            && !moduleAccessManager.pilotWorkspaceAllowed()) {
            appState.openDefaultWorkspace(moduleAccessManager.defaultWorkspace());
            return;
        }
        if (current == QStringLiteral("planner")
            && appState.operationalMode() != QStringLiteral("pilot")
            && !moduleAccessManager.missionWorkspaceAllowed()) {
            appState.openDefaultWorkspace(moduleAccessManager.defaultWorkspace());
            return;
        }
        if (current == QStringLiteral("gcsTools")
            && !moduleAccessManager.gcsToolsWorkspaceAllowed()) {
            appState.openDefaultWorkspace(moduleAccessManager.defaultWorkspace());
            return;
        }
    };
    QObject::connect(&moduleAccessManager, &ModuleAccessManager::modulesChanged, &appState, enforceWorkspaceAccess);
    QObject::connect(&appState, &AppState::navigationChanged, &appState, enforceWorkspaceAccess);
    QObject::connect(&accessManager, &AccessManager::accessChanged, &appState, &AppState::resolveWorkspaceForAccess);
    QObject::connect(&preferencesManager, &PreferencesManager::preferencesChanged, &mapProvider, [&]() {
        mapProvider.setLayerMode(preferencesManager.mapLayer());
    });
    bool workspaceBootstrapLoaded = false;
    QObject::connect(&authManager, &AuthManager::loginSucceeded, &missionSyncManager, [&]() {
        workspaceBootstrapLoaded = false;
    });
    QObject::connect(&sessionManager, &SessionManager::forceLogout, &missionSyncManager, [&]() {
        workspaceBootstrapLoaded = false;
    });
    QObject::connect(&sessionManager, &SessionManager::sessionStateChanged, &missionSyncManager, [&]() {
        accessManager.setSessionState(sessionManager.operationsAllowed(),
                                      sessionManager.controlCenterReachable(),
                                      sessionManager.blockReason());
        if (sessionManager.operationsAllowed()) {
            sessionManager.startMonitoring();
            appStartupManager.connecting();
            if (!workspaceBootstrapLoaded) {
                workspaceBootstrapLoaded = true;
                gcsEventSyncManager.recordEvent(QStringLiteral("bootstrap_sync"), QStringLiteral("info"), QStringLiteral("Control Center session trusted"));
                missionSyncManager.bootstrap(true);
            }
            telemetrySyncManager.start();
        } else {
            workspaceBootstrapLoaded = false;
            sessionManager.stopMonitoring();
            telemetrySyncManager.stop();
        }
    });
    QTimer targetedAccessSyncTimer;
    targetedAccessSyncTimer.setSingleShot(true);
    targetedAccessSyncTimer.setInterval(performanceMode == QStringLiteral("fast") ? 3500 : 1500);
    QObject::connect(&targetedAccessSyncTimer, &QTimer::timeout, &missionSyncManager, [&]() {
        if (sessionManager.operationsAllowed()) {
            missionSyncManager.bootstrap(true);
        }
    });
    QTimer missionListSyncTimer;
    missionListSyncTimer.setSingleShot(true);
    missionListSyncTimer.setInterval(performanceMode == QStringLiteral("fast") ? 2500 : 1200);
    QObject::connect(&missionListSyncTimer, &QTimer::timeout, &missionSyncManager, [&]() {
        if (sessionManager.operationsAllowed()) {
            missionSyncManager.syncMissions();
        }
    });
    QObject::connect(&webSocketClient, &WebSocketClient::messageReceived, &missionSyncManager, [&](const QString &message) {
        const QJsonObject envelope = QJsonDocument::fromJson(message.toUtf8()).object();
        const QString eventName = envelope.value(QStringLiteral("event")).toString();
        const QJsonObject payload = envelope.value(QStringLiteral("payload")).toObject();
        const QString streamName = envelope.value(QStringLiteral("stream")).toString();
        const QString payloadType = payload.value(QStringLiteral("type")).toString();
        const QString eventKey = QStringLiteral("%1 %2 %3").arg(eventName, streamName, payloadType).toLower();
        if (eventName == QStringLiteral("WEATHER_UPDATE") || payload.value(QStringLiteral("type")).toString() == QStringLiteral("WEATHER_UPDATE")) {
            QVariantMap weather = payload.toVariantMap();
            weather[QStringLiteral("assessment")] = payload.value(QStringLiteral("assessment")).toObject().toVariantMap();
            weather[QStringLiteral("current")] = payload.value(QStringLiteral("current")).toObject().toVariantMap();
            weatherManager.applyWeather(weather);
            windCheckManager.evaluate();
            return;
        }
        const bool accessCriticalEvent = eventKey.contains(QStringLiteral("permission"))
            || eventKey.contains(QStringLiteral("role"))
            || eventKey.contains(QStringLiteral("organization"))
            || eventKey.contains(QStringLiteral("device_trust"))
            || eventKey.contains(QStringLiteral("device trust"))
            || eventKey.contains(QStringLiteral("account_suspension"))
            || eventKey.contains(QStringLiteral("account_activation"))
            || eventKey.contains(QStringLiteral("aircraft_access"));
        if (accessCriticalEvent) {
            targetedAccessSyncTimer.start();
            if (eventKey.contains(QStringLiteral("device")) || eventKey.contains(QStringLiteral("account"))) {
                sessionManager.validateSession();
            }
            return;
        }
        if (eventName.contains(QStringLiteral("mission"), Qt::CaseInsensitive)
            || streamName == QStringLiteral("missions")) {
            missionListSyncTimer.start();
            return;
        }
        if (eventName.contains(QStringLiteral("flight_session"), Qt::CaseInsensitive)
            || streamName == QStringLiteral("flight_sessions")) {
            missionListSyncTimer.start();
        }
    });

    QObject::connect(&vehicleManager, &MavsdkVehicleManager::systemReady, &gcsEventSyncManager, [&]() {
        gcsEventSyncManager.recordEvent(QStringLiteral("vehicle_connected"), QStringLiteral("info"), QStringLiteral("MAVSDK vehicle connected"));
    });

    QTimer missionChangeSyncTimer;
    missionChangeSyncTimer.setSingleShot(true);
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    missionChangeSyncTimer.setInterval(qBound(900,
                                              env.value(QStringLiteral("SKYGRID_MISSION_CHANGE_BATCH_MS"),
                                                        env.value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true")
                                                            ? QStringLiteral("2500")
                                                            : QStringLiteral("1800")).toInt(),
                                              10000));
    QObject::connect(missionStore.plan(), &MissionPlanModel::planChanged, &missionChangeSyncTimer, [&]() {
        missionChangeSyncTimer.start();
    });
    QObject::connect(&missionChangeSyncTimer, &QTimer::timeout, &gcsEventSyncManager, [&]() {
        const QVariantList routeItems = missionStore.plan()->generatedRoute();
        QVariantMap mission{
            {QStringLiteral("mission_id"), missionStore.plan()->missionId()},
            {QStringLiteral("mission_name"), missionStore.plan()->name()},
            {QStringLiteral("mission_type"), missionStore.plan()->missionType()},
            {QStringLiteral("selected_aircraft_id"), missionStore.plan()->selectedAircraftId()},
            {QStringLiteral("sync_state"), missionStore.plan()->syncState()},
            {QStringLiteral("mission_state"), missionStore.plan()->missionState()},
            {QStringLiteral("upload_state"), missionStore.plan()->uploadState()},
            {QStringLiteral("execution_state"), missionStore.plan()->executionState()},
            {QStringLiteral("waypoint_count"), missionStore.plan()->waypoints().size()},
            {QStringLiteral("polygon_vertex_count"), missionStore.plan()->polygon().size()},
            {QStringLiteral("has_poi"), missionStore.plan()->hasPoi()},
            {QStringLiteral("route_item_count"), routeItems.size()},
            {QStringLiteral("route_distance_km"), missionStore.plan()->routeDistanceKm()},
            {QStringLiteral("area_ha"), missionStore.plan()->missionAreaHa()},
            {QStringLiteral("estimated_time"), missionStore.plan()->estimatedTime()},
            {QStringLiteral("estimated_battery"), missionStore.plan()->estimatedBattery()},
            {QStringLiteral("altitude_m"), missionStore.plan()->altitude()},
            {QStringLiteral("speed_mps"), missionStore.plan()->speed()},
            {QStringLiteral("front_overlap"), missionStore.plan()->frontOverlap()},
            {QStringLiteral("side_overlap"), missionStore.plan()->sideOverlap()},
            {QStringLiteral("readiness_percent"), missionStore.plan()->missionReadiness()},
            {QStringLiteral("active_waypoint"), missionStore.plan()->activeWaypointIndex()},
            {QStringLiteral("progress_percent"), missionStore.plan()->progressPercent()}
        };
        mission[QStringLiteral("operation_mode")] = appState.operationalMode() == QStringLiteral("pilot")
            ? QStringLiteral("PILOT")
            : QStringLiteral("MISSION");
        mission[QStringLiteral("event_scope")] = QStringLiteral("mission_planning_change");
        gcsEventSyncManager.recordEvent(QStringLiteral("mission_plan_changed"),
                                        QStringLiteral("info"),
                                        QStringLiteral("Mission planning state changed"),
                                        QJsonObject::fromVariantMap(mission));
    });

    if (tokenManager.hasSession() && tokenManager.deviceTrusted()) {
        appStartupManager.begin();
        missionSyncManager.publishCached();
        appStartupManager.checkingSession();
        sessionManager.validateSession();
    } else {
        appStartupManager.completeForLogin();
    }

    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/SkyGrid/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
