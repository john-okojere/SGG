#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class ApiClient;
class GcsEventSyncManager;
class MavsdkVehicleManager;
class PermissionManager;
class SessionManager;
class VehicleProfileManager;

class VehicleConfigManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString connectionMode READ connectionMode WRITE setConnectionMode NOTIFY configChanged)
    Q_PROPERTY(QString serialPort READ serialPort WRITE setSerialPort NOTIFY configChanged)
    Q_PROPERTY(int baudRate READ baudRate WRITE setBaudRate NOTIFY configChanged)
    Q_PROPERTY(QString udpHost READ udpHost WRITE setUdpHost NOTIFY configChanged)
    Q_PROPERTY(int udpPort READ udpPort WRITE setUdpPort NOTIFY configChanged)
    Q_PROPERTY(QString tcpHost READ tcpHost WRITE setTcpHost NOTIFY configChanged)
    Q_PROPERTY(int tcpPort READ tcpPort WRITE setTcpPort NOTIFY configChanged)
    Q_PROPERTY(QString connectionUrl READ connectionUrl NOTIFY configChanged)
    Q_PROPERTY(QString status READ status NOTIFY configChanged)
    Q_PROPERTY(QVariantMap parameterSnapshot READ parameterSnapshot NOTIFY configChanged)
    Q_PROPERTY(QVariantMap rcMapping READ rcMapping NOTIFY configChanged)
    Q_PROPERTY(QVariantList auditTrail READ auditTrail NOTIFY configChanged)

public:
    explicit VehicleConfigManager(MavsdkVehicleManager *vehicle,
                                  VehicleProfileManager *profiles,
                                  ApiClient *api,
                                  SessionManager *session,
                                  PermissionManager *permissions,
                                  GcsEventSyncManager *events,
                                  QObject *parent = nullptr);

    QString connectionMode() const;
    QString serialPort() const;
    int baudRate() const;
    QString udpHost() const;
    int udpPort() const;
    QString tcpHost() const;
    int tcpPort() const;
    QString connectionUrl() const;
    QString status() const;
    QVariantMap parameterSnapshot() const;
    QVariantMap rcMapping() const;
    QVariantList auditTrail() const;

    void setConnectionMode(const QString &mode);
    void setSerialPort(const QString &port);
    void setBaudRate(int baudRate);
    void setUdpHost(const QString &host);
    void setUdpPort(int port);
    void setTcpHost(const QString &host);
    void setTcpPort(int port);

    Q_INVOKABLE void connectVehicle();
    Q_INVOKABLE void disconnectVehicle();
    Q_INVOKABLE void bindFlightController();
    Q_INVOKABLE void readParameterSnapshot();
    Q_INVOKABLE void saveParameterSnapshot(const QVariantMap &snapshot = {});
    Q_INVOKABLE void saveRcMapping(const QVariantMap &mapping);
    Q_INVOKABLE void releaseActiveProfile(int organizationId, const QString &notes);

signals:
    void configChanged();

private:
    bool requirePermission(const QString &permission, const QString &message);
    bool requireActiveProfile(QString *profileId = nullptr);
    QString buildConnectionUrl() const;
    QVariantMap connectionPayload() const;
    void postProfileAction(const QString &action, const QJsonObject &payload, const QString &successStatus);
    void setStatus(const QString &status);
    void setParameterSnapshot(const QVariantMap &snapshot);
    void setRcMapping(const QVariantMap &mapping);
    void appendAudit(const QString &action, const QString &message, const QVariantMap &details = {});

    MavsdkVehicleManager *m_vehicle = nullptr;
    VehicleProfileManager *m_profiles = nullptr;
    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    PermissionManager *m_permissions = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QString m_connectionMode = "UDP";
    QString m_serialPort = "COM3";
    int m_baudRate = 57600;
    QString m_udpHost = "0.0.0.0";
    int m_udpPort = 14540;
    QString m_tcpHost = "127.0.0.1";
    int m_tcpPort = 5760;
    QString m_status = "Vehicle configuration idle";
    QVariantMap m_parameterSnapshot;
    QVariantMap m_rcMapping;
    QVariantList m_auditTrail;
};
