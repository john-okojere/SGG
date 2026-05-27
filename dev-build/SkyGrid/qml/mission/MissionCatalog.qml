import QtQuick

QtObject {
    function titleFor(type) {
        if (type === "waypointRoute") return "Waypoint Route"
        if (type === "map3dArea") return "3D Map Area"
        if (type === "map3dPoi") return "3D Map POI"
        if (type === "virtualFence") return "Virtual Fence"
        if (type === "towerInspection") return "Tower Inspection"
        return "Photomap"
    }
}
