#include "ParameterManager.h"

#include "ParameterMetadataManager.h"
#include "../auth/SessionManager.h"
#include "../flight/ManualControlManager.h"
#include "../security/AccessManager.h"
#include "../sync/GcsEventSyncManager.h"
#include "../vehicle/MavsdkVehicleManager.h"
#include "../vehicle/MissionExecutionManager.h"

#include <mavsdk/plugins/param/param.h>

#include <QFile>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QPointer>
#include <QUrl>
#include <QtGlobal>

#include <thread>

namespace {
QString paramResultString(mavsdk::Param::Result result)
{
    switch (result) {
    case mavsdk::Param::Result::Success: return QStringLiteral("Success");
    case mavsdk::Param::Result::Timeout: return QStringLiteral("Timeout");
    case mavsdk::Param::Result::ConnectionError: return QStringLiteral("Connection error");
    case mavsdk::Param::Result::WrongType: return QStringLiteral("Wrong type");
    case mavsdk::Param::Result::ParamNameTooLong: return QStringLiteral("Parameter name too long");
    case mavsdk::Param::Result::NoSystem: return QStringLiteral("No system");
    case mavsdk::Param::Result::ParamValueTooLong: return QStringLiteral("Parameter value too long");
    case mavsdk::Param::Result::DoesNotExist: return QStringLiteral("Parameter does not exist");
    case mavsdk::Param::Result::ValueOutOfRange: return QStringLiteral("Value out of range");
    case mavsdk::Param::Result::PermissionDenied: return QStringLiteral("Permission denied");
    case mavsdk::Param::Result::ComponentNotFound: return QStringLiteral("Component not found");
    case mavsdk::Param::Result::ReadOnly: return QStringLiteral("Read only");
    case mavsdk::Param::Result::TypeUnsupported: return QStringLiteral("Type unsupported");
    case mavsdk::Param::Result::TypeMismatch: return QStringLiteral("Type mismatch");
    case mavsdk::Param::Result::ReadFail: return QStringLiteral("Read failed");
    default: return QStringLiteral("Parameter operation failed");
    }
}
}

ParameterManager::ParameterManager(MavsdkVehicleManager *vehicle,
                                   ParameterMetadataManager *metadata,
                                   AccessManager *access,
                                   SessionManager *session,
                                   MissionExecutionManager *missionExecution,
                                   ManualControlManager *manualControl,
                                   GcsEventSyncManager *events,
                                   QObject *parent)
    : QObject(parent),
      m_vehicle(vehicle),
      m_metadata(metadata),
      m_access(access),
      m_session(session),
      m_missionExecution(missionExecution),
      m_manualControl(manualControl),
      m_events(events)
{
}

QVariantList ParameterManager::parameters() const { return m_parameters; }
QString ParameterManager::searchText() const { return m_searchText; }
bool ParameterManager::modifiedOnly() const { return m_modifiedOnly; }
bool ParameterManager::busy() const { return m_busy; }
QString ParameterManager::status() const { return m_status; }
int ParameterManager::changedCount() const { return m_changedCount; }
QString ParameterManager::compareStatus() const { return m_compareStatus; }
QVariantList ParameterManager::setupGroups() const
{
    QVariantList groups{QStringLiteral("All")};
    if (m_metadata) {
        groups += m_metadata->groups();
    }
    return groups;
}

QString ParameterManager::selectedGroup() const { return m_selectedGroup; }

bool ParameterManager::rawEditorAllowed() const
{
    return m_access && m_access->canPerform(QStringLiteral("parameter_safety_override"));
}

QVariantList ParameterManager::filteredParameters() const
{
    const QString query = m_searchText.trimmed().toLower();
    QVariantList rows;
    for (const QVariant &entry : m_parameters) {
        const QVariantMap row = entry.toMap();
        if (m_modifiedOnly && !row.value(QStringLiteral("modified")).toBool()) {
            continue;
        }
        if (m_selectedGroup != QStringLiteral("All")
            && row.value(QStringLiteral("group"), row.value(QStringLiteral("category"))).toString() != m_selectedGroup) {
            continue;
        }
        if (!query.isEmpty()) {
            const QString haystack = QStringLiteral("%1 %2 %3 %4")
                .arg(row.value(QStringLiteral("name")).toString(),
                     row.value(QStringLiteral("category")).toString(),
                     row.value(QStringLiteral("group")).toString(),
                     row.value(QStringLiteral("draftValue")).toString())
                .toLower();
            if (!haystack.contains(query)) {
                continue;
            }
        }
        rows << row;
        if (rows.size() >= 350) {
            break;
        }
    }
    return rows;
}

