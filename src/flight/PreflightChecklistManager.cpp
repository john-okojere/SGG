#include "PreflightChecklistManager.h"

#include "../auth/SessionManager.h"
#include "../controllers/AppState.h"
#include "../models/MissionPlanModel.h"
#include "../network/ApiClient.h"
#include "../sync/GcsEventSyncManager.h"
#include "../telemetry/WeatherManager.h"
#include "../telemetry/WindCheckManager.h"
#include "../vehicle/VehicleTelemetryModel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>

#include <cmath>

namespace {

bool severityBlocks(const QString &severity)
{
    return severity == QStringLiteral("critical");
}

bool severityWarns(const QString &severity)
{
    return severity == QStringLiteral("warning");
}

bool numericId(const QString &value)
{
    if (value.isEmpty()) {
        return false;
    }
    for (const QChar &ch : value) {
        if (!ch.isDigit()) {
            return false;
        }
    }
    return true;
}

} // namespace

PreflightChecklistManager::PreflightChecklistManager(SessionManager *session,
                                                     ApiClient *api,
                                                     VehicleTelemetryModel *telemetry,
                                                     MissionPlanModel *plan,
                                                     AppState *appState,
                                                     WeatherManager *weather,
                                                     WindCheckManager *windCheck,
                                                     GcsEventSyncManager *events,
                                                     QObject *parent)
    : QObject(parent),
      m_session(session),
      m_api(api),
      m_telemetry(telemetry),
      m_plan(plan),
      m_appState(appState),
      m_weather(weather),
      m_windCheck(windCheck),
      m_events(events)
{
    if (m_telemetry) {
        connect(m_telemetry, &VehicleTelemetryModel::telemetryChanged, this, [this]() { runChecklist(false); });
    }
    if (m_plan) {
        connect(m_plan, &MissionPlanModel::validationChanged, this, [this]() { runChecklist(false); });
    }
    if (m_windCheck) {
        connect(m_windCheck, &WindCheckManager::windCheckChanged, this, [this]() { runChecklist(false); });
    }
    if (m_weather) {
        connect(m_weather, &WeatherManager::weatherChanged, this, [this]() { runChecklist(false); });
    }
    if (m_session) {
        connect(m_session, &SessionManager::sessionStateChanged, this, [this]() { runChecklist(false); });
    }
    if (m_appState) {
        connect(m_appState, &AppState::operationalModeChanged, this, [this]() { runChecklist(false); });
    }
    runChecklist(false);
}

QVariantList PreflightChecklistManager::checks() const { return m_checks; }
int PreflightChecklistManager::readinessPercent() const { return m_readinessPercent; }
bool PreflightChecklistManager::blocked() const { return m_blocked; }
bool PreflightChecklistManager::cautionRequired() const { return m_cautionRequired; }
bool PreflightChecklistManager::canUpload() const { return m_canUpload; }
bool PreflightChecklistManager::canArm() const { return m_canArm; }
bool PreflightChecklistManager::canStartMission() const { return m_canStartMission; }
QString PreflightChecklistManager::status() const { return m_status; }
QString PreflightChecklistManager::lastRunAt() const { return m_lastRunAt; }

