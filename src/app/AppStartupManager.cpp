#include "AppStartupManager.h"

AppStartupManager::AppStartupManager(QObject *parent)
    : QObject(parent)
{
}

QString AppStartupManager::startupState() const { return m_state; }
QString AppStartupManager::startupMessage() const { return m_message; }
int AppStartupManager::startupProgress() const { return m_progress; }
bool AppStartupManager::startupComplete() const { return m_complete; }
bool AppStartupManager::backgroundSyncing() const { return m_backgroundSyncing; }
QString AppStartupManager::startupError() const { return m_error; }
QString AppStartupManager::defaultWorkspace() const { return m_defaultWorkspace; }

void AppStartupManager::begin()
{
    m_startupOverlayConsumed = false;
    setState(QStringLiteral("starting"), QStringLiteral("Starting SkyGrid GCS"), 8, false, false);
}

void AppStartupManager::completeForLogin()
{
    setState(QStringLiteral("login_required"), QStringLiteral("Sign in with your Control Center account."), 100, true, false);
}

void AppStartupManager::checkingSession()
{
    if (m_startupOverlayConsumed) {
        setState(QStringLiteral("checking_session"),
                 QStringLiteral("Checking local session"),
                 100,
                 true,
                 true,
                 QString(),
                 m_defaultWorkspace);
        return;
    }
    setState(QStringLiteral("checking_session"), QStringLiteral("Checking local session"), 24, false, false);
}

void AppStartupManager::connecting()
{
    if (m_startupOverlayConsumed) {
        setState(QStringLiteral("connecting"),
                 QStringLiteral("Connecting to Control Center"),
                 100,
                 true,
                 true,
                 QString(),
                 m_defaultWorkspace);
        return;
    }
    setState(QStringLiteral("connecting"), QStringLiteral("Connecting to Control Center"), 42, false, false);
}

void AppStartupManager::deviceBlocked(const QString &reason)
{
    const bool foregroundStartupWasActive = !m_complete;
    setState(QStringLiteral("device_blocked"),
             reason.isEmpty() ? QStringLiteral("Device approval required.") : reason,
             100,
             true,
             false,
             reason);
    if (foregroundStartupWasActive) {
        m_startupOverlayConsumed = true;
    }
}

void AppStartupManager::cachedWorkspaceReady(const QString &workspace)
{
    setState(QStringLiteral("cached_workspace"),
             QStringLiteral("Control Center unavailable. Loading cached workspace."),
             72,
             true,
             true,
             QString(),
             workspace);
    m_startupOverlayConsumed = true;
}

void AppStartupManager::accessReady(const QString &workspace)
{
    setState(QStringLiteral("workspace_ready"),
             QStringLiteral("Preparing workspace"),
             88,
             true,
             true,
             QString(),
             workspace);
    m_startupOverlayConsumed = true;
}

void AppStartupManager::backgroundSyncStarted()
{
    setState(QStringLiteral("background_sync"),
             QStringLiteral("Syncing in background"),
             94,
             true,
             true,
             QString(),
             m_defaultWorkspace);
}

void AppStartupManager::backgroundSyncFinished(const QString &message)
{
    setState(QStringLiteral("ready"),
             message.isEmpty() ? QStringLiteral("Workspace ready") : message,
             100,
             true,
             false,
             QString(),
             m_defaultWorkspace);
}

void AppStartupManager::fail(const QString &message)
{
    setState(QStringLiteral("offline"),
             message.isEmpty() ? QStringLiteral("Control Center unavailable. Loading cached workspace.") : message,
             100,
             true,
             false,
             message,
             m_defaultWorkspace);
    m_startupOverlayConsumed = true;
}

void AppStartupManager::setState(const QString &state,
                                 const QString &message,
                                 int progress,
                                 bool complete,
                                 bool backgroundSyncing,
                                 const QString &error,
                                 const QString &workspace)
{
    const QString nextWorkspace = workspace.isEmpty() ? m_defaultWorkspace : workspace;
    if (m_state == state
        && m_message == message
        && m_progress == progress
        && m_complete == complete
        && m_backgroundSyncing == backgroundSyncing
        && m_error == error
        && m_defaultWorkspace == nextWorkspace) {
        return;
    }
    m_state = state;
    m_message = message;
    m_progress = progress;
    m_complete = complete;
    m_backgroundSyncing = backgroundSyncing;
    m_error = error;
    m_defaultWorkspace = nextWorkspace;
    emit startupChanged();
}
