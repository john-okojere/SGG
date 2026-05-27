#include "SecureStorage.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QHostInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStandardPaths>

SecureStorage::SecureStorage(QObject *parent) : QObject(parent) {}

bool SecureStorage::writeSecret(const QString &key, const QString &value)
{
    if (writeKeyring(key, value)) {
        return true;
    }

    const QByteArray plain = value.toUtf8();
    const QByteArray stream = keystream(plain.size());
    QByteArray cipher;
    cipher.resize(plain.size());
    for (int i = 0; i < plain.size(); ++i) {
        cipher[i] = plain.at(i) ^ stream.at(i);
    }

    QSettings settings;
    settings.setValue(storageKey(key), QString::fromLatin1(cipher.toBase64()));
    settings.sync();
    return settings.status() == QSettings::NoError;
}

QString SecureStorage::readSecret(const QString &key) const
{
    const QString keyringValue = readKeyring(key);
    if (!keyringValue.isEmpty()) {
        return keyringValue;
    }

    QSettings settings;
    const QString encoded = settings.value(storageKey(key)).toString();
    if (encoded.isEmpty()) {
        return {};
    }

    const QByteArray cipher = QByteArray::fromBase64(encoded.toLatin1());
    const QByteArray stream = keystream(cipher.size());
    QByteArray plain;
    plain.resize(cipher.size());
    for (int i = 0; i < cipher.size(); ++i) {
        plain[i] = cipher.at(i) ^ stream.at(i);
    }
    return QString::fromUtf8(plain);
}

void SecureStorage::deleteSecret(const QString &key)
{
    deleteKeyring(key);
    QSettings settings;
    settings.remove(storageKey(key));
    settings.sync();
}

void SecureStorage::clearNamespace()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("secure"));
    settings.remove(QString());
    settings.endGroup();
    settings.sync();
}

QByteArray SecureStorage::keystream(int length) const
{
    QByteArray output;
    QByteArray seed = machineSecret();
    int counter = 0;
    while (output.size() < length) {
        QByteArray block = seed + QByteArray::number(counter++);
        output += QCryptographicHash::hash(block, QCryptographicHash::Sha256);
    }
    return output.left(length);
}

QString SecureStorage::storageKey(const QString &key) const
{
    return QStringLiteral("secure/%1").arg(key);
}

QByteArray SecureStorage::machineSecret() const
{
    QByteArray material;
    QFile machineId(QStringLiteral("/etc/machine-id"));
    if (machineId.open(QIODevice::ReadOnly)) {
        material += machineId.readAll().trimmed();
    }
    material += QHostInfo::localHostName().toUtf8();
    material += QProcessEnvironment::systemEnvironment().value(QStringLiteral("USER")).toUtf8();
    material += QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toUtf8();
    return QCryptographicHash::hash(material, QCryptographicHash::Sha256);
}

bool SecureStorage::keyringAvailable() const
{
    return !QStandardPaths::findExecutable(QStringLiteral("secret-tool")).isEmpty();
}

bool SecureStorage::writeKeyring(const QString &key, const QString &value) const
{
    if (!keyringAvailable()) {
        return false;
    }
    QProcess process;
    process.start(QStringLiteral("secret-tool"),
                  {QStringLiteral("store"),
                   QStringLiteral("--label"),
                   QStringLiteral("SkyGrid GCS"),
                   QStringLiteral("service"),
                   QStringLiteral("SkyGridGCS"),
                   QStringLiteral("key"),
                   key});
    if (!process.waitForStarted(1500)) {
        return false;
    }
    process.write(value.toUtf8());
    process.closeWriteChannel();
    return process.waitForFinished(3000) && process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
}

QString SecureStorage::readKeyring(const QString &key) const
{
    if (!keyringAvailable()) {
        return {};
    }
    QProcess process;
    process.start(QStringLiteral("secret-tool"),
                  {QStringLiteral("lookup"),
                   QStringLiteral("service"),
                   QStringLiteral("SkyGridGCS"),
                   QStringLiteral("key"),
                   key});
    if (!process.waitForFinished(2500) || process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        return {};
    }
    return QString::fromUtf8(process.readAllStandardOutput()).trimmed();
}

bool SecureStorage::deleteKeyring(const QString &key) const
{
    if (!keyringAvailable()) {
        return false;
    }
    QProcess process;
    process.start(QStringLiteral("secret-tool"),
                  {QStringLiteral("clear"),
                   QStringLiteral("service"),
                   QStringLiteral("SkyGridGCS"),
                   QStringLiteral("key"),
                   key});
    return process.waitForFinished(2500) && process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
}
