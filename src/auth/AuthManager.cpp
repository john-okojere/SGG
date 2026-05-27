#include "AuthManager.h"

#include "TokenManager.h"
#include "../network/ApiClient.h"
#include "../security/DeviceManager.h"

#include <QJsonObject>

AuthManager::AuthManager(ApiClient *api, TokenManager *tokens, DeviceManager *device, QObject *parent)
    : QObject(parent), m_api(api), m_tokens(tokens), m_device(device)
{
    if (m_tokens) {
        connect(m_tokens, &TokenManager::sessionChanged, this, &AuthManager::authChanged);
    }
    if (m_api) {
        connect(m_api, &ApiClient::securityFailure, this, [this](const QString &message) {
            setSecurityWarning(message);
            logout();
        });
    }
}

bool AuthManager::authenticated() const { return m_tokens && m_tokens->hasSession(); }
bool AuthManager::devicePending() const { return m_tokens && m_tokens->deviceStatus() == QStringLiteral("pending"); }
bool AuthManager::deviceTrusted() const { return m_tokens && m_tokens->deviceTrusted(); }
bool AuthManager::busy() const { return m_busy; }
QString AuthManager::statusMessage() const { return m_statusMessage; }
QString AuthManager::securityWarning() const { return m_securityWarning; }

void AuthManager::login(const QString &email, const QString &password)
{
    if (!m_api || !m_tokens || !m_device) {
        setSecurityWarning(QStringLiteral("Authentication services are not initialized."));
        return;
    }
    setBusy(true);
    setSecurityWarning(QString());
    setStatus(QStringLiteral("Connecting to Control Center..."));

    QJsonObject payload{
        {QStringLiteral("email"), email.trimmed()},
        {QStringLiteral("password"), password},
        {QStringLiteral("device"), QJsonObject::fromVariantMap(m_device->devicePayload())}
    };

    m_api->post(QStringLiteral("/api/auth/login/"), payload, false, false,
                [this](int status, const QJsonObject &body, const QString &error) {
        setBusy(false);
        if (status < 200 || status >= 300) {
            setSecurityWarning(error.isEmpty() ? QStringLiteral("Login failed. Check credentials or backend availability.") : error);
            setStatus(QStringLiteral("Sign in failed."));
            return;
        }
        const QString access = firstString(body, {QStringLiteral("access"), QStringLiteral("access_token")});
        const QString refresh = firstString(body, {QStringLiteral("refresh"), QStringLiteral("refresh_token")});
        const QString statusText = body.value(QStringLiteral("device_status")).toString(QStringLiteral("pending"));
        const QString trustToken = firstString(body, {QStringLiteral("device_trust_token"), QStringLiteral("trust_token")});
        if (access.isEmpty() || refresh.isEmpty()) {
            setSecurityWarning(QStringLiteral("Login response did not include required tokens."));
            return;
        }
        m_tokens->saveLogin(access, refresh, m_device->deviceUuid(), statusText, trustToken);
        if (statusText == QStringLiteral("pending")) {
            setStatus(QStringLiteral("Device pending Control Center approval."));
            emit operationsBlocked(m_statusMessage);
        } else {
            setStatus(QStringLiteral("Control Center session established."));
            emit loginSucceeded();
        }
        emit authChanged();
    });
}

void AuthManager::logout()
{
    if (m_tokens) {
        m_tokens->clear();
    }
    setStatus(QStringLiteral("Signed out. Mission operations are blocked."));
    emit operationsBlocked(QStringLiteral("Signed out."));
    emit authChanged();
}

void AuthManager::checkDeviceApproval()
{
    if (!m_api || !m_tokens || !m_tokens->hasSession()) {
        setSecurityWarning(QStringLiteral("Sign in before checking device approval."));
        return;
    }
    setBusy(true);
    setStatus(QStringLiteral("Checking device approval..."));
    const QString path = QStringLiteral("/api/devices/me/?device_uuid=%1").arg(m_tokens->deviceUuid());
    m_api->get(path, true, false, [this](int status, const QJsonObject &body, const QString &error) {
        setBusy(false);
        if (status < 200 || status >= 300) {
            setSecurityWarning(error.isEmpty() ? QStringLiteral("Device approval check failed.") : error);
            return;
        }
        const QString statusText = firstString(body, {
            QStringLiteral("device_status"),
            QStringLiteral("status")
        });
        const bool approved = body.value(QStringLiteral("approved")).toBool(false)
            || body.value(QStringLiteral("is_trusted")).toBool(false)
            || statusText == QStringLiteral("approved");
        const QString trustToken = firstString(body, {QStringLiteral("device_trust_token"), QStringLiteral("trust_token")});
        if (approved && !trustToken.isEmpty()) {
            m_tokens->updateDeviceTrust(QStringLiteral("approved"), trustToken);
            setStatus(QStringLiteral("Device approved. Mission operations unlocked."));
            emit loginSucceeded();
        } else {
            m_tokens->updateDeviceTrust(statusText.isEmpty() ? QStringLiteral("pending") : statusText, QString());
            setStatus(statusText.isEmpty()
                          ? QStringLiteral("Device pending Control Center approval.")
                          : QStringLiteral("Device status: %1").arg(statusText));
            emit operationsBlocked(m_statusMessage);
        }
        emit authChanged();
    });
}

void AuthManager::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit authChanged();
}

void AuthManager::setStatus(const QString &message)
{
    if (m_statusMessage == message) {
        return;
    }
    m_statusMessage = message;
    emit authChanged();
}

void AuthManager::setSecurityWarning(const QString &message)
{
    if (m_securityWarning == message) {
        return;
    }
    m_securityWarning = message;
    emit authChanged();
}

QString AuthManager::firstString(const QJsonObject &body, const QStringList &keys) const
{
    for (const QString &key : keys) {
        const QString value = body.value(key).toString();
        if (!value.isEmpty()) {
            return value;
        }
    }
    return {};
}
