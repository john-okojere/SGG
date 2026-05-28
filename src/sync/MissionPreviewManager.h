#pragma once

#include <QObject>
#include <QString>

class ApiClient;
class SessionManager;

class MissionPreviewManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit MissionPreviewManager(ApiClient *api, SessionManager *session, QObject *parent = nullptr);

    QString status() const;

    Q_INVOKABLE QString previewPath(const QString &missionId, const QString &stage) const;
    Q_INVOKABLE void syncPreview(const QString &missionId, const QString &stage, const QString &filePath, int width, int height);

signals:
    void statusChanged();
    void previewSynced(const QString &missionId, const QString &stage, bool success, const QString &message);

private:
    void setStatus(const QString &status);

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    QString m_status = "No mission preview synced";
};
