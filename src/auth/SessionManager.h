#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

class ApiClient;
class TokenManager;

class SessionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool operationsAllowed READ operationsAllowed NOTIFY sessionStateChanged)
    Q_PROPERTY(bool controlCenterReachable READ controlCenterReachable NOTIFY sessionStateChanged)
    Q_PROPERTY(QString blockReason READ blockReason NOTIFY sessionStateChanged)

public:
    explicit SessionManager(ApiClient *api, TokenManager *tokens, QObject *parent = nullptr);

    bool operationsAllowed() const;
    bool controlCenterReachable() const;
    QString blockReason() const;

    Q_INVOKABLE void validateSession();
    Q_INVOKABLE void startMonitoring();
    Q_INVOKABLE void stopMonitoring();
    void blockOperations(const QString &reason);

signals:
    void sessionStateChanged();
    void forceLogout(const QString &reason);

private:
    ApiClient *m_api = nullptr;
    TokenManager *m_tokens = nullptr;
    QTimer m_monitorTimer;
    bool m_operationsAllowed = false;
    bool m_controlCenterReachable = false;
    QString m_blockReason = "Sign in required.";
};
