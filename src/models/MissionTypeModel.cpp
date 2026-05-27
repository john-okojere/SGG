#include "MissionTypeModel.h"

MissionTypeModel::MissionTypeModel(QObject *parent) : QAbstractListModel(parent)
{
    m_types = {
        {"photomap", "PhotoMap", "2D orthomosaic survey", "lucide_grid_3x3"},
        {"virtualFence", "Virtual Fence", "Geofence boundary", "cube"},
        {"map3dArea", "3D Map Area", "Polygon 3D capture", "lucide_mountain"},
        {"map3dPoi", "3D Map POI", "Structure orbit capture", "lucide_building_2"},
        {"waypointRoute", "Waypoint Route", "Custom GPS path", "lucide_route"},
        {"towerInspection", "Tower Inspection", "Vertical asset scan", "waypoint"}
    };
}

int MissionTypeModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_types.size();
}

QVariant MissionTypeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_types.size()) {
        return {};
    }
    const auto &type = m_types.at(index.row());
    switch (role) {
    case KeyRole: return type.key;
    case NameRole: return type.name;
    case DescriptionRole: return type.description;
    case IconRole: return type.icon;
    default: return {};
    }
}

QHash<int, QByteArray> MissionTypeModel::roleNames() const
{
    return {
        {KeyRole, "key"},
        {NameRole, "name"},
        {DescriptionRole, "description"},
        {IconRole, "icon"}
    };
}
