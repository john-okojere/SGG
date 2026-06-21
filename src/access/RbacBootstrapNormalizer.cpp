#include "RbacBootstrapNormalizer.h"

#include <QMetaType>
#include <QVariant>

namespace {

QString snakeCase(QString value)
{
    value = value.trimmed();
    QString out;
    out.reserve(value.size() + 8);
    QChar previous;
    for (const QChar ch : value) {
        if (ch == QLatin1Char('-') || ch == QLatin1Char(' ') || ch == QLatin1Char('.') || ch == QLatin1Char('/')) {
            if (!out.endsWith(QLatin1Char('_'))) {
                out.append(QLatin1Char('_'));
            }
        } else if (ch.isUpper()) {
            if (!out.isEmpty() && previous.isLetterOrNumber() && !previous.isUpper() && !out.endsWith(QLatin1Char('_'))) {
                out.append(QLatin1Char('_'));
            }
            out.append(ch.toLower());
        } else {
            out.append(ch.toLower());
        }
        previous = ch;
    }
    while (out.contains(QStringLiteral("__"))) {
        out.replace(QStringLiteral("__"), QStringLiteral("_"));
    }
    if (out.startsWith(QLatin1Char('_'))) {
        out.remove(0, 1);
    }
    if (out.endsWith(QLatin1Char('_'))) {
        out.chop(1);
    }
    return out;
}

QString firstStringValue(const QVariantMap &map, const QStringList &keys)
{
    for (const QString &key : keys) {
        const QString value = map.value(key).toString().trimmed();
        if (!value.isEmpty()) {
            return value;
        }
    }
    return {};
}

QVariantMap firstMapValue(const QVariantMap &map, const QStringList &keys)
{
    for (const QString &key : keys) {
        const QVariantMap value = map.value(key).toMap();
        if (!value.isEmpty()) {
            return value;
        }
    }
    return {};
}

QList<QVariantMap> accessContexts(const QVariantMap &bootstrap)
{
    QList<QVariantMap> contexts{bootstrap};
    const QStringList keys{
        QStringLiteral("user"),
        QStringLiteral("pilot"),
        QStringLiteral("pilot_profile"),
        QStringLiteral("profile"),
        QStringLiteral("operator_profile"),
        QStringLiteral("rbac"),
        QStringLiteral("access"),
        QStringLiteral("authorization")
    };
    for (const QString &key : keys) {
        const QVariantMap map = bootstrap.value(key).toMap();
        if (!map.isEmpty()) {
            contexts << map;
        }
    }
    return contexts;
}

void appendUnique(QStringList &values, const QString &value)
{
    if (!value.isEmpty() && !values.contains(value)) {
        values << value;
    }
}

void appendList(QStringList &values, const QStringList &items)
{
    for (const QString &item : items) {
        appendUnique(values, item);
    }
}

QVariant valueForKeys(const QVariantMap &map, const QStringList &keys)
{
    for (const QString &key : keys) {
        const QVariant value = map.value(key);
        if (value.isValid() && !value.isNull()) {
            return value;
        }
    }
    return {};
}

QVariantMap permissionEntryMap(const QVariantMap &entry)
{
    const QString key = firstStringValue(entry, {
        QStringLiteral("key"),
        QStringLiteral("code"),
        QStringLiteral("codename"),
        QStringLiteral("name"),
        QStringLiteral("permission"),
        QStringLiteral("scope"),
        QStringLiteral("slug")
    });
    if (key.isEmpty()) {
        return {};
    }
    const QVariant allowed = valueForKeys(entry, {
        QStringLiteral("allowed"),
        QStringLiteral("enabled"),
        QStringLiteral("granted"),
        QStringLiteral("value"),
        QStringLiteral("has_permission")
    });
    return QVariantMap{{key, allowed.isValid() ? allowed : true}};
}

QSet<QString> idsFromVariant(const QVariant &value, const QStringList &keys)
{
    QSet<QString> ids;
    if (value.metaType().id() == QMetaType::QVariantList || value.metaType().id() == QMetaType::QStringList) {
        const QVariantList list = value.toList();
        for (const QVariant &entry : list) {
            const QVariantMap map = entry.toMap();
            if (!map.isEmpty()) {
                for (const QString &key : keys) {
                    const QString id = map.value(key).toString().trimmed();
                    if (!id.isEmpty()) {
                        ids.insert(id);
                        break;
                    }
                }
            } else {
                const QString id = entry.toString().trimmed();
                if (!id.isEmpty()) {
                    ids.insert(id);
                }
            }
        }
    } else {
        const QString id = value.toString().trimmed();
        if (!id.isEmpty()) {
            ids.insert(id);
        }
    }
    return ids;
}

void mergeIds(QSet<QString> &ids, const QVariantMap &map, const QStringList &fieldKeys, const QStringList &idKeys)
{
    for (const QString &field : fieldKeys) {
        const QVariant value = map.value(field);
        if (value.isValid() && !value.isNull()) {
            ids.unite(idsFromVariant(value, idKeys));
        }
    }
}

enum class TrustValueState {
    Unknown,
    Trusted,
    Blocked
};

TrustValueState trustValue(const QVariantMap &map, const QStringList &boolKeys, const QStringList &statusKeys)
{
    for (const QString &key : boolKeys) {
        const QVariant value = map.value(key);
        if (value.isValid() && !value.isNull()) {
            if (value.metaType().id() == QMetaType::Bool) {
                return value.toBool() ? TrustValueState::Trusted : TrustValueState::Blocked;
            }
            const QString text = value.toString().trimmed().toLower();
            if (text == QStringLiteral("true") || text == QStringLiteral("1") || text == QStringLiteral("yes")
                || text == QStringLiteral("allowed") || text == QStringLiteral("approved")
                || text == QStringLiteral("trusted") || text == QStringLiteral("active")) {
                return TrustValueState::Trusted;
            }
            if (text == QStringLiteral("false") || text == QStringLiteral("0") || text == QStringLiteral("no")
                || text == QStringLiteral("blocked") || text == QStringLiteral("pending")
                || text == QStringLiteral("revoked") || text == QStringLiteral("disabled")
                || text == QStringLiteral("suspended")) {
                return TrustValueState::Blocked;
            }
        }
    }
    for (const QString &key : statusKeys) {
        const QString text = map.value(key).toString().trimmed().toLower();
        if (text == QStringLiteral("approved") || text == QStringLiteral("trusted") || text == QStringLiteral("active")
            || text == QStringLiteral("verified")) {
            return TrustValueState::Trusted;
        }
        if (text == QStringLiteral("pending") || text == QStringLiteral("revoked") || text == QStringLiteral("disabled")
            || text == QStringLiteral("blocked") || text == QStringLiteral("suspended")
            || text == QStringLiteral("untrusted")) {
            return TrustValueState::Blocked;
        }
    }
    return TrustValueState::Unknown;
}

void mergePermissionSource(QVariantMap &target, const QVariant &source)
{
    const QVariantMap map = RbacBootstrapNormalizer::permissionMapFromVariant(source);
    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        const bool enabled = it.value().toBool();
        if (enabled || !target.contains(it.key())) {
            target[it.key()] = enabled;
        }
    }
}

} // namespace

