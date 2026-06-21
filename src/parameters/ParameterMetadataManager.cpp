#include "ParameterMetadataManager.h"

#include <QRegularExpression>

namespace {
QString normalizedName(const QString &value)
{
    return value.trimmed().toUpper();
}

QVariantMap option(const QString &label, const QString &value)
{
    return QVariantMap{{QStringLiteral("label"), label}, {QStringLiteral("value"), value}};
}
}

ParameterMetadataManager::ParameterMetadataManager(QObject *parent)
    : QObject(parent)
{
    m_groups = {
        QStringLiteral("Airframe"),
        QStringLiteral("Flight Modes"),
        QStringLiteral("RC"),
        QStringLiteral("Battery"),
        QStringLiteral("Failsafe"),
        QStringLiteral("GPS/RTK"),
        QStringLiteral("Compass"),
        QStringLiteral("EKF"),
        QStringLiteral("Telemetry"),
        QStringLiteral("Payload"),
        QStringLiteral("Camera"),
        QStringLiteral("Gimbal"),
        QStringLiteral("Servo"),
        QStringLiteral("Motor/ESC"),
        QStringLiteral("Logging"),
        QStringLiteral("Advanced")
    };
    loadDefaultMetadata();
}

QVariantList ParameterMetadataManager::groups() const
{
    return m_groups;
}

int ParameterMetadataManager::metadataCount() const
{
    return m_exact.size();
}

QVariantMap ParameterMetadataManager::metadataForParameter(const QString &name) const
{
    const QString key = normalizedName(name);
    if (key.isEmpty()) {
        return {};
    }
    bool known = false;
    QVariantMap metadata = exactMetadata(key);
    known = !metadata.isEmpty();
    if (metadata.isEmpty()) {
        metadata = patternMetadata(key);
        known = !metadata.isEmpty();
    }
    if (metadata.isEmpty()) {
        metadata = categoryMetadata(key);
    }
    metadata[QStringLiteral("known")] = known;
    if (!metadata.contains(QStringLiteral("label"))) {
        metadata[QStringLiteral("label")] = key;
    }
    if (!metadata.contains(QStringLiteral("group"))) {
        metadata[QStringLiteral("group")] = QStringLiteral("Advanced");
    }
    if (!metadata.contains(QStringLiteral("controlType"))) {
        metadata[QStringLiteral("controlType")] = QStringLiteral("readonly");
    }
    if (!metadata.contains(QStringLiteral("readOnly"))) {
        metadata[QStringLiteral("readOnly")] = false;
    }
    return metadata;
}

QVariantMap ParameterMetadataManager::decorateParameter(const QVariantMap &row, bool rawEditorAllowed) const
{
    QVariantMap out = row;
    const QString name = normalizedName(row.value(QStringLiteral("name")).toString());
    QVariantMap metadata = metadataForParameter(name);
    const bool known = metadata.value(QStringLiteral("known")).toBool();
    if (!known) {
        metadata[QStringLiteral("known")] = false;
        metadata[QStringLiteral("group")] = categoryMetadata(name).value(QStringLiteral("group"), QStringLiteral("Advanced"));
        metadata[QStringLiteral("controlType")] = rawEditorAllowed ? QStringLiteral("raw") : QStringLiteral("readonly");
        metadata[QStringLiteral("readOnly")] = !rawEditorAllowed;
        metadata[QStringLiteral("description")] = rawEditorAllowed
            ? QStringLiteral("No bundled metadata. Advanced raw editing is enabled for this session.")
            : QStringLiteral("No bundled metadata. Read-only until ArduPilot/PX4 metadata is added or advanced override is granted.");
    }

    out[QStringLiteral("name")] = name;
    out[QStringLiteral("label")] = metadata.value(QStringLiteral("label"), name);
    out[QStringLiteral("group")] = metadata.value(QStringLiteral("group"), row.value(QStringLiteral("category"), QStringLiteral("Advanced")));
    out[QStringLiteral("category")] = out.value(QStringLiteral("group"));
    out[QStringLiteral("description")] = metadata.value(QStringLiteral("description"));
    out[QStringLiteral("controlType")] = metadata.value(QStringLiteral("controlType"));
    out[QStringLiteral("options")] = metadata.value(QStringLiteral("options")).toList();
    out[QStringLiteral("unit")] = metadata.value(QStringLiteral("unit"));
    out[QStringLiteral("minimum")] = metadata.value(QStringLiteral("minimum"));
    out[QStringLiteral("maximum")] = metadata.value(QStringLiteral("maximum"));
    out[QStringLiteral("step")] = metadata.value(QStringLiteral("step"), 1);
    out[QStringLiteral("defaultValue")] = metadata.value(QStringLiteral("defaultValue"));
    out[QStringLiteral("known")] = metadata.value(QStringLiteral("known")).toBool();
    out[QStringLiteral("readOnly")] = metadata.value(QStringLiteral("readOnly")).toBool();
    out[QStringLiteral("reason")] = metadata.value(QStringLiteral("description"));
    return out;
}

