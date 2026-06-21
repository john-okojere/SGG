#pragma once

#include <QObject>
#include <QVariantList>

class AccessManager;
class AdvancedMissionManager;
class FirmwareUpdateManager;
class GcsEventSyncManager;
class LogAnalysisManager;
class MavsdkVehicleManager;
class ParameterManager;
class SessionManager;
class VideoStreamManager;

class ProductionReadinessManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList readinessRows READ readinessRows NOTIFY readinessChanged)
    Q_PROPERTY(QVariantList blockers READ blockers NOTIFY readinessChanged)
    Q_PROPERTY(int readinessPercent READ readinessPercent NOTIFY readinessChanged)
    Q_PROPERTY(bool readyForProduction READ readyForProduction NOTIFY readinessChanged)
    Q_PROPERTY(QString status READ status NOTIFY readinessChanged)

public:
    explicit ProductionReadinessManager(MavsdkVehicleManager *vehicle,
                                        ParameterManager *parameters,
                                        AdvancedMissionManager *missions,
                                        FirmwareUpdateManager *firmware,
                                        LogAnalysisManager *logs,
                                        VideoStreamManager *video,
                                        SessionManager *session,
                                        AccessManager *access,
                                        GcsEventSyncManager *events,
                                        QObject *parent = nullptr);

    QVariantList readinessRows() const;
    QVariantList blockers() const;
    int readinessPercent() const;
    bool readyForProduction() const;
    QString status() const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void recordFieldValidation(const QString &area, bool passed, const QString &notes);

signals:
    void readinessChanged();

private:
    QVariantMap row(const QString &area,
                    const QString &state,
                    const QString &detail,
                    bool blocker) const;
    void rebuild();
    void setStatus(const QString &status);
    void audit(const QString &eventType,
               const QString &severity,
               const QString &message,
               const QString &area,
               bool passed) const;

    MavsdkVehicleManager *m_vehicle = nullptr;
    ParameterManager *m_parameters = nullptr;
    AdvancedMissionManager *m_missions = nullptr;
    FirmwareUpdateManager *m_firmware = nullptr;
    LogAnalysisManager *m_logs = nullptr;
    VideoStreamManager *m_video = nullptr;
    SessionManager *m_session = nullptr;
    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QVariantList m_rows;
    QVariantList m_blockers;
    int m_readinessPercent = 0;
    bool m_readyForProduction = false;
    QString m_status = "Production readiness not evaluated.";
};