void PreflightChecklistManager::runChecklist(bool logEvents)
{
    rebuildChecks();
    m_lastRunAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
    if (logEvents) {
        setStatus(QStringLiteral("Running preflight checks."));
    }

    if (logEvents && m_events) {
        for (const QVariant &entry : m_checks) {
            logCheckEvent(entry.toMap());
        }
        m_events->recordEvent(QStringLiteral("preflight_checklist_completed"),
                              m_blocked ? QStringLiteral("error") : (m_cautionRequired ? QStringLiteral("warning") : QStringLiteral("info")),
                              m_blocked ? QStringLiteral("Preflight checklist blocked flight operations")
                                        : (m_cautionRequired ? QStringLiteral("Preflight checklist completed with cautions")
                                                               : QStringLiteral("Preflight checklist passed")),
                              QJsonObject{
                                  {QStringLiteral("readiness_percent"), m_readinessPercent},
                                  {QStringLiteral("blocked"), m_blocked},
                                  {QStringLiteral("caution_required"), m_cautionRequired},
                                  {QStringLiteral("check_count"), m_checks.size()}
                              });
        syncChecklistRun();
    }

    if (m_blocked) {
        const QString reason = blockReason();
        setStatus(reason.isEmpty() ? QStringLiteral("Preflight failed.") : QStringLiteral("Preflight failed: %1").arg(reason));
    } else if (m_cautionRequired) {
        setStatus(QStringLiteral("Preflight failed: acknowledge caution items before starting."));
    } else {
        setStatus(QStringLiteral("Preflight checks passed (%1%).").arg(m_readinessPercent));
    }
    emit checklistChanged();
    if (logEvents) {
        const bool success = !m_blocked && !m_cautionRequired;
        emit checklistCompleted(success, success ? QStringLiteral("Preflight checks passed.") : m_status);
    }
}

void PreflightChecklistManager::acknowledgeCheck(const QString &checkId)
{
    if (checkId.isEmpty()) {
        return;
    }
    m_acknowledgedIds.insert(checkId);
    runChecklist(false);
}

void PreflightChecklistManager::acknowledgeAllCautions()
{
    for (const QVariant &entry : m_checks) {
        const QVariantMap check = entry.toMap();
        if (!check.value(QStringLiteral("passed")).toBool() && severityWarns(check.value(QStringLiteral("severity")).toString())) {
            m_acknowledgedIds.insert(check.value(QStringLiteral("id")).toString());
        }
    }
    runChecklist(false);
}

QString PreflightChecklistManager::blockReason() const
{
    for (const QVariant &entry : m_checks) {
        const QVariantMap check = entry.toMap();
        if (!check.value(QStringLiteral("passed")).toBool() && severityBlocks(check.value(QStringLiteral("severity")).toString())) {
            return QStringLiteral("%1: %2")
                .arg(check.value(QStringLiteral("label")).toString(),
                     check.value(QStringLiteral("message")).toString());
        }
    }
    if (m_cautionRequired) {
        return QStringLiteral("Acknowledge preflight caution items before continuing.");
    }
    return QString();
}

