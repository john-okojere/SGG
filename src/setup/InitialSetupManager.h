#pragma once

#include <QObject>
#include <QVariantList>

#include <memory>

namespace mavsdk {
class Calibration;
}

class AccessManager;
class GcsEventSyncManager;
class MavsdkVehicleManager;
class SessionManager;

class InitialSetupManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList setupRows READ setupRows NOTIFY setupChanged)
    Q_PROPERTY(QVariantList calibrationSteps READ calibrationSteps NOTIFY setupChanged)
    Q_PROPERTY(QString status READ status NOTIFY setupChanged)
    Q_PROPERTY(QString activeStep READ activeStep NOTIFY setupChanged)
    Q_PROPERTY(int progress READ progress NOTIFY setupChanged)
    Q_PROPERTY(bool running READ running NOTIFY setupChanged)

public:
    explicit InitialSetupManager(MavsdkVehicleManager *vehicle,
                                 AccessManager *access,
                                 SessionManager *session,
                                 GcsEventSyncManager *events,
                                 QObject *parent = nullptr);

    QVariantList setupRows() const;
    QVariantList calibrationSteps() const;
    QString status() const;
    QString activeStep() const;
    int progress() const;
    bool running() const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void startCalibration(const QString &kind);
    Q_INVOKABLE void cancelCalibration();
    Q_INVOKABLE void markUnsupported(const QString &toolName);

signals:
    void setupChanged();

private:
    QVariantMap row(const QString &name, const QString &value) const;
    QVariantMap step(const QString &key,
                     const QString &name,
                     const QString &description,
                     bool supported) const;
    bool authorize(const QString &label);
    bool preflightCalibration(const QString &label);
    void setStatus(const QString &status);
    void setActiveStep(const QString &activeStep);
    void setProgress(int progress);
    void setRunning(bool running);
    void audit(const QString &eventType,
               const QString &severity,
               const QString &message,
               const QString &toolName) const;

    MavsdkVehicleManager *m_vehicle = nullptr;
    AccessManager *m_access = nullptr;
    SessionManager *m_session = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    std::shared_ptr<mavsdk::Calibration> m_activeCalibration;
    QString m_status = "Initial setup idle.";
    QString m_activeStep = "None";
    int m_progress = 0;
    bool m_running = false;
};
