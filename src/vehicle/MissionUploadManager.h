#pragma once

#include <QObject>
#include <QString>

class ApiClient;
class GcsEventSyncManager;
class PreflightChecklistManager;
class MissionPlanModel;
class MavsdkVehicleManager;
class SessionManager;
class VehicleTelemetryModel;

class MissionUploadManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadChanged)
    Q_PROPERTY(bool uploaded READ uploaded NOTIFY uploadChanged)
    Q_PROPERTY(int progress READ progress NOTIFY uploadChanged)
    Q_PROPERTY(QString status READ status NOTIFY uploadChanged)

public:
    explicit MissionUploadManager(MavsdkVehicleManager *vehicle,
                                  VehicleTelemetryModel *telemetry,
                                  MissionPlanModel *plan,
                                  ApiClient *api,
                                  SessionManager *session,
                                  PreflightChecklistManager *preflight,
                                  GcsEventSyncManager *events,
                                  QObject *parent = nullptr);

    bool uploading() const;
    bool uploaded() const;
    int progress() const;
    QString status() const;

    Q_INVOKABLE void uploadActiveMission();

signals:
    void uploadChanged();
    void missionUploaded();
    void missionUploadFailed(const QString &message);

private:
    void setStatus(const QString &status);
    void markBackendUploaded(int itemCount);

    MavsdkVehicleManager *m_vehicle = nullptr;
    VehicleTelemetryModel *m_telemetry = nullptr;
    MissionPlanModel *m_plan = nullptr;
    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    PreflightChecklistManager *m_preflight = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    bool m_uploading = false;
    bool m_uploaded = false;
    int m_progress = 0;
    QString m_status = "No mission uploaded";
};
