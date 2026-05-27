#pragma once

#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class QSqlDatabase;

class LocalSyncCache : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY cacheChanged)
    Q_PROPERTY(QString status READ status NOTIFY cacheChanged)

public:
    explicit LocalSyncCache(QObject *parent = nullptr);
    ~LocalSyncCache() override;

    bool available() const;
    QString status() const;

    QVariantMap loadObject(const QString &namespaceName, const QString &key) const;
    QVariantList loadList(const QString &namespaceName, const QString &key) const;
    void saveObject(const QString &namespaceName, const QString &key, const QVariantMap &value);
    void saveList(const QString &namespaceName, const QString &key, const QVariantList &value);
    void enqueueSync(const QString &kind, const QVariantMap &payload);
    QVariantList queuedSyncItems(int limit = 50) const;
    void markSyncDone(int id);
    void markSyncFailed(int id, const QString &error);
    void cacheTelemetry(const QVariantMap &payload);

signals:
    void cacheChanged();

private:
    void initialize();
    void execute(const QString &sql);
    QString jsonForVariant(const QVariant &value) const;
    QVariant variantFromJson(const QString &json) const;

    QString m_connectionName;
    QString m_status = "Cache initializing";
    bool m_available = false;
};