void PreflightChecklistManager::rebuildChecks()
{
    QVariantList checks;
    const bool connected = m_telemetry && m_telemetry->connected();
    const bool trusted = m_session && m_session->operationsAllowed();
    const bool pilotMode = isPilotMode();

    addCheck(checks,
             QStringLiteral("aircraft_connection"),
             QStringLiteral("Aircraft Systems"),
             QStringLiteral("Aircraft Connection"),
             connected,
             QStringLiteral("critical"),
             connected ? QStringLiteral("Aircraft link is online.")
                         : QStringLiteral("Connect an aircraft before flight operations."));

    addCheck(checks,
             QStringLiteral("aircraft_health"),
             QStringLiteral("Aircraft Systems"),
             QStringLiteral("System Health"),
             !connected || m_telemetry->obstacleState() == QStringLiteral("OK"),
             QStringLiteral("critical"),
             !connected ? QStringLiteral("Health will be evaluated after aircraft connection.")
                          : (m_telemetry->obstacleState() == QStringLiteral("OK")
                                 ? QStringLiteral("Aircraft systems report nominal status.")
                                 : QStringLiteral("Aircraft health requires operator review.")));

    const int battery = m_telemetry ? m_telemetry->battery() : 0;
    addCheck(checks,
             QStringLiteral("battery_reserve"),
             QStringLiteral("Aircraft Systems"),
             QStringLiteral("Battery Reserve"),
             !connected || battery > 25,
             QStringLiteral("critical"),
             !connected ? QStringLiteral("Battery state pending aircraft connection.")
                          : QStringLiteral("Battery at %1%.").arg(battery));

    const int satellites = m_telemetry ? m_telemetry->satellites() : 0;
    addCheck(checks,
             QStringLiteral("gps_fix"),
             QStringLiteral("Aircraft Systems"),
             QStringLiteral("GNSS Fix"),
             !connected || satellites >= 8,
             QStringLiteral("critical"),
             !connected ? QStringLiteral("GNSS state pending aircraft connection.")
                          : QStringLiteral("%1 satellites · %2.")
                                .arg(satellites)
                                .arg(m_telemetry->gpsMode()));

    const QString cameraState = m_telemetry ? m_telemetry->recordingState() : QStringLiteral("NO CAM");
    addCheck(checks,
             QStringLiteral("payload_camera"),
             QStringLiteral("Payload Systems"),
             QStringLiteral("Camera / Payload"),
             !connected || cameraState != QStringLiteral("NO CAM"),
             QStringLiteral("warning"),
             !connected ? QStringLiteral("Payload status pending aircraft connection.")
                          : QStringLiteral("Camera state: %1.").arg(cameraState));

    addCheck(checks,
             QStringLiteral("device_trust"),
             QStringLiteral("Communications"),
             QStringLiteral("Control Center Trust"),
             trusted,
             QStringLiteral("critical"),
             trusted ? QStringLiteral("GCS device is approved for operations.")
                       : (m_session ? m_session->blockReason() : QStringLiteral("Device approval is required.")));

    const int rcSignal = m_telemetry ? m_telemetry->rcSignal() : 0;
    addCheck(checks,
             QStringLiteral("rc_link"),
             QStringLiteral("Communications"),
             QStringLiteral("RC Link"),
             !connected || rcSignal >= 50,
             rcSignal >= 30 ? QStringLiteral("warning") : QStringLiteral("critical"),
             !connected ? QStringLiteral("RC link pending aircraft connection.")
                          : QStringLiteral("RC signal at %1% (%2).")
                                .arg(rcSignal)
                                .arg(m_telemetry->rcQuality()));

    const int telemetrySignal = m_telemetry ? m_telemetry->transmission() : 0;
    addCheck(checks,
             QStringLiteral("telemetry_link"),
             QStringLiteral("Communications"),
             QStringLiteral("Telemetry Link"),
             !connected || telemetrySignal >= 50,
             telemetrySignal >= 30 ? QStringLiteral("warning") : QStringLiteral("critical"),
             !connected ? QStringLiteral("Telemetry link pending aircraft connection.")
                          : QStringLiteral("Telemetry signal at %1%.").arg(telemetrySignal));

    const QString windResult = m_windCheck ? m_windCheck->result() : QStringLiteral("Source unavailable");
    const bool weatherDnf = windResult == QStringLiteral("Do Not Fly");
    const bool weatherCaution = windResult == QStringLiteral("Caution");
    addCheck(checks,
             QStringLiteral("weather_envelope"),
             QStringLiteral("Weather"),
             QStringLiteral("Wind / Weather Envelope"),
             !weatherDnf,
             weatherDnf ? QStringLiteral("critical") : (weatherCaution ? QStringLiteral("warning") : QStringLiteral("info")),
             m_windCheck ? m_windCheck->status() : QStringLiteral("Weather source unavailable."));

    if (!pilotMode && m_plan) {
        for (const QVariant &missionCheck : m_plan->validationChecks()) {
            const QVariantMap map = missionCheck.toMap();
            const QString label = map.value(QStringLiteral("label")).toString();
            const bool passed = map.value(QStringLiteral("passed")).toBool();
            const bool critical = map.value(QStringLiteral("critical")).toBool();
            addCheck(checks,
                     QStringLiteral("mission_%1").arg(label.toLower().replace(QLatin1Char(' '), QLatin1Char('_'))),
                     QStringLiteral("Mission Data"),
                     label,
                     passed,
                     critical ? QStringLiteral("critical") : map.value(QStringLiteral("severity"), QStringLiteral("warning")).toString(),
                     map.value(QStringLiteral("message")).toString());
        }

        const bool uploadableRoute = m_plan->serializeForMavsdkMission().size() >= 2
            || m_plan->missionType() == QStringLiteral("virtualFence");
        addCheck(checks,
                 QStringLiteral("mission_upload_route"),
                 QStringLiteral("Mission Data"),
                 m_plan->missionType() == QStringLiteral("virtualFence") ? QStringLiteral("Boundary Sync") : QStringLiteral("Upload Route"),
                 uploadableRoute,
                 m_plan->missionType() == QStringLiteral("virtualFence") ? QStringLiteral("info") : QStringLiteral("critical"),
                 m_plan->missionType() == QStringLiteral("virtualFence")
                     ? QStringLiteral("Virtual fence will sync as a Control Center safety boundary.")
                     : (uploadableRoute ? QStringLiteral("Mission can be converted into uploadable route items.")
                                        : QStringLiteral("Mission needs at least two route items before upload.")));

        addCheck(checks,
                 QStringLiteral("mission_control_center_validation"),
                 QStringLiteral("Mission Data"),
                 QStringLiteral("Control Center Validation"),
                 m_plan->backendSyncReady() && (m_plan->backendUploadEligible() || m_plan->boundaryOnly()),
                 QStringLiteral("critical"),
                 m_plan->backendValidationState() == QStringLiteral("not_validated")
                     ? QStringLiteral("Run Control Center mission validation before flight operations.")
                     : (m_plan->backendUploadEligible() || m_plan->boundaryOnly()
                            ? QStringLiteral("Control Center validation is ready.")
                            : QStringLiteral("Control Center validation needs review.")));
    } else if (!pilotMode) {
        addCheck(checks,
                 QStringLiteral("mission_plan"),
                 QStringLiteral("Mission Data"),
                 QStringLiteral("Mission Plan"),
                 false,
                 QStringLiteral("critical"),
                 QStringLiteral("No active mission plan is loaded."));
    }

    int passedCount = 0;
    m_blocked = false;
    m_cautionRequired = false;
    for (const QVariant &entry : checks) {
        const QVariantMap check = entry.toMap();
        if (check.value(QStringLiteral("passed")).toBool()) {
            ++passedCount;
            continue;
        }
        const QString severity = check.value(QStringLiteral("severity")).toString();
        if (severityBlocks(severity)) {
            m_blocked = true;
        } else if (severityWarns(severity) && !m_acknowledgedIds.contains(check.value(QStringLiteral("id")).toString())) {
            m_cautionRequired = true;
        }
    }

    m_readinessPercent = checks.isEmpty() ? 0 : static_cast<int>(std::round((passedCount * 100.0) / checks.size()));
    const bool opsAllowed = trusted && connected && !m_blocked && !cautionBlocksOperations();
    m_canArm = opsAllowed;
    m_canUpload = opsAllowed && !pilotMode && m_plan && m_plan->missionReady()
        && m_plan->backendSyncReady()
        && m_plan->backendUploadEligible()
        && !m_plan->createdLocally()
        && m_plan->missionType() != QStringLiteral("virtualFence")
        && m_plan->serializeForMavsdkMission().size() >= 2;
    m_canStartMission = opsAllowed && m_plan && m_plan->missionReady()
        && m_plan->backendSyncReady()
        && m_plan->backendUploadEligible()
        && !m_plan->createdLocally()
        && m_plan->missionType() != QStringLiteral("virtualFence")
        && m_plan->serializeForMavsdkMission().size() >= 2
        && m_plan->uploadState() == QStringLiteral("uploaded");

    QVariantList enriched;
    for (const QVariant &entry : checks) {
        QVariantMap check = entry.toMap();
        check[QStringLiteral("acknowledged")] = m_acknowledgedIds.contains(check.value(QStringLiteral("id")).toString());
        enriched << check;
    }
    m_checks = enriched;
}

