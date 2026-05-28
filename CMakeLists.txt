cmake_minimum_required(VERSION 3.22)

project(SkyGridGCS
    VERSION 0.1.0
    DESCRIPTION "SkyGrid GCS premium mission planning UI"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
option(SKYGRID_DEV_BUILD "Enable DigiSkyGrid development build defaults" OFF)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Network
    WebSockets
    Sql
    Widgets
    Qml
    Quick
    QuickControls2
)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBRSVG REQUIRED IMPORTED_TARGET librsvg-2.0)
find_package(MAVSDK REQUIRED CONFIG)

set(SKYGRID_ARCGIS_RUNTIME_QT_PATH "$ENV{ARCGIS_RUNTIME_QT_PATH}" CACHE PATH "ArcGIS Maps SDK for Qt root")
set(SKYGRID_HAS_ARCGIS FALSE)
set(SKYGRID_HAS_ARCGIS_QML FALSE)
set(SKYGRID_ARCGIS_QML_IMPORT_ROOT "")
set(SKYGRID_ARCGIS_QML_FILES "")

if(SKYGRID_ARCGIS_RUNTIME_QT_PATH AND EXISTS "${SKYGRID_ARCGIS_RUNTIME_QT_PATH}")
    list(APPEND CMAKE_PREFIX_PATH "${SKYGRID_ARCGIS_RUNTIME_QT_PATH}")
    list(APPEND CMAKE_PREFIX_PATH "${SKYGRID_ARCGIS_RUNTIME_QT_PATH}/sdk")
    list(APPEND CMAKE_PREFIX_PATH "${SKYGRID_ARCGIS_RUNTIME_QT_PATH}/sdk/cmake")
    list(APPEND CMAKE_PREFIX_PATH "${SKYGRID_ARCGIS_RUNTIME_QT_PATH}/lib/cmake")

    find_package(ArcGISRuntime QUIET CONFIG)

    if(ArcGISRuntime_FOUND)
        set(SKYGRID_HAS_ARCGIS TRUE)
        message(STATUS "ArcGIS Maps SDK for Qt found at ${SKYGRID_ARCGIS_RUNTIME_QT_PATH}")
    else()
        message(WARNING "ARCGIS_RUNTIME_QT_PATH is set, but ArcGISRuntimeConfig.cmake was not found. Offline map fallback will be used.")
    endif()

    foreach(_arcgis_qml_root
        "${SKYGRID_ARCGIS_RUNTIME_QT_PATH}/qml"
        "${SKYGRID_ARCGIS_RUNTIME_QT_PATH}/sdk/qml"
        "${SKYGRID_ARCGIS_RUNTIME_QT_PATH}/Resources/qml"
    )
        if(EXISTS "${_arcgis_qml_root}/Esri/ArcGISRuntime/qmldir")
            set(SKYGRID_HAS_ARCGIS_QML TRUE)
            set(SKYGRID_ARCGIS_QML_IMPORT_ROOT "${_arcgis_qml_root}")
            set(SKYGRID_ARCGIS_QML_FILES
                qml/map/ArcGisRuntimeMapSurface.qml
                qml/map/ArcGisSceneSurface.qml
            )
            message(STATUS "ArcGIS QML import found at ${SKYGRID_ARCGIS_QML_IMPORT_ROOT}")
            break()
        endif()
    endforeach()
else()
    message(STATUS "ARCGIS_RUNTIME_QT_PATH is not set or does not exist; offline map fallback enabled.")
endif()

if(COMMAND qt_standard_project_setup)
    qt_standard_project_setup(REQUIRES 6.5)
endif()

