#pragma once

#include <QObject>
#include <QString>

class AppStartupManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString startupState READ startupState NOTIFY startupChanged)
    Q_PROPERTY(QString startupMessage READ startupMessage NOTIFY startupChanged)
    Q_PROPERTY(int startupProgress READ startupProgress NOTIFY startupChanged)
    Q_PROPERTY(bool startupComplete READ startupComplete NOTIFY startupChanged)
    Q_PROPERTY(bool backgroundSyncing READ backgroundSyncing NOTIFY startupChanged)
    Q_PROPERTY(QString startupError READ startupError NOTIFY startupChanged)
    Q_PROPERTY(QString defaultWorkspace READ defaultWorkspace NOTIFY startupChanged)

public:
    explicit AppStartupManager(QObject *parent = nullptr);

    QString startupState() const;
    QString startupMessage() const;
    int startupProgress() const;
    bool startupComplete() const;
    bool backgroundSyncing() const;
    QString startupError() const;
    QString defaultWorkspace() const;

    Q_INVOKABLE void begin();
    Q_INVOKABLE void completeForLogin();
    Q_INVOKABLE void checkingSession();
    Q_INVOKABLE void connecting();
    Q_INVOKABLE void deviceBlocked(const QString &reason);
    Q_INVOKABLE void cachedWorkspaceReady(const QString &workspace);
    Q_INVOKABLE void accessReady(const QString &workspace);
    Q_INVOKABLE void backgroundSyncStarted();
    Q_INVOKABLE void backgroundSyncFinished(const QString &message = QString());
    Q_INVOKABLE void fail(const QString &message);

signals:
    void startupChanged();

private:
    void setState(const QString &state,
                  const QString &message,
                  int progress,
                  bool complete,
                  bool backgroundSyncing,
                  const QString &error = QString(),
                  const QString &workspace = QString());

    QString m_state = QStringLiteral("starting");
    QString m_message = QStringLiteral("Starting SkyGrid GCS");
    int m_progress = 5;
    bool m_complete = false;
    bool m_backgroundSyncing = false;
    bool m_startupOverlayConsumed = false;
    QString m_error;
    QString m_defaultWorkspace = QStringLiteral("home");
};
