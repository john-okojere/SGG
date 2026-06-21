#pragma once

#include <QObject>
#include <QVariantMap>
#include <QVariantList>

class AccessManager;
class GcsEventSyncManager;
class MavsdkVehicleManager;
class SessionManager;

class AdvancedMissionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList missionItems READ missionItems NOTIFY missionChanged)
    Q_PROPERTY(QVariantList geofenceItems READ geofenceItems NOTIFY missionChanged)
    Q_PROPERTY(QVariantList rallyItems READ rallyItems NOTIFY missionChanged)
    Q_PROPERTY(QVariantList commandCatalog READ commandCatalog CONSTANT)
    Q_PROPERTY(QVariantList frameCatalog READ frameCatalog CONSTANT)
    Q_PROPERTY(bool useForUpload READ useForUpload WRITE setUseForUpload NOTIFY missionChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY missionChanged)
    Q_PROPERTY(QString status READ status NOTIFY missionChanged)
    Q_PROPERTY(QString compareStatus READ compareStatus NOTIFY missionChanged)

public:
    explicit AdvancedMissionManager(MavsdkVehicleManager *vehicle,
                                    AccessManager *access,
                                    SessionManager *session,
                                    GcsEventSyncManager *events,
                                    QObject *parent = nullptr);

    QVariantList missionItems() const;
    QVariantList geofenceItems() const;
    QVariantList rallyItems() const;
    QVariantList commandCatalog() const;
    QVariantList frameCatalog() const;
    bool useForUpload() const;
    bool busy() const;
    QString status() const;
    QString compareStatus() const;

    void setUseForUpload(bool useForUpload);
    bool hasUploadableMissionItems() const;
    bool containsTakeoffCommand() const;

    Q_INVOKABLE void readMissionFromAircraft();
    Q_INVOKABLE void readGeofenceFromAircraft();
    Q_INVOKABLE void readRallyPointsFromAircraft();
    Q_INVOKABLE void writeMissionToAircraft();
    Q_INVOKABLE void writeGeofenceToAircraft();
    Q_INVOKABLE void writeRallyPointsToAircraft();
    Q_INVOKABLE void compareLocalToAircraft();
    Q_INVOKABLE void saveMissionFile(const QString &pathOrUrl);
    Q_INVOKABLE void loadMissionFile(const QString &pathOrUrl);
    Q_INVOKABLE void addMissionItem(const QVariantMap &item);
    Q_INVOKABLE void addMissionCommand(int command,
                                       int frame,
                                       double param1 = 0.0,
                                       double param2 = 0.0,
                                       double param3 = 0.0,
                                       double param4 = 0.0,
                                       double latitude = 0.0,
                                       double longitude = 0.0,
                                       double altitude = 0.0,
                                       int missionType = 0);
    Q_INVOKABLE void buildFromSkyGridRoute(const QVariantList &route,
                                           const QVariantMap &takeoffPoint,
                                           const QString &finishAction);
    Q_INVOKABLE void updateMissionItem(int index, const QVariantMap &changes);
    Q_INVOKABLE void duplicateMissionItem(int index);
    Q_INVOKABLE void moveMissionItem(int fromIndex, int toIndex);
    Q_INVOKABLE void removeMissionItem(int index);
    Q_INVOKABLE void clearMissionItems();
    Q_INVOKABLE void addGeofencePoint(double latitude, double longitude, double altitude, int command);
    Q_INVOKABLE void updateGeofenceItem(int index, const QVariantMap &changes);
    Q_INVOKABLE void removeGeofenceItem(int index);
    Q_INVOKABLE void clearGeofenceItems();
    Q_INVOKABLE void addRallyPoint(double latitude, double longitude, double altitude);
    Q_INVOKABLE void updateRallyItem(int index, const QVariantMap &changes);
    Q_INVOKABLE void removeRallyItem(int index);
    Q_INVOKABLE void clearRallyItems();

signals:
    void missionChanged();

private:
    QString normalizePath(const QString &pathOrUrl) const;
    bool guardWritableMissionOperation(const QString &label, const QString &eventType);
    void reindexMissionItems();
    void setBusy(bool busy);
    void setStatus(const QString &status);
    void setCompareStatus(const QString &status);

    MavsdkVehicleManager *m_vehicle = nullptr;
    AccessManager *m_access = nullptr;
    SessionManager *m_session = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    QVariantList m_missionItems;
    QVariantList m_geofenceItems;
    QVariantList m_rallyItems;
    bool m_useForUpload = false;
    bool m_busy = false;
    QString m_status = "Advanced mission editor idle";
    QString m_compareStatus = "No mission comparison loaded.";
};
