#include "LocalSyncCache.h"

#include <QDateTime>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QUuid>

LocalSyncCache::LocalSyncCache(QObject *parent)
    : QObject(parent),
      m_connectionName(QStringLiteral("skygrid_cache_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces)))
{
    initialize();
}

LocalSyncCache::~LocalSyncCache()
{
    if (!m_connectionName.isEmpty()) {
        QSqlDatabase::database(m_connectionName).close();
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool LocalSyncCache::available() const { return m_available; }
QString LocalSyncCache::status() const { return m_status; }

QVariantMap LocalSyncCache::loadObject(const QString &namespaceName, const QString &key) const
{
    if (!m_available) {
        return {};
    }
    const QVariant value = variantFromJson([&]() {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare(QStringLiteral("SELECT json FROM cache_items WHERE namespace = ? AND key = ?"));
        query.addBindValue(namespaceName);
        query.addBindValue(key);
        return query.exec() && query.next() ? query.value(0).toString() : QString();
    }());
    return value.toMap();
}

QVariantList LocalSyncCache::loadList(const QString &namespaceName, const QString &key) const
{
    if (!m_available) {
        return {};
    }
    const QVariant value = variantFromJson([&]() {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare(QStringLiteral("SELECT json FROM cache_items WHERE namespace = ? AND key = ?"));
        query.addBindValue(namespaceName);
        query.addBindValue(key);
        return query.exec() && query.next() ? query.value(0).toString() : QString();
    }());
    return value.toList();
}

void LocalSyncCache::saveObject(const QString &namespaceName, const QString &key, const QVariantMap &value)
{
    if (!m_available) {
        return;
    }
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare(QStringLiteral("INSERT OR REPLACE INTO cache_items(namespace, key, json, updated_at) VALUES(?, ?, ?, ?)"));
    query.addBindValue(namespaceName);
    query.addBindValue(key);
    query.addBindValue(jsonForVariant(value));
    query.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    query.exec();
    emit cacheChanged();
}

void LocalSyncCache::saveList(const QString &namespaceName, const QString &key, const QVariantList &value)
{
    if (!m_available) {
        return;
    }
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare(QStringLiteral("INSERT OR REPLACE INTO cache_items(namespace, key, json, updated_at) VALUES(?, ?, ?, ?)"));
    query.addBindValue(namespaceName);
    query.addBindValue(key);
    query.addBindValue(jsonForVariant(value));
    query.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    query.exec();
    emit cacheChanged();
}

void LocalSyncCache::enqueueSync(const QString &kind, const QVariantMap &payload)
{
    if (!m_available) {
        return;
    }
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare(QStringLiteral("INSERT INTO sync_queue(kind, payload, status, attempts, created_at) VALUES(?, ?, 'queued', 0, ?)"));
    query.addBindValue(kind);
    query.addBindValue(jsonForVariant(payload));
    query.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    query.exec();
    emit cacheChanged();
}

QVariantList LocalSyncCache::queuedSyncItems(int limit) const
{
    QVariantList items;
    if (!m_available) {
        return items;
    }
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare(QStringLiteral("SELECT id, kind, payload, attempts FROM sync_queue WHERE status = 'queued' ORDER BY id ASC LIMIT ?"));
    query.addBindValue(qMax(1, limit));
    if (!query.exec()) {
        return items;
    }
    while (query.next()) {
        QVariantMap item;
        item[QStringLiteral("id")] = query.value(0).toInt();
        item[QStringLiteral("kind")] = query.value(1).toString();
        item[QStringLiteral("payload")] = variantFromJson(query.value(2).toString()).toMap();
        item[QStringLiteral("attempts")] = query.value(3).toInt();
        items << item;
    }
    return items;
}

void LocalSyncCache::markSyncDone(int id)
{
    if (!m_available) {
        return;
    }
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare(QStringLiteral("UPDATE sync_queue SET status = 'done' WHERE id = ?"));
    query.addBindValue(id);
    query.exec();
    emit cacheChanged();
}

void LocalSyncCache::markSyncFailed(int id, const QString &error)
{
    if (!m_available) {
        return;
    }
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare(QStringLiteral("UPDATE sync_queue SET attempts = attempts + 1, last_error = ? WHERE id = ?"));
    query.addBindValue(error);
    query.addBindValue(id);
    query.exec();
    emit cacheChanged();
}

void LocalSyncCache::cacheTelemetry(const QVariantMap &payload)
{
    if (!m_available) {
        return;
    }
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare(QStringLiteral("INSERT INTO telemetry_cache(payload, synced, recorded_at) VALUES(?, 0, ?)"));
    query.addBindValue(jsonForVariant(payload));
    const QString recordedAt = payload.value(QStringLiteral("recorded_at")).toString();
    query.addBindValue(recordedAt.isEmpty() ? QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs) : recordedAt);
    query.exec();
}

void LocalSyncCache::initialize()
{
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(base + QStringLiteral("/digiskygrid_cache.sqlite"));
    if (!db.open()) {
        m_status = QStringLiteral("SQLite cache unavailable: %1").arg(db.lastError().text());
        m_available = false;
        emit cacheChanged();
        return;
    }

    execute(QStringLiteral("CREATE TABLE IF NOT EXISTS cache_items(namespace TEXT NOT NULL, key TEXT NOT NULL, json TEXT NOT NULL, updated_at TEXT NOT NULL, PRIMARY KEY(namespace, key))"));
    execute(QStringLiteral("CREATE TABLE IF NOT EXISTS sync_queue(id INTEGER PRIMARY KEY AUTOINCREMENT, kind TEXT NOT NULL, payload TEXT NOT NULL, status TEXT NOT NULL, attempts INTEGER NOT NULL DEFAULT 0, last_error TEXT, created_at TEXT NOT NULL)"));
    execute(QStringLiteral("CREATE TABLE IF NOT EXISTS telemetry_cache(id INTEGER PRIMARY KEY AUTOINCREMENT, payload TEXT NOT NULL, synced INTEGER NOT NULL DEFAULT 0, recorded_at TEXT NOT NULL)"));
    m_status = QStringLiteral("Offline cache ready");
    m_available = true;
    emit cacheChanged();
}

void LocalSyncCache::execute(const QString &sql)
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.exec(sql)) {
        m_status = query.lastError().text();
        m_available = false;
    }
}

QString LocalSyncCache::jsonForVariant(const QVariant &value) const
{
    return QString::fromUtf8(QJsonDocument::fromVariant(value).toJson(QJsonDocument::Compact));
}

QVariant LocalSyncCache::variantFromJson(const QString &json) const
{
    if (json.isEmpty()) {
        return {};
    }
    return QJsonDocument::fromJson(json.toUtf8()).toVariant();
}
