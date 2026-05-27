#include "DeviceManager.h"

#include "SecureStorage.h"

#include <QCryptographicHash>
#include <QFile>
#include <QHostInfo>
#include <QSysInfo>
#include <QUuid>

DeviceManager::DeviceManager(SecureStorage *storage, QObject *parent)
    : QObject(parent), m_storage(storage)
{
    m_deviceUuid = ensureDeviceUuid();
    QByteArray material = QHostInfo::localHostName().toUtf8() + QSysInfo::machineUniqueId();
    QFile machineId(QStringLiteral("/etc/machine-id"));
    if (machineId.open(QIODevice::ReadOnly)) {
        material += machineId.readAll().trimmed();
    }
    m_hardwareFingerprint = QString::fromLatin1(QCryptographicHash::hash(material, QCryptographicHash::Sha256).toHex());
}

QString DeviceManager::deviceUuid() const { return m_deviceUuid; }
QString DeviceManager::hostname() const { return QHostInfo::localHostName(); }
QString DeviceManager::osName() const { return QSysInfo::prettyProductName(); }
QString DeviceManager::osVersion() const { return QSysInfo::productVersion(); }
QString DeviceManager::hardwareFingerprint() const { return m_hardwareFingerprint; }
QString DeviceManager::appVersion() const { return QStringLiteral("0.1.0"); }

QVariantMap DeviceManager::devicePayload() const
{
    return {
        {QStringLiteral("device_uuid"), deviceUuid()},
        {QStringLiteral("hostname"), hostname()},
        {QStringLiteral("os_name"), osName()},
        {QStringLiteral("os_version"), osVersion()},
        {QStringLiteral("hardware_fingerprint"), hardwareFingerprint()},
        {QStringLiteral("app_version"), appVersion()}
    };
}

QString DeviceManager::ensureDeviceUuid()
{
    if (!m_storage) {
        return QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    QString uuid = m_storage->readSecret(QStringLiteral("device_uuid"));
    if (uuid.isEmpty()) {
        uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        m_storage->writeSecret(QStringLiteral("device_uuid"), uuid);
    }
    return uuid;
}