void PreflightChecklistManager::addCheck(QVariantList &checks,
                                         const QString &id,
                                         const QString &category,
                                         const QString &label,
                                         bool passed,
                                         const QString &severity,
                                         const QString &message) const
{
    checks << QVariantMap{
        {QStringLiteral("id"), id},
        {QStringLiteral("category"), category},
        {QStringLiteral("label"), label},
        {QStringLiteral("passed"), passed},
        {QStringLiteral("severity"), passed ? QStringLiteral("pass") : severity},
        {QStringLiteral("message"), message},
        {QStringLiteral("acknowledged"), m_acknowledgedIds.contains(id)}
    };
}

bool PreflightChecklistManager::isPilotMode() const
{
    return m_appState && m_appState->operationalMode() == QStringLiteral("pilot");
}

bool PreflightChecklistManager::cautionBlocksOperations() const
{
    return m_cautionRequired;
}

void PreflightChecklistManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
}

void PreflightChecklistManager::logCheckEvent(const QVariantMap &check) const
{
    if (!m_events) {
        return;
    }
    const bool passed = check.value(QStringLiteral("passed")).toBool();
    m_events->recordEvent(QStringLiteral("preflight_check"),
                          passed ? QStringLiteral("info")
                                 : (severityBlocks(check.value(QStringLiteral("severity")).toString())
                                        ? QStringLiteral("error")
                                        : QStringLiteral("warning")),
                          QStringLiteral("Preflight %1: %2")
                              .arg(check.value(QStringLiteral("label")).toString(),
                                   passed ? QStringLiteral("PASS") : check.value(QStringLiteral("message")).toString()),
                          QJsonObject{
                              {QStringLiteral("check_id"), check.value(QStringLiteral("id")).toString()},
                              {QStringLiteral("category"), check.value(QStringLiteral("category")).toString()},
                              {QStringLiteral("severity"), check.value(QStringLiteral("severity")).toString()},
                              {QStringLiteral("passed"), passed},
                              {QStringLiteral("message"), check.value(QStringLiteral("message")).toString()}
                          });
}

