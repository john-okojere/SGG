#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class AccessManager;
class ApiClient;
class GcsEventSyncManager;
class SessionManager;

class ManufacturerVehicleManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading NOTIFY stateChanged)
    Q_PROPERTY(QString status READ status NOTIFY stateChanged)
    Q_PROPERTY(QString error READ error NOTIFY stateChanged)
    Q_PROPERTY(QVariantList profiles READ profiles NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap selectedProfile READ selectedProfile NOTIFY dataChanged)
    Q_PROPERTY(QVariantList auditRecords READ auditRecords NOTIFY dataChanged)

public:
    explicit ManufacturerVehicleManager(ApiClient *api,
                                        SessionManager *session,
                                        AccessManager *access,
                                        GcsEventSyncManager *events = nullptr,
                                        QObject *parent = nullptr);

    bool loading() const;
    QString status() const;
    QString error() const;
    QVariantList profiles() const;
    QVariantMap selectedProfile() const;
    QVariantList auditRecords() const;

    Q_INVOKABLE void fetchVehicleProfiles();
    Q_INVOKABLE void fetchProfileDetail(const QVariant &profileId);
    Q_INVOKABLE void selectProfile(const QVariant &profileId);
    Q_INVOKABLE void createVehicleProfile(const QVariantMap &profile);
    Q_INVOKABLE void updateVehicleProfile(const QVariant &profileId, const QVariantMap &profile);
    Q_INVOKABLE void bindFlightController(const QVariant &profileId, const QString &uid, const QString &autopilot);
    Q_INVOKABLE void saveRcMapping(const QVariant &profileId, const QVariantMap &mapping);
    Q_INVOKABLE void saveParameterSnapshot(const QVariant &profileId, const QVariantMap &snapshot);
    Q_INVOKABLE void releaseVehicleToOrganization(const QVariant &profileId, const QVariant &organizationId, const QString &notes);
    Q_INVOKABLE void lockVehicle(const QVariant &profileId);
    Q_INVOKABLE void fetchVehicleAudit(const QVariant &profileId);

signals:
    void stateChanged();
    void dataChanged();

private:
    bool ensureOnlineAction(const QString &action,
                            const QStringList &permissions,
                            const QString &message,
                            const QVariantMap &context = {});
    bool profileInScope(const QString &profileId) const;
    QString idString(const QVariant &value) const;
    QVariantList listFromBody(const QJsonObject &body) const;
    void upsertProfile(const QVariantMap &profile);
    void setLoading(bool loading);
    void setStatus(const QString &status);
    void setError(const QString &error);
    void recordEvent(const QString &eventType, const QString &severity, const QString &message, const QVariantMap &payload = {});

    ApiClient *m_api = nullptr;
    SessionManager *m_session = nullptr;
    AccessManager *m_access = nullptr;
    GcsEventSyncManager *m_events = nullptr;
    bool m_loading = false;
    QString m_status = "Manufacturer vehicle manager idle";
    QString m_error;
    QVariantList m_profiles;
    QVariantMap m_selectedProfile;
    QVariantList m_auditRecords;
};
