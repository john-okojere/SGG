#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class QJsonObject;

class ApiClient;
class DeviceManager;
class TokenManager;

class AuthManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool authenticated READ authenticated NOTIFY authChanged)
    Q_PROPERTY(bool devicePending READ devicePending NOTIFY authChanged)
    Q_PROPERTY(bool deviceTrusted READ deviceTrusted NOTIFY authChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY authChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY authChanged)
    Q_PROPERTY(QString securityWarning READ securityWarning NOTIFY authChanged)

public:
    explicit AuthManager(ApiClient *api, TokenManager *tokens, DeviceManager *device, QObject *parent = nullptr);

    bool authenticated() const;
    bool devicePending() const;
    bool deviceTrusted() const;
    bool busy() const;
    QString statusMessage() const;
    QString securityWarning() const;

    Q_INVOKABLE void login(const QString &email, const QString &password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void checkDeviceApproval();

signals:
    void authChanged();
    void loginSucceeded();
    void operationsBlocked(const QString &reason);

private:
    void setBusy(bool busy);
    void setStatus(const QString &message);
    void setSecurityWarning(const QString &message);
    QString firstString(const QJsonObject &body, const QStringList &keys) const;

    ApiClient *m_api = nullptr;
    TokenManager *m_tokens = nullptr;
    DeviceManager *m_device = nullptr;
    bool m_busy = false;
    QString m_statusMessage = "Sign in with your Control Center account.";
    QString m_securityWarning;
};
