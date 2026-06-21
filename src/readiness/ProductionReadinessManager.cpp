#include "ProductionReadinessManager.h"

#include "../auth/SessionManager.h"
#include "../firmware/FirmwareUpdateManager.h"
#include "../logs/LogAnalysisManager.h"
#include "../mission/AdvancedMissionManager.h"
#include "../parameters/ParameterManager.h"
#include "../payload/VideoStreamManager.h"
#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"

#include <QJsonObject>

ProductionReadinessManager::ProductionReadinessManager(MavsdkVehicleManager *vehicle,
                                                       ParameterManager *parameters,
                                                       AdvancedMissionManager *missions,
                                                       FirmwareUpdateManager *firmware,
                                                       LogAnalysisManager *logs,
                                                       VideoStreamManager *video,
                                                       SessionManager *session,
                                                       AccessManager *access,
                                                       GcsEventSyncManager *events,
                                                       QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_parameters(parameters),
      m_missions(missions),
      m_firmware(firmware),
      m_logs(logs),
      m_video(video),
      m_session(session),
      m_access(access),
      m_events(events)
{
    if (m_vehicle) {
        connect(m_vehicle, &MavsdkVehicleManager::vehicleChanged, this, &ProductionReadinessManager::refresh);
    }
    if (m_parameters) {
        connect(m_parameters, &ParameterManager::parametersChanged, this, &ProductionReadinessManager::refresh);
    }
    if (m_missions) {
        connect(m_missions, &AdvancedMissionManager::missionChanged, this, &ProductionReadinessManager::refresh);
    }
    if (m_firmware) {
        connect(m_firmware, &FirmwareUpdateManager::firmwareChanged, this, &ProductionReadinessManager::refresh);
    }
    if (m_logs) {
        connect(m_logs, &LogAnalysisManager::logsChanged, this, &ProductionReadinessManager::refresh);
    }
    if (m_video) {
        connect(m_video, &VideoStreamManager::videoChanged, this, &ProductionReadinessManager::refresh);
    }
    if (m_access) {
        connect(m_access, &AccessManager::accessChanged, this, &ProductionReadinessManager::refresh);
    }
    rebuild();
}

QVariantList ProductionReadinessManager::readinessRows() const { return m_rows; }
QVariantList ProductionReadinessManager::blockers() const { return m_blockers; }
int ProductionReadinessManager::readinessPercent() const { return m_readinessPercent; }
bool ProductionReadinessManager::readyForProduction() const { return m_readyForProduction; }
QString ProductionReadinessManager::status() const { return m_status; }

void ProductionReadinessManager::refresh()
{
    rebuild();
}

void ProductionReadinessManager::recordFieldValidation(const QString &area, bool passed, const QString &notes)
{
    const QString cleanArea = area.trimmed().isEmpty() ? QStringLiteral("field_validation") : area.trimmed();
    setStatus(QStringLiteral("%1 validation %2: %3")
                  .arg(cleanArea,
                       passed ? QStringLiteral("passed") : QStringLiteral("failed"),
                       notes.trimmed()));
    audit(QStringLiteral("production_field_validation"),
          passed ? QStringLiteral("info") : QStringLiteral("warning"),
          m_status,
          cleanArea,
          passed);
    refresh();
}

QVariantMap ProductionReadinessManager::row(const QString &area,
                                            const QString &state,
                                            const QString &detail,
                                            bool blocker) const
{
    return QVariantMap{{QStringLiteral("name"), area},
                       {QStringLiteral("area"), area},
                       {QStringLiteral("value"), state},
                       {QStringLiteral("state"), state},
                       {QStringLiteral("detail"), detail},
                       {QStringLiteral("blocker"), blocker}};
}

void ProductionReadinessManager::rebuild()
{
    QVariantList rows;
    const bool connected = m_vehicle && m_vehicle->connected();
    const bool trustedSession = m_session && m_session->operationsAllowed();
    const bool accessReady = m_access && (m_access->accessLoaded() || m_access->offlineAuthorizationValid());
    const int parameterCount = m_parameters ? m_parameters->parameters().size() : 0;
    const bool criticalState = m_vehicle && (m_vehicle->armed() || m_vehicle->inAir());
    const int missionCount = m_missions ? m_missions->missionItems().size() : 0;
    const int logCount = m_logs ? m_logs->logEntries().size() : 0;
    const bool videoReady = m_video && m_video->connected();
    const bool videoConfigured = m_video && m_video->hasSource();
    const bool firmwareFlashSupported = m_firmware && m_firmware->supported();

    rows << row(QStringLiteral("Command Center / Session"),
                trustedSession && accessReady ? QStringLiteral("Ready") : QStringLiteral("Blocked"),
                trustedSession && accessReady
                    ? QStringLiteral("Trusted session and cached access are available.")
                    : QStringLiteral("Login, trusted-device state, or cached permissions are not ready."),
                !(trustedSession && accessReady));
    rows << row(QStringLiteral("Flight Controller Connection"),
                connected ? QStringLiteral("Ready") : QStringLiteral("Blocked"),
                connected
                    ? QStringLiteral("MAVSDK heartbeat received from %1 system %2.").arg(m_vehicle->autopilot(), m_vehicle->systemId())
                    : QStringLiteral("Connect a PX4/ArduPilot controller over selected serial, UDP, or TCP link."),
                !connected);
    rows << row(QStringLiteral("Parameter Setup"),
                parameterCount > 0 ? QStringLiteral("Ready") : QStringLiteral("Blocked"),
                parameterCount > 0
                    ? QStringLiteral("%1 parameters loaded with safety-gated writes.").arg(parameterCount)
                    : QStringLiteral("Read all parameters from a real vehicle or SITL before production setup."),
                parameterCount == 0);
    rows << row(QStringLiteral("Mission Transfer"),
                connected ? QStringLiteral("Adapter ready") : QStringLiteral("Blocked"),
                missionCount > 0
                    ? QStringLiteral("%1 raw mission items available for upload/download workflow.").arg(missionCount)
                    : QStringLiteral("MAVSDK MissionRaw path is wired; field validation needs real mission read/write."),
                !connected);
    rows << row(QStringLiteral("Firmware Flashing"),
                firmwareFlashSupported ? QStringLiteral("Ready") : QStringLiteral("Blocked"),
                firmwareFlashSupported
                    ? QStringLiteral("Active firmware adapter reports erase/program/verify support.")
                    : QStringLiteral("Serial bootloader erase/program/verify is implemented, but production readiness remains blocked until PX4 and ArduPilot boards are field-tested."),
                !firmwareFlashSupported);
    rows << row(QStringLiteral("Initial Setup / Calibration"),
                connected && !criticalState ? QStringLiteral("Ready") : QStringLiteral("Blocked"),
                connected && !criticalState
                    ? QStringLiteral("MAVSDK calibration commands are available for disarmed bench setup.")
                    : QStringLiteral("Vehicle must be connected, disarmed, and not in air for calibration."),
                !(connected && !criticalState));
    rows << row(QStringLiteral("Payload Video"),
                videoReady ? QStringLiteral("Ready") : (videoConfigured ? QStringLiteral("Needs validation") : QStringLiteral("Blocked")),
                videoReady
                    ? QStringLiteral("RTSP/H264 stream opened through Qt Multimedia.")
                    : (videoConfigured ? QStringLiteral("Video URL configured; connect and field-test latency/reconnect behavior.")
                                       : QStringLiteral("Configure and connect a real RTSP/H264 payload stream.")),
                !videoReady);
    rows << row(QStringLiteral("Logs / Analysis"),
                logCount > 0 ? QStringLiteral("Ready") : (connected ? QStringLiteral("Needs validation") : QStringLiteral("Blocked")),
                logCount > 0
                    ? QStringLiteral("%1 onboard logs listed.").arg(logCount)
                    : QStringLiteral("MAVSDK LogFiles path is wired; list/download logs from a real board."),
                logCount == 0);
    rows << row(QStringLiteral("Safety Interlocks"),
                criticalState ? QStringLiteral("Guarded") : QStringLiteral("Ready"),
                criticalState
                    ? QStringLiteral("Vehicle is armed or in air; setup writes and calibration are blocked.")
                    : QStringLiteral("Dangerous setup actions are blocked unless required preconditions pass."),
                false);
    rows << row(QStringLiteral("Audit / Offline Sync"),
                m_events ? QStringLiteral("Ready") : QStringLiteral("Blocked"),
                m_events ? m_events->status() : QStringLiteral("GCS event sync manager is not available."),
                !m_events);
    rows << row(QStringLiteral("Installer / Release"),
                QStringLiteral("Needs validation"),
                QStringLiteral("Release build and MSI can be generated; production still needs signing, versioning, update strategy, and installed-app smoke tests."),
                true);

    int readyLike = 0;
    QVariantList blockers;
    for (const QVariant &entry : rows) {
        const QVariantMap item = entry.toMap();
        if (item.value(QStringLiteral("state")).toString() == QStringLiteral("Ready")
            || item.value(QStringLiteral("state")).toString() == QStringLiteral("Adapter ready")
            || item.value(QStringLiteral("state")).toString() == QStringLiteral("Guarded")) {
            ++readyLike;
        }
        if (item.value(QStringLiteral("blocker")).toBool()) {
            blockers << item;
        }
    }

    m_rows = rows;
    m_blockers = blockers;
    m_readinessPercent = rows.isEmpty() ? 0 : int((100.0 * readyLike) / rows.size());
    m_readyForProduction = blockers.isEmpty() && m_readinessPercent == 100;
    setStatus(m_readyForProduction
                  ? QStringLiteral("Production readiness complete.")
                  : QStringLiteral("%1% ready. %2 production blocker(s) remain.")
                        .arg(m_readinessPercent)
                        .arg(m_blockers.size()));
    emit readinessChanged();
}

void ProductionReadinessManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit readinessChanged();
}

void ProductionReadinessManager::audit(const QString &eventType,
                                       const QString &severity,
                                       const QString &message,
                                       const QString &area,
                                       bool passed) const
{
    if (!m_events) {
        return;
    }
    m_events->recordEvent(eventType,
                          severity,
                          message,
                          QJsonObject{{QStringLiteral("area"), area},
                                      {QStringLiteral("passed"), passed}});
}
