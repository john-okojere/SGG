#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class PermissionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap permissions READ permissions NOTIFY permissionsChanged)
    Q_PROPERTY(QStringList permissionKeys READ permissionKeys NOTIFY permissionsChanged)
    Q_PROPERTY(bool canConfigureVehicle READ canConfigureVehicle NOTIFY permissionsChanged)
    Q_PROPERTY(bool canRegisterVehicle READ canRegisterVehicle NOTIFY permissionsChanged)
    Q_PROPERTY(bool canBindFlightController READ canBindFlightController NOTIFY permissionsChanged)
    Q_PROPERTY(bool canConfigureRc READ canConfigureRc NOTIFY permissionsChanged)
    Q_PROPERTY(bool canEditVehicleProfile READ canEditVehicleProfile NOTIFY permissionsChanged)
    Q_PROPERTY(bool canReadVehicleParameters READ canReadVehicleParameters NOTIFY permissionsChanged)
    Q_PROPERTY(bool canWriteVehicleParameters READ canWriteVehicleParameters NOTIFY permissionsChanged)
    Q_PROPERTY(bool canReleaseVehicleToOrganization READ canReleaseVehicleToOrganization NOTIFY permissionsChanged)
    Q_PROPERTY(bool canPlanMission READ canPlanMission NOTIFY permissionsChanged)
    Q_PROPERTY(bool canUploadMission READ canUploadMission NOTIFY permissionsChanged)
    Q_PROPERTY(bool canStartMission READ canStartMission NOTIFY permissionsChanged)
    Q_PROPERTY(bool canFlyManual READ canFlyManual NOTIFY permissionsChanged)
    Q_PROPERTY(bool canFlyManualTest READ canFlyManualTest NOTIFY permissionsChanged)
    Q_PROPERTY(bool canRunManufacturerTestFlight READ canRunManufacturerTestFlight NOTIFY permissionsChanged)
    Q_PROPERTY(bool canStreamTelemetry READ canStreamTelemetry NOTIFY permissionsChanged)
    Q_PROPERTY(bool canAssignAircraft READ canAssignAircraft NOTIFY permissionsChanged)
    Q_PROPERTY(bool canAssignPilots READ canAssignPilots NOTIFY permissionsChanged)
    Q_PROPERTY(bool canViewFleet READ canViewFleet NOTIFY permissionsChanged)
    Q_PROPERTY(bool canViewMissionLogs READ canViewMissionLogs NOTIFY permissionsChanged)
    Q_PROPERTY(bool canViewTelemetry READ canViewTelemetry NOTIFY permissionsChanged)
    Q_PROPERTY(bool canViewReports READ canViewReports NOTIFY permissionsChanged)
    Q_PROPERTY(bool canApproveDevices READ canApproveDevices NOTIFY permissionsChanged)
    Q_PROPERTY(bool canRevokeDevices READ canRevokeDevices NOTIFY permissionsChanged)
    Q_PROPERTY(bool canManageUsers READ canManageUsers NOTIFY permissionsChanged)
    Q_PROPERTY(bool canManageRoles READ canManageRoles NOTIFY permissionsChanged)
    Q_PROPERTY(bool canManageManufacturers READ canManageManufacturers NOTIFY permissionsChanged)
    Q_PROPERTY(bool canManageOrganizations READ canManageOrganizations NOTIFY permissionsChanged)
    Q_PROPERTY(bool canUseFlightData READ canUseFlightData NOTIFY permissionsChanged)
    Q_PROPERTY(bool canRunInitialSetup READ canRunInitialSetup NOTIFY permissionsChanged)
    Q_PROPERTY(bool canTuneVehicle READ canTuneVehicle NOTIFY permissionsChanged)
    Q_PROPERTY(bool canFlashFirmware READ canFlashFirmware NOTIFY permissionsChanged)
    Q_PROPERTY(bool canViewLogs READ canViewLogs NOTIFY permissionsChanged)
    Q_PROPERTY(bool canDownloadLogs READ canDownloadLogs NOTIFY permissionsChanged)
    Q_PROPERTY(bool canUseSimulation READ canUseSimulation NOTIFY permissionsChanged)
    Q_PROPERTY(bool canUseAdvancedMavlink READ canUseAdvancedMavlink NOTIFY permissionsChanged)
    Q_PROPERTY(bool canConfigurePayload READ canConfigurePayload NOTIFY permissionsChanged)
    Q_PROPERTY(bool canManageMultiVehicle READ canManageMultiVehicle NOTIFY permissionsChanged)
    Q_PROPERTY(bool canConfigureOptionalHardware READ canConfigureOptionalHardware NOTIFY permissionsChanged)
    Q_PROPERTY(bool canViewVideoStream READ canViewVideoStream NOTIFY permissionsChanged)
    Q_PROPERTY(bool canConfigureVideoPayload READ canConfigureVideoPayload NOTIFY permissionsChanged)
    Q_PROPERTY(bool canUseTerminal READ canUseTerminal NOTIFY permissionsChanged)
    Q_PROPERTY(bool canOverrideParameterSafety READ canOverrideParameterSafety NOTIFY permissionsChanged)

public:
    explicit PermissionManager(QObject *parent = nullptr);

    QVariantMap permissions() const;
    QStringList permissionKeys() const;

    bool canConfigureVehicle() const;
    bool canRegisterVehicle() const;
    bool canBindFlightController() const;
    bool canConfigureRc() const;
    bool canEditVehicleProfile() const;
    bool canReadVehicleParameters() const;
    bool canWriteVehicleParameters() const;
    bool canReleaseVehicleToOrganization() const;
    bool canPlanMission() const;
    bool canUploadMission() const;
    bool canStartMission() const;
    bool canFlyManual() const;
    bool canFlyManualTest() const;
    bool canRunManufacturerTestFlight() const;
    bool canStreamTelemetry() const;
    bool canAssignAircraft() const;
    bool canAssignPilots() const;
    bool canViewFleet() const;
    bool canViewMissionLogs() const;
    bool canViewTelemetry() const;
    bool canViewReports() const;
    bool canApproveDevices() const;
    bool canRevokeDevices() const;
    bool canManageUsers() const;
    bool canManageRoles() const;
    bool canManageManufacturers() const;
    bool canManageOrganizations() const;
    bool canUseFlightData() const;
    bool canRunInitialSetup() const;
    bool canTuneVehicle() const;
    bool canFlashFirmware() const;
    bool canViewLogs() const;
    bool canDownloadLogs() const;
    bool canUseSimulation() const;
    bool canUseAdvancedMavlink() const;
    bool canConfigurePayload() const;
    bool canManageMultiVehicle() const;
    bool canConfigureOptionalHardware() const;
    bool canViewVideoStream() const;
    bool canConfigureVideoPayload() const;
    bool canUseTerminal() const;
    bool canOverrideParameterSafety() const;

    Q_INVOKABLE void applyBootstrap(const QVariantMap &bootstrap);
    Q_INVOKABLE void reset();
    Q_INVOKABLE bool hasPermission(const QString &permission) const;
    Q_INVOKABLE QString denialMessage(const QString &permission) const;

signals:
    void permissionsChanged();

private:
    static QString normalizePermission(const QString &permission);
    static QVariantMap mapFromVariant(const QVariant &value);
    static QStringList listFromVariant(const QVariant &value);
    void setPermissionMap(const QVariantMap &permissions);

    QVariantMap m_permissions;
};