RbacBootstrapProfile RbacBootstrapNormalizer::fromBootstrap(const QVariantMap &bootstrap)
{
    RbacBootstrapProfile profile;
    profile.user = bootstrap.value(QStringLiteral("user")).toMap();
    const QList<QVariantMap> contexts = accessContexts(bootstrap);

    for (const QVariantMap &context : contexts) {
        appendList(profile.rawRoles, listFromVariant(context.value(QStringLiteral("roles"))));
        appendUnique(profile.rawRoles, context.value(QStringLiteral("role")).toString().trimmed());
        appendUnique(profile.rawRoles, context.value(QStringLiteral("role_name")).toString().trimmed());
        appendUnique(profile.rawRoles, context.value(QStringLiteral("primary_role")).toString().trimmed());
    }
    for (const QString &role : profile.rawRoles) {
        appendUnique(profile.normalizedRoles, normalizeRole(role));
    }

    for (const QVariantMap &context : contexts) {
        mergePermissionSource(profile.permissions, context.value(QStringLiteral("permissions")));
        mergePermissionSource(profile.permissions, context.value(QStringLiteral("permission_scopes")));
        mergePermissionSource(profile.permissions, context.value(QStringLiteral("scopes")));
        mergePermissionSource(profile.permissions, context.value(QStringLiteral("allowed_permissions")));
    }
    profile.permissionKeys = enabledPermissionKeys(profile.permissions);

    QStringList modules;
    for (const QVariantMap &context : contexts) {
        appendList(modules, listFromVariant(context.value(QStringLiteral("allowed_gcs_modules"))));
        appendList(modules, listFromVariant(context.value(QStringLiteral("allowed_modules"))));
        appendList(modules, listFromVariant(context.value(QStringLiteral("gcs_modules"))));
        appendList(modules, listFromVariant(context.value(QStringLiteral("visible_modules"))));
        appendList(modules, listFromVariant(context.value(QStringLiteral("modules"))));
    }
    appendList(modules, derivedModulesForPermissions(profile.permissions));
    for (QString &module : modules) {
        module = normalizeModule(module);
    }
    modules.removeAll(QString());
    modules.removeDuplicates();
    modules.sort();
    profile.allowedModules = modules;

    QVariantMap organization = bootstrap.value(QStringLiteral("organization")).toMap();
    if (organization.isEmpty()) {
        for (const QVariantMap &context : contexts) {
            organization = firstMapValue(context, {
                QStringLiteral("organization"),
                QStringLiteral("organization_profile"),
                QStringLiteral("org")
            });
            if (!organization.isEmpty()) {
                break;
            }
        }
    }
    profile.organizationId = organization.value(QStringLiteral("id"),
                                                bootstrap.value(QStringLiteral("organization_id"),
                                                                bootstrap.value(QStringLiteral("org_id")))).toInt();

    for (const QVariantMap &context : contexts) {
        mergeIds(profile.aircraftIds,
                 context,
                 {QStringLiteral("assigned_aircraft"),
                  QStringLiteral("aircraft"),
                  QStringLiteral("assigned_aircraft_ids"),
                  QStringLiteral("aircraft_ids")},
                 {QStringLiteral("id"), QStringLiteral("aircraft_id"), QStringLiteral("uuid")});
        mergeIds(profile.missionIds,
                 context,
                 {QStringLiteral("assigned_missions"),
                  QStringLiteral("assigned_mission_ids"),
                  QStringLiteral("mission_ids"),
                  QStringLiteral("missions"),
                  QStringLiteral("active_missions"),
                  QStringLiteral("approved_missions"),
                  QStringLiteral("mission_history"),
                  QStringLiteral("completed_missions")},
                 {QStringLiteral("id"), QStringLiteral("mission_id"), QStringLiteral("uuid")});
    }

    profile.sessionStatus = firstMapValue(bootstrap, {QStringLiteral("session_status"), QStringLiteral("session")});
    profile.deviceSummary = firstMapValue(bootstrap, {QStringLiteral("device_summary"), QStringLiteral("device")});

    const TrustValueState sessionTrust = trustValue(profile.sessionStatus,
                                                   {QStringLiteral("operations_allowed"),
                                                    QStringLiteral("trusted"),
                                                    QStringLiteral("device_trusted"),
                                                    QStringLiteral("session_trusted"),
                                                    QStringLiteral("is_trusted")},
                                                   {QStringLiteral("status"),
                                                    QStringLiteral("device_status"),
                                                    QStringLiteral("trust_state")});
    const TrustValueState deviceTrust = trustValue(profile.deviceSummary,
                                                  {QStringLiteral("trusted"),
                                                   QStringLiteral("is_trusted"),
                                                   QStringLiteral("approved"),
                                                   QStringLiteral("device_trusted")},
                                                  {QStringLiteral("status"),
                                                   QStringLiteral("device_status"),
                                                   QStringLiteral("trust_state")});
    if (sessionTrust != TrustValueState::Unknown) {
        profile.hasSessionTrusted = true;
        profile.sessionTrusted = sessionTrust == TrustValueState::Trusted;
    }
    if (deviceTrust != TrustValueState::Unknown) {
        profile.sessionTrusted = profile.hasSessionTrusted
            ? (profile.sessionTrusted && deviceTrust == TrustValueState::Trusted)
            : (deviceTrust == TrustValueState::Trusted);
        profile.hasSessionTrusted = true;
    }

    const TrustValueState reachable = trustValue(profile.sessionStatus,
                                                {QStringLiteral("control_center_reachable"),
                                                 QStringLiteral("backend_reachable"),
                                                 QStringLiteral("reachable"),
                                                 QStringLiteral("online")},
                                                {});
    if (reachable != TrustValueState::Unknown) {
        profile.hasBackendReachable = true;
        profile.backendReachable = reachable == TrustValueState::Trusted;
    }

    return profile;
}

