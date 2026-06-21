#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

class AccessManager;
class GcsEventSyncManager;
class MavsdkVehicleManager;

class OptionalHardwareManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList tools READ tools NOTIFY hardwareChanged)
    Q_PROPERTY(QVariantList serialRows READ serialRows NOTIFY hardwareChanged)
    Q_PROPERTY(QVariantList hardwareRows READ hardwareRows NOTIFY hardwareChanged)
    Q_PROPERTY(QString status READ status NOTIFY hardwareChanged)
    Q_PROPERTY(QString activeTool READ activeTool NOTIFY hardwareChanged)
    Q_PROPERTY(QString sikPort READ sikPort WRITE setSikPort NOTIFY hardwareChanged)
    Q_PROPERTY(int sikBaud READ sikBaud WRITE setSikBaud NOTIFY hardwareChanged)

public:
    explicit OptionalHardwareManager(MavsdkVehicleManager *vehicle,
                                     AccessManager *access,
                                     GcsEventSyncManager *events,
                                     QObject *parent = nullptr);

    QVariantList tools() const;
    QVariantList serialRows() const;
    QVariantList hardwareRows() const;
    QString status() const;
    QString activeTool() const;
    QString sikPort() const;
    int sikBaud() const;

    void setSikPort(const QString &port);
    void setSikBaud(int baud);

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void selectTool(const QString &toolKey);
    Q_INVOKABLE void configureTool(const QString &toolKey);
    Q_INVOKABLE void configureSikRadio(const QString &portName, int baudRate, int netId, int airSpeed);
    Q_INVOKABLE void probeDroneCan();
    Q_INVOKABLE void probePx4Flow();
    Q_INVOKABLE void probeRadioStatus();
    Q_INVOKABLE void scanBluetooth();
    Q_INVOKABLE void updateAntennaTrackerTarget(double latitude, double longitude, double altitude);
    Q_INVOKABLE void injectRtcmSample();
    Q_INVOKABLE void injectRtcmBase64(const QString &base64Data);
    Q_INVOKABLE void sendTerminalCommand(const QString &command);

signals:
    void hardwareChanged();

private:
    QVariantMap tool(const QString &key,
                     const QString &name,
                     const QString &description,
                     const QString &status,
                     bool supported) const;
    QVariantMap row(const QString &name, const QString &value) const;
    bool authorize(const QString &action, const QString &label);
    bool ensureVehicleLink(const QString &label);
    void requestMavlinkMessage(int messageId,
                               const QString &label,
                               const QString &eventType,
                               const QString &toolKey);
    void setStatus(const QString &status);
    void audit(const QString &eventType,
               const QString &severity,
               const QString &message,
               const QString &toolKey = QString()) const;

    MavsdkVehicleManager *m_vehicle = nullptr;
    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QStringList m_serialPorts;
    QVariantList m_hardwareRows;
    QString m_status = "Optional hardware tools idle.";
    QString m_activeTool = "rtkGpsInject";
    QString m_sikPort;
    int m_sikBaud = 57600;
};
