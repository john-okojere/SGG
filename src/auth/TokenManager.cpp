#include "TokenManager.h"

#include "../security/SecureStorage.h"

TokenManager::TokenManager(SecureStorage *storage, QObject *parent)
    : QObject(parent), m_storage(storage)
{
    load();
}

bool TokenManager::hasSession() const { return !m_accessToken.isEmpty() && !m_refreshToken.isEmpty(); }
bool TokenManager::deviceTrusted() const { return m_deviceStatus == QStringLiteral("approved") && !m_deviceTrustToken.isEmpty(); }
QString TokenManager::deviceStatus() const { return m_deviceStatus; }
QString TokenManager::accessToken() const { return m_accessToken; }
QString TokenManager::refreshToken() const { return m_refreshToken; }
QString TokenManager::deviceUuid() const { return m_deviceUuid; }
QString TokenManager::deviceTrustToken() const { return m_deviceTrustToken; }

void TokenManager::load()
{
    if (!m_storage) {
        return;
    }
    m_accessToken = m_storage->readSecret(QStringLiteral("access_token"));
    m_refreshToken = m_storage->readSecret(QStringLiteral("refresh_token"));
    m_deviceUuid = m_storage->readSecret(QStringLiteral("device_uuid"));
    m_deviceTrustToken = m_storage->readSecret(QStringLiteral("device_trust_token"));
    m_deviceStatus = m_storage->readSecret(QStringLiteral("device_status"));
    if (m_deviceStatus.isEmpty()) {
        m_deviceStatus = QStringLiteral("unknown");
    }
    emit sessionChanged();
}

void TokenManager::saveLogin(const QString &accessToken,
                             const QString &refreshToken,
                             const QString &deviceUuid,
                             const QString &deviceStatus,
                             const QString &deviceTrustToken)
{
    m_accessToken = accessToken;
    m_refreshToken = refreshToken;
    m_deviceUuid = deviceUuid;
    m_deviceStatus = deviceStatus.isEmpty() ? QStringLiteral("unknown") : deviceStatus;
    m_deviceTrustToken = deviceTrustToken;

    if (m_storage) {
        m_storage->writeSecret(QStringLiteral("access_token"), m_accessToken);
        m_storage->writeSecret(QStringLiteral("refresh_token"), m_refreshToken);
        m_storage->writeSecret(QStringLiteral("device_uuid"), m_deviceUuid);
        m_storage->writeSecret(QStringLiteral("device_status"), m_deviceStatus);
        if (!m_deviceTrustToken.isEmpty()) {
            m_storage->writeSecret(QStringLiteral("device_trust_token"), m_deviceTrustToken);
        }
    }
    emit sessionChanged();
}

void TokenManager::updateAccessToken(const QString &accessToken)
{
    m_accessToken = accessToken;
    if (m_storage) {
        m_storage->writeSecret(QStringLiteral("access_token"), m_accessToken);
    }
    emit sessionChanged();
}

void TokenManager::updateDeviceTrust(const QString &status, const QString &trustToken)
{
    m_deviceStatus = status;
    m_deviceTrustToken = trustToken;
    if (m_storage) {
        m_storage->writeSecret(QStringLiteral("device_status"), m_deviceStatus);
        if (!m_deviceTrustToken.isEmpty()) {
            m_storage->writeSecret(QStringLiteral("device_trust_token"), m_deviceTrustToken);
        }
    }
    emit sessionChanged();
}

void TokenManager::clear()
{
    m_accessToken.clear();
    m_refreshToken.clear();
    m_deviceTrustToken.clear();
    m_deviceStatus = QStringLiteral("unknown");
    if (m_storage) {
        m_storage->deleteSecret(QStringLiteral("access_token"));
        m_storage->deleteSecret(QStringLiteral("refresh_token"));
        m_storage->deleteSecret(QStringLiteral("device_trust_token"));
        m_storage->deleteSecret(QStringLiteral("device_status"));
    }
    emit sessionChanged();
}