QString RbacBootstrapNormalizer::normalizePermission(const QString &permission)
{
    QString key = snakeCase(permission);
    if (!key.isEmpty() && !key.startsWith(QStringLiteral("can_"))) {
        key.prepend(QStringLiteral("can_"));
    }
    return key;
}

QString RbacBootstrapNormalizer::normalizeModule(const QString &module)
{
    const QString normalized = snakeCase(module);
    if (normalized == QStringLiteral("missions") || normalized == QStringLiteral("mission")) {
        return QStringLiteral("mission_planning");
    }
    if (normalized == QStringLiteral("pilot") || normalized == QStringLiteral("manual_flight")
        || normalized == QStringLiteral("pilot_mode")) {
        return QStringLiteral("pilot_operations");
    }
    if (normalized == QStringLiteral("vehicle") || normalized == QStringLiteral("vehicle_config")) {
        return QStringLiteral("vehicle_configuration");
    }
    if (normalized == QStringLiteral("vehicle_profiles")) {
        return QStringLiteral("vehicle_profile");
    }
    if (normalized == QStringLiteral("tools") || normalized == QStringLiteral("gcs_tool_catalog")) {
        return QStringLiteral("gcs_tools");
    }
    if (normalized == QStringLiteral("configuration") || normalized == QStringLiteral("tuning")) {
        return QStringLiteral("configuration_tuning");
    }
    if (normalized == QStringLiteral("advanced_mavlink") || normalized == QStringLiteral("diagnostics")) {
        return QStringLiteral("advanced_tools");
    }
    if (normalized == QStringLiteral("logs") || normalized == QStringLiteral("analysis")) {
        return QStringLiteral("logs_analysis");
    }
    if (normalized == QStringLiteral("manufacturer") || normalized == QStringLiteral("manufacturer_operations")) {
        return QStringLiteral("manufacturer_dashboard");
    }
    if (normalized == QStringLiteral("admin") || normalized == QStringLiteral("security")) {
        return QStringLiteral("administration");
    }
    return normalized;
}

