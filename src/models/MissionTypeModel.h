#pragma once

#include <QAbstractListModel>
#include <QVector>

struct MissionType
{
    QString key;
    QString name;
    QString description;
    QString icon;
};

class MissionTypeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        KeyRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        IconRole
    };

    explicit MissionTypeModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<MissionType> m_types;
};