set_source_files_properties(qml/AssetRegistry.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

set(SKYGRID_ASSET_FILES
    Asset/full_logo.svg
    Asset/icon.png
    Asset/logo.png
    Asset/icons/Vector.svg
    Asset/icons/arrow.svg
    Asset/icons/boxicons_battery-3.svg
    Asset/icons/boxicons_camera.svg
    Asset/icons/boxicons_cursor-pointer.svg
    Asset/icons/boxicons_filter.svg
    Asset/icons/boxicons_search-big.svg
    Asset/icons/boxicons_git-branch.svg
    Asset/icons/boxicons_play.svg
    Asset/icons/boxicons_save.svg
    Asset/icons/boxicons_trash.svg
    Asset/icons/boxicons_undo.svg
    Asset/icons/boxicons_wifi.svg
    Asset/icons/boxicons_wind-filled.svg
    Asset/icons/cloud.svg
    Asset/icons/compass_border.svg
    Asset/icons/cube.svg
    Asset/icons/divider_lines.svg
    Asset/icons/edit.svg
    Asset/icons/iconamoon_arrow-up-2.svg
    Asset/icons/iconoir_cancel.svg
    Asset/icons/lucide_building-2.svg
    Asset/icons/lucide_grid-3x3.svg
    Asset/icons/lucide_mountain.svg
    Asset/icons/lucide_route.svg
    Asset/icons/lucide_satellite.svg
    Asset/icons/lucide_waypoints.svg
    Asset/icons/mdi_person-outline.svg
    Asset/icons/mdi_check-circle.svg
    Asset/icons/mdi_home-outline.svg
    Asset/icons/mingcute_compass-line.svg
    Asset/icons/pin_location.svg
    Asset/icons/plane.svg
    Asset/icons/plus.svg
    Asset/icons/waypioint.svg
)
file(GLOB SKYGRID_GENERATED_ASSET_FILES CONFIGURE_DEPENDS
    Asset/generated/*.png
    Asset/dashboard/*.png
    Asset/aircraft/*.png
    Asset/weather/*.png
    Asset/sidebar/*.png
    Asset/mission_badges/*.png
)

set_source_files_properties(Asset/full_logo.svg PROPERTIES QT_RESOURCE_ALIAS assets/full_logo.svg)
set_source_files_properties(Asset/icon.png PROPERTIES QT_RESOURCE_ALIAS assets/icon.png)
set_source_files_properties(Asset/logo.png PROPERTIES QT_RESOURCE_ALIAS assets/logo.png)
foreach(_skygrid_asset IN LISTS SKYGRID_ASSET_FILES)
    if(_skygrid_asset MATCHES "^Asset/icons/(.+)$")
        set_source_files_properties("${_skygrid_asset}" PROPERTIES QT_RESOURCE_ALIAS "assets/icons/${CMAKE_MATCH_1}")
    endif()
endforeach()
foreach(_skygrid_generated_asset IN LISTS SKYGRID_GENERATED_ASSET_FILES)
    if(_skygrid_generated_asset MATCHES "/Asset/generated/(.+)$")
        set_source_files_properties("${_skygrid_generated_asset}" PROPERTIES QT_RESOURCE_ALIAS "assets/${CMAKE_MATCH_1}")
    elseif(_skygrid_generated_asset MATCHES "/Asset/(dashboard|aircraft|weather|sidebar|mission_badges)/(.+)$")
        set_source_files_properties("${_skygrid_generated_asset}" PROPERTIES QT_RESOURCE_ALIAS "assets/${CMAKE_MATCH_1}/${CMAKE_MATCH_2}")
    endif()
endforeach()

qt_add_executable(SkyGridGCS
    src/main.cpp
    src/auth/AuthManager.cpp
    src/auth/AuthManager.h
    src/auth/SessionManager.cpp
    src/auth/SessionManager.h
    src/auth/TokenManager.cpp
    src/auth/TokenManager.h
    src/cache/LocalSyncCache.cpp
    src/cache/LocalSyncCache.h
    src/controllers/AppState.cpp
    src/controllers/AppState.h
    src/controllers/MapState.cpp
    src/controllers/MapState.h
    src/controllers/MapProviderController.cpp
    src/controllers/MapProviderController.h
    src/controllers/MissionStore.cpp
    src/controllers/MissionStore.h
    src/controllers/ThemeController.h
    src/flight/EventLogManager.cpp
    src/flight/EventLogManager.h
    src/flight/FlightStatsManager.cpp
    src/flight/FlightStatsManager.h
    src/flight/PostMissionSummaryManager.cpp
    src/flight/PostMissionSummaryManager.h
    src/flight/PreflightChecklistManager.cpp
    src/flight/PreflightChecklistManager.h
    src/flight/ManualControlManager.cpp
    src/flight/ManualControlManager.h
    src/map/TileCacheManager.cpp
    src/map/TileCacheManager.h
    src/network/ApiClient.cpp
    src/network/ApiClient.h
    src/network/WebSocketClient.cpp
    src/network/WebSocketClient.h
    src/preferences/PreferencesManager.cpp
    src/preferences/PreferencesManager.h
    src/profile/OperatorStateManager.cpp
    src/profile/OperatorStateManager.h
    src/profile/ProfileManager.cpp
    src/profile/ProfileManager.h
    src/security/BackendTrustManager.cpp
    src/security/BackendTrustManager.h
    src/security/DeviceManager.cpp
    src/security/DeviceManager.h
    src/security/SecureStorage.cpp
    src/security/SecureStorage.h
    src/models/MissionPlanModel.cpp
    src/models/MissionPlanModel.h
    src/models/MissionGenerationService.cpp
    src/models/MissionGenerationService.h
    src/models/MissionTypeModel.cpp
    src/models/MissionTypeModel.h
    src/sync/MissionSyncManager.cpp
    src/sync/MissionSyncManager.h
    src/sync/MissionPreviewManager.cpp
    src/sync/MissionPreviewManager.h
    src/sync/PreferencesSyncManager.cpp
    src/sync/PreferencesSyncManager.h
    src/sync/ProfileSyncManager.cpp
    src/sync/ProfileSyncManager.h
    src/sync/GcsEventSyncManager.cpp
    src/sync/GcsEventSyncManager.h
    src/sync/TelemetrySyncManager.cpp
    src/sync/TelemetrySyncManager.h
    src/sync/FlightSessionSyncManager.cpp
    src/sync/FlightSessionSyncManager.h
    src/sync/PilotActionSyncManager.cpp
    src/sync/PilotActionSyncManager.h
    src/sync/WeatherSyncManager.cpp
    src/sync/WeatherSyncManager.h
    src/telemetry/WeatherManager.cpp
    src/telemetry/WeatherManager.h
    src/telemetry/WindCheckManager.cpp
    src/telemetry/WindCheckManager.h
    src/telemetry/WindTelemetryManager.cpp
    src/telemetry/WindTelemetryManager.h
    src/vehicle/MavsdkVehicleManager.cpp
    src/vehicle/MavsdkVehicleManager.h
    src/vehicle/HomePositionManager.cpp
    src/vehicle/HomePositionManager.h
    src/vehicle/VehicleActionManager.cpp
    src/vehicle/VehicleActionManager.h
    src/vehicle/MissionExecutionManager.cpp
    src/vehicle/MissionExecutionManager.h
    src/vehicle/MissionUploadManager.cpp
    src/vehicle/MissionUploadManager.h
    src/vehicle/VehicleTelemetryModel.cpp
    src/vehicle/VehicleTelemetryModel.h
)

if(SKYGRID_DEV_BUILD)
    target_compile_definitions(SkyGridGCS PRIVATE SKYGRID_DEV_BUILD=1)
endif()

qt_add_qml_module(SkyGridGCS
    URI SkyGrid
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    QML_FILES
        qml/Main.qml
        qml/app/AppShell.qml
        qml/AssetRegistry.qml
        qml/app/Router.qml
        qml/app/DesignTokens.qml
        qml/app/TelemetryBar.qml
        qml/screens/LoginScreen.qml
        qml/screens/HomeScreen.qml
        qml/screens/MissionPlannerScreen.qml
        qml/dashboard/TopStatusPill.qml
        qml/dashboard/SidebarTabButton.qml
        qml/dashboard/MissionHistoryItem.qml
        qml/dashboard/HeroOperationsCard.qml
        qml/dashboard/AircraftSummaryCard.qml
        qml/dashboard/QuickActionTile.qml
        qml/dashboard/ReadinessCard.qml
        qml/dashboard/RecentActivityCard.qml
        qml/dashboard/SystemStatusCard.qml
        qml/dashboard/PilotAssignmentCard.qml
        qml/dashboard/WeatherForecastCard.qml
        qml/pilot/AircraftMarker.qml
        qml/pilot/AircraftStatusPanel.qml
        qml/pilot/CameraFpvPanel.qml
        qml/pilot/CompassHudStack.qml
        qml/pilot/FlightStateCard.qml
        qml/pilot/FlightTrailOverlay.qml
        qml/pilot/FlightHud.qml
        qml/pilot/HomeMarker.qml
        qml/pilot/JoystickVisibilityToggle.qml
        qml/pilot/MapPrimaryView.qml
        qml/pilot/ManualFlightMap.qml
        qml/pilot/ManualFlightScreen.qml
        qml/pilot/ManualSettingsPanel.qml
        qml/pilot/ManualSettingsDrawer.qml
        qml/pilot/ModeSwitchTabs.qml
        qml/pilot/PilotModeTopBar.qml
        qml/pilot/PilotViewSwitcher.qml
        qml/pilot/QuickActionsPanel.qml
        qml/pilot/SplitFlightView.qml
        qml/pilot/SmallHudWidget.qml
        qml/pilot/SystemStatusBar.qml
        qml/pilot/TelemetryGrid.qml
        qml/pilot/TelemetryMetricCard.qml
        qml/pilot/TelemetryPanel.qml
        qml/pilot/TelemetryRow.qml
        qml/pilot/VideoPrimaryView.qml
        qml/pilot/VirtualJoystickOverlay.qml
        qml/pilot/WindCompassWidget.qml
        qml/pilot/GoToCoordinateDialog.qml
        qml/pilot/EventLogPanel.qml
        qml/mission/MissionActionBar.qml
        qml/mission/MissionCatalog.qml
        qml/mission/MissionCommandPanel.qml
        qml/mission/MissionConfigPanel.qml
        qml/mission/MissionElevationProfile.qml
        qml/mission/MissionPlanningPreviewPanel.qml
        qml/mission/PilotFlightPanel.qml
        qml/mission/MissionSelectionModal.qml
        qml/mission/MissionSummaryStrip.qml
        qml/mission/PostMissionSummaryModal.qml
        qml/mission/PreflightChecklistModal.qml
        qml/mission/PreflightChecklistPanel.qml
        qml/mission/MissionValidationPanel.qml
        qml/mission/MissionWeatherCard.qml
        qml/mission/MissionWorkspace.qml
        qml/mission/types/Map3DAreaConfig.qml
        qml/mission/types/Map3DPoiConfig.qml
        qml/mission/types/PhotomapConfig.qml
        qml/mission/types/TowerInspectionConfig.qml
        qml/mission/types/VirtualFenceConfig.qml
        qml/mission/types/WaypointRouteConfig.qml
        qml/map/CompassControl.qml
        qml/map/ArcGisMapSurface.qml
        qml/map/MapControls.qml
        qml/map/MapHud.qml
        qml/map/MissionMap.qml
        qml/map/OfflineMapSurface.qml
        qml/map/TileMapSurface.qml
        ${SKYGRID_ARCGIS_QML_FILES}
        qml/map/overlays/AircraftMarkerOverlay.qml
        qml/map/overlays/AltitudeLabelOverlay.qml
        qml/map/overlays/GeofenceOverlay.qml
        qml/map/overlays/OrbitOverlay.qml
        qml/map/overlays/OverlayLayer.qml
        qml/map/overlays/PoiMarkerOverlay.qml
        qml/map/overlays/PolygonOverlay.qml
        qml/map/overlays/ScanLineOverlay.qml
        qml/map/overlays/TowerInspectionOverlay.qml
        qml/map/overlays/WaypointOverlay.qml
        qml/controls/AircraftCard.qml
        qml/controls/AssetIcon.qml
        qml/controls/CollapsibleSection.qml
        qml/controls/GlassPanel.qml
        qml/controls/IconButton.qml
        qml/controls/MetricTile.qml
        qml/controls/MissionHistoryRow.qml
        qml/controls/MissionToolButton.qml
        qml/controls/ParameterSlider.qml
        qml/controls/PrimaryButton.qml
        qml/controls/SegmentedControl.qml
        qml/controls/SelectRow.qml
        qml/controls/StatusPill.qml
        qml/controls/ToggleRow.qml
        qml/controls/WindowCloseButton.qml
    RESOURCES
        ${SKYGRID_ASSET_FILES}
        ${SKYGRID_GENERATED_ASSET_FILES}
)

target_link_libraries(SkyGridGCS PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::WebSockets
    Qt6::Sql
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    PkgConfig::LIBRSVG
    MAVSDK::mavsdk
)

if(SKYGRID_HAS_ARCGIS)
    target_compile_definitions(SkyGridGCS PRIVATE SKYGRID_HAS_ARCGIS=1)
    if(TARGET ArcGISRuntime::ArcGISRuntime)
        target_link_libraries(SkyGridGCS PRIVATE ArcGISRuntime::ArcGISRuntime)
    elseif(TARGET Esri::ArcGISRuntime)
        target_link_libraries(SkyGridGCS PRIVATE Esri::ArcGISRuntime)
    endif()
endif()

if(SKYGRID_HAS_ARCGIS_QML)
    target_compile_definitions(SkyGridGCS PRIVATE SKYGRID_HAS_ARCGIS_QML=1)
    set_property(TARGET SkyGridGCS APPEND PROPERTY QT_QML_IMPORT_PATH "${SKYGRID_ARCGIS_QML_IMPORT_ROOT}")
endif()

set_target_properties(SkyGridGCS PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_target(ui-smoke
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/smoke_ui.sh ${CMAKE_BINARY_DIR}
    DEPENDS SkyGridGCS
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Launching SkyGridGCS offscreen UI smoke test"
)