QVariantMap ParameterMetadataManager::exactMetadata(const QString &name) const
{
    return m_exact.value(normalizedName(name));
}

QVariantMap ParameterMetadataManager::patternMetadata(const QString &name) const
{
    const QString key = normalizedName(name);
    static const QRegularExpression servoFunction(QStringLiteral("^SERVO\\d+_FUNCTION$"));
    static const QRegularExpression pwmRange(QStringLiteral("^SERVO\\d+_(MIN|MAX|TRIM)$"));
    static const QRegularExpression flightMode(QStringLiteral("^(FLTMODE|FLIGHT_MODE|COM_FLTMODE)\\d+$"));
    static const QRegularExpression rcMap(QStringLiteral("^RC_MAP_(ROLL|PITCH|THROTTLE|YAW|MODE)$"));
    static const QRegularExpression rcChannel(QStringLiteral("^RC\\d+_(MIN|MAX|TRIM|DZ|REVERSED|OPTION)$"));
    static const QRegularExpression batteryNumber(QStringLiteral("^(BATT|BAT|BAT1|BAT2)_(CAPACITY|ARM_VOLT|LOW_VOLT|CRIT_VOLT|VOLT_MULT|AMP_PERVLT)$"));
    static const QRegularExpression gpsType(QStringLiteral("^(GPS|GPS_1|GPS_2|GPS2)_TYPE$"));
    static const QRegularExpression compassUse(QStringLiteral("^COMPASS.*USE.*$"));
    static const QRegularExpression ekfSource(QStringLiteral("^(EK3|EKF2|EKF3)_.*"));
    static const QRegularExpression camera(QStringLiteral("^(CAM|TRIG|MAV_CMD_CAMERA).*"));
    static const QRegularExpression gimbal(QStringLiteral("^(MNT|MOUNT|GMBL|GIMBAL).*"));
    static const QRegularExpression logging(QStringLiteral("^(LOG|SDLOG)_.*"));

    if (servoFunction.match(key).hasMatch()) {
        return makeEnum(QStringLiteral("Servo"), QStringLiteral("Servo function"),
                        QStringLiteral("Select the output function assigned to this servo/motor channel."),
                        enumOptions({{"Disabled", "0"}, {"Motor", "33"}, {"RC pass-through", "1"},
                                     {"Mount pitch", "6"}, {"Mount yaw", "8"}, {"Camera trigger", "10"},
                                     {"Landing gear", "29"}, {"Parachute", "27"}}));
    }
    if (pwmRange.match(key).hasMatch()) {
        return makeNumber(QStringLiteral("Servo"), QStringLiteral("PWM output"),
                          QStringLiteral("Servo output PWM limit or trim value."), 800, 2200, 1, QStringLiteral("us"));
    }
    if (flightMode.match(key).hasMatch()) {
        return makeEnum(QStringLiteral("Flight Modes"), QStringLiteral("Flight mode"),
                        QStringLiteral("Assign the mode selected by this switch position."),
                        enumOptions({{"Stabilize", "0"}, {"Acro", "1"}, {"AltHold", "2"},
                                     {"Auto", "3"}, {"Guided", "4"}, {"Loiter", "5"},
                                     {"RTL", "6"}, {"Land", "9"}, {"Brake", "17"}}));
    }
    if (rcMap.match(key).hasMatch()) {
        return makeEnum(QStringLiteral("RC"), QStringLiteral("RC channel mapping"),
                        QStringLiteral("Map this control axis to an RC input channel."),
                        enumOptions({{"Channel 1", "1"}, {"Channel 2", "2"}, {"Channel 3", "3"},
                                     {"Channel 4", "4"}, {"Channel 5", "5"}, {"Channel 6", "6"},
                                     {"Channel 7", "7"}, {"Channel 8", "8"}}));
    }
    if (rcChannel.match(key).hasMatch()) {
        if (key.endsWith(QStringLiteral("_REVERSED"))) {
            return makeSwitch(QStringLiteral("RC"), QStringLiteral("RC reversed"),
                              QStringLiteral("Reverse this RC channel."));
        }
        if (key.endsWith(QStringLiteral("_OPTION"))) {
            return makeEnum(QStringLiteral("RC"), QStringLiteral("RC option"),
                            QStringLiteral("Assign an auxiliary RC option."),
                            enumOptions({{"Disabled", "0"}, {"Arm/disarm", "41"}, {"Motor emergency stop", "31"},
                                         {"Camera trigger", "9"}, {"Relay", "28"}, {"Gripper", "19"}}));
        }
        return makeNumber(QStringLiteral("RC"), QStringLiteral("RC calibration value"),
                          QStringLiteral("RC input PWM calibration value."), 800, 2200, 1, QStringLiteral("us"));
    }
    if (batteryNumber.match(key).hasMatch()) {
        return makeNumber(QStringLiteral("Battery"), QStringLiteral("Battery value"),
                          QStringLiteral("Battery monitor calibration or threshold."), 0, 100000, 0.1);
    }
    if (gpsType.match(key).hasMatch()) {
        return makeEnum(QStringLiteral("GPS/RTK"), QStringLiteral("GPS type"),
                        QStringLiteral("Select the GPS/RTK receiver driver."),
                        enumOptions({{"Auto", "1"}, {"u-blox", "2"}, {"MAVLink GPS", "14"},
                                     {"DroneCAN GPS", "9"}, {"None", "0"}}));
    }
    if (compassUse.match(key).hasMatch()) {
        return makeSwitch(QStringLiteral("Compass"), QStringLiteral("Compass enabled"),
                          QStringLiteral("Enable or disable this compass source."));
    }
    if (ekfSource.match(key).hasMatch()) {
        return makeEnum(QStringLiteral("EKF"), QStringLiteral("EKF source"),
                        QStringLiteral("Select the navigation source for this EKF field."),
                        enumOptions({{"None", "0"}, {"Compass", "1"}, {"GPS", "3"},
                                     {"Beacon", "4"}, {"Optical Flow", "5"}, {"ExternalNav", "6"}}));
    }
    if (camera.match(key).hasMatch()) {
        return makeEnum(QStringLiteral("Camera"), QStringLiteral("Camera trigger"),
                        QStringLiteral("Camera or trigger configuration."),
                        enumOptions({{"Disabled", "0"}, {"Relay", "1"}, {"Servo", "2"},
                                     {"MAVLink camera", "3"}, {"Distance trigger", "4"}}));
    }
    if (gimbal.match(key).hasMatch()) {
        return makeEnum(QStringLiteral("Gimbal"), QStringLiteral("Gimbal/mount mode"),
                        QStringLiteral("Gimbal or mount configuration."),
                        enumOptions({{"Disabled", "0"}, {"MAVLink mount", "2"}, {"Servo mount", "1"},
                                     {"DroneCAN gimbal", "4"}}));
    }
    if (logging.match(key).hasMatch()) {
        return makeEnum(QStringLiteral("Logging"), QStringLiteral("Logging option"),
                        QStringLiteral("Autopilot logging configuration."),
                        enumOptions({{"Disabled", "0"}, {"Enabled", "1"}, {"Mission only", "2"}}));
    }
    return {};
}

