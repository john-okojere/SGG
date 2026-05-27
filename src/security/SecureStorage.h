#pragma once

#include <QObject>
#include <QString>

class SecureStorage : public QObject
{
    Q_OBJECT

public:
    explicit SecureStorage(QObject *parent = nullptr);

    bool writeSecret(const QString &key, const QString &value);
    QString readSecret(const QString &key) const;
    void deleteSecret(const QString &key);
    void clearNamespace();

private:
    QByteArray keystream(int length) const;
    QString storageKey(const QString &key) const;
    QByteArray machineSecret() const;
    bool keyringAvailable() const;
    bool writeKeyring(const QString &key, const QString &value) const;
    QString readKeyring(const QString &key) const;
    bool deleteKeyring(const QString &key) const;
};
