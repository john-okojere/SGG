#include "MultiVehicleManager.h"

#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "MavsdkVehicleManager.h"
#include "VehicleTelemetryModel.h"

#include <QDateTime>
#include <QJsonObject>

MultiVehicleManager::MultiVehicleManager(MavsdkVehicleManager *vehicle,
                                         VehicleTelemetryModel *telemetry,
                                         AccessManager *access,
                                         GcsEventSyncManager *events,
                                         QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_telemetry(telemetry),
      m_access(access),
      m_events(events)
{
    m_connectionProfiles = {
        QVariantMap{{QStringLiteral("id"), QStringLiteral("primary-udp-14540")},
                    {QStringLiteral("name"), QStringLiteral("Primary UDP 14540")},
                    {QStringLiteral("url"), QStringLiteral("udpin://0.0.0.0:14540")},
                    {QStringLiteral("stack"), QStringLiteral("PX4 / ArduPilot")}},
        QVariantMap{{QStringLiteral("id"), QStringLiteral("sitl-udp-14550")},
                    {QStringLiteral("name"), QStringLiteral("SITL UDP 14550")},
                    {QStringLiteral("url"), QStringLiteral("udpin://0.0.0.0:14550")},
                    {QStringLiteral("stack"), QStringLiteral("ArduPilot")}},
        QVariantMap{{QStringLiteral("id"), QStringLiteral("serial-auto")},
                    {QStringLiteral("name"), QStringLiteral("Serial Auto")},
                    {QStringLiteral("url"), QStringLiteral("serial://COMx:57600")},
                    {QStringLiteral("stack"), QStringLiteral("PX4 / ArduPilot")}}
    };
    if (m_vehicle) {
        connect(m_vehicle, &MavsdkVehicleManager::vehicleChanged, this, [this]() {
            rebuildConnectedVehicles();
            emit multiVehicleChanged();
        });
    }
    if (m_telemetry) {
        connect(m_telemetry, &VehicleTelemetryModel::uiTelemetryChanged, this, [this]() {
            rebuildConnectedVehicles();
            emit multiVehicleChanged();
        });
    }
    rebuildConnectedVehicles();
}

QVariantList MultiVehicleManager::connectionProfiles() const { return m_connectionProfiles; }
QVariantList MultiVehicleManager::connectedVehicles() const { return m_connectedVehicles; }
QString MultiVehicleManager::activeVehicleId() const { return m_activeVehicleId; }
QString MultiVehicleManager::status() const { return m_status; }

void MultiVehicleManager::refreshVehicles()
{
    rebuildConnectedVehicles();
    setStatus(QStringLiteral("Vehicle list refreshed from current MAVSDK state."));
    emit multiVehicleChanged();
}

void MultiVehicleManager::selectVehicle(const QString &vehicleId)
{
    if (m_access && !m_access->authorizeAction(QStringLiteral("multi_vehicle"),
                                                QVariantMap{{QStringLiteral("vehicle_id"), vehicleId}},
                                                QStringLiteral("Vehicle selection blocked by local permissions."))) {
        setStatus(QStringLiteral("Multi-vehicle selection blocked by RBAC."));
        return;
    }
    for (const QVariant &entry : m_connectedVehicles) {
        const QVariantMap row = entry.toMap();
        if (row.value(QStringLiteral("id")).toString() == vehicleId) {
            m_activeVehicleId = vehicleId;
            setStatus(QStringLiteral("Active vehicle selected: %1.").arg(row.value(QStringLiteral("name")).toString()));
            emit multiVehicleChanged();
            if (m_events) {
                m_events->recordEvent(QStringLiteral("multi_vehicle_selected"),
                                      QStringLiteral("info"),
                                      QStringLiteral("Active vehicle selected"),
                                      QJsonObject{{QStringLiteral("vehicle_id"), vehicleId}});
            }
            return;
        }
    }
    setStatus(QStringLiteral("Vehicle is not available in the current session."));
}

void MultiVehicleManager::addConnectionProfile(const QVariantMap &profile)
{
    QVariantMap row = profile;
    if (!row.contains(QStringLiteral("id"))) {
        row[QStringLiteral("id")] = QStringLiteral("profile-%1").arg(m_connectionProfiles.size() + 1);
    }
    if (!row.contains(QStringLiteral("name"))) {
        row[QStringLiteral("name")] = QStringLiteral("Connection Profile");
    }
    m_connectionProfiles << row;
    setStatus(QStringLiteral("Connection profile added."));
    emit multiVehicleChanged();
}

void MultiVehicleManager::removeConnectionProfile(const QString &profileId)
{
    for (int i = 0; i < m_connectionProfiles.size(); ++i) {
        if (m_connectionProfiles.at(i).toMap().value(QStringLiteral("id")).toString() == profileId) {
            m_connectionProfiles.removeAt(i);
            setStatus(QStringLiteral("Connection profile removed."));
            emit multiVehicleChanged();
            return;
        }
    }
    setStatus(QStringLiteral("Connection profile not found."));
}

void MultiVehicleManager::rebuildConnectedVehicles()
{
    m_connectedVehicles.clear();
    if (!m_vehicle || !m_telemetry || !m_vehicle->connected()) {
        m_connectedVehicles << QVariantMap{{QStringLiteral("id"), QStringLiteral("primary")},
                                           {QStringLiteral("name"), QStringLiteral("No Connected Aircraft")},
                                           {QStringLiteral("active"), true},
                                           {QStringLiteral("connected"), false},
                                           {QStringLiteral("missionState"), QStringLiteral("Idle")},
                                           {QStringLiteral("logState"), QStringLiteral("No logs")},
                                           {QStringLiteral("updatedAt"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}};
        return;
    }
    m_connectedVehicles << QVariantMap{{QStringLiteral("id"), QStringLiteral("primary")},
                                       {QStringLiteral("name"), m_telemetry->aircraftId()},
                                       {QStringLiteral("active"), m_activeVehicleId == QStringLiteral("primary")},
                                       {QStringLiteral("connected"), true},
                                       {QStringLiteral("systemId"), m_vehicle->systemId()},
                                       {QStringLiteral("autopilot"), m_vehicle->autopilot()},
                                       {QStringLiteral("telemetryState"), m_vehicle->status()},
                                       {QStringLiteral("missionState"), m_telemetry->flightMode()},
                                       {QStringLiteral("logState"), QStringLiteral("Ready")},
                                       {QStringLiteral("battery"), m_telemetry->battery()},
                                       {QStringLiteral("updatedAt"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}};
}

void MultiVehicleManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit multiVehicleChanged();
}