QVariantMap ParameterMetadataManager::categoryMetadata(const QString &name) const
{
    const QString key = normalizedName(name);
    if (key.startsWith(QStringLiteral("BATT")) || key.startsWith(QStringLiteral("BAT"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Battery")}};
    }
    if (key.startsWith(QStringLiteral("FS_")) || key.contains(QStringLiteral("FAILSAFE"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Failsafe")}};
    }
    if (key.startsWith(QStringLiteral("GPS")) || key.startsWith(QStringLiteral("RTK"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("GPS/RTK")}};
    }
    if (key.startsWith(QStringLiteral("COMPASS")) || key.startsWith(QStringLiteral("MAG"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Compass")}};
    }
    if (key.startsWith(QStringLiteral("RC")) || key.startsWith(QStringLiteral("MAN"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("RC")}};
    }
    if (key.startsWith(QStringLiteral("SERVO")) || key.startsWith(QStringLiteral("PWM"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Servo")}};
    }
    if (key.startsWith(QStringLiteral("MOT")) || key.startsWith(QStringLiteral("ESC"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Motor/ESC")}};
    }
    if (key.startsWith(QStringLiteral("FRAME")) || key.startsWith(QStringLiteral("SYS_AUTOSTART")) || key == QStringLiteral("MAV_TYPE")) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Airframe")}};
    }
    if (key.startsWith(QStringLiteral("FLTMODE")) || key.startsWith(QStringLiteral("COM_FLTMODE"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Flight Modes")}};
    }
    if (key.startsWith(QStringLiteral("CAM")) || key.startsWith(QStringLiteral("TRIG"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Camera")}};
    }
    if (key.startsWith(QStringLiteral("MNT")) || key.startsWith(QStringLiteral("GIMBAL"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Gimbal")}};
    }
    if (key.startsWith(QStringLiteral("LOG")) || key.startsWith(QStringLiteral("SDLOG"))) {
        return QVariantMap{{QStringLiteral("group"), QStringLiteral("Logging")}};
    }
    return QVariantMap{{QStringLiteral("group"), QStringLiteral("Advanced")}};
}

QVariantMap ParameterMetadataManager::makeEnum(const QString &group,
                                               const QString &label,
                                               const QString &description,
                                               const QVariantList &options,
                                               const QString &defaultValue) const
{
    return QVariantMap{{QStringLiteral("group"), group},
                       {QStringLiteral("label"), label},
                       {QStringLiteral("description"), description},
                       {QStringLiteral("controlType"), QStringLiteral("enum")},
                       {QStringLiteral("options"), options},
                       {QStringLiteral("defaultValue"), defaultValue},
                       {QStringLiteral("readOnly"), false}};
}

QVariantMap ParameterMetadataManager::makeSwitch(const QString &group,
                                                 const QString &label,
                                                 const QString &description,
                                                 const QString &defaultValue) const
{
    return QVariantMap{{QStringLiteral("group"), group},
                       {QStringLiteral("label"), label},
                       {QStringLiteral("description"), description},
                       {QStringLiteral("controlType"), QStringLiteral("switch")},
                       {QStringLiteral("options"), enumOptions({{"Off", "0"}, {"On", "1"}})},
                       {QStringLiteral("defaultValue"), defaultValue},
                       {QStringLiteral("readOnly"), false}};
}

QVariantMap ParameterMetadataManager::makeNumber(const QString &group,
                                                 const QString &label,
                                                 const QString &description,
                                                 double minimum,
                                                 double maximum,
                                                 double step,
                                                 const QString &unit,
                                                 const QString &defaultValue) const
{
    return QVariantMap{{QStringLiteral("group"), group},
                       {QStringLiteral("label"), label},
                       {QStringLiteral("description"), description},
                       {QStringLiteral("controlType"), QStringLiteral("number")},
                       {QStringLiteral("minimum"), minimum},
                       {QStringLiteral("maximum"), maximum},
                       {QStringLiteral("step"), step},
                       {QStringLiteral("unit"), unit},
                       {QStringLiteral("defaultValue"), defaultValue},
                       {QStringLiteral("readOnly"), false}};
}

QVariantMap ParameterMetadataManager::makeText(const QString &group,
                                               const QString &label,
                                               const QString &description,
                                               const QString &defaultValue) const
{
    return QVariantMap{{QStringLiteral("group"), group},
                       {QStringLiteral("label"), label},
                       {QStringLiteral("description"), description},
                       {QStringLiteral("controlType"), QStringLiteral("text")},
                       {QStringLiteral("defaultValue"), defaultValue},
                       {QStringLiteral("readOnly"), false}};
}

QVariantList ParameterMetadataManager::enumOptions(std::initializer_list<std::pair<const char *, const char *>> values) const
{
    QVariantList options;
    for (const auto &entry : values) {
        options << option(QString::fromLatin1(entry.first), QString::fromLatin1(entry.second));
    }
    return options;
}

void ParameterMetadataManager::addExact(const QString &name, const QVariantMap &metadata)
{
    m_exact.insert(normalizedName(name), metadata);
}

void ParameterMetadataManager::loadDefaultMetadata()
{
    addExact(QStringLiteral("FRAME_CLASS"), makeEnum(QStringLiteral("Airframe"), QStringLiteral("Frame class"),
                                                     QStringLiteral("ArduPilot frame class for the vehicle."),
                                                     enumOptions({{"Quad", "1"}, {"Hexa", "2"}, {"Octa", "3"},
                                                                  {"OctaQuad", "4"}, {"Y6", "5"}, {"Tri", "7"},
                                                                  {"Heli", "6"}, {"Plane", "255"}})));
    addExact(QStringLiteral("FRAME_TYPE"), makeEnum(QStringLiteral("Airframe"), QStringLiteral("Frame type"),
                                                    QStringLiteral("ArduPilot motor geometry for the selected frame."),
                                                    enumOptions({{"Plus", "0"}, {"X", "1"}, {"V", "2"},
                                                                 {"H", "3"}, {"V-tail", "4"}, {"A-tail", "5"}})));
    addExact(QStringLiteral("MAV_TYPE"), makeEnum(QStringLiteral("Airframe"), QStringLiteral("MAV type"),
                                                  QStringLiteral("MAVLink vehicle type reported by the autopilot."),
                                                  enumOptions({{"Generic", "0"}, {"Fixed-wing", "1"}, {"Quadrotor", "2"},
                                                               {"VTOL", "22"}, {"Ground Rover", "10"}, {"Submarine", "12"}})));
    addExact(QStringLiteral("SYS_AUTOSTART"), makeNumber(QStringLiteral("Airframe"), QStringLiteral("PX4 airframe id"),
                                                         QStringLiteral("PX4 airframe/autostart id."), 0, 99999, 1));
    addExact(QStringLiteral("SYS_AUTOCONFIG"), makeSwitch(QStringLiteral("Airframe"), QStringLiteral("PX4 autoconfig"),
                                                          QStringLiteral("Apply PX4 airframe defaults on reboot.")));
    addExact(QStringLiteral("ARMING_CHECK"), makeNumber(QStringLiteral("Airframe"), QStringLiteral("Arming checks"),
                                                        QStringLiteral("ArduPilot arming-check bitmask. Use the default full-check value unless advanced setup requires otherwise."), 0, 32767, 1));
    addExact(QStringLiteral("COM_ARM_WO_GPS"), makeSwitch(QStringLiteral("Airframe"), QStringLiteral("PX4 arm without GPS"),
                                                          QStringLiteral("Permit PX4 arming without GPS. Keep disabled for normal GPS aircraft.")));
    addExact(QStringLiteral("COM_DISARM_LAND"), makeNumber(QStringLiteral("Airframe"), QStringLiteral("PX4 disarm after landing"),
                                                           QStringLiteral("Seconds after landing before automatic disarm."), 0, 60, 1, QStringLiteral("s")));
    addExact(QStringLiteral("RTL_ALT"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("RTL altitude"),
                                                   QStringLiteral("ArduPilot return-to-launch altitude."), 0, 100000, 100, QStringLiteral("cm")));
    addExact(QStringLiteral("RTL_ALT_FINAL"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("RTL final altitude"),
                                                         QStringLiteral("ArduPilot final RTL altitude above home."), 0, 100000, 100, QStringLiteral("cm")));
    addExact(QStringLiteral("RTL_RETURN_ALT"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("PX4 RTL return altitude"),
                                                          QStringLiteral("PX4 return altitude before landing sequence."), 0, 1000, 1, QStringLiteral("m")));
    addExact(QStringLiteral("RTL_DESCEND_ALT"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("PX4 RTL descend altitude"),
                                                           QStringLiteral("PX4 descend altitude during RTL."), 0, 1000, 1, QStringLiteral("m")));

    addExact(QStringLiteral("BATT_MONITOR"), makeEnum(QStringLiteral("Battery"), QStringLiteral("Battery monitor"),
                                                      QStringLiteral("Select battery monitor backend."),
                                                      enumOptions({{"Disabled", "0"}, {"Analog voltage/current", "4"},
                                                                   {"SMBus", "7"}, {"DroneCAN", "8"}, {"MAVLink", "10"}})));
    addExact(QStringLiteral("BATT_CAPACITY"), makeNumber(QStringLiteral("Battery"), QStringLiteral("Battery capacity"),
                                                         QStringLiteral("Battery capacity used for remaining estimate."), 0, 100000, 10, QStringLiteral("mAh")));
    addExact(QStringLiteral("BATT_LOW_VOLT"), makeNumber(QStringLiteral("Battery"), QStringLiteral("Low battery voltage"),
                                                         QStringLiteral("ArduPilot low battery voltage threshold."), 0, 80, 0.1, QStringLiteral("V")));
    addExact(QStringLiteral("BATT_CRT_VOLT"), makeNumber(QStringLiteral("Battery"), QStringLiteral("Critical battery voltage"),
                                                         QStringLiteral("ArduPilot critical battery voltage threshold."), 0, 80, 0.1, QStringLiteral("V")));
    addExact(QStringLiteral("BATT_ARM_VOLT"), makeNumber(QStringLiteral("Battery"), QStringLiteral("Minimum arming voltage"),
                                                         QStringLiteral("Minimum pack voltage required for arming."), 0, 80, 0.1, QStringLiteral("V")));
    addExact(QStringLiteral("BAT1_N_CELLS"), makeNumber(QStringLiteral("Battery"), QStringLiteral("Battery cells"),
                                                        QStringLiteral("PX4 battery cell count."), 1, 16, 1, QStringLiteral("cells")));
    addExact(QStringLiteral("BAT_LOW_THR"), makeNumber(QStringLiteral("Battery"), QStringLiteral("PX4 low battery threshold"),
                                                       QStringLiteral("PX4 low battery percentage threshold."), 0, 1, 0.01));
    addExact(QStringLiteral("BAT_CRIT_THR"), makeNumber(QStringLiteral("Battery"), QStringLiteral("PX4 critical battery threshold"),
                                                        QStringLiteral("PX4 critical battery percentage threshold."), 0, 1, 0.01));
    addExact(QStringLiteral("BAT_EMERGEN_THR"), makeNumber(QStringLiteral("Battery"), QStringLiteral("PX4 emergency battery threshold"),
                                                           QStringLiteral("PX4 emergency battery percentage threshold."), 0, 1, 0.01));
    addExact(QStringLiteral("COM_LOW_BAT_ACT"), makeEnum(QStringLiteral("Failsafe"), QStringLiteral("PX4 low battery action"),
                                                         QStringLiteral("Action when PX4 battery failsafe triggers."),
                                                         enumOptions({{"Warning", "0"}, {"Return", "2"}, {"Land", "3"}})));

    addExact(QStringLiteral("FS_THR_ENABLE"), makeEnum(QStringLiteral("Failsafe"), QStringLiteral("Throttle failsafe"),
                                                       QStringLiteral("ArduPilot throttle/RC failsafe behavior."),
                                                       enumOptions({{"Disabled", "0"}, {"Enabled always RTL", "1"},
                                                                    {"Continue mission", "2"}, {"Land", "4"}})));
    addExact(QStringLiteral("FS_GCS_ENABLE"), makeEnum(QStringLiteral("Failsafe"), QStringLiteral("GCS failsafe"),
                                                       QStringLiteral("Behavior when GCS link is lost."),
                                                       enumOptions({{"Disabled", "0"}, {"RTL", "1"}, {"Continue mission", "2"},
                                                                    {"Smart RTL/RTL", "3"}, {"Land", "5"}})));
    addExact(QStringLiteral("FS_BATT_ENABLE"), makeEnum(QStringLiteral("Failsafe"), QStringLiteral("Battery failsafe"),
                                                        QStringLiteral("ArduPilot battery failsafe action."),
                                                        enumOptions({{"Disabled", "0"}, {"Land", "1"}, {"RTL", "2"},
                                                                     {"SmartRTL or RTL", "3"}, {"Terminate", "5"}})));
    addExact(QStringLiteral("FS_EKF_ACTION"), makeEnum(QStringLiteral("Failsafe"), QStringLiteral("EKF failsafe action"),
                                                       QStringLiteral("ArduPilot EKF failsafe action."),
                                                       enumOptions({{"Land", "1"}, {"AltHold", "2"}, {"Land even in Stabilize", "3"}})));
    addExact(QStringLiteral("COM_RC_LOSS_T"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("RC loss timeout"),
                                                         QStringLiteral("PX4 timeout before RC loss action."), 0, 60, 0.1, QStringLiteral("s")));
    addExact(QStringLiteral("NAV_RCL_ACT"), makeEnum(QStringLiteral("Failsafe"), QStringLiteral("PX4 RC loss action"),
                                                     QStringLiteral("PX4 action after RC signal loss."),
                                                     enumOptions({{"Disabled", "0"}, {"Loiter", "1"}, {"Return", "2"},
                                                                  {"Land", "3"}, {"Terminate", "5"}})));
    addExact(QStringLiteral("NAV_DLL_ACT"), makeEnum(QStringLiteral("Failsafe"), QStringLiteral("PX4 data-link loss action"),
                                                     QStringLiteral("PX4 action after GCS/data-link loss."),
                                                     enumOptions({{"Disabled", "0"}, {"Loiter", "1"}, {"Return", "2"},
                                                                  {"Land", "3"}, {"Terminate", "5"}})));
    addExact(QStringLiteral("FENCE_ENABLE"), makeSwitch(QStringLiteral("Failsafe"), QStringLiteral("Geofence enabled"),
                                                        QStringLiteral("Enable ArduPilot geofence enforcement.")));
    addExact(QStringLiteral("FENCE_TYPE"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("Geofence type"),
                                                      QStringLiteral("ArduPilot geofence type bitmask."), 0, 15, 1));
    addExact(QStringLiteral("FENCE_RADIUS"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("Fence radius"),
                                                        QStringLiteral("ArduPilot circular fence radius."), 0, 100000, 1, QStringLiteral("m")));
    addExact(QStringLiteral("FENCE_ALT_MAX"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("Fence max altitude"),
                                                         QStringLiteral("ArduPilot maximum geofence altitude."), 0, 100000, 1, QStringLiteral("m")));
    addExact(QStringLiteral("GF_ACTION"), makeEnum(QStringLiteral("Failsafe"), QStringLiteral("PX4 geofence action"),
                                                   QStringLiteral("PX4 action when geofence is breached."),
                                                   enumOptions({{"None", "0"}, {"Warning", "1"}, {"Return", "2"},
                                                                {"Terminate", "3"}, {"Land", "4"}})));
    addExact(QStringLiteral("GF_MAX_HOR_DIST"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("PX4 max horizontal fence"),
                                                           QStringLiteral("PX4 maximum horizontal geofence distance."), 0, 100000, 1, QStringLiteral("m")));
    addExact(QStringLiteral("GF_MAX_VER_DIST"), makeNumber(QStringLiteral("Failsafe"), QStringLiteral("PX4 max vertical fence"),
                                                           QStringLiteral("PX4 maximum vertical geofence distance."), 0, 100000, 1, QStringLiteral("m")));

    addExact(QStringLiteral("GPS_AUTO_CONFIG"), makeSwitch(QStringLiteral("GPS/RTK"), QStringLiteral("GPS auto config"),
                                                           QStringLiteral("Allow the autopilot to configure supported GPS modules.")));
    addExact(QStringLiteral("GPS_INJECT_TO"), makeEnum(QStringLiteral("GPS/RTK"), QStringLiteral("RTCM inject target"),
                                                       QStringLiteral("Select GPS receivers that should receive RTK correction injection."),
                                                       enumOptions({{"All GPS", "127"}, {"GPS 1", "1"}, {"GPS 2", "2"}})));

    addExact(QStringLiteral("COMPASS_ENABLE"), makeSwitch(QStringLiteral("Compass"), QStringLiteral("Compass enabled"),
                                                          QStringLiteral("Enable compass use for navigation.")));
    addExact(QStringLiteral("EK3_ENABLE"), makeSwitch(QStringLiteral("EKF"), QStringLiteral("EKF3 enabled"),
                                                      QStringLiteral("Enable EKF3 navigation stack.")));
    addExact(QStringLiteral("EKF2_EN"), makeSwitch(QStringLiteral("EKF"), QStringLiteral("PX4 EKF2 enabled"),
                                                   QStringLiteral("Enable PX4 EKF2 estimator.")));

    addExact(QStringLiteral("SERIAL0_BAUD"), makeEnum(QStringLiteral("Telemetry"), QStringLiteral("Serial baud"),
                                                      QStringLiteral("Serial telemetry baud rate."),
                                                      enumOptions({{"9600", "9"}, {"57600", "57"}, {"115200", "115"},
                                                                   {"921600", "921"}})));
    addExact(QStringLiteral("MAV_PROTO_VER"), makeEnum(QStringLiteral("Telemetry"), QStringLiteral("MAVLink protocol"),
                                                       QStringLiteral("MAVLink protocol version preference."),
                                                       enumOptions({{"Default", "0"}, {"MAVLink 1", "1"}, {"MAVLink 2", "2"}})));
    addExact(QStringLiteral("SERIAL1_PROTOCOL"), makeEnum(QStringLiteral("Telemetry"), QStringLiteral("Serial protocol"),
                                                          QStringLiteral("Telemetry serial protocol selection."),
                                                          enumOptions({{"None", "-1"}, {"MAVLink", "1"}, {"MAVLink 2", "2"},
                                                                       {"GPS", "5"}, {"RCIN", "23"}, {"Scripting", "28"}})));
    addExact(QStringLiteral("SERIAL2_PROTOCOL"), makeEnum(QStringLiteral("Telemetry"), QStringLiteral("Serial protocol"),
                                                          QStringLiteral("Telemetry serial protocol selection."),
                                                          enumOptions({{"None", "-1"}, {"MAVLink", "1"}, {"MAVLink 2", "2"},
                                                                       {"GPS", "5"}, {"RCIN", "23"}, {"Scripting", "28"}})));

    addExact(QStringLiteral("MOT_PWM_TYPE"), makeEnum(QStringLiteral("Motor/ESC"), QStringLiteral("Motor PWM type"),
                                                      QStringLiteral("Motor output protocol for supported ArduPilot vehicles."),
                                                      enumOptions({{"Normal", "0"}, {"OneShot", "1"}, {"OneShot125", "2"},
                                                                   {"Brushed", "3"}, {"DShot150", "4"}, {"DShot300", "5"},
                                                                   {"DShot600", "6"}})));
    addExact(QStringLiteral("MOT_SPIN_ARM"), makeNumber(QStringLiteral("Motor/ESC"), QStringLiteral("Motor spin armed"),
                                                        QStringLiteral("Motor output when armed but not flying."), 0, 1, 0.01));
    addExact(QStringLiteral("MOT_SPIN_MIN"), makeNumber(QStringLiteral("Motor/ESC"), QStringLiteral("Motor spin minimum"),
                                                        QStringLiteral("Minimum motor output in flight."), 0, 1, 0.01));
    addExact(QStringLiteral("PWM_MAIN_RATE"), makeNumber(QStringLiteral("Motor/ESC"), QStringLiteral("PX4 main PWM rate"),
                                                         QStringLiteral("PX4 main PWM output rate."), 50, 2000, 1, QStringLiteral("Hz")));

    addExact(QStringLiteral("CAM_TRIGG_TYPE"), makeEnum(QStringLiteral("Camera"), QStringLiteral("Camera trigger type"),
                                                        QStringLiteral("PX4 camera trigger output mode."),
                                                        enumOptions({{"Disabled", "0"}, {"GPIO", "1"}, {"Seagull", "2"},
                                                                     {"MAVLink", "3"}})));
    addExact(QStringLiteral("CAM_DURATION"), makeNumber(QStringLiteral("Camera"), QStringLiteral("Camera trigger duration"),
                                                        QStringLiteral("Camera trigger pulse duration."), 0, 10, 0.01, QStringLiteral("s")));
    addExact(QStringLiteral("CAM_SERVO_ON"), makeNumber(QStringLiteral("Camera"), QStringLiteral("Camera servo on PWM"),
                                                        QStringLiteral("PWM value for camera trigger active state."), 800, 2200, 1, QStringLiteral("us")));
    addExact(QStringLiteral("CAM_SERVO_OFF"), makeNumber(QStringLiteral("Camera"), QStringLiteral("Camera servo off PWM"),
                                                         QStringLiteral("PWM value for camera trigger inactive state."), 800, 2200, 1, QStringLiteral("us")));
    addExact(QStringLiteral("MNT_TYPE"), makeEnum(QStringLiteral("Gimbal"), QStringLiteral("Mount type"),
                                                  QStringLiteral("ArduPilot mount/gimbal backend."),
                                                  enumOptions({{"None", "0"}, {"Servo", "1"}, {"MAVLink", "2"},
                                                               {"AlexMos", "4"}, {"DroneCAN", "8"}})));
    addExact(QStringLiteral("MNT_DEFLT_MODE"), makeEnum(QStringLiteral("Gimbal"), QStringLiteral("Default mount mode"),
                                                        QStringLiteral("ArduPilot default mount/gimbal mode."),
                                                        enumOptions({{"Retracted", "0"}, {"Neutral", "1"}, {"MAVLink target", "2"},
                                                                     {"RC targeting", "3"}, {"GPS point", "4"}})));
    addExact(QStringLiteral("LOG_BACKEND_TYPE"), makeEnum(QStringLiteral("Logging"), QStringLiteral("Log backend"),
                                                          QStringLiteral("Select where onboard logs are written."),
                                                          enumOptions({{"Disabled", "0"}, {"File", "1"}, {"MAVLink", "2"},
                                                                       {"File and MAVLink", "3"}})));
    addExact(QStringLiteral("LOG_DISARMED"), makeSwitch(QStringLiteral("Logging"), QStringLiteral("Log while disarmed"),
                                                        QStringLiteral("Record logs while the vehicle is disarmed.")));
    addExact(QStringLiteral("SDLOG_MODE"), makeEnum(QStringLiteral("Logging"), QStringLiteral("PX4 SD log mode"),
                                                    QStringLiteral("PX4 SD logging behavior."),
                                                    enumOptions({{"Disabled", "0"}, {"When armed", "1"}, {"Always", "2"}})));
}
