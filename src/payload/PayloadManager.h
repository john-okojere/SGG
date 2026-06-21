#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class AccessManager;
class GcsEventSyncManager;
class MavsdkVehicleManager;
class VideoStreamManager;
class VehicleTelemetryModel;

class PayloadManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList payloadRows READ payloadRows NOTIFY payloadChanged)
    Q_PROPERTY(QVariantList cameraRows READ cameraRows NOTIFY payloadChanged)
    Q_PROPERTY(QVariantList gimbalRows READ gimbalRows NOTIFY payloadChanged)
    Q_PROPERTY(QString videoStatus READ videoStatus NOTIFY payloadChanged)
    Q_PROPERTY(QString geotagStatus READ geotagStatus NOTIFY payloadChanged)
    Q_PROPERTY(QString overlapStatus READ overlapStatus NOTIFY payloadChanged)
    Q_PROPERTY(QString status READ status NOTIFY payloadChanged)

public:
    explicit PayloadManager(VehicleTelemetryModel *telemetry,
                            MavsdkVehicleManager *vehicle,
                            VideoStreamManager *video,
                            AccessManager *access,
                            GcsEventSyncManager *events,
                            QObject *parent = nullptr);

    QVariantList payloadRows() const;
    QVariantList cameraRows() const;
    QVariantList gimbalRows() const;
    QString videoStatus() const;
    QString geotagStatus() const;
    QString overlapStatus() const;
    QString status() const;

    Q_INVOKABLE void triggerCamera();
    Q_INVOKABLE void validateCamera();
    Q_INVOKABLE void validateGimbal();
    Q_INVOKABLE void setGimbalPitch(double pitch);
    Q_INVOKABLE void configurePayload(const QVariantMap &config);
    Q_INVOKABLE void verifyOverlap();
    Q_INVOKABLE void startGeotagWorkflow();

signals:
    void payloadChanged();

private:
    QVariantMap row(const QString &name, const QString &value) const;
    bool authorizePayloadAction(const QString &label);
    void setStatus(const QString &status);

    VehicleTelemetryModel *m_telemetry = nullptr;
    MavsdkVehicleManager *m_vehicle = nullptr;
    VideoStreamManager *m_video = nullptr;
    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QString m_videoStatus = "Live video adapter not configured.";
    QString m_geotagStatus = "Geotagging workflow placeholder ready.";
    QString m_overlapStatus = "Photo overlap verification awaits mission/camera adapter data.";
    QString m_status = "Payload tools idle";
};
