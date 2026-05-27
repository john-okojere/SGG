#pragma once

#include <QObject>
#include <QSet>
#include <QString>
#include <QVariantList>

class AppState;
class ApiClient;
class GcsEventSyncManager;
class MissionPlanModel;
class SessionManager;
class VehicleTelemetryModel;
class WeatherManager;
class WindCheckManager;

class PreflightChecklistManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList checks READ checks NOTIFY checklistChanged)
    Q_PROPERTY(int readinessPercent READ readinessPercent NOTIFY checklistChanged)
    Q_PROPERTY(bool blocked READ blocked NOTIFY checklistChanged)
    Q_PROPERTY(bool cautionRequired READ cautionRequired NOTIFY checklistChanged)
    Q_PROPERTY(bool canUpload READ canUpload NOTIFY checklistChanged)
    Q_PROPERTY(bool canArm READ canArm NOTIFY checklistChanged)
    Q_PROPERTY(bool canStartMission READ canStartMission NOTIFY checklistChanged)
    Q_PROPERTY(QString status READ status NOTIFY checklistChanged)
    Q_PROPERTY(QString lastRunAt READ lastRunAt NOTIFY checklistChanged)

public:
    explicit PreflightChecklistManager(SessionManager *session,
                                       ApiClient *api,
                                       VehicleTelemetryModel *telemetry,
                                       MissionPlanModel *plan,
                                       AppState *appState,
                                       WeatherManager *weather,
                                       WindCheckManager *windCheck,
                                       GcsEventSyncManager *events,
                                       QObject *parent = nullptr);

    QVariantList checks() const;
    int readinessPercent() const;
    bool blocked() const;
    bool cautionRequired() const;
    bool canUpload() const;
    bool canArm() const;
    bool canStartMission() const;
    QString status() const;
    QString lastRunAt() const;

    Q_INVOKABLE void runChecklist(bool logEvents = true);
    Q_INVOKABLE void acknowledgeCheck(const QString &checkId);
    Q_INVOKABLE void acknowledgeAllCautions();
    Q_INVOKABLE QString blockReason() const;

signals:
    void checklistChanged();
    void checklistCompleted(bool success, const QString &message);

private:
    void rebuildChecks();
    void addCheck(QVariantList &checks,
                  const QString &id,
                  const QString &category,
                  const QString &label,
                  bool passed,
                  const QString &severity,
                  const QString &message) const;
    bool isPilotMode() const;
    bool cautionBlocksOperations() const;
    void setStatus(const QString &status);
    void logCheckEvent(const QVariantMap &check) const;
    void syncChecklistRun() const;

    SessionManager *m_session = nullptr;
    ApiClient *m_api = nullptr;
    VehicleTelemetryModel *m_telemetry = nullptr;
    MissionPlanModel *m_plan = nullptr;
    AppState *m_appState = nullptr;
    WeatherManager *m_weather = nullptr;
    WindCheckManager *m_windCheck = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QVariantList m_checks;
    QSet<QString> m_acknowledgedIds;
    int m_readinessPercent = 0;
    bool m_blocked = true;
    bool m_cautionRequired = false;
    bool m_canUpload = false;
    bool m_canArm = false;
    bool m_canStartMission = false;
    QString m_status = QStringLiteral("Preflight checks not run");
    QString m_lastRunAt;
};