QString RbacBootstrapNormalizer::normalizeRole(const QString &role)
{
    QString normalized = snakeCase(role).toUpper();
    if (normalized == QStringLiteral("SUPER_ADMIN")) {
        return QStringLiteral("SKYGRID_ADMIN");
    }
    if (normalized == QStringLiteral("OPERATIONS_ADMIN") || normalized == QStringLiteral("ORG_ADMIN")
        || normalized == QStringLiteral("ORGANISATION_ADMIN")) {
        return QStringLiteral("ORGANIZATION_ADMIN");
    }
    if (normalized == QStringLiteral("MANUFACTURER")) {
        return QStringLiteral("MANUFACTURER_ADMIN");
    }
    if (normalized == QStringLiteral("PILOT_USER")) {
        return QStringLiteral("PILOT");
    }
    return normalized;
}

QString RbacBootstrapNormalizer::normalizeKey(const QString &value)
{
    return snakeCase(value);
}

QVariantMap RbacBootstrapNormalizer::permissionMapFromVariant(const QVariant &value)
{
    QVariantMap normalized;
    const QVariantMap source = value.toMap();
    if (!source.isEmpty()) {
        for (auto it = source.constBegin(); it != source.constEnd(); ++it) {
            const QVariantMap nested = it.value().toMap();
            if (!nested.isEmpty() && (nested.contains(QStringLiteral("allowed"))
                                      || nested.contains(QStringLiteral("enabled"))
                                      || nested.contains(QStringLiteral("granted"))
                                      || nested.contains(QStringLiteral("value")))) {
                normalized[normalizePermission(it.key())] = truthy(valueForKeys(nested, {
                    QStringLiteral("allowed"),
                    QStringLiteral("enabled"),
                    QStringLiteral("granted"),
                    QStringLiteral("value")
                }));
            } else {
                normalized[normalizePermission(it.key())] = truthy(it.value());
            }
        }
    }

    const QVariantList list = value.toList();
    for (const QVariant &entry : list) {
        const QVariantMap entryMap = entry.toMap();
        if (!entryMap.isEmpty()) {
            const QVariantMap mapped = permissionEntryMap(entryMap);
            for (auto it = mapped.constBegin(); it != mapped.constEnd(); ++it) {
                normalized[normalizePermission(it.key())] = truthy(it.value());
            }
        } else {
            const QString text = entry.toString().trimmed();
            if (!text.isEmpty()) {
                normalized[normalizePermission(text)] = true;
            }
        }
    }

    if (normalized.isEmpty()) {
        const QString text = value.toString().trimmed();
        if (!text.isEmpty()) {
            for (const QString &entry : text.split(QLatin1Char(','), Qt::SkipEmptyParts)) {
                normalized[normalizePermission(entry)] = true;
            }
        }
    }
    return normalized;
}

