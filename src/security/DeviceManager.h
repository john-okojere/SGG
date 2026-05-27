#pragma once

#include <QObject>
#include <QVariantMap>

class SecureStorage;

class DeviceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceUuid READ deviceUuid CONSTANT)
    Q_PROPERTY(QString hostname READ hostname CONSTANT)
    Q_PROPERTY(QString osName READ osName CONSTANT)
    Q_PROPERTY(QString osVersion READ osVersion CONSTANT)
    Q_PROPERTY(QString hardwareFingerprint READ hardwareFingerprint CONSTANT)
    Q_PROPERTY(QString appVersion READ appVersion CONSTANT)

public:
    explicit DeviceManager(SecureStorage *storage, QObject *parent = nullptr);

    QString deviceUuid() const;
    QString hostname() const;
    QString osName() const;
    QString osVersion() const;
    QString hardwareFingerprint() const;
    QString appVersion() const;
    QVariantMap devicePayload() const;

private:
    QString ensureDeviceUuid();

    SecureStorage *m_storage = nullptr;
    QString m_deviceUuid;
    QString m_hardwareFingerprint;
};
