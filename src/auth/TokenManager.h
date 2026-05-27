#pragma once

#include <QObject>
#include <QString>

class SecureStorage;

class TokenManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasSession READ hasSession NOTIFY sessionChanged)
    Q_PROPERTY(bool deviceTrusted READ deviceTrusted NOTIFY sessionChanged)
    Q_PROPERTY(QString deviceStatus READ deviceStatus NOTIFY sessionChanged)

public:
    explicit TokenManager(SecureStorage *storage, QObject *parent = nullptr);

    bool hasSession() const;
    bool deviceTrusted() const;
    QString deviceStatus() const;
    QString accessToken() const;
    QString refreshToken() const;
    QString deviceUuid() const;
    QString deviceTrustToken() const;

    void load();
    void saveLogin(const QString &accessToken,
                   const QString &refreshToken,
                   const QString &deviceUuid,
                   const QString &deviceStatus,
                   const QString &deviceTrustToken = {});
    void updateAccessToken(const QString &accessToken);
    void updateDeviceTrust(const QString &status, const QString &trustToken);
    void clear();

signals:
    void sessionChanged();

private:
    SecureStorage *m_storage = nullptr;
    QString m_accessToken;
    QString m_refreshToken;
    QString m_deviceUuid;
    QString m_deviceTrustToken;
    QString m_deviceStatus = "unknown";
};
