#include "MissionStore.h"

MissionStore::MissionStore(QObject *parent) : QObject(parent)
{
    connect(&m_plan, &MissionPlanModel::planChanged, this, &MissionStore::overlayInputsChanged);
}

MissionTypeModel *MissionStore::missionTypes() { return &m_missionTypes; }
MissionPlanModel *MissionStore::plan() { return &m_plan; }
QVariantList MissionStore::missionHistory() const { return m_history; }
QVariantList MissionStore::aircraft() const { return m_aircraft; }

void MissionStore::startDraft(const QString &missionType)
{
    m_plan.createDraft(missionType, QString());
}