void ParameterManager::setSelectedGroup(const QString &group)
{
    const QString next = group.trimmed().isEmpty() ? QStringLiteral("All") : group.trimmed();
    if (m_selectedGroup == next) {
        return;
    }
    m_selectedGroup = next;
    emit parametersChanged();
}

void ParameterManager::setSearchText(const QString &text)
{
    const QString next = text.trimmed();
    if (m_searchText == next) {
        return;
    }
    m_searchText = next;
    emit parametersChanged();
}

void ParameterManager::setModifiedOnly(bool modifiedOnly)
{
    if (m_modifiedOnly == modifiedOnly) {
        return;
    }
    m_modifiedOnly = modifiedOnly;
    emit parametersChanged();
}

void ParameterManager::readAllParameters()
{
    if (m_busy) {
        return;
    }
    if (!m_access || !m_access->authorizeAction(QStringLiteral("vehicle_parameter_read"),
                                                 {},
                                                 QStringLiteral("Parameter read blocked by local permissions."))) {
        setStatus(QStringLiteral("Parameter read blocked by RBAC."));
        return;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        setStatus(QStringLiteral("Connect a flight controller before reading parameters."));
        return;
    }

    const auto system = m_vehicle->system();
    setBusy(true);
    setStatus(QStringLiteral("Reading all parameters from vehicle."));

    QPointer<ParameterManager> self(this);
    std::thread([self, system]() {
        mavsdk::Param param(system);
        const mavsdk::Param::AllParams allParams = param.get_all_params();
        QVariantList rows;
        for (const mavsdk::Param::IntParam &item : allParams.int_params) {
            const QString name = QString::fromStdString(item.name);
            rows << QVariantMap{{QStringLiteral("name"), name},
                                {QStringLiteral("value"), QString::number(item.value)},
                                {QStringLiteral("draftValue"), QString::number(item.value)},
                                {QStringLiteral("type"), QStringLiteral("int")},
                                {QStringLiteral("category"), name.section(QLatin1Char('_'), 0, 0)},
                                {QStringLiteral("modified"), false}};
        }
        for (const mavsdk::Param::FloatParam &item : allParams.float_params) {
            const QString name = QString::fromStdString(item.name);
            rows << QVariantMap{{QStringLiteral("name"), name},
                                {QStringLiteral("value"), QString::number(item.value, 'g', 7)},
                                {QStringLiteral("draftValue"), QString::number(item.value, 'g', 7)},
                                {QStringLiteral("type"), QStringLiteral("float")},
                                {QStringLiteral("category"), name.section(QLatin1Char('_'), 0, 0)},
                                {QStringLiteral("modified"), false}};
        }
        for (const mavsdk::Param::CustomParam &item : allParams.custom_params) {
            const QString name = QString::fromStdString(item.name);
            const QString value = QString::fromStdString(item.value);
            rows << QVariantMap{{QStringLiteral("name"), name},
                                {QStringLiteral("value"), value},
                                {QStringLiteral("draftValue"), value},
                                {QStringLiteral("type"), QStringLiteral("custom")},
                                {QStringLiteral("category"), name.section(QLatin1Char('_'), 0, 0)},
                                {QStringLiteral("modified"), false}};
        }
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, rows]() {
            if (!self) {
                return;
            }
            self->applyParameterRows(rows, QStringLiteral("Read %1 parameters from vehicle.").arg(rows.size()));
            self->setBusy(false);
        }, Qt::QueuedConnection);
    }).detach();
}

void ParameterManager::refreshParameters()
{
    readAllParameters();
}

