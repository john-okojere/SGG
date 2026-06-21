#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class ModuleAccessManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList allowedModules READ allowedModules NOTIFY modulesChanged)
    Q_PROPERTY(QStringList visibleModules READ visibleModules NOTIFY modulesChanged)
    Q_PROPERTY(QString defaultWorkspace READ defaultWorkspace NOTIFY modulesChanged)
    Q_PROPERTY(bool missionWorkspaceAllowed READ missionWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool pilotWorkspaceAllowed READ pilotWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool manufacturerTestFlightWorkspaceAllowed READ manufacturerTestFlightWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool vehicleConfigurationWorkspaceAllowed READ vehicleConfigurationWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool fleetWorkspaceAllowed READ fleetWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool administrationWorkspaceAllowed READ administrationWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool gcsToolsWorkspaceAllowed READ gcsToolsWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool flightDataWorkspaceAllowed READ flightDataWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool initialSetupWorkspaceAllowed READ initialSetupWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool configurationTuningWorkspaceAllowed READ configurationTuningWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool logsAnalysisWorkspaceAllowed READ logsAnalysisWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool simulationWorkspaceAllowed READ simulationWorkspaceAllowed NOTIFY modulesChanged)
    Q_PROPERTY(bool advancedToolsWorkspaceAllowed READ advancedToolsWorkspaceAllowed NOTIFY modulesChanged)

public:
    explicit ModuleAccessManager(QObject *parent = nullptr);

    QStringList allowedModules() const;
    QStringList visibleModules() const;
    QString defaultWorkspace() const;
    bool missionWorkspaceAllowed() const;
    bool pilotWorkspaceAllowed() const;
    bool manufacturerTestFlightWorkspaceAllowed() const;
    bool vehicleConfigurationWorkspaceAllowed() const;
    bool fleetWorkspaceAllowed() const;
    bool administrationWorkspaceAllowed() const;
    bool gcsToolsWorkspaceAllowed() const;
    bool flightDataWorkspaceAllowed() const;
    bool initialSetupWorkspaceAllowed() const;
    bool configurationTuningWorkspaceAllowed() const;
    bool logsAnalysisWorkspaceAllowed() const;
    bool simulationWorkspaceAllowed() const;
    bool advancedToolsWorkspaceAllowed() const;

    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);
    Q_INVOKABLE void reset();
    Q_INVOKABLE bool moduleAllowed(const QString &module) const;

signals:
    void modulesChanged();

private:
    static QString normalizeModule(const QString &module);
    static QStringList listFromVariant(const QVariant &value);
    static bool permissionEnabled(const QVariant &permissions, const QString &scope);
    void setAllowedModules(const QStringList &modules);

    QStringList m_allowedModules;
};
