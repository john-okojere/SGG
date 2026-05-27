#pragma once

#include <QObject>
#include <QVariantList>

#include "../models/MissionPlanModel.h"
#include "../models/MissionTypeModel.h"

class MissionStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MissionTypeModel *missionTypes READ missionTypes CONSTANT)
    Q_PROPERTY(MissionPlanModel *plan READ plan CONSTANT)
    Q_PROPERTY(QVariantList missionHistory READ missionHistory CONSTANT)
    Q_PROPERTY(QVariantList aircraft READ aircraft CONSTANT)

public:
    explicit MissionStore(QObject *parent = nullptr);

    MissionTypeModel *missionTypes();
    MissionPlanModel *plan();
    QVariantList missionHistory() const;
    QVariantList aircraft() const;

public slots:
    void startDraft(const QString &missionType);

signals:
    void overlayInputsChanged();

private:
    MissionTypeModel m_missionTypes;
    MissionPlanModel m_plan;
    QVariantList m_history;
    QVariantList m_aircraft;
};
