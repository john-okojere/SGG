#include "GcsEventSyncManager.h"

#include "../auth/SessionManager.h"
#include "../cache/LocalSyncCache.h"
#include "../network/ApiClient.h"

#include <QDateTime>
#include <QJsonArray>
#include <QProcessEnvironment>
#include <cmath>
#include <utility>

GcsEventSyncManager::GcsEventSyncManager(ApiClient *api,
                                         SessionManager *session,
                                         LocalSyncCache *cache,
                                         QObject *parent)
    : QObject(parent),
      m_api(api),
      m_session(session),
      m_cache(cache)
{
    const int interval = QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("SKYGRID_EVENT_BATCH_INTERVAL_MS"),
               QProcessEnvironment::systemEnvironment().value(QStringLiteral("DEV_BUILD")) == QStringLiteral("true")
                   ? QStringLiteral("2500")
                   : QStringLiteral("3000"))
        .toInt();
    m_flushTimer.setInterval(qBound(1000, interval, 10000));
    connect(&m_flushTimer, &QTimer::timeout, this, &GcsEventSyncManager::flushBatch);
    m_flushTimer.start();
}

QString GcsEventSyncManager::status() const { return m_status; }

void GcsEventSyncManager::recordEvent(const QString &eventType,
                                      const QString &severity,
                                      const QString &message,
                                      const QJsonObject &payload)
{
    const QJsonObject normalizedPayload = normalizePayload(payload);
    const QJsonObject event{
        {QStringLiteral("event_type"), eventType},
        {QStringLiteral("severity"), severity.isEmpty() ? QStringLiteral("info") : severity},
        {QStringLiteral("message"), message},
        {QStringLiteral("payload"), normalizedPayload},
        {QStringLiteral("recorded_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}
    };

    enqueueOrSend(eventType, event, isCriticalEvent(eventType, severity, normalizedPayload));
}

bool GcsEventSyncManager::isCriticalEvent(const QString &eventType, const QString &severity, const QJsonObject &payload) const
{
    const QString event = eventType.toLower();
    const QString level = severity.toLower();
    const QString weatherResult = payload.value(QStringLiteral("weather_result")).toString().toLower();
    static const QStringList criticalEvents{
        QStringLiteral("mission_abort"),
        QStringLiteral("mission_aborted"),
        QStringLiteral("vehicle_emergency_stop"),
        QStringLiteral("emergency_stop_requested"),
        QStringLiteral("emergency_stop"),
        QStringLiteral("device_revoked"),
        QStringLiteral("security_failure"),
        QStringLiteral("weather_do_not_fly"),
        QStringLiteral("mission_failed"),
        QStringLiteral("upload_failed"),
        QStringLiteral("mission_upload_failed")
    };
    if (level == QStringLiteral("critical") || level == QStringLiteral("error")) {
        return true;
    }
    if (weatherResult == QStringLiteral("do_not_fly") || weatherResult == QStringLiteral("do not fly")) {
        return true;
    }
    for (const QString &needle : criticalEvents) {
        if (event.contains(needle)) {
            return true;
        }
    }
    return false;
}

void GcsEventSyncManager::enqueueOrSend(const QString &eventType, const QJsonObject &event, bool immediate)
{
    if (!immediate) {
        m_pendingEvents.push_back({eventType, event});
        setStatus(QStringLiteral("GCS event queued for batch: %1").arg(eventType));
        return;
    }
    postEvent(eventType, event);
}

void GcsEventSyncManager::flushBatch()
{
    if (m_pendingEvents.isEmpty()) {
        return;
    }
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        if (m_cache) {
            for (const QueuedEvent &entry : std::as_const(m_pendingEvents)) {
                m_cache->enqueueSync(QStringLiteral("gcs_event"), entry.payload.toVariantMap());
            }
        }
        setStatus(QStringLiteral("GCS events cached locally: %1").arg(m_pendingEvents.size()));
        m_pendingEvents.clear();
        return;
    }

    const QVector<QueuedEvent> batch = m_pendingEvents;
    m_pendingEvents.clear();
    for (const QueuedEvent &entry : batch) {
        postEvent(entry.eventType, entry.payload);
    }
}

void GcsEventSyncManager::postEvent(const QString &eventType, const QJsonObject &event)
{
    if (!m_api || !m_session || !m_session->operationsAllowed()) {
        if (m_cache) {
            m_cache->enqueueSync(QStringLiteral("gcs_event"), event.toVariantMap());
        }
        setStatus(QStringLiteral("GCS event queued locally: %1").arg(eventType));
        return;
    }
    m_api->post(QStringLiteral("/api/gcs-events/"), event, true, true,
                [this, event, eventType](int statusCode, const QJsonObject &, const QString &error) {
        if (statusCode >= 200 && statusCode < 300) {
            setStatus(QStringLiteral("GCS event synced: %1").arg(eventType));
        } else {
            if (m_cache) {
                m_cache->enqueueSync(QStringLiteral("gcs_event"), event.toVariantMap());
            }
            setStatus(error.isEmpty()
                          ? QStringLiteral("GCS event queued: %1").arg(eventType)
                          : error);
        }
    });
}

QJsonObject GcsEventSyncManager::normalizePayload(const QJsonObject &payload) const
{
    QJsonObject normalized;
    for (auto it = payload.constBegin(); it != payload.constEnd(); ++it) {
        normalized.insert(it.key(), normalizeValue(it.key(), it.value()));
    }
    return normalized;
}

QJsonValue GcsEventSyncManager::normalizeValue(const QString &key, const QJsonValue &value) const
{
    const QString normalizedKey = key.toLower();
    static const QStringList nullableIdKeys{
        QStringLiteral("flight_session_id"),
        QStringLiteral("mission"),
        QStringLiteral("mission_id"),
        QStringLiteral("id")
    };
    if (nullableIdKeys.contains(normalizedKey) && value.isString() && value.toString().trimmed().isEmpty()) {
        return QJsonValue(QJsonValue::Null);
    }
    if (value.isObject()) {
        return normalizePayload(value.toObject());
    }
    if (value.isArray()) {
        QJsonArray normalizedArray;
        const QJsonArray array = value.toArray();
        for (const QJsonValue &entry : array) {
            if (entry.isObject()) {
                normalizedArray.append(normalizePayload(entry.toObject()));
            } else {
                normalizedArray.append(entry);
            }
        }
        return normalizedArray;
    }
    if (!value.isDouble()) {
        return value;
    }
    if (normalizedKey == QStringLiteral("latitude") || normalizedKey == QStringLiteral("longitude")) {
        return roundNumber(value.toDouble(), 7);
    }
    static const QStringList twoDecimalKeys{
        QStringLiteral("altitude_m"),
        QStringLiteral("relative_altitude_m"),
        QStringLiteral("ground_speed_mps"),
        QStringLiteral("vertical_speed_mps"),
        QStringLiteral("speed_mps"),
        QStringLiteral("heading"),
        QStringLiteral("heading_deg"),
        QStringLiteral("battery_percent"),
        QStringLiteral("battery_voltage"),
        QStringLiteral("rc_signal"),
        QStringLiteral("telemetry_signal"),
        QStringLiteral("hdop"),
        QStringLiteral("distance_from_home_m"),
        QStringLiteral("distance_travelled_m"),
        QStringLiteral("direction_deg"),
        QStringLiteral("gust_mps")
    };
    return twoDecimalKeys.contains(normalizedKey)
        ? QJsonValue(roundNumber(value.toDouble(), 2))
        : value;
}

double GcsEventSyncManager::roundNumber(double value, int decimals)
{
    const double factor = std::pow(10.0, decimals);
    return std::round(value * factor) / factor;
}

void GcsEventSyncManager::setStatus(const QString &status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit eventSyncChanged();
}