void PreflightChecklistManager::syncChecklistRun() const
{
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        return;
    }

    QJsonArray checks;
    QJsonArray acknowledgements;
    for (const QVariant &entry : m_checks) {
        const QVariantMap check = entry.toMap();
        QJsonObject object;
        for (auto it = check.constBegin(); it != check.constEnd(); ++it) {
            object.insert(it.key(), QJsonValue::fromVariant(it.value()));
        }
        checks.append(object);
        const QString checkId = check.value(QStringLiteral("id")).toString();
        if (m_acknowledgedIds.contains(checkId)) {
            acknowledgements.append(QJsonObject{
                {QStringLiteral("check"), checkId},
                {QStringLiteral("acknowledged_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
            });
        }
    }

    QJsonObject payload{
        {QStringLiteral("status"), m_blocked ? QStringLiteral("blocked") : (m_cautionRequired ? QStringLiteral("caution") : QStringLiteral("passed"))},
        {QStringLiteral("checks"), checks},
        {QStringLiteral("caution_acknowledgements"), acknowledgements},
        {QStringLiteral("block_reason"), blockReason()},
        {QStringLiteral("completed_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
    };
    if (m_plan && numericId(m_plan->missionId()) && !m_plan->createdLocally()) {
        payload.insert(QStringLiteral("mission"), m_plan->missionId().toInt());
    }
    if (m_telemetry && numericId(m_telemetry->aircraftId())) {
        payload.insert(QStringLiteral("aircraft"), m_telemetry->aircraftId().toInt());
    }

    m_api->post(QStringLiteral("/api/preflight/runs/"),
                payload,
                true,
                true,
                [this](int statusCode, const QJsonObject &, const QString &error) {
                    if (statusCode >= 400 && m_events) {
                        m_events->recordEvent(QStringLiteral("preflight_sync_failed"),
                                              QStringLiteral("warning"),
                                              QStringLiteral("Preflight checklist could not be stored in Control Center"),
                                              QJsonObject{
                                                  {QStringLiteral("status_code"), statusCode},
                                                  {QStringLiteral("error"), error}
                                              });
                    }
                });
}