void ParameterManager::setDraftValue(const QString &name, const QString &value)
{
    const QString key = name.trimmed();
    for (int i = 0; i < m_parameters.size(); ++i) {
        QVariantMap row = m_parameters.at(i).toMap();
        if (row.value(QStringLiteral("name")).toString() != key) {
            continue;
        }
        if (row.value(QStringLiteral("readOnly")).toBool() && !rawEditorAllowed()) {
            setStatus(QStringLiteral("%1 is read-only until metadata or override permission is available.").arg(key));
            return;
        }
        QString next = value.trimmed();
        const QString controlType = row.value(QStringLiteral("controlType")).toString();
        if (controlType == QStringLiteral("number")) {
            bool ok = false;
            double numeric = next.toDouble(&ok);
            if (!ok) {
                setStatus(QStringLiteral("%1 expects a numeric value.").arg(key));
                return;
            }
            const QVariant minimum = row.value(QStringLiteral("minimum"));
            const QVariant maximum = row.value(QStringLiteral("maximum"));
            if (minimum.isValid()) {
                numeric = qMax(minimum.toDouble(), numeric);
            }
            if (maximum.isValid()) {
                numeric = qMin(maximum.toDouble(), numeric);
            }
            next = QString::number(numeric, 'g', 10);
        }
        row[QStringLiteral("draftValue")] = next;
        row[QStringLiteral("modified")] = next != row.value(QStringLiteral("value")).toString();
        m_parameters[i] = row;
        recalculateChangedCount();
        emit parametersChanged();
        return;
    }
}

void ParameterManager::resetDraft(const QString &name)
{
    const QString key = name.trimmed();
    for (int i = 0; i < m_parameters.size(); ++i) {
        QVariantMap row = m_parameters.at(i).toMap();
        if (row.value(QStringLiteral("name")).toString() != key) {
            continue;
        }
        row[QStringLiteral("draftValue")] = row.value(QStringLiteral("value")).toString();
        row[QStringLiteral("modified")] = false;
        m_parameters[i] = row;
        recalculateChangedCount();
        emit parametersChanged();
        return;
    }
}

void ParameterManager::writeChangedParameters()
{
    QString reason;
    if (!canWriteParameters(&reason)) {
        setStatus(reason);
        return;
    }
    QVariantList writes;
    for (const QVariant &entry : m_parameters) {
        const QVariantMap row = entry.toMap();
        if (row.value(QStringLiteral("modified")).toBool()) {
            writes << row;
        }
    }
    if (writes.isEmpty()) {
        setStatus(QStringLiteral("No changed parameters to write."));
        return;
    }

    writeRows(writes, QStringLiteral("changed"));
}

void ParameterManager::writeParameterGroup(const QString &group)
{
    QString reason;
    if (!canWriteParameters(&reason)) {
        setStatus(reason);
        return;
    }
    const QString targetGroup = group.trimmed().isEmpty() ? m_selectedGroup : group.trimmed();
    if (targetGroup == QStringLiteral("All")) {
        setStatus(QStringLiteral("Select a setup group before writing a whole group."));
        return;
    }
    QVariantList writes;
    for (const QVariant &entry : m_parameters) {
        const QVariantMap row = entry.toMap();
        if (row.value(QStringLiteral("group"), row.value(QStringLiteral("category"))).toString() == targetGroup
            && !row.value(QStringLiteral("readOnly")).toBool()) {
            writes << row;
        }
    }
    if (writes.isEmpty()) {
        setStatus(QStringLiteral("No writable parameters in %1.").arg(targetGroup));
        return;
    }
    writeRows(writes, targetGroup);
}

