#include "MissionPreviewManager.h"

#include "../auth/SessionManager.h"
#include "../network/ApiClient.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>

MissionPreviewManager::MissionPreviewManager(ApiClient *api, SessionManager *session, QObject *parent)
    : QObject(parent), m_api(api), m_session(session)
{
}

QString MissionPreviewManager::status() const { return m_status; }

QString MissionPreviewManager::previewPath(const QString &missionId, const QString &stage) const
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (base.isEmpty()) {
        base = QDir::tempPath() + QStringLiteral("/SkyGridGCS");
    }
    QDir dir(base + QStringLiteral("/mission-previews"));
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }
    const QString safeMission = missionId.trimmed().isEmpty() ? QStringLiteral("local") : missionId;
    const QString safeStage = stage.trimmed().isEmpty() ? QStringLiteral("saved") : stage;
    return dir.filePath(QStringLiteral("%1-%2.png").arg(safeMission, safeStage));
}

void MissionPreviewManager::syncPreview(const QString &missionId, const QString &stage, const QString &filePath, int width, int height)
{
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        setStatus(QStringLiteral("Preview held until Control Center session is trusted"));
        emit previewSynced(missionId, stage, false, m_status);
        return;
    }
    bool numeric = false;
    const int id = missionId.toInt(&numeric);
    if (!numeric || id <= 0) {
        setStatus(QStringLiteral("Preview held until mission has a Control Center id"));
        emit previewSynced(missionId, stage, false, m_status);
        return;
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        setStatus(QStringLiteral("Preview sync failed: image file is unavailable"));
        emit previewSynced(missionId, stage, false, m_status);
        return;
    }
    const QByteArray data = file.readAll();
    if (data.isEmpty()) {
        setStatus(QStringLiteral("Preview sync failed: image file is empty"));
        emit previewSynced(missionId, stage, false, m_status);
        return;
    }
    QJsonObject payload{
        {QStringLiteral("stage"), stage},
        {QStringLiteral("format"), QStringLiteral("png")},
        {QStringLiteral("image_base64"), QString::fromLatin1(data.toBase64())},
        {QStringLiteral("width"), width},
        {QStringLiteral("height"), height},
        {QStringLiteral("captured_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
    };
    setStatus(QStringLiteral("Syncing mission preview"));
    m_api->post(QStringLiteral("/api/missions/%1/preview/").arg(id), payload, true, true,
                [this, missionId, stage](int statusCode, const QJsonObject &, const QString &error) {
        const bool ok = statusCode >= 200 && statusCode < 300;
        const QString message = ok
            ? QStringLiteral("Mission preview synced")
            : QStringLiteral("Mission preview sync failed: %1").arg(error.isEmpty() ? QString::number(statusCode) : error);
        setStatus(message);
        emit previewSynced(missionId, stage, ok, message);
    });
}

void MissionPreviewManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit statusChanged();
}
