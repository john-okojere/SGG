import QtQuick
import QtQuick.Layouts
import SkyGrid 1.0

GridLayout {
    id: root
    columns: width < 250 ? 1 : 2
    columnSpacing: 8
    rowSpacing: 8

    function valueOrDash(v, decimals) {
        return telemetryStore.connected ? Number(v).toFixed(decimals || 0) : "--"
    }

    function coordText(lat, lon) {
        if (!telemetryStore.connected || Math.abs(lat) < 0.000001 || Math.abs(lon) < 0.000001) return "--"
        return Number(lat).toFixed(5) + ", " + Number(lon).toFixed(5)
    }

    function homeCoordText() {
        var path = telemetryStore.livePath
        if (!telemetryStore.connected || path.length === 0) return "--"
        return coordText(path[0].latitude, path[0].longitude)
    }

    function distanceFromHome() {
        var path = telemetryStore.livePath
        if (!telemetryStore.connected || path.length === 0) return "--"
        var lat1 = Number(path[0].latitude) * Math.PI / 180
        var lat2 = Number(telemetryStore.latitude) * Math.PI / 180
        var dLat = lat2 - lat1
        var dLon = (Number(telemetryStore.longitude) - Number(path[0].longitude)) * Math.PI / 180
        var a = Math.sin(dLat / 2) * Math.sin(dLat / 2) + Math.cos(lat1) * Math.cos(lat2) * Math.sin(dLon / 2) * Math.sin(dLon / 2)
        return Math.round(6371000 * 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a)))
    }

    function metricStatusColor(kind, value) {
        if (!telemetryStore.connected) return "#9b94a6"
        if (kind === "battery") return value < 25 ? "#ef233c" : (value < 40 ? "#f5c542" : "#18b83f")
        if (kind === "gps") return value < 8 ? "#ef233c" : (value < 12 ? "#f5c542" : "#18b83f")
        if (kind === "signal") return value < 40 ? "#ef233c" : (value < 70 ? "#f5c542" : "#18b83f")
        return "#5b22a8"
    }

    TelemetryMetricCard { Layout.fillWidth: true; label: "ALT AGL"; iconText: "△"; value: valueOrDash(telemetryStore.altitude, 1); unit: "m"; statusColor: "#5b22a8" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "ALT AMSL"; iconText: "△"; value: telemetryStore.connected ? Number(telemetryStore.altitude + 587.3).toFixed(1) : "--"; unit: "m"; statusColor: "#5b22a8" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "GROUND SPEED"; iconSource: AssetRegistry.icons.boxicons_wind_filled; value: valueOrDash(telemetryStore.speed, 1); unit: "m/s"; statusColor: "#5b22a8" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "VERT SPEED"; iconText: "↕"; value: valueOrDash(telemetryStore.verticalSpeed, 1); unit: "m/s"; statusColor: "#5b22a8" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "HEADING"; iconText: "◴"; value: telemetryStore.connected ? ("000" + Math.round(telemetryStore.heading)).slice(-3) : "--"; unit: "deg"; statusColor: "#5b22a8" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "BATTERY"; iconSource: AssetRegistry.icons.boxicons_battery_3; value: telemetryStore.connected ? telemetryStore.battery : "--"; unit: "%"; subValue: telemetryStore.batteryVoltage > 0 ? Number(telemetryStore.batteryVoltage).toFixed(1) + " V" : ""; statusColor: metricStatusColor("battery", telemetryStore.battery) }
    TelemetryMetricCard { Layout.fillWidth: true; label: "GPS FIX"; iconSource: AssetRegistry.icons.lucide_satellite; value: telemetryStore.connected ? telemetryStore.gpsMode : "--"; statusColor: metricStatusColor("gps", telemetryStore.satellites) }
    TelemetryMetricCard { Layout.fillWidth: true; label: "SATELLITES"; iconSource: AssetRegistry.icons.lucide_satellite; value: telemetryStore.connected ? telemetryStore.satellites : "--"; unit: "sat"; subValue: telemetryStore.hdop > 0 ? "HDOP " + Number(telemetryStore.hdop).toFixed(1) : ""; statusColor: metricStatusColor("gps", telemetryStore.satellites) }
    TelemetryMetricCard { Layout.fillWidth: true; label: "RC SIGNAL"; iconSource: AssetRegistry.icons.boxicons_wifi; value: telemetryStore.connected ? telemetryStore.rcSignal : "--"; unit: "%"; subValue: telemetryStore.rcQuality; statusColor: metricStatusColor("signal", telemetryStore.rcSignal) }
    TelemetryMetricCard { Layout.fillWidth: true; label: "DATA LINK"; iconSource: AssetRegistry.icons.boxicons_wifi; value: telemetryStore.connected ? telemetryStore.transmission : "--"; unit: "%"; statusColor: metricStatusColor("signal", telemetryStore.transmission) }
    TelemetryMetricCard { Layout.fillWidth: true; label: "FLIGHT MODE"; iconSource: AssetRegistry.icons.plane; value: telemetryStore.connected ? telemetryStore.flightMode : "--"; subValue: telemetryStore.armed ? "ARMED" : "DISARMED"; statusColor: telemetryStore.armed ? "#18b83f" : "#9b94a6" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "DIST HOME"; iconText: "◇"; value: distanceFromHome(); unit: telemetryStore.connected ? "m" : ""; statusColor: "#5b22a8" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "DIST FLOWN"; iconText: "⟲"; value: telemetryStore.connected ? Number(flightStatsManager.distanceKm).toFixed(2) : "--"; unit: "km"; statusColor: "#5b22a8" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "FLIGHT TIME"; iconText: "◷"; value: flightStatsManager.flightTimeText; statusColor: flightStatsManager.active ? "#18b83f" : "#9b94a6" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "WIND"; iconSource: AssetRegistry.icons.boxicons_wind_filled; value: windTelemetryManager.available ? Number(windTelemetryManager.windSpeed).toFixed(1) : "--"; unit: "m/s"; subValue: windTelemetryManager.available ? windTelemetryManager.directionLabel : "Unavailable"; statusColor: windTelemetryManager.available ? "#18b83f" : "#9b94a6" }
    TelemetryMetricCard { Layout.fillWidth: true; label: "GUST"; iconSource: AssetRegistry.icons.boxicons_wind_filled; value: windTelemetryManager.available ? Number(windTelemetryManager.gust).toFixed(1) : "--"; unit: "m/s"; statusColor: windTelemetryManager.available ? "#18b83f" : "#9b94a6" }
    TelemetryMetricCard { Layout.columnSpan: root.columns; Layout.fillWidth: true; label: "HOME COORDINATE"; iconSource: AssetRegistry.icons.pin_location; value: homeCoordText(); statusColor: "#5b22a8" }
    TelemetryMetricCard { Layout.columnSpan: root.columns; Layout.fillWidth: true; label: "AIRCRAFT COORDINATE"; iconSource: AssetRegistry.icons.plane; value: coordText(telemetryStore.latitude, telemetryStore.longitude); statusColor: telemetryStore.connected ? "#18b83f" : "#9b94a6" }
}