void ParameterManager::writeRows(const QVariantList &writes, const QString &scopeLabel)
{
    const auto system = m_vehicle->system();
    setBusy(true);
    setStatus(QStringLiteral("Writing %1 %2 parameters.").arg(writes.size()).arg(scopeLabel));
    QPointer<ParameterManager> self(this);
    std::thread([self, system, writes, scopeLabel]() {
        mavsdk::Param param(system);
        QString failure;
        for (const QVariant &entry : writes) {
            const QVariantMap row = entry.toMap();
            const QString type = row.value(QStringLiteral("type")).toString();
            const QString name = row.value(QStringLiteral("name")).toString();
            const QString value = row.value(QStringLiteral("draftValue")).toString();
            mavsdk::Param::Result result = mavsdk::Param::Result::Unknown;
            if (type == QStringLiteral("int")) {
                result = param.set_param_int(name.toStdString(), value.toInt());
            } else if (type == QStringLiteral("float")) {
                result = param.set_param_float(name.toStdString(), float(value.toDouble()));
            } else {
                result = param.set_param_custom(name.toStdString(), value.toStdString());
            }
            if (result != mavsdk::Param::Result::Success) {
                failure = QStringLiteral("%1: %2").arg(name, paramResultString(result));
                break;
            }
        }
        if (!self) {
            return;
        }
        QMetaObject::invokeMethod(self, [self, writes, failure, scopeLabel]() {
            if (!self) {
                return;
            }
            if (!failure.isEmpty()) {
                self->setStatus(QStringLiteral("Parameter write failed: %1").arg(failure));
                self->setBusy(false);
                return;
            }
            for (int i = 0; i < self->m_parameters.size(); ++i) {
                QVariantMap row = self->m_parameters.at(i).toMap();
                if (!row.value(QStringLiteral("modified")).toBool()) {
                    continue;
                }
                row[QStringLiteral("value")] = row.value(QStringLiteral("draftValue"));
                row[QStringLiteral("modified")] = false;
                self->m_parameters[i] = row;
            }
            self->recalculateChangedCount();
            self->setStatus(QStringLiteral("Wrote %1 %2 parameters to vehicle.").arg(writes.size()).arg(scopeLabel));
            if (self->m_events) {
                self->m_events->recordEvent(QStringLiteral("parameter_write_batch"),
                                            QStringLiteral("warning"),
                                            QStringLiteral("Vehicle parameters written"),
                                            QJsonObject{{QStringLiteral("count"), writes.size()},
                                                        {QStringLiteral("scope"), scopeLabel}});
            }
            self->setBusy(false);
            emit self->parametersChanged();
        }, Qt::QueuedConnection);
    }).detach();
}

void ParameterManager::saveToFile(const QString &pathOrUrl)
{
    const QString path = normalizePath(pathOrUrl);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        setStatus(QStringLiteral("Could not save parameter file."));
        return;
    }
    QJsonArray array;
    for (const QVariant &entry : m_parameters) {
        array.append(QJsonObject::fromVariantMap(entry.toMap()));
    }
    file.write(QJsonDocument(array).toJson(QJsonDocument::Indented));
    setStatus(QStringLiteral("Saved %1 parameters to file.").arg(m_parameters.size()));
}

