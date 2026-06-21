#include "InitialSetupManager.h"

#include "../auth/SessionManager.h"
#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"

#include <mavsdk/plugins/calibration/calibration.h>

#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>

namespace {
QString calibrationResultString(mavsdk::Calibration::Result result)
{
    switch (result) {
    case mavsdk::Calibration::Result::Success: return QStringLiteral("Success");
    case mavsdk::Calibration::Result::Next: return QStringLiteral("Progress");
    case mavsdk::Calibration::Result::Failed: return QStringLiteral("Failed");
    case mavsdk::Calibration::Result::NoSystem: return QStringLiteral("No system");
    case mavsdk::Calibration::Result::ConnectionError: return QStringLiteral("Connection error");
    case mavsdk::Calibration::Result::Busy: return QStringLiteral("Vehicle busy");
    case mavsdk::Calibration::Result::CommandDenied: return QStringLiteral("Command denied");
    case mavsdk::Calibration::Result::Timeout: return QStringLiteral("Timeout");
    case mavsdk::Calibration::Result::Cancelled: return QStringLiteral("Cancelled");
    case mavsdk::Calibration::Result::FailedArmed: return QStringLiteral("Failed: vehicle armed");
    case mavsdk::Calibration::Result::Unsupported: return QStringLiteral("Unsupported");
    default: return QStringLiteral("Unknown");
    }
}
}

