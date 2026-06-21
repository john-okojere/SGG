#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>

class AutopilotToolsFacade;
class GcsEventSyncManager;
class MavsdkVehicleManager;
class PermissionManager;
class SessionManager;

class FirmwareUpdateManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath NOTIFY firmwareChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY firmwareChanged)
    Q_PROPERTY(QString packageType READ packageType WRITE setPackageType NOTIFY firmwareChanged)
    Q_PROPERTY(QString version READ version NOTIFY firmwareChanged)
    Q_PROPERTY(QString target READ target NOTIFY firmwareChanged)
    Q_PROPERTY(QString checksum READ checksum NOTIFY firmwareChanged)
    Q_PROPERTY(QString fileSize READ fileSize NOTIFY firmwareChanged)
    Q_PROPERTY(QString status READ status NOTIFY firmwareChanged)
    Q_PROPERTY(QStringList logs READ logs NOTIFY firmwareChanged)
    Q_PROPERTY(bool fileLoaded READ fileLoaded NOTIFY firmwareChanged)
    Q_PROPERTY(bool uploading READ uploading NOTIFY firmwareChanged)
    Q_PROPERTY(int progress READ progress NOTIFY firmwareChanged)
    Q_PROPERTY(QString state READ state NOTIFY firmwareChanged)
    Q_PROPERTY(QString adapterName READ adapterName NOTIFY firmwareChanged)
    Q_PROPERTY(QString boardSummary READ boardSummary NOTIFY firmwareChanged)
    Q_PROPERTY(QString bootloaderStatus READ bootloaderStatus NOTIFY firmwareChanged)
    Q_PROPERTY(QStringList bootloaderPorts READ bootloaderPorts NOTIFY firmwareChanged)
    Q_PROPERTY(QString bootloaderPort READ bootloaderPort WRITE setBootloaderPort NOTIFY firmwareChanged)
    Q_PROPERTY(int bootloaderBaud READ bootloaderBaud WRITE setBootloaderBaud NOTIFY firmwareChanged)
    Q_PROPERTY(QString validationStatus READ validationStatus NOTIFY firmwareChanged)
    Q_PROPERTY(bool supported READ supported NOTIFY firmwareChanged)
    Q_PROPERTY(bool canStartUpload READ canStartUpload NOTIFY firmwareChanged)

public:
    explicit FirmwareUpdateManager(MavsdkVehicleManager *vehicle,
                                   SessionManager *session,
                                   PermissionManager *permissions,
                                   AutopilotToolsFacade *autopilotTools,
                                   GcsEventSyncManager *events,
                                   QObject *parent = nullptr);

    QString filePath() const;
    QString fileName() const;
    QString packageType() const;
    QString version() const;
    QString target() const;
    QString checksum() const;
    QString fileSize() const;
    QString status() const;
    QStringList logs() const;
    bool fileLoaded() const;
    bool uploading() const;
    int progress() const;
    QString state() const;
    QString adapterName() const;
    QString boardSummary() const;
    QString bootloaderStatus() const;
    QStringList bootloaderPorts() const;
    QString bootloaderPort() const;
    int bootloaderBaud() const;
    QString validationStatus() const;
    bool supported() const;
    bool canStartUpload() const;

    void setPackageType(const QString &packageType);
    void setBootloaderPort(const QString &port);
    void setBootloaderBaud(int baud);

    Q_INVOKABLE void loadFirmwareFile(const QString &pathOrUrl);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void clearLogs();
    Q_INVOKABLE void validatePackage();
    Q_INVOKABLE void detectBoard();
    Q_INVOKABLE void refreshBootloaderPorts();
    Q_INVOKABLE void requestBootloaderMode();
    Q_INVOKABLE void retryRecovery();
    Q_INVOKABLE void startUpload();

signals:
    void firmwareChanged();

private:
    QString normalizePath(const QString &pathOrUrl) const;
    QString inferPackageType(const QString &suffix) const;
    QString inferVersion(const QString &baseName) const;
    QString inferTarget(const QString &packageType) const;
    void parsePackageMetadata(const QString &path);
    QString formatSize(qint64 bytes) const;
    QString sha256ForFile(const QString &path) const;
    QByteArray firmwareImageBytes(QString *reason) const;
    QByteArray firmwareImageFromJsonPackage(const QByteArray &fileBytes, QString *reason) const;
    QByteArray firmwareImageFromIntelHex(const QByteArray &fileBytes, QString *reason) const;
    quint32 firmwareCrc32(const QByteArray &image) const;
    void appendLog(const QString &message);
    void fail(const QString &message);
    void completeFlash(const QString &message);
    void resetSelection();
    void setStatus(const QString &status);
    void setState(const QString &state);
    void setProgress(int progress);
    void setAdapterForPackage();
    bool validateAgainstBoard(QString *reason = nullptr);
    bool firmwareActionAllowed(const QString &label);
    void runSerialBootloaderFlash(const QByteArray &image,
                                  const QString &port,
                                  int baud,
                                  quint32 localCrc);
    void audit(const QString &eventType, const QString &severity, const QString &message) const;

    MavsdkVehicleManager *m_vehicle = nullptr;
    SessionManager *m_session = nullptr;
    PermissionManager *m_permissions = nullptr;
    AutopilotToolsFacade *m_autopilotTools = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QString m_filePath;
    QString m_fileName = "No firmware selected";
    QString m_packageType = "Auto";
    QString m_version = "--";
    QString m_target = "--";
    QString m_checksum = "--";
    QString m_fileSize = "--";
    QString m_status = "Select a signed firmware package.";
    QStringList m_logs;
    QString m_state = "idle";
    QString m_adapterName = "Unsupported adapter";
    QString m_boardSummary = "No controller detected";
    QString m_bootloaderStatus = "Bootloader not requested.";
    QStringList m_bootloaderPorts;
    QString m_bootloaderPort;
    int m_bootloaderBaud = 921600;
    QString m_validationStatus = "No package loaded.";
    bool m_uploading = false;
    bool m_supported = false;
    int m_progress = 0;
};