QStringList RbacBootstrapNormalizer::listFromVariant(const QVariant &value)
{
    QStringList out;
    const QVariantMap map = value.toMap();
    if (!map.isEmpty()) {
        for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
            if (truthy(it.value())) {
                out << it.key();
            }
        }
    } else if (value.metaType().id() == QMetaType::QStringList) {
        out = value.toStringList();
    } else if (value.metaType().id() == QMetaType::QVariantList) {
        const QVariantList list = value.toList();
        for (const QVariant &entry : list) {
            const QVariantMap map = entry.toMap();
            const QString text = map.isEmpty()
                ? entry.toString().trimmed()
                : firstStringValue(map, {QStringLiteral("key"), QStringLiteral("name"), QStringLiteral("code"), QStringLiteral("slug"), QStringLiteral("module")});
            if (!text.isEmpty()) {
                out << text;
            }
        }
    } else {
        const QString text = value.toString().trimmed();
        if (!text.isEmpty()) {
            out = text.split(QLatin1Char(','), Qt::SkipEmptyParts);
        }
    }
    for (QString &entry : out) {
        entry = entry.trimmed();
    }
    out.removeAll(QString());
    out.removeDuplicates();
    return out;
}

QStringList RbacBootstrapNormalizer::enabledPermissionKeys(const QVariantMap &permissions)
{
    QStringList keys;
    for (auto it = permissions.constBegin(); it != permissions.constEnd(); ++it) {
        if (it.value().toBool()) {
            keys << it.key();
        }
    }
    keys.removeDuplicates();
    keys.sort();
    return keys;
}