InitialSetupManager::InitialSetupManager(MavsdkVehicleManager *vehicle,
                                         AccessManager *access,
                                         SessionManager *session,
                                         GcsEventSyncManager *events,
                                         QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_access(access),
      m_session(session),
      m_events(events)
{
    if (m_vehicle) {
        connect(m_vehicle, &MavsdkVehicleManager::vehicleChanged, this, &InitialSetupManager::setupChanged);
    }
}

QVariantList InitialSetupManager::setupRows() const
{
    return {
        row(QStringLiteral("Aircraft Link"), m_vehicle && m_vehicle->connected() ? QStringLiteral("Connected") : QStringLiteral("Disconnected")),
        row(QStringLiteral("Armed"), m_vehicle && m_vehicle->armed() ? QStringLiteral("Yes") : QStringLiteral("No")),
        row(QStringLiteral("In Air"), m_vehicle && m_vehicle->inAir() ? QStringLiteral("Yes") : QStringLiteral("No")),
        row(QStringLiteral("Health"), m_vehicle ? m_vehicle->health() : QStringLiteral("Unknown")),
        row(QStringLiteral("Active Setup"), m_activeStep),
        row(QStringLiteral("Status"), m_status)
    };
}

QVariantList InitialSetupManager::calibrationSteps() const
{
    return {
        step(QStringLiteral("gyro"), QStringLiteral("Gyro calibration"),
             QStringLiteral("Runs MAVSDK gyro calibration on the connected autopilot."), true),
        step(QStringLiteral("accelerometer"), QStringLiteral("Accelerometer calibration"),
             QStringLiteral("Runs MAVSDK accelerometer calibration with progress/instruction text."), true),
        step(QStringLiteral("compass"), QStringLiteral("Compass calibration"),
             QStringLiteral("Runs MAVSDK magnetometer calibration where supported."), true),
        step(QStringLiteral("levelHorizon"), QStringLiteral("Level horizon"),
             QStringLiteral("Runs MAVSDK board level-horizon calibration."), true),
        step(QStringLiteral("gimbalAccelerometer"), QStringLiteral("Gimbal accelerometer"),
             QStringLiteral("Runs MAVSDK gimbal accelerometer calibration where supported."), true),
        step(QStringLiteral("esc"), QStringLiteral("ESC calibration"),
             QStringLiteral("Adapter-gated; use Motor/ESC parameter setup until a tested ESC workflow is enabled."), false),
        step(QStringLiteral("servo"), QStringLiteral("Servo output setup"),
             QStringLiteral("Parameter-backed servo setup is available in the Servo group."), false),
        step(QStringLiteral("battery"), QStringLiteral("Battery monitor setup"),
             QStringLiteral("Parameter-backed battery monitor setup is available in the Battery group."), false),
        step(QStringLiteral("failsafe"), QStringLiteral("Failsafe setup"),
             QStringLiteral("Parameter-backed failsafe setup is available in the Failsafe group."), false)
    };
}

QString InitialSetupManager::status() const { return m_status; }
QString InitialSetupManager::activeStep() const { return m_activeStep; }
int InitialSetupManager::progress() const { return m_progress; }
bool InitialSetupManager::running() const { return m_running; }

void InitialSetupManager::refresh()
{
    setStatus(m_vehicle && m_vehicle->connected()
                  ? QStringLiteral("Initial setup ready for connected autopilot.")
                  : QStringLiteral("Connect a flight controller before running setup tools."));
    emit setupChanged();
}

void InitialSetupManager::startCalibration(const QString &kind)
{
    const QString key = kind.trimmed();
    const QString label = key == QStringLiteral("accelerometer") ? QStringLiteral("Accelerometer calibration")
        : key == QStringLiteral("compass") ? QStringLiteral("Compass calibration")
        : key == QStringLiteral("levelHorizon") ? QStringLiteral("Level horizon calibration")
        : key == QStringLiteral("gimbalAccelerometer") ? QStringLiteral("Gimbal accelerometer calibration")
        : key == QStringLiteral("gyro") ? QStringLiteral("Gyro calibration")
        : key;

    if (key != QStringLiteral("gyro")
        && key != QStringLiteral("accelerometer")
        && key != QStringLiteral("compass")
        && key != QStringLiteral("levelHorizon")
        && key != QStringLiteral("gimbalAccelerometer")) {
        markUnsupported(label);
        return;
    }
    if (!preflightCalibration(label)) {
        return;
    }

    m_activeCalibration = std::make_shared<mavsdk::Calibration>(m_vehicle->system());
    setRunning(true);
    setProgress(0);
    setActiveStep(label);
    setStatus(QStringLiteral("%1 started. Follow vehicle instruction text.").arg(label));
    audit(QStringLiteral("initial_setup_calibration_started"),
          QStringLiteral("info"),
          QStringLiteral("Initial setup calibration started"),
          key);

    QPointer<InitialSetupManager> self(this);
    auto callback = [self, calibration = m_activeCalibration, key](mavsdk::Calibration::Result result,
                                                                  mavsdk::Calibration::ProgressData progressData) {
        Q_UNUSED(calibration)
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, result, progressData, key]() {
            if (!self) {
                return;
            }
            if (progressData.has_progress) {
                self->setProgress(qBound(0, int(progressData.progress), 100));
            }
            if (progressData.has_status_text && !progressData.status_text.empty()) {
                self->setStatus(QString::fromStdString(progressData.status_text));
            }
            if (result == mavsdk::Calibration::Result::Next) {
                return;
            }

            const bool ok = result == mavsdk::Calibration::Result::Success;
            self->setRunning(false);
            self->setProgress(ok ? 100 : self->m_progress);
            self->setStatus(ok
                                ? QStringLiteral("%1 completed.").arg(self->m_activeStep)
                                : QStringLiteral("%1 failed: %2").arg(self->m_activeStep,
                                                                      calibrationResultString(result)));
            self->audit(QStringLiteral("initial_setup_calibration_finished"),
                        ok ? QStringLiteral("info") : QStringLiteral("warning"),
                        self->m_status,
                        key);
            self->m_activeCalibration.reset();
            emit self->setupChanged();
        }, Qt::QueuedConnection);
    };

    if (key == QStringLiteral("gyro")) {
        m_activeCalibration->calibrate_gyro_async(callback);
    } else if (key == QStringLiteral("accelerometer")) {
        m_activeCalibration->calibrate_accelerometer_async(callback);
    } else if (key == QStringLiteral("compass")) {
        m_activeCalibration->calibrate_magnetometer_async(callback);
    } else if (key == QStringLiteral("levelHorizon")) {
        m_activeCalibration->calibrate_level_horizon_async(callback);
    } else {
        m_activeCalibration->calibrate_gimbal_accelerometer_async(callback);
    }
}

