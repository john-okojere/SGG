#include "AutopilotToolsFacade.h"

#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"

#include <QDateTime>
#include <QJsonObject>
#include <QTimer>

AutopilotToolsFacade::AutopilotToolsFacade(MavsdkVehicleManager *vehicle,
                                           AccessManager *access,
                                           GcsEventSyncManager *events,
                                           QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_access(access),
      m_events(events)
{
    if (m_vehicle) {
        connect(m_vehicle, &MavsdkVehicleManager::vehicleChanged, this, [this]() {
            rebuildBoardInfo();
            refreshCapabilities();
        });
    }
    rebuildBoardInfo();
    refreshCapabilities();
}

QVariantMap AutopilotToolsFacade::boardInfo() const { return m_boardInfo; }
QVariantList AutopilotToolsFacade::capabilities() const { return m_capabilities; }
QString AutopilotToolsFacade::status() const { return m_status; }
bool AutopilotToolsFacade::detecting() const { return m_detecting; }

void AutopilotToolsFacade::detectBoard()
{
    if (m_detecting) {
        return;
    }
    m_detecting = true;
    setStatus(QStringLiteral("Detecting connected flight-controller board."));
    emit facadeChanged();

    QTimer::singleShot(150, this, [this]() {
        rebuildBoardInfo();
        refreshCapabilities();
        m_detecting = false;
        setStatus(m_vehicle && m_vehicle->connected()
                      ? QStringLiteral("Board metadata detected from active MAVSDK system.")
                      : QStringLiteral("No connected controller detected."));
        if (m_events) {
            m_events->recordEvent(QStringLiteral("board_info_detected"),
                                  QStringLiteral("info"),
                                  QStringLiteral("Autopilot board metadata refreshed"),
                                  QJsonObject::fromVariantMap(m_boardInfo));
        }
        emit facadeChanged();
    });
}

void AutopilotToolsFacade::refreshCapabilities()
{
    const bool connected = m_vehicle && m_vehicle->connected();
    const bool canParamRead = connected && m_access && m_access->canPerform(QStringLiteral("vehicle_parameter_read"));
    const bool canParamWrite = connected && m_access && m_access->canPerform(QStringLiteral("vehicle_parameter_write"));
    const bool canMission = connected && m_access && m_access->canPerform(QStringLiteral("advanced_mission_editor"));
    const bool canLogs = connected && m_access && m_access->canPerform(QStringLiteral("logs_analysis"));
    const bool canAdvanced = connected && m_access && m_access->canPerform(QStringLiteral("advanced_mavlink"));
    const bool canSetup = connected && m_access && m_access->canPerform(QStringLiteral("initial_setup"));
    const bool canPayload = connected && m_access && m_access->canPerform(QStringLiteral("payload_configuration"));
    const bool canOptional = connected && m_access && m_access->canPerform(QStringLiteral("optional_hardware"));
    const bool canTerminal = connected && m_access && m_access->canPerform(QStringLiteral("terminal"));
    const bool canFirmware = m_access && m_access->canPerform(QStringLiteral("firmware_manager"));

    m_capabilities = {
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Board detection")},
                    {QStringLiteral("state"), connected ? QStringLiteral("ready") : QStringLiteral("waiting")},
                    {QStringLiteral("supported"), true},
                    {QStringLiteral("detail"), connected ? QStringLiteral("Connected system metadata available.")
                                                          : QStringLiteral("Connect an aircraft to detect board metadata.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Parameter read")},
                    {QStringLiteral("state"), canParamRead ? QStringLiteral("ready") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canParamRead},
                    {QStringLiteral("detail"), QStringLiteral("Uses MAVSDK Param where available.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Parameter write")},
                    {QStringLiteral("state"), canParamWrite ? QStringLiteral("guarded") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canParamWrite},
                    {QStringLiteral("detail"), QStringLiteral("Blocked during active mission/manual critical states without advanced override.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Raw mission transfer")},
                    {QStringLiteral("state"), canMission ? QStringLiteral("adapter-ready") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canMission},
                    {QStringLiteral("detail"), QStringLiteral("MAVSDK MissionRaw backed where connected.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Onboard log listing")},
                    {QStringLiteral("state"), canLogs ? QStringLiteral("adapter-ready") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canLogs},
                    {QStringLiteral("detail"), QStringLiteral("MAVSDK LogFiles listing is attempted when connected.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Sensor calibration")},
                    {QStringLiteral("state"), canSetup ? QStringLiteral("adapter-ready") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canSetup},
                    {QStringLiteral("detail"), QStringLiteral("MAVSDK Calibration is used for gyro, accelerometer, compass, level horizon, and gimbal accelerometer where supported.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Camera / gimbal commands")},
                    {QStringLiteral("state"), canPayload ? QStringLiteral("adapter-ready") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canPayload},
                    {QStringLiteral("detail"), QStringLiteral("MAVSDK Camera and Gimbal commands are attempted on connected MAVLink payloads.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("RTK correction injection")},
                    {QStringLiteral("state"), canOptional ? QStringLiteral("adapter-ready") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canOptional},
                    {QStringLiteral("detail"), QStringLiteral("MAVSDK RTK sends base64 RTCM payloads when a correction source is configured.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Autopilot shell")},
                    {QStringLiteral("state"), canTerminal ? QStringLiteral("guarded") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canTerminal},
                    {QStringLiteral("detail"), QStringLiteral("MAVSDK Shell sends terminal commands only for authorized users and supported autopilots.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Firmware erase/program/verify")},
                    {QStringLiteral("state"), canFirmware ? QStringLiteral("guarded") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canFirmware},
                    {QStringLiteral("detail"), QStringLiteral("Serial PX4/ArduPilot bootloader sync, erase, program, CRC verify, and reboot are implemented; production approval still requires real board validation.")}},
        QVariantMap{{QStringLiteral("name"), QStringLiteral("Advanced MAVLink diagnostics")},
                    {QStringLiteral("state"), canAdvanced ? QStringLiteral("adapter-shell") : QStringLiteral("restricted")},
                    {QStringLiteral("supported"), canAdvanced},
                    {QStringLiteral("detail"), QStringLiteral("Inspector, sender, MAVFTP, terminal, DroneCAN, and signing shells are prepared.")}}
    };
    emit facadeChanged();
}

void AutopilotToolsFacade::markUnsupported(const QString &feature)
{
    setStatus(QStringLiteral("%1 is not supported by the active v1 adapter.").arg(feature.trimmed()));
    if (m_events) {
        m_events->recordEvent(QStringLiteral("autopilot_tool_unsupported"),
                              QStringLiteral("warning"),
                              QStringLiteral("Autopilot adapter operation unsupported"),
                              QJsonObject{{QStringLiteral("feature"), feature}});
    }
}

void AutopilotToolsFacade::rebuildBoardInfo()
{
    const bool connected = m_vehicle && m_vehicle->connected();
    const QString autopilot = connected ? m_vehicle->autopilot() : QStringLiteral("Unknown");
    QString family = QStringLiteral("Unknown");
    const QString lower = autopilot.toLower();
    if (lower.contains(QStringLiteral("px4"))) {
        family = QStringLiteral("PX4");
    } else if (lower.contains(QStringLiteral("ardu"))
               || lower.contains(QStringLiteral("apm"))
               || lower.contains(QStringLiteral("ardupilot"))) {
        family = QStringLiteral("ArduPilot");
    }

    m_boardInfo = QVariantMap{
        {QStringLiteral("connected"), connected},
        {QStringLiteral("systemId"), connected ? m_vehicle->systemId() : QStringLiteral("--")},
        {QStringLiteral("autopilot"), autopilot},
        {QStringLiteral("stack"), family},
        {QStringLiteral("connectionUrl"), connected ? m_vehicle->connectionUrl() : QStringLiteral("--")},
        {QStringLiteral("health"), connected ? m_vehicle->health() : QStringLiteral("Unknown")},
        {QStringLiteral("flightMode"), connected ? m_vehicle->flightMode() : QStringLiteral("DISCONNECTED")},
        {QStringLiteral("detectedAt"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
    };
}

void AutopilotToolsFacade::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit facadeChanged();
}