QStringList RbacBootstrapNormalizer::derivedModulesForPermissions(const QVariantMap &permissions)
{
    auto can = [&permissions](const QString &permission) {
        return permissions.value(RbacBootstrapNormalizer::normalizePermission(permission)).toBool();
    };
    QStringList modules;
    auto add = [&modules](const QString &module) {
        appendUnique(modules, module);
    };

    if (can(QStringLiteral("can_plan_mission")) || can(QStringLiteral("can_upload_mission")) || can(QStringLiteral("can_start_mission"))) {
        add(QStringLiteral("mission_planning"));
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("advanced_mission_editor"));
    }
    if (can(QStringLiteral("can_use_flight_data")) || can(QStringLiteral("can_stream_telemetry")) || can(QStringLiteral("can_view_telemetry"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("connect"));
        add(QStringLiteral("flight_data"));
    }
    if (can(QStringLiteral("can_fly_manual"))) {
        add(QStringLiteral("pilot_operations"));
        add(QStringLiteral("pilot_mode"));
        add(QStringLiteral("gcs_tools"));
    }
    if (can(QStringLiteral("can_view_fleet")) || can(QStringLiteral("can_assign_aircraft")) || can(QStringLiteral("can_assign_pilots"))) {
        add(QStringLiteral("fleet"));
        add(QStringLiteral("gcs_tools"));
    }
    if (can(QStringLiteral("can_manage_users")) || can(QStringLiteral("can_manage_roles"))) {
        add(QStringLiteral("administration"));
    }
    if (can(QStringLiteral("can_view_reports"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("command_center_sync"));
    }
    if (can(QStringLiteral("can_view_mission_logs")) || can(QStringLiteral("can_view_vehicle_audit"))
        || can(QStringLiteral("can_view_logs")) || can(QStringLiteral("can_download_logs"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("logs"));
        add(QStringLiteral("logs_analysis"));
        add(QStringLiteral("audit"));
    }
    if (can(QStringLiteral("can_run_initial_setup"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("initial_setup"));
    }
    if (can(QStringLiteral("can_tune_vehicle")) || can(QStringLiteral("can_read_vehicle_parameters"))
        || can(QStringLiteral("can_write_vehicle_parameters"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("configuration_tuning"));
        add(QStringLiteral("parameter_snapshot"));
    }
    if (can(QStringLiteral("can_flash_firmware"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("firmware_manager"));
    }
    if (can(QStringLiteral("can_configure_optional_hardware"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("optional_hardware"));
    }
    if (can(QStringLiteral("can_use_simulation"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("simulation"));
    }
    if (can(QStringLiteral("can_use_advanced_mavlink"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("advanced_tools"));
    }
    if (can(QStringLiteral("can_use_terminal"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("advanced_tools"));
        add(QStringLiteral("terminal"));
    }
    if (can(QStringLiteral("can_configure_payload"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("payload"));
        add(QStringLiteral("payload_configuration"));
    }
    if (can(QStringLiteral("can_view_video_stream")) || can(QStringLiteral("can_configure_video_payload"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("payload"));
        add(QStringLiteral("video_payload"));
    }
    if (can(QStringLiteral("can_manage_multi_vehicle"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("multi_vehicle"));
    }
    if (can(QStringLiteral("can_configure_vehicle"))) {
        add(QStringLiteral("gcs_tools"));
        add(QStringLiteral("initial_setup"));
        add(QStringLiteral("configuration_tuning"));
        add(QStringLiteral("firmware_manager"));
        add(QStringLiteral("optional_hardware"));
        add(QStringLiteral("payload"));
        add(QStringLiteral("payload_configuration"));
        add(QStringLiteral("video_payload"));
        add(QStringLiteral("manufacturer_dashboard"));
        add(QStringLiteral("vehicle_configuration"));
        add(QStringLiteral("vehicle_setup_wizard"));
        add(QStringLiteral("bench_test"));
    }
    if (can(QStringLiteral("can_bind_flight_controller"))) {
        add(QStringLiteral("vehicle_connection"));
    }
    if (can(QStringLiteral("can_configure_rc"))) {
        add(QStringLiteral("controller_mapping"));
    }
    if (can(QStringLiteral("can_register_vehicle")) || can(QStringLiteral("can_edit_vehicle_profile"))) {
        add(QStringLiteral("vehicle_profile"));
        add(QStringLiteral("vehicle_profiles"));
    }
    if (can(QStringLiteral("can_run_manufacturer_test_flight")) || can(QStringLiteral("can_fly_manual_test"))) {
        add(QStringLiteral("manufacturer_dashboard"));
        add(QStringLiteral("manufacturer_test_flight"));
        add(QStringLiteral("preflight"));
    }
    if (can(QStringLiteral("can_release_vehicle_to_organization"))) {
        add(QStringLiteral("release_vehicle"));
    }
    if (can(QStringLiteral("can_manage_manufacturers")) || can(QStringLiteral("can_manage_organizations"))) {
        add(QStringLiteral("administration"));
    }

    for (QString &module : modules) {
        module = normalizeModule(module);
    }
    modules.removeAll(QString());
    modules.removeDuplicates();
    modules.sort();
    return modules;
}

bool RbacBootstrapNormalizer::truthy(const QVariant &value)
{
    if (!value.isValid() || value.isNull()) {
        return false;
    }
    if (value.metaType().id() == QMetaType::Bool) {
        return value.toBool();
    }
    const QString text = value.toString().trimmed().toLower();
    return text == QStringLiteral("1")
        || text == QStringLiteral("true")
        || text == QStringLiteral("yes")
        || text == QStringLiteral("allowed")
        || text == QStringLiteral("granted")
        || text == QStringLiteral("approved");
}
