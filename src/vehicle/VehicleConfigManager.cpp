#include "VehicleConfigManager.h"

#include "MavsdkVehicleManager.h"
#include "VehicleProfileManager.h"
#include "../access/PermissionManager.h"
#include "../auth/SessionManager.h"
#include "../network/ApiClient.h"
#include "../sync/GcsEventSyncManager.h"

#include <QDateTime>
#include <QJsonObject>

VehicleConfigManager::VehicleConfigManager(MavsdkVehicleManager *vehicle,
                                           VehicleProfileManager *profiles,
                                           ApiClient *api,
                                           SessionManager *session,
                                           PermissionManager *permissions,
                                           GcsEventSyncManager *events,
                                           QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_profiles(profiles),
      m_api(api),
      m_session(session),
      m_permissions(permissions),
      m_events(events)
{
}

QString VehicleConfigManager::connectionMode() const { return m_connectionMode; }
QString VehicleConfigManager::serialPort() const { return m_serialPort; }
int VehicleConfigManager::baudRate() const { return m_baudRate; }
QString VehicleConfigManager::udpHost() const { return m_udpHost; }
int VehicleConfigManager::udpPort() const { return m_udpPort; }
QString VehicleConfigManager::tcpHost() const { return m_tcpHost; }
int VehicleConfigManager::tcpPort() const { return m_tcpPort; }
QString VehicleConfigManager::connectionUrl() const { return buildConnectionUrl(); }
QString VehicleConfigManager::status() const { return m_status; }
QVariantMap VehicleConfigManager::parameterSnapshot() const { return m_parameterSnapshot; }
QVariantMap VehicleConfigManager::rcMapping() const { return m_rcMapping; }
QVariantList VehicleConfigManager::auditTrail() const { return m_auditTrail; }

void VehicleConfigManager::setConnectionMode(const QString &mode)
{
    const QString normalized = mode.trimmed().toUpper();
    const QString next = normalized == QStringLiteral("SERIAL") || normalized == QStringLiteral("TCP")
        ? normalized
        : QStringLiteral("UDP");
    if (m_connectionMode == next) {
        return;
    }
    m_connectionMode = next;
    emit configChanged();
}

void VehicleConfigManager::setSerialPort(const QString &port)
{
    const QString next = port.trimmed();
    if (m_serialPort == next) {
        return;
    }
    m_serialPort = next;
    emit configChanged();
}

void VehicleConfigManager::setBaudRate(int baudRate)
{
    const int next = qBound(9600, baudRate, 921600);
    if (m_baudRate == next) {
        return;
    }
    m_baudRate = next;
    emit configChanged();
}

void VehicleConfigManager::setUdpHost(const QString &host)
{
    const QString next = host.trimmed().isEmpty() ? QStringLiteral("0.0.0.0") : host.trimmed();
    if (m_udpHost == next) {
        return;
    }
    m_udpHost = next;
    emit configChanged();
}

void VehicleConfigManager::setUdpPort(int port)
{
    const int next = qBound(1, port, 65535);
    if (m_udpPort == next) {
        return;
    }
    m_udpPort = next;
    emit configChanged();
}

void VehicleConfigManager::setTcpHost(const QString &host)
{
    const QString next = host.trimmed().isEmpty() ? QStringLiteral("127.0.0.1") : host.trimmed();
    if (m_tcpHost == next) {
        return;
    }
    m_tcpHost = next;
    emit configChanged();
}

void VehicleConfigManager::setTcpPort(int port)
{
    const int next = qBound(1, port, 65535);
    if (m_tcpPort == next) {
        return;
    }
    m_tcpPort = next;
    emit configChanged();
}

void VehicleConfigManager::connectVehicle()
{
    if (!requirePermission(QStringLiteral("can_bind_flight_controller"),
                           QStringLiteral("Vehicle connection changes are not permitted."))) {
        return;
    }
    if (!m_vehicle) {
        setStatus(QStringLiteral("MAVSDK vehicle service is unavailable."));
        return;
    }

    const QString url = buildConnectionUrl();
    qputenv("SKYGRID_MAVSDK_URLS", url.toUtf8());
    qputenv("SKYGRID_MAVSDK_ALLOW_MULTIPLE_URLS", "0");
    m_vehicle->connectToUrl(url, false);
    appendAudit(QStringLiteral("vehicle_connection_requested"),
                QStringLiteral("Vehicle discovery started from configuration workspace"),
                connectionPayload());
    setStatus(QStringLiteral("Connecting with %1. No arm, takeoff, or motor test commands are sent.").arg(url));
}

void VehicleConfigManager::disconnectVehicle()
{
    if (!m_vehicle) {
        return;
    }
    m_vehicle->stopDiscovery();
    appendAudit(QStringLiteral("vehicle_connection_stopped"),
                QStringLiteral("Vehicle discovery stopped from configuration workspace"));
    setStatus(QStringLiteral("Vehicle discovery stopped."));
}

void VehicleConfigManager::bindFlightController()
{
    if (!requirePermission(QStringLiteral("can_bind_flight_controller"),
                           QStringLiteral("Binding flight controllers is not permitted."))) {
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || m_vehicle->systemId().isEmpty()) {
        setStatus(QStringLiteral("Connect a flight controller before binding."));
        return;
    }
    QJsonObject payload = QJsonObject::fromVariantMap(connectionPayload());
    payload.insert(QStringLiteral("flight_controller_uid"), m_vehicle->systemId());
    payload.insert(QStringLiteral("autopilot"), m_vehicle->autopilot());
    postProfileAction(QStringLiteral("bind-flight-controller"),
                      payload,
                      QStringLiteral("Flight controller bound to vehicle profile."));
}

void VehicleConfigManager::readParameterSnapshot()
{
    if (!requirePermission(QStringLiteral("can_read_vehicle_parameters"),
                           QStringLiteral("Reading vehicle parameters is not permitted."))) {
        return;
    }
    QVariantMap snapshot{
        {QStringLiteral("captured_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)},
        {QStringLiteral("source"), QStringLiteral("mavsdk_telemetry_snapshot")},
        {QStringLiteral("system_id"), m_vehicle ? m_vehicle->systemId() : QString()},
        {QStringLiteral("autopilot"), m_vehicle ? m_vehicle->autopilot() : QStringLiteral("Unknown")},
        {QStringLiteral("connection_url"), buildConnectionUrl()},
        {QStringLiteral("connected"), m_vehicle && m_vehicle->connected()},
        {QStringLiteral("armed"), m_vehicle && m_vehicle->armed()},
        {QStringLiteral("in_air"), m_vehicle && m_vehicle->inAir()},
        {QStringLiteral("flight_mode"), m_vehicle ? m_vehicle->flightMode() : QStringLiteral("DISCONNECTED")},
        {QStringLiteral("health"), m_vehicle ? m_vehicle->health() : QStringLiteral("Unknown")},
        {QStringLiteral("safe_mode"), QStringLiteral("read_only_no_motor_commands")}
    };
    setParameterSnapshot(snapshot);
    appendAudit(QStringLiteral("vehicle_parameter_snapshot_read"),
                QStringLiteral("Read-only vehicle status snapshot captured"),
                snapshot);
    setStatus(QStringLiteral("Read-only parameter snapshot captured."));
}

void VehicleConfigManager::saveParameterSnapshot(const QVariantMap &snapshot)
{
    if (!requirePermission(QStringLiteral("can_write_vehicle_parameters"),
                           QStringLiteral("Saving vehicle parameter snapshots is not permitted."))) {
        return;
    }
    const QVariantMap payloadMap = snapshot.isEmpty() ? m_parameterSnapshot : snapshot;
    if (payloadMap.isEmpty()) {
        setStatus(QStringLiteral("Capture a parameter snapshot before saving."));
        return;
    }
    postProfileAction(QStringLiteral("save-parameter-snapshot"),
                      QJsonObject{{QStringLiteral("parameter_snapshot"), QJsonObject::fromVariantMap(payloadMap)}},
                      QStringLiteral("Vehicle parameter snapshot saved."));
}

void VehicleConfigManager::saveRcMapping(const QVariantMap &mapping)
{
    if (!requirePermission(QStringLiteral("can_configure_rc"),
                           QStringLiteral("RC mapping changes are not permitted."))) {
        return;
    }
    setRcMapping(mapping);
    postProfileAction(QStringLiteral("save-rc-mapping"),
                      QJsonObject{{QStringLiteral("rc_mapping"), QJsonObject::fromVariantMap(mapping)}},
                      QStringLiteral("RC mapping saved."));
}

void VehicleConfigManager::releaseActiveProfile(int organizationId, const QString &notes)
{
    if (!requirePermission(QStringLiteral("can_release_vehicle_to_organization"),
                           QStringLiteral("Releasing vehicles to organizations is not permitted."))) {
        return;
    }
    if (organizationId <= 0) {
        setStatus(QStringLiteral("Enter a valid organization id before release."));
        return;
    }
    postProfileAction(QStringLiteral("release-to-organization"),
                      QJsonObject{
                          {QStringLiteral("organization"), organizationId},
                          {QStringLiteral("notes"), notes}
                      },
                      QStringLiteral("Vehicle profile released to organization."));
}

bool VehicleConfigManager::requirePermission(const QString &permission, const QString &message)
{
    if (m_permissions && m_permissions->hasPermission(permission)) {
        return true;
    }
    setStatus(message);
    appendAudit(QStringLiteral("permission_denied"), message, {{QStringLiteral("permission"), permission}});
    return false;
}

bool VehicleConfigManager::requireActiveProfile(QString *profileId)
{
    const QString id = m_profiles ? m_profiles->activeProfileId() : QString();
    if (id.isEmpty()) {
        setStatus(QStringLiteral("Select or create a vehicle profile first."));
        return false;
    }
    if (profileId) {
        *profileId = id;
    }
    return true;
}

QString VehicleConfigManager::buildConnectionUrl() const
{
    if (m_connectionMode == QStringLiteral("SERIAL")) {
        QString serialPort = m_serialPort.trimmed();
        if (serialPort.startsWith(QStringLiteral("\\\\.\\"))) {
            serialPort = serialPort.mid(4);
        }
        return QStringLiteral("serial://%1:%2").arg(serialPort, QString::number(m_baudRate));
    }
    if (m_connectionMode == QStringLiteral("TCP")) {
        return QStringLiteral("tcp://%1:%2").arg(m_tcpHost, QString::number(m_tcpPort));
    }
    return QStringLiteral("udpin://%1:%2").arg(m_udpHost, QString::number(m_udpPort));
}

QVariantMap VehicleConfigManager::connectionPayload() const
{
    return QVariantMap{
        {QStringLiteral("connection_mode"), m_connectionMode},
        {QStringLiteral("connection_url"), buildConnectionUrl()},
        {QStringLiteral("serial_port"), m_serialPort},
        {QStringLiteral("baud_rate"), m_baudRate},
        {QStringLiteral("udp_host"), m_udpHost},
        {QStringLiteral("udp_port"), m_udpPort},
        {QStringLiteral("tcp_host"), m_tcpHost},
        {QStringLiteral("tcp_port"), m_tcpPort},
    };
}

void VehicleConfigManager::postProfileAction(const QString &action,
                                             const QJsonObject &payload,
                                             const QString &successStatus)
{
    QString profileId;
    if (!requireActiveProfile(&profileId)) {
        return;
    }
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Vehicle configuration sync blocked: Control Center session unavailable."));
        return;
    }

    setStatus(QStringLiteral("Syncing vehicle configuration action..."));
    m_api->post(QStringLiteral("/api/vehicle/profiles/%1/%2/").arg(profileId, action),
                payload,
                true,
                true,
                [this, action, successStatus, payload](int statusCode, const QJsonObject &body, const QString &error) {
        if (statusCode < 200 || statusCode >= 300) {
            setStatus(error.isEmpty() ? QStringLiteral("Vehicle configuration action failed.") : error);
            appendAudit(action + QStringLiteral("_failed"), m_status, payload.toVariantMap());
            return;
        }
        appendAudit(action, successStatus, body.toVariantMap());
        setStatus(successStatus);
        if (m_profiles) {
            m_profiles->refreshProfiles();
        }
    });
}

void VehicleConfigManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit configChanged();
}

void VehicleConfigManager::setParameterSnapshot(const QVariantMap &snapshot)
{
    if (m_parameterSnapshot == snapshot) {
        return;
    }
    m_parameterSnapshot = snapshot;
    emit configChanged();
}

void VehicleConfigManager::setRcMapping(const QVariantMap &mapping)
{
    if (m_rcMapping == mapping) {
        return;
    }
    m_rcMapping = mapping;
    emit configChanged();
}

void VehicleConfigManager::appendAudit(const QString &action, const QString &message, const QVariantMap &details)
{
    QVariantMap entry{
        {QStringLiteral("action"), action},
        {QStringLiteral("message"), message},
        {QStringLiteral("created_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)},
        {QStringLiteral("details"), details}
    };
    m_auditTrail.prepend(entry);
    while (m_auditTrail.size() > 50) {
        m_auditTrail.removeLast();
    }
    if (m_events) {
        m_events->recordEvent(action, action.contains(QStringLiteral("failed")) || action == QStringLiteral("permission_denied")
                                    ? QStringLiteral("warning")
                                    : QStringLiteral("info"),
                              message,
                              QJsonObject::fromVariantMap(details));
    }
    emit configChanged();
}
