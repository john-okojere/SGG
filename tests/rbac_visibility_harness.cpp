#include "../src/access/ModuleAccessManager.h"
#include "../src/access/PermissionManager.h"
#include "../src/access/RbacBootstrapNormalizer.h"
#include "../src/access/RoleAccessManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <QVariantList>
#include <QVariantMap>

namespace {

struct Expected {
    QString name;
    QString role;
    QStringList permissions;
    bool trusted = true;
    bool reachable = true;
    bool pilot = false;
    bool mission = false;
    bool flightData = false;
    bool gcsTools = false;
    bool fleet = false;
    bool manufacturer = false;
    bool administration = false;
    bool logs = false;
    bool release = false;
    QStringList aircraftIds;
    QStringList missionIds;
};

QVariantMap permissionMap(const QStringList &permissions)
{
    QVariantMap map;
    for (const QString &permission : permissions) {
        map.insert(permission, true);
    }
    return map;
}

QVariantList objectIds(const QStringList &ids)
{
    QVariantList rows;
    for (const QString &id : ids) {
        rows << QVariantMap{{QStringLiteral("id"), id}};
    }
    return rows;
}

QVariantMap bootstrapFor(const Expected &expected, bool nestedPilotPermissions = false)
{
    QVariantMap bootstrap{
        {QStringLiteral("roles"), QStringList{expected.role}},
        {QStringLiteral("session_status"), QVariantMap{
             {QStringLiteral("operations_allowed"), expected.trusted},
             {QStringLiteral("backend_reachable"), expected.reachable},
             {QStringLiteral("status"), expected.trusted ? QStringLiteral("approved") : QStringLiteral("pending")}
         }},
        {QStringLiteral("device_summary"), QVariantMap{
             {QStringLiteral("status"), expected.trusted ? QStringLiteral("approved") : QStringLiteral("revoked")},
             {QStringLiteral("approved"), expected.trusted}
         }},
        {QStringLiteral("organization"), QVariantMap{{QStringLiteral("id"), 101}}},
        {QStringLiteral("assigned_aircraft"), objectIds(expected.aircraftIds)},
        {QStringLiteral("assigned_missions"), objectIds(expected.missionIds)}
    };

    if (nestedPilotPermissions) {
        bootstrap.insert(QStringLiteral("pilot_profile"), QVariantMap{
            {QStringLiteral("role"), expected.role},
            {QStringLiteral("permissions"), permissionMap(expected.permissions)}
        });
    } else {
        bootstrap.insert(QStringLiteral("permissions"), permissionMap(expected.permissions));
    }
    return bootstrap;
}

bool containsAll(const QStringList &actual, const QStringList &expected)
{
    for (const QString &value : expected) {
        if (!actual.contains(value)) {
            return false;
        }
    }
    return true;
}

QStringList sortedIds(const QSet<QString> &ids)
{
    QStringList out(ids.cbegin(), ids.cend());
    out.sort();
    return out;
}

bool check(bool condition, const QString &message, QStringList &errors)
{
    if (!condition) {
        errors << message;
    }
    return condition;
}

bool runCase(const Expected &expected, bool nestedPilotPermissions, QStringList &errors)
{
    const QVariantMap bootstrap = bootstrapFor(expected, nestedPilotPermissions);
    PermissionManager permissionManager;
    ModuleAccessManager moduleAccessManager;
    RoleAccessManager roleAccessManager;
    permissionManager.applyBootstrap(bootstrap);
    moduleAccessManager.applyBootstrap(bootstrap);
    roleAccessManager.applyBootstrap(bootstrap);
    const RbacBootstrapProfile profile = RbacBootstrapNormalizer::fromBootstrap(bootstrap);

    const QString prefix = expected.name + QStringLiteral(": ");
    check(containsAll(permissionManager.permissionKeys(), RbacBootstrapNormalizer::enabledPermissionKeys(permissionMap(expected.permissions))),
          prefix + QStringLiteral("permission keys did not normalize from bootstrap"),
          errors);
    check(profile.sessionTrusted == expected.trusted,
          prefix + QStringLiteral("trusted-device state mismatch"),
          errors);
    check(profile.backendReachable == expected.reachable,
          prefix + QStringLiteral("backend reachability mismatch"),
          errors);
    check(sortedIds(profile.aircraftIds) == expected.aircraftIds,
          prefix + QStringLiteral("aircraft scope mismatch"),
          errors);
    check(sortedIds(profile.missionIds) == expected.missionIds,
          prefix + QStringLiteral("mission scope mismatch"),
          errors);

    check((permissionManager.canFlyManual() && moduleAccessManager.pilotWorkspaceAllowed() && profile.sessionTrusted) == expected.pilot,
          prefix + QStringLiteral("Pilot Mode visibility mismatch"),
          errors);
    check((permissionManager.canPlanMission() && moduleAccessManager.missionWorkspaceAllowed()) == expected.mission,
          prefix + QStringLiteral("Mission Planner visibility mismatch"),
          errors);
    const bool flightDataVisible = (permissionManager.canUseFlightData()
                                    || permissionManager.canStreamTelemetry()
                                    || permissionManager.canViewTelemetry())
        && moduleAccessManager.flightDataWorkspaceAllowed();
    check(flightDataVisible == expected.flightData,
          prefix + QStringLiteral("Flight Data visibility mismatch"),
          errors);
    check(moduleAccessManager.gcsToolsWorkspaceAllowed() == expected.gcsTools,
          prefix + QStringLiteral("GCS Tools workspace visibility mismatch"),
          errors);
    check(moduleAccessManager.fleetWorkspaceAllowed() == expected.fleet,
          prefix + QStringLiteral("Fleet workspace visibility mismatch"),
          errors);
    check((moduleAccessManager.vehicleConfigurationWorkspaceAllowed()
           || moduleAccessManager.manufacturerTestFlightWorkspaceAllowed()
           || moduleAccessManager.moduleAllowed(QStringLiteral("manufacturer_dashboard"))) == expected.manufacturer,
          prefix + QStringLiteral("Manufacturer workspace visibility mismatch"),
          errors);
    check(moduleAccessManager.administrationWorkspaceAllowed() == expected.administration,
          prefix + QStringLiteral("Administration workspace visibility mismatch"),
          errors);
    check(moduleAccessManager.logsAnalysisWorkspaceAllowed() == expected.logs,
          prefix + QStringLiteral("Logs workspace visibility mismatch"),
          errors);
    check(moduleAccessManager.moduleAllowed(QStringLiteral("release_vehicle")) == expected.release,
          prefix + QStringLiteral("Release action visibility mismatch"),
          errors);
    const bool uploadVisible = permissionManager.canUploadMission();
    const bool startVisible = permissionManager.canStartMission();
    if (expected.permissions.contains(QStringLiteral("can_upload_mission"))) {
        check(uploadVisible, prefix + QStringLiteral("mission upload action hidden despite can_upload_mission"), errors);
    }
    if (expected.permissions.contains(QStringLiteral("can_start_mission"))) {
        check(startVisible, prefix + QStringLiteral("mission start action hidden despite can_start_mission"), errors);
    }
    if (!expected.permissions.contains(QStringLiteral("can_fly_manual"))) {
        check(!permissionManager.canFlyManual(), prefix + QStringLiteral("manual flight action visible without can_fly_manual"), errors);
    }
    if (!expected.permissions.contains(QStringLiteral("can_plan_mission"))) {
        check(!permissionManager.canPlanMission(), prefix + QStringLiteral("mission planning action visible without can_plan_mission"), errors);
    }
    return true;
}

bool runPermissionRefreshCase(QStringList &errors)
{
    Expected fullPilot{
        QStringLiteral("Changed permissions after re-login"),
        QStringLiteral("pilot"),
        {QStringLiteral("can_fly_manual"), QStringLiteral("can_plan_mission"), QStringLiteral("can_view_telemetry")},
        true,
        true,
        true,
        true,
        true,
        true,
        false,
        false,
        false,
        false,
        false,
        {QStringLiteral("AC-1")},
        {QStringLiteral("MS-1")}
    };
    Expected viewOnly = fullPilot;
    viewOnly.permissions = {QStringLiteral("can_view_telemetry")};
    viewOnly.pilot = false;
    viewOnly.mission = false;
    viewOnly.flightData = true;

    PermissionManager permissionManager;
    ModuleAccessManager moduleAccessManager;
    permissionManager.applyBootstrap(bootstrapFor(fullPilot, true));
    moduleAccessManager.applyBootstrap(bootstrapFor(fullPilot, true));
    check(permissionManager.canFlyManual(), QStringLiteral("Refresh: full pilot did not gain can_fly_manual"), errors);
    check(moduleAccessManager.pilotWorkspaceAllowed(), QStringLiteral("Refresh: full pilot did not gain pilot module"), errors);

    permissionManager.applyBootstrap(bootstrapFor(viewOnly, true));
    moduleAccessManager.applyBootstrap(bootstrapFor(viewOnly, true));
    check(!permissionManager.canFlyManual(), QStringLiteral("Refresh: removed can_fly_manual stayed enabled"), errors);
    check(!moduleAccessManager.pilotWorkspaceAllowed(), QStringLiteral("Refresh: removed pilot module stayed visible"), errors);
    check(permissionManager.canViewTelemetry(), QStringLiteral("Refresh: telemetry permission was lost"), errors);
    check(moduleAccessManager.flightDataWorkspaceAllowed(), QStringLiteral("Refresh: flight data module was lost"), errors);
    return true;
}

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList errors;
    const QList<Expected> cases{
        {QStringLiteral("Pilot full flight permissions"),
         QStringLiteral("pilot"),
         {QStringLiteral("can_fly_manual"),
          QStringLiteral("can_plan_mission"),
          QStringLiteral("can_upload_mission"),
          QStringLiteral("can_start_mission"),
          QStringLiteral("can_stream_telemetry"),
          QStringLiteral("can_view_telemetry")},
         true, true, true, true, true, true, false, false, false, false, false,
         {QStringLiteral("AC-1")}, {QStringLiteral("MS-1")}},
        {QStringLiteral("Pilot view-only telemetry"),
         QStringLiteral("pilot"),
         {QStringLiteral("can_view_telemetry")},
         true, true, false, false, true, true, false, false, false, false, false,
         {QStringLiteral("AC-2")}, {QStringLiteral("MS-2")}},
        {QStringLiteral("Pilot mission planning no manual"),
         QStringLiteral("pilot"),
         {QStringLiteral("can_plan_mission")},
         true, true, false, true, false, true, false, false, false, false, false,
         {QStringLiteral("AC-3")}, {QStringLiteral("MS-3")}},
        {QStringLiteral("Fleet manager no flight authority"),
         QStringLiteral("fleet_manager"),
         {QStringLiteral("can_view_fleet"), QStringLiteral("can_assign_aircraft")},
         true, true, false, false, false, true, true, false, false, false, false,
         {QStringLiteral("AC-4"), QStringLiteral("AC-5")}, {}},
        {QStringLiteral("Manufacturer engineer setup"),
         QStringLiteral("manufacturer_engineer"),
         {QStringLiteral("can_configure_vehicle"),
          QStringLiteral("can_bind_flight_controller"),
          QStringLiteral("can_configure_rc"),
          QStringLiteral("can_read_vehicle_parameters")},
         true, true, false, false, false, true, false, true, false, false, false,
         {QStringLiteral("MFG-1")}, {}},
        {QStringLiteral("Manufacturer admin release"),
         QStringLiteral("manufacturer_admin"),
         {QStringLiteral("can_configure_vehicle"), QStringLiteral("can_release_vehicle_to_organization")},
         true, true, false, false, false, true, false, true, false, false, true,
         {QStringLiteral("MFG-2")}, {}},
        {QStringLiteral("Manufacturer manual flight authority"),
         QStringLiteral("manufacturer_engineer"),
         {QStringLiteral("can_configure_vehicle"), QStringLiteral("can_fly_manual")},
         true, true, true, false, false, true, false, true, false, false, false,
         {QStringLiteral("MFG-3")}, {}},
        {QStringLiteral("Manufacturer mission planning authority"),
         QStringLiteral("manufacturer_engineer"),
         {QStringLiteral("can_configure_vehicle"), QStringLiteral("can_plan_mission")},
         true, true, false, true, false, true, false, true, false, false, false,
         {QStringLiteral("MFG-4")}, {QStringLiteral("MFG-MS-1")}},
        {QStringLiteral("Auditor read only"),
         QStringLiteral("auditor"),
         {QStringLiteral("can_view_reports"),
          QStringLiteral("can_view_mission_logs"),
          QStringLiteral("can_view_vehicle_audit")},
         true, true, false, false, false, true, false, false, false, true, false,
         {}, {QStringLiteral("AUD-1")}},
        {QStringLiteral("User with no permissions"),
         QStringLiteral("pilot"),
         {},
         true, true, false, false, false, false, false, false, false, false, false,
         {}, {}},
        {QStringLiteral("Pending trusted device"),
         QStringLiteral("pilot"),
         {QStringLiteral("can_fly_manual")},
         false, true, false, false, false, true, false, false, false, false, false,
         {QStringLiteral("AC-P")}, {}},
        {QStringLiteral("Revoked trusted device"),
         QStringLiteral("pilot"),
         {QStringLiteral("can_fly_manual")},
         false, true, false, false, false, true, false, false, false, false, false,
         {QStringLiteral("AC-R")}, {}},
        {QStringLiteral("Offline cached access"),
         QStringLiteral("pilot"),
         {QStringLiteral("can_view_telemetry")},
         true, false, false, false, true, true, false, false, false, false, false,
         {QStringLiteral("AC-O")}, {QStringLiteral("MS-O")}}
    };

    for (const Expected &testCase : cases) {
        runCase(testCase, testCase.role == QStringLiteral("pilot"), errors);
    }
    runPermissionRefreshCase(errors);

    if (!errors.isEmpty()) {
        QTextStream err(stderr);
        err << "RBAC visibility harness failed:\n";
        for (const QString &error : errors) {
            err << " - " << error << '\n';
        }
        return 1;
    }
    QTextStream(stdout) << "RBAC visibility harness passed " << cases.size() + 1 << " scenarios\n";
    return 0;
}
