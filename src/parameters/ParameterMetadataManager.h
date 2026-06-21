#pragma once

#include <QObject>
#include <QHash>
#include <QVariantList>
#include <QVariantMap>

#include <initializer_list>
#include <utility>

class ParameterMetadataManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList groups READ groups CONSTANT)
    Q_PROPERTY(int metadataCount READ metadataCount CONSTANT)

public:
    explicit ParameterMetadataManager(QObject *parent = nullptr);

    QVariantList groups() const;
    int metadataCount() const;

    Q_INVOKABLE QVariantMap metadataForParameter(const QString &name) const;
    QVariantMap decorateParameter(const QVariantMap &row, bool rawEditorAllowed) const;

private:
    QVariantMap exactMetadata(const QString &name) const;
    QVariantMap patternMetadata(const QString &name) const;
    QVariantMap categoryMetadata(const QString &name) const;
    QVariantMap makeEnum(const QString &group,
                         const QString &label,
                         const QString &description,
                         const QVariantList &options,
                         const QString &defaultValue = QString()) const;
    QVariantMap makeSwitch(const QString &group,
                           const QString &label,
                           const QString &description,
                           const QString &defaultValue = QStringLiteral("0")) const;
    QVariantMap makeNumber(const QString &group,
                           const QString &label,
                           const QString &description,
                           double minimum,
                           double maximum,
                           double step,
                           const QString &unit = QString(),
                           const QString &defaultValue = QString()) const;
    QVariantMap makeText(const QString &group,
                         const QString &label,
                         const QString &description,
                         const QString &defaultValue = QString()) const;
    QVariantList enumOptions(std::initializer_list<std::pair<const char *, const char *>> values) const;
    void addExact(const QString &name, const QVariantMap &metadata);
    void loadDefaultMetadata();

    QHash<QString, QVariantMap> m_exact;
    QVariantList m_groups;
};
