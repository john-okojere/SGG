#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class AccessManager;
class GcsEventSyncManager;
class MavsdkVehicleManager;

class AutopilotToolsFacade : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap boardInfo READ boardInfo NOTIFY facadeChanged)
    Q_PROPERTY(QVariantList capabilities READ capabilities NOTIFY facadeChanged)
    Q_PROPERTY(QString status READ status NOTIFY facadeChanged)
    Q_PROPERTY(bool detecting READ detecting NOTIFY facadeChanged)

public:
    explicit AutopilotToolsFacade(MavsdkVehicleManager *vehicle,
                                  AccessManager *access,
                                  GcsEventSyncManager *events,
                                  QObject *parent = nullptr);

    QVariantMap boardInfo() const;
    QVariantList capabilities() const;
    QString status() const;
    bool detecting() const;

    Q_INVOKABLE void detectBoard();
    Q_INVOKABLE void refreshCapabilities();
    Q_INVOKABLE void markUnsupported(const QString &feature);

signals:
    void facadeChanged();

private:
    void rebuildBoardInfo();
    void setStatus(const QString &status);

    MavsdkVehicleManager *m_vehicle = nullptr;
    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QVariantMap m_boardInfo;
    QVariantList m_capabilities;
    QString m_status = "Autopilot tools idle";
    bool m_detecting = false;
};
