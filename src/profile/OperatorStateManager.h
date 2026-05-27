#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

class LocalSyncCache;

class OperatorStateManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString activeAircraftName READ activeAircraftName NOTIFY operatorStateChanged)
    Q_PROPERTY(QString activeAircraftId READ activeAircraftId NOTIFY operatorStateChanged)
    Q_PROPERTY(QString workspaceMode READ workspaceMode NOTIFY operatorStateChanged)
    Q_PROPERTY(QVariantMap layoutState READ layoutState NOTIFY operatorStateChanged)

public:
    explicit OperatorStateManager(LocalSyncCache *cache, QObject *parent = nullptr);

    QString activeAircraftName() const;
    QString activeAircraftId() const;
    QString workspaceMode() const;
    QVariantMap layoutState() const;

    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);
    Q_INVOKABLE void setActiveAircraft(const QVariantMap &aircraft);

signals:
    void operatorStateChanged();

private:
    void loadCached();
    void persist();

    LocalSyncCache *m_cache = nullptr;
    QVariantMap m_activeAircraft;
    QString m_workspaceMode = "Mission Operations";
    QVariantMap m_layoutState;
};