void ParameterManager::loadFromFile(const QString &pathOrUrl)
{
    QFile file(normalizePath(pathOrUrl));
    if (!file.open(QIODevice::ReadOnly)) {
        setStatus(QStringLiteral("Could not load parameter file."));
        return;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QVariantList rows;
    for (const QJsonValue &value : doc.array()) {
        QVariantMap row = value.toObject().toVariantMap();
        if (!row.contains(QStringLiteral("draftValue"))) {
            row[QStringLiteral("draftValue")] = row.value(QStringLiteral("value")).toString();
        }
        row[QStringLiteral("modified")] = false;
        if (!row.contains(QStringLiteral("category"))) {
            row[QStringLiteral("category")] = categoryForName(row.value(QStringLiteral("name")).toString());
        }
        rows << row;
    }
    applyParameterRows(rows, QStringLiteral("Loaded %1 parameters from file.").arg(rows.size()));
}

void ParameterManager::compareWithFile(const QString &pathOrUrl)
{
    QFile file(normalizePath(pathOrUrl));
    if (!file.open(QIODevice::ReadOnly)) {
        setStatus(QStringLiteral("Could not compare parameter file."));
        return;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QHash<QString, QString> fileValues;
    for (const QJsonValue &value : doc.array()) {
        const QVariantMap row = value.toObject().toVariantMap();
        fileValues.insert(row.value(QStringLiteral("name")).toString(),
                          row.value(QStringLiteral("value")).toString());
    }
    int differences = 0;
    for (int i = 0; i < m_parameters.size(); ++i) {
        QVariantMap row = m_parameters.at(i).toMap();
        const QString name = row.value(QStringLiteral("name")).toString();
        const QString fileValue = fileValues.value(name);
        const bool differs = fileValues.contains(name) && fileValue != row.value(QStringLiteral("value")).toString();
        row[QStringLiteral("fileValue")] = fileValues.contains(name) ? fileValue : QStringLiteral("--");
        row[QStringLiteral("differsFromFile")] = differs;
        if (differs) {
            ++differences;
        }
        m_parameters[i] = row;
    }
    m_compareStatus = QStringLiteral("%1 differences against file.").arg(differences);
    setStatus(m_compareStatus);
    emit parametersChanged();
}

QString ParameterManager::normalizePath(const QString &pathOrUrl) const
{
    const QUrl url(pathOrUrl);
    if (url.isValid() && url.isLocalFile()) {
        return url.toLocalFile();
    }
    QString path = pathOrUrl;
    if (path.startsWith(QStringLiteral("file:///"))) {
        path = QUrl(path).toLocalFile();
    }
    return path.trimmed();
}

QString ParameterManager::categoryForName(const QString &name) const
{
    const QString category = name.section(QLatin1Char('_'), 0, 0).trimmed();
    return category.isEmpty() ? QStringLiteral("General") : category;
}

bool ParameterManager::canWriteParameters(QString *reason) const
{
    if (!m_access || !m_access->authorizeAction(QStringLiteral("vehicle_parameter_write"),
                                                 {},
                                                 QStringLiteral("Parameter write blocked by local permissions."))) {
        if (reason) {
            *reason = QStringLiteral("Parameter write blocked by RBAC.");
        }
        return false;
    }
    if (!m_session || !m_session->operationsAllowed()) {
        if (reason) {
            *reason = QStringLiteral("Parameter write blocked: trusted session required.");
        }
        return false;
    }
    if (!m_vehicle || !m_vehicle->connected() || !m_vehicle->system()) {
        if (reason) {
            *reason = QStringLiteral("Connect a flight controller before writing parameters.");
        }
        return false;
    }
    const bool criticalState = (m_missionExecution && m_missionExecution->executing())
        || (m_manualControl && m_manualControl->active())
        || (m_vehicle && (m_vehicle->armed() || m_vehicle->inAir()));
    if (criticalState && (!m_access || !m_access->canPerform(QStringLiteral("parameter_safety_override")))) {
        if (reason) {
            *reason = QStringLiteral("Parameter write blocked while armed, in flight, mission execution, or manual control is active.");
        }
        return false;
    }
    return true;
}

QVariantMap ParameterManager::decorateRow(const QVariantMap &row) const
{
    if (!m_metadata) {
        QVariantMap out = row;
        if (!out.contains(QStringLiteral("group"))) {
            out[QStringLiteral("group")] = out.value(QStringLiteral("category"), categoryForName(out.value(QStringLiteral("name")).toString()));
        }
        if (!out.contains(QStringLiteral("controlType"))) {
            out[QStringLiteral("controlType")] = QStringLiteral("raw");
        }
        return out;
    }
    return m_metadata->decorateParameter(row, rawEditorAllowed());
}

void ParameterManager::applyParameterRows(const QVariantList &rows, const QString &status)
{
    QVariantList decorated;
    for (const QVariant &entry : rows) {
        decorated << decorateRow(entry.toMap());
    }
    m_parameters = decorated;
    recalculateChangedCount();
    m_compareStatus = QStringLiteral("No comparison loaded.");
    setStatus(status);
    emit parametersChanged();
}

void ParameterManager::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit parametersChanged();
}

void ParameterManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit parametersChanged();
}

void ParameterManager::recalculateChangedCount()
{
    int count = 0;
    for (const QVariant &entry : m_parameters) {
        if (entry.toMap().value(QStringLiteral("modified")).toBool()) {
            ++count;
        }
    }
    m_changedCount = count;
}
