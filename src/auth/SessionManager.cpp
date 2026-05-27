#include "SessionManager.h"

#include "TokenManager.h"
#include "../network/ApiClient.h"

#include <QJsonObject>

SessionManager::SessionManager(ApiClient *api, TokenManager *tokens, QObject *parent)
    : QObject(parent), m_api(api), m_tokens(tokens)
{
    connect(&m_monitorTimer, &QTimer::timeout, this, &SessionManager::validateSession);
    m_monitorTimer.setInterval(30000);

    if (m_tokens) {
        connect(m_tokens, &TokenManager::sessionChanged, this, [this]() {
            m_operationsAllowed = m_tokens->hasSession() && m_tokens->deviceTrusted();
            m_blockReason = m_operationsAllowed ? QString() : QStringLiteral("Device approval required.");
            if (m_operationsAllowed) {
                startMonitoring();
            } else {
                stopMonitoring();
            }
            emit sessionStateChanged();
        });
    }
}

bool SessionManager::operationsAllowed() const { return m_operationsAllowed; }
bool SessionManager::controlCenterReachable() const { return m_controlCenterReachable; }
QString SessionManager::blockReason() const { return m_blockReason; }

void SessionManager::validateSession()
{
    if (!m_api || !m_tokens || !m_tokens->hasSession() || !m_tokens->deviceTrusted()) {
        stopMonitoring();
        blockOperations(QStringLiteral("Device is not trusted."));
        return;
    }
    const QString path = QStringLiteral("/api/auth/session/?device_uuid=%1").arg(m_tokens->deviceUuid());
    m_api->get(path, true, true, [this](int status, const QJsonObject &body, const QString &error) {
        if (status < 200 || status >= 300) {
            if (status == 0 && m_tokens && m_tokens->hasSession() && m_tokens->deviceTrusted()) {
                m_controlCenterReachable = false;
                m_operationsAllowed = true;
                m_blockReason = QStringLiteral("Control Center unreachable; using trusted local session and cache.");
                startMonitoring();
                emit sessionStateChanged();
                return;
            }
            m_controlCenterReachable = false;
            blockOperations(error.isEmpty() ? QStringLiteral("Session validation failed.") : error);
            return;
        }
        const QString statusText = body.value(QStringLiteral("status")).toString();
        const bool trusted = body.value(QStringLiteral("device_trusted")).toBool(false)
            || body.value(QStringLiteral("is_trusted")).toBool(false)
            || statusText == QStringLiteral("approved");
        const bool forceLogoutValue = body.value(QStringLiteral("force_logout")).toBool(false);
        if (!trusted || forceLogoutValue) {
            if (m_tokens) {
                m_tokens->clear();
            }
            stopMonitoring();
            blockOperations(QStringLiteral("Control Center revoked this device session."));
            emit forceLogout(m_blockReason);
            return;
        }
        m_operationsAllowed = true;
        m_controlCenterReachable = true;
        m_blockReason.clear();
        startMonitoring();
        emit sessionStateChanged();
    });
}

void SessionManager::startMonitoring()
{
    if (!m_monitorTimer.isActive()) {
        m_monitorTimer.start();
    }
}

void SessionManager::stopMonitoring()
{
    if (m_monitorTimer.isActive()) {
        m_monitorTimer.stop();
    }
}

void SessionManager::blockOperations(const QString &reason)
{
    m_operationsAllowed = false;
    m_blockReason = reason;
    emit sessionStateChanged();
}
