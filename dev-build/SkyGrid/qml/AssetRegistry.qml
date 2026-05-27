pragma Singleton

import QtQuick

QtObject {
    id: root

    readonly property string baseUrl: "qrc:/qt/qml/SkyGrid/assets/"

    function asset(fileName) {
        return baseUrl + fileName
    }

    function icon(fileName) {
        return baseUrl + "icons/" + fileName
    }

    function svg(fileName) {
        return "image://svgicon/" + fileName
    }

    readonly property var logos: ({
        full_logo: svg("full_logo.svg"),
        logo: asset("logo.png"),
        icon: asset("icon.png")
    })

    readonly property var dashboard: ({
        mission_operations_center_bg: asset("dashboard/mission_operations_center_bg.png")
    })

    readonly property var aircraft: ({
        x8_01: asset("aircraft/SkyGrid_X8-01.png"),
        vtol_03: asset("aircraft/SkyGrid_VTOL-03.png")
    })

    readonly property var weather: ({
        widgets: asset("weather/weather_widgets.png"),
        wind_compass_arrow: asset("weather/weather_widgets.png"),
        badge_good: asset("weather/weather_widgets.png"),
        badge_caution: asset("weather/weather_widgets.png"),
        badge_dnf: asset("weather/weather_widgets.png")
    })

    readonly property var sidebar: ({
        widgets: asset("sidebar/sidebar_badges.png"),
        tab_mission: asset("sidebar/sidebar_badges.png"),
        tab_material: asset("sidebar/sidebar_badges.png"),
        tab_map: asset("sidebar/sidebar_badges.png")
    })

    readonly property var mission_badges: ({
        widgets: asset("mission_badges/mission_badges.png"),
        surveillance: asset("mission_badges/mission_badges.png"),
        logistics: asset("mission_badges/mission_badges.png"),
        recon: asset("mission_badges/mission_badges.png"),
        training: asset("mission_badges/mission_badges.png")
    })

    readonly property var icons: ({
        arrow: icon("arrow.svg"),
        boxicons_battery_3: icon("boxicons_battery-3.svg"),
        "boxicons_battery-3": icon("boxicons_battery-3.svg"),
        boxicons_camera: icon("boxicons_camera.svg"),
        boxicons_cursor_pointer: icon("boxicons_cursor-pointer.svg"),
        "boxicons_cursor-pointer": icon("boxicons_cursor-pointer.svg"),
        boxicons_filter: icon("boxicons_filter.svg"),
        boxicons_search_big: icon("boxicons_search-big.svg"),
        boxicons_git_branch: icon("boxicons_git-branch.svg"),
        "boxicons_git-branch": icon("boxicons_git-branch.svg"),
        boxicons_play: icon("boxicons_play.svg"),
        boxicons_save: icon("boxicons_save.svg"),
        boxicons_trash: icon("boxicons_trash.svg"),
        boxicons_undo: icon("boxicons_undo.svg"),
        boxicons_wifi: icon("boxicons_wifi.svg"),
        boxicons_wind_filled: icon("boxicons_wind-filled.svg"),
        "boxicons_wind-filled": icon("boxicons_wind-filled.svg"),
        cloud: icon("cloud.svg"),
        compass_border: icon("compass_border.svg"),
        cube: icon("cube.svg"),
        divider_lines: icon("divider_lines.svg"),
        edit: icon("edit.svg"),
        iconoir_arrow_up_2: icon("iconamoon_arrow-up-2.svg"),
        iconamoon_arrow_up_2: icon("iconamoon_arrow-up-2.svg"),
        iconoir_cancel: icon("iconoir_cancel.svg"),
        lucide_building_2: icon("lucide_building-2.svg"),
        "lucide_building-2": icon("lucide_building-2.svg"),
        lucide_grid_3x3: icon("lucide_grid-3x3.svg"),
        "lucide_grid-3x3": icon("lucide_grid-3x3.svg"),
        lucide_mountain: icon("lucide_mountain.svg"),
        lucide_route: icon("lucide_route.svg"),
        lucide_satellite: icon("lucide_satellite.svg"),
        lucide_waypoints: icon("lucide_waypoints.svg"),
        mdi_person_outline: icon("mdi_person-outline.svg"),
        "mdi_person-outline": icon("mdi_person-outline.svg"),
        mdi_check_circle: icon("mdi_check-circle.svg"),
        mdi_home_outline: icon("mdi_home-outline.svg"),
        mingcute_compass_line: icon("mingcute_compass-line.svg"),
        "mingcute_compass-line": icon("mingcute_compass-line.svg"),
        pin_location: icon("pin_location.svg"),
        plane: icon("plane.svg"),
        plus: icon("plus.svg"),
        vector: icon("Vector.svg"),
        Vector: icon("Vector.svg"),
        waypoint: icon("waypioint.svg"),
        waypioint: icon("waypioint.svg")
    })
}
