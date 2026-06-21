#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class AccessManager;

class GcsToolCatalogManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList tools READ tools NOTIFY toolsChanged)
    Q_PROPERTY(QVariantList sections READ sections NOTIFY toolsChanged)
    Q_PROPERTY(int availableCount READ availableCount NOTIFY toolsChanged)

public:
    explicit GcsToolCatalogManager(AccessManager *access, QObject *parent = nullptr);

    QVariantList tools() const;
    QVariantList sections() const;
    int availableCount() const;

    Q_INVOKABLE QVariantMap toolForKey(const QString &key) const;
    Q_INVOKABLE QVariantList toolsForSection(const QString &section) const;
    Q_INVOKABLE QVariantList hiddenTools() const;
    Q_INVOKABLE bool canOpenTool(const QString &key) const;
    Q_INVOKABLE QString actionForTool(const QString &key) const;

signals:
    void toolsChanged();

private:
    struct ToolDefinition {
        QString key;
        QString section;
        QString title;
        QString description;
        QString iconText;
        QString action;
        QString route;
    };

    bool catalogAvailable() const;
    QVariantMap toMap(const ToolDefinition &tool) const;
    static QList<ToolDefinition> definitions();

    AccessManager *m_access = nullptr;
};
