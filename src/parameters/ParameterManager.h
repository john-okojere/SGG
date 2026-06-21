#pragma once

#include <QObject>
#include <QVariantList>

class AccessManager;
class GcsEventSyncManager;
class ManualControlManager;
class MavsdkVehicleManager;
class MissionExecutionManager;
class ParameterMetadataManager;
class SessionManager;

class ParameterManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList parameters READ parameters NOTIFY parametersChanged)
    Q_PROPERTY(QVariantList filteredParameters READ filteredParameters NOTIFY parametersChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY parametersChanged)
    Q_PROPERTY(bool modifiedOnly READ modifiedOnly WRITE setModifiedOnly NOTIFY parametersChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY parametersChanged)
    Q_PROPERTY(QString status READ status NOTIFY parametersChanged)
    Q_PROPERTY(int changedCount READ changedCount NOTIFY parametersChanged)
    Q_PROPERTY(QString compareStatus READ compareStatus NOTIFY parametersChanged)
    Q_PROPERTY(QVariantList setupGroups READ setupGroups NOTIFY parametersChanged)
    Q_PROPERTY(QString selectedGroup READ selectedGroup WRITE setSelectedGroup NOTIFY parametersChanged)
    Q_PROPERTY(bool rawEditorAllowed READ rawEditorAllowed NOTIFY parametersChanged)

public:
    explicit ParameterManager(MavsdkVehicleManager *vehicle,
                              ParameterMetadataManager *metadata,
                              AccessManager *access,
                              SessionManager *session,
                              MissionExecutionManager *missionExecution,
                              ManualControlManager *manualControl,
                              GcsEventSyncManager *events,
                              QObject *parent = nullptr);

    QVariantList parameters() const;
    QVariantList filteredParameters() const;
    QString searchText() const;
    bool modifiedOnly() const;
    bool busy() const;
    QString status() const;
    int changedCount() const;
    QString compareStatus() const;
    QVariantList setupGroups() const;
    QString selectedGroup() const;
    bool rawEditorAllowed() const;

    void setSearchText(const QString &text);
    void setModifiedOnly(bool modifiedOnly);
    void setSelectedGroup(const QString &group);

    Q_INVOKABLE void readAllParameters();
    Q_INVOKABLE void refreshParameters();
    Q_INVOKABLE void setDraftValue(const QString &name, const QString &value);
    Q_INVOKABLE void resetDraft(const QString &name);
    Q_INVOKABLE void writeChangedParameters();
    Q_INVOKABLE void writeParameterGroup(const QString &group);
    Q_INVOKABLE void saveToFile(const QString &pathOrUrl);
    Q_INVOKABLE void loadFromFile(const QString &pathOrUrl);
    Q_INVOKABLE void compareWithFile(const QString &pathOrUrl);

signals:
    void parametersChanged();

private:
    QString normalizePath(const QString &pathOrUrl) const;
    QString categoryForName(const QString &name) const;
    bool canWriteParameters(QString *reason = nullptr) const;
    QVariantMap decorateRow(const QVariantMap &row) const;
    void applyParameterRows(const QVariantList &rows, const QString &status);
    void writeRows(const QVariantList &writes, const QString &scopeLabel);
    void setBusy(bool busy);
    void setStatus(const QString &status);
    void recalculateChangedCount();

    MavsdkVehicleManager *m_vehicle = nullptr;
    ParameterMetadataManager *m_metadata = nullptr;
    AccessManager *m_access = nullptr;
    SessionManager *m_session = nullptr;
    MissionExecutionManager *m_missionExecution = nullptr;
    ManualControlManager *m_manualControl = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QVariantList m_parameters;
    QString m_searchText;
    bool m_modifiedOnly = false;
    bool m_busy = false;
    QString m_status = "Parameter editor idle";
    QString m_compareStatus = "No comparison loaded.";
    QString m_selectedGroup = "All";
    int m_changedCount = 0;
};