void InitialSetupManager::cancelCalibration()
{
    if (!m_running || !m_activeCalibration) {
        setStatus(QStringLiteral("No active calibration to cancel."));
        return;
    }
    const mavsdk::Calibration::Result result = m_activeCalibration->cancel();
    setRunning(false);
    setStatus(QStringLiteral("Calibration cancel result: %1").arg(calibrationResultString(result)));
    audit(QStringLiteral("initial_setup_calibration_cancelled"),
          result == mavsdk::Calibration::Result::Success ? QStringLiteral("info") : QStringLiteral("warning"),
          m_status,
          m_activeStep);
    m_activeCalibration.reset();
}

void InitialSetupManager::markUnsupported(const QString &toolName)
{
    const QString label = toolName.trimmed().isEmpty() ? QStringLiteral("Setup tool") : toolName.trimmed();
    setStatus(QStringLiteral("%1 is adapter-gated until a tested autopilot workflow is enabled.").arg(label));
    audit(QStringLiteral("initial_setup_tool_unsupported"),
          QStringLiteral("warning"),
          m_status,
          label);
}

QVariantMap InitialSetupManager::row(const QString &name, const QString &value) const
{
    return QVariantMap{{QStringLiteral("name"), name}, {QStringLiteral("value"), value}};
}

QVariantMap InitialSetupManager::step(const QString &key,
                                      const QString &name,
                                      const QString &description,
                                      bool supported) const
{
    return QVariantMap{{QStringLiteral("key"), key},
                       {QStringLiteral("name"), name},
                       {QStringLiteral("description"), description},
                       {QStringLiteral("supported"), supported}};
}

bool InitialSetupManager::authorize(const QString &label)
{
    if (m_access && !m_access->authorizeAction(QStringLiteral("initial_setup"),
                                                QVariantMap{{QStringLiteral("setup_action"), label}},
                                                QStringLiteral("Initial setup action blocked by local permissions."))) {
        setStatus(QStringLiteral("Initial setup action blocked by RBAC."));
        return false;
    }
    return true;
}

bool InitialSetupManager::preflightCalibration(const QString &label)
{
    if (m_running) {
        setStatus(QStringLiteral("Finish or cancel the active setup operation first."));
        return false;
    }
    if (!authorize(label)) {
        return false;
    }
    if (!m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Initial setup blocked: trusted session required."));
        return false;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a flight controller before running %1.").arg(label));
        return false;
    }
    if (m_vehicle->armed() || m_vehicle->inAir()) {
        setStatus(QStringLiteral("%1 blocked while vehicle is armed or in air.").arg(label));
        return false;
    }
    return true;
}

void InitialSetupManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit setupChanged();
}

void InitialSetupManager::setActiveStep(const QString &activeStep)
{
    if (m_activeStep == activeStep) {
        return;
    }
    m_activeStep = activeStep;
    emit setupChanged();
}

void InitialSetupManager::setProgress(int progress)
{
    const int next = qBound(0, progress, 100);
    if (m_progress == next) {
        return;
    }
    m_progress = next;
    emit setupChanged();
}

void InitialSetupManager::setRunning(bool running)
{
    if (m_running == running) {
        return;
    }
    m_running = running;
    emit setupChanged();
}

void InitialSetupManager::audit(const QString &eventType,
                                const QString &severity,
                                const QString &message,
                                const QString &toolName) const
{
    if (!m_events) {
        return;
    }
    m_events->recordEvent(eventType,
                          severity,
                          message,
                          QJsonObject{{QStringLiteral("tool"), toolName},
                                      {QStringLiteral("connected"), m_vehicle && m_vehicle->connected()},
                                      {QStringLiteral("system_id"), m_vehicle ? m_vehicle->systemId() : QString()}});
}
