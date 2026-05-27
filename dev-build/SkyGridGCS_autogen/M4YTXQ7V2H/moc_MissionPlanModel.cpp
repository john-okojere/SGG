/****************************************************************************
** Meta object code from reading C++ file 'MissionPlanModel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/models/MissionPlanModel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MissionPlanModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MissionPlanModel_t {
    const uint offsetsAndSize[258];
    char stringdata0[1786];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MissionPlanModel_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MissionPlanModel_t qt_meta_stringdata_MissionPlanModel = {
    {
QT_MOC_LITERAL(0, 16), // "MissionPlanModel"
QT_MOC_LITERAL(17, 11), // "planChanged"
QT_MOC_LITERAL(29, 0), // ""
QT_MOC_LITERAL(30, 17), // "validationChanged"
QT_MOC_LITERAL(48, 22), // "operationStatusChanged"
QT_MOC_LITERAL(71, 11), // "createDraft"
QT_MOC_LITERAL(83, 11), // "missionType"
QT_MOC_LITERAL(95, 10), // "aircraftId"
QT_MOC_LITERAL(106, 11), // "loadMission"
QT_MOC_LITERAL(118, 7), // "mission"
QT_MOC_LITERAL(126, 19), // "serializeForBackend"
QT_MOC_LITERAL(146, 25), // "serializeForMavsdkMission"
QT_MOC_LITERAL(172, 17), // "validateForUpload"
QT_MOC_LITERAL(190, 17), // "aircraftConnected"
QT_MOC_LITERAL(208, 13), // "aircraftReady"
QT_MOC_LITERAL(222, 13), // "markUploading"
QT_MOC_LITERAL(236, 12), // "markUploaded"
QT_MOC_LITERAL(249, 6), // "result"
QT_MOC_LITERAL(256, 16), // "markUploadFailed"
QT_MOC_LITERAL(273, 6), // "reason"
QT_MOC_LITERAL(280, 13), // "markExecuting"
QT_MOC_LITERAL(294, 13), // "markCompleted"
QT_MOC_LITERAL(308, 7), // "success"
QT_MOC_LITERAL(316, 20), // "setExecutionProgress"
QT_MOC_LITERAL(337, 14), // "activeWaypoint"
QT_MOC_LITERAL(352, 15), // "progressPercent"
QT_MOC_LITERAL(368, 11), // "addWaypoint"
QT_MOC_LITERAL(380, 8), // "latitude"
QT_MOC_LITERAL(389, 9), // "longitude"
QT_MOC_LITERAL(399, 15), // "setTakeoffPoint"
QT_MOC_LITERAL(415, 17), // "clearTakeoffPoint"
QT_MOC_LITERAL(433, 16), // "addPolygonVertex"
QT_MOC_LITERAL(450, 17), // "movePolygonVertex"
QT_MOC_LITERAL(468, 5), // "index"
QT_MOC_LITERAL(474, 12), // "moveWaypoint"
QT_MOC_LITERAL(487, 19), // "setWaypointAltitude"
QT_MOC_LITERAL(507, 8), // "altitude"
QT_MOC_LITERAL(516, 16), // "setWaypointSpeed"
QT_MOC_LITERAL(533, 5), // "speed"
QT_MOC_LITERAL(539, 18), // "setWaypointHeading"
QT_MOC_LITERAL(558, 7), // "heading"
QT_MOC_LITERAL(566, 22), // "setWaypointGimbalPitch"
QT_MOC_LITERAL(589, 5), // "pitch"
QT_MOC_LITERAL(595, 17), // "setWaypointAction"
QT_MOC_LITERAL(613, 6), // "action"
QT_MOC_LITERAL(620, 21), // "setWaypointCameraMode"
QT_MOC_LITERAL(642, 10), // "cameraMode"
QT_MOC_LITERAL(653, 24), // "setWaypointLoiterSeconds"
QT_MOC_LITERAL(678, 7), // "seconds"
QT_MOC_LITERAL(686, 23), // "setWaypointHoverSeconds"
QT_MOC_LITERAL(710, 14), // "deleteWaypoint"
QT_MOC_LITERAL(725, 17), // "moveWaypointOrder"
QT_MOC_LITERAL(743, 9), // "fromIndex"
QT_MOC_LITERAL(753, 7), // "toIndex"
QT_MOC_LITERAL(761, 10), // "waypointAt"
QT_MOC_LITERAL(772, 6), // "setPoi"
QT_MOC_LITERAL(779, 12), // "setPoiRadius"
QT_MOC_LITERAL(792, 6), // "radius"
QT_MOC_LITERAL(799, 20), // "setPrimaryCoordinate"
QT_MOC_LITERAL(820, 16), // "undoLastGeometry"
QT_MOC_LITERAL(837, 18), // "clearDraftGeometry"
QT_MOC_LITERAL(856, 15), // "validateMission"
QT_MOC_LITERAL(872, 22), // "applyBackendValidation"
QT_MOC_LITERAL(895, 10), // "validation"
QT_MOC_LITERAL(906, 11), // "saveMission"
QT_MOC_LITERAL(918, 9), // "saveLocal"
QT_MOC_LITERAL(928, 13), // "resetGeometry"
QT_MOC_LITERAL(942, 16), // "chooseImportFile"
QT_MOC_LITERAL(959, 17), // "importMissionFile"
QT_MOC_LITERAL(977, 13), // "fileUrlOrPath"
QT_MOC_LITERAL(991, 9), // "missionId"
QT_MOC_LITERAL(1001, 4), // "name"
QT_MOC_LITERAL(1006, 18), // "selectedAircraftId"
QT_MOC_LITERAL(1025, 9), // "syncState"
QT_MOC_LITERAL(1035, 12), // "missionState"
QT_MOC_LITERAL(1048, 11), // "uploadState"
QT_MOC_LITERAL(1060, 14), // "executionState"
QT_MOC_LITERAL(1075, 19), // "activeWaypointIndex"
QT_MOC_LITERAL(1095, 14), // "createdLocally"
QT_MOC_LITERAL(1110, 5), // "dirty"
QT_MOC_LITERAL(1116, 6), // "hasPoi"
QT_MOC_LITERAL(1123, 15), // "captureInterval"
QT_MOC_LITERAL(1139, 3), // "gsd"
QT_MOC_LITERAL(1143, 12), // "frontOverlap"
QT_MOC_LITERAL(1156, 11), // "sideOverlap"
QT_MOC_LITERAL(1168, 14), // "buildingRadius"
QT_MOC_LITERAL(1183, 10), // "safeMargin"
QT_MOC_LITERAL(1194, 11), // "minAltitude"
QT_MOC_LITERAL(1206, 11), // "maxAltitude"
QT_MOC_LITERAL(1218, 11), // "courseAngle"
QT_MOC_LITERAL(1230, 6), // "margin"
QT_MOC_LITERAL(1237, 11), // "gimbalPitch"
QT_MOC_LITERAL(1249, 11), // "cameraModel"
QT_MOC_LITERAL(1261, 13), // "shootingAngle"
QT_MOC_LITERAL(1275, 11), // "captureMode"
QT_MOC_LITERAL(1287, 16), // "flightCourseMode"
QT_MOC_LITERAL(1304, 10), // "insideMode"
QT_MOC_LITERAL(1315, 11), // "headingMode"
QT_MOC_LITERAL(1327, 16), // "aircraftRotation"
QT_MOC_LITERAL(1344, 14), // "corneringStyle"
QT_MOC_LITERAL(1359, 12), // "finishAction"
QT_MOC_LITERAL(1372, 10), // "fenceShape"
QT_MOC_LITERAL(1383, 12), // "boundaryType"
QT_MOC_LITERAL(1396, 12), // "breachAction"
QT_MOC_LITERAL(1409, 13), // "warningAction"
QT_MOC_LITERAL(1423, 15), // "flightDirection"
QT_MOC_LITERAL(1439, 9), // "waypoints"
QT_MOC_LITERAL(1449, 12), // "takeoffPoint"
QT_MOC_LITERAL(1462, 15), // "hasTakeoffPoint"
QT_MOC_LITERAL(1478, 15), // "routeDistanceKm"
QT_MOC_LITERAL(1494, 7), // "polygon"
QT_MOC_LITERAL(1502, 3), // "poi"
QT_MOC_LITERAL(1506, 15), // "primaryLatitude"
QT_MOC_LITERAL(1522, 16), // "primaryLongitude"
QT_MOC_LITERAL(1539, 13), // "missionAreaHa"
QT_MOC_LITERAL(1553, 16), // "estimatedBattery"
QT_MOC_LITERAL(1570, 13), // "estimatedTime"
QT_MOC_LITERAL(1584, 16), // "missionReadiness"
QT_MOC_LITERAL(1601, 18), // "validationWarnings"
QT_MOC_LITERAL(1620, 16), // "validationChecks"
QT_MOC_LITERAL(1637, 17), // "validationRunning"
QT_MOC_LITERAL(1655, 12), // "missionReady"
QT_MOC_LITERAL(1668, 22), // "backendValidationState"
QT_MOC_LITERAL(1691, 16), // "backendSyncReady"
QT_MOC_LITERAL(1708, 21), // "backendUploadEligible"
QT_MOC_LITERAL(1730, 12), // "boundaryOnly"
QT_MOC_LITERAL(1743, 12), // "importStatus"
QT_MOC_LITERAL(1756, 13), // "importSummary"
QT_MOC_LITERAL(1770, 15) // "operationStatus"

    },
    "MissionPlanModel\0planChanged\0\0"
    "validationChanged\0operationStatusChanged\0"
    "createDraft\0missionType\0aircraftId\0"
    "loadMission\0mission\0serializeForBackend\0"
    "serializeForMavsdkMission\0validateForUpload\0"
    "aircraftConnected\0aircraftReady\0"
    "markUploading\0markUploaded\0result\0"
    "markUploadFailed\0reason\0markExecuting\0"
    "markCompleted\0success\0setExecutionProgress\0"
    "activeWaypoint\0progressPercent\0"
    "addWaypoint\0latitude\0longitude\0"
    "setTakeoffPoint\0clearTakeoffPoint\0"
    "addPolygonVertex\0movePolygonVertex\0"
    "index\0moveWaypoint\0setWaypointAltitude\0"
    "altitude\0setWaypointSpeed\0speed\0"
    "setWaypointHeading\0heading\0"
    "setWaypointGimbalPitch\0pitch\0"
    "setWaypointAction\0action\0setWaypointCameraMode\0"
    "cameraMode\0setWaypointLoiterSeconds\0"
    "seconds\0setWaypointHoverSeconds\0"
    "deleteWaypoint\0moveWaypointOrder\0"
    "fromIndex\0toIndex\0waypointAt\0setPoi\0"
    "setPoiRadius\0radius\0setPrimaryCoordinate\0"
    "undoLastGeometry\0clearDraftGeometry\0"
    "validateMission\0applyBackendValidation\0"
    "validation\0saveMission\0saveLocal\0"
    "resetGeometry\0chooseImportFile\0"
    "importMissionFile\0fileUrlOrPath\0"
    "missionId\0name\0selectedAircraftId\0"
    "syncState\0missionState\0uploadState\0"
    "executionState\0activeWaypointIndex\0"
    "createdLocally\0dirty\0hasPoi\0captureInterval\0"
    "gsd\0frontOverlap\0sideOverlap\0"
    "buildingRadius\0safeMargin\0minAltitude\0"
    "maxAltitude\0courseAngle\0margin\0"
    "gimbalPitch\0cameraModel\0shootingAngle\0"
    "captureMode\0flightCourseMode\0insideMode\0"
    "headingMode\0aircraftRotation\0"
    "corneringStyle\0finishAction\0fenceShape\0"
    "boundaryType\0breachAction\0warningAction\0"
    "flightDirection\0waypoints\0takeoffPoint\0"
    "hasTakeoffPoint\0routeDistanceKm\0polygon\0"
    "poi\0primaryLatitude\0primaryLongitude\0"
    "missionAreaHa\0estimatedBattery\0"
    "estimatedTime\0missionReadiness\0"
    "validationWarnings\0validationChecks\0"
    "validationRunning\0missionReady\0"
    "backendValidationState\0backendSyncReady\0"
    "backendUploadEligible\0boundaryOnly\0"
    "importStatus\0importSummary\0operationStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MissionPlanModel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      43,   14, // methods
      64,  415, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  272,    2, 0x06,   65 /* Public */,
       3,    0,  273,    2, 0x06,   66 /* Public */,
       4,    0,  274,    2, 0x06,   67 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       5,    2,  275,    2, 0x02,   68 /* Public */,
       8,    1,  280,    2, 0x02,   71 /* Public */,
      10,    0,  283,    2, 0x102,   73 /* Public | MethodIsConst  */,
      11,    0,  284,    2, 0x102,   74 /* Public | MethodIsConst  */,
      12,    2,  285,    2, 0x02,   75 /* Public */,
      15,    0,  290,    2, 0x02,   78 /* Public */,
      16,    1,  291,    2, 0x02,   79 /* Public */,
      18,    1,  294,    2, 0x02,   81 /* Public */,
      20,    0,  297,    2, 0x02,   83 /* Public */,
      21,    2,  298,    2, 0x02,   84 /* Public */,
      23,    2,  303,    2, 0x02,   87 /* Public */,
      26,    2,  308,    2, 0x02,   90 /* Public */,
      29,    2,  313,    2, 0x02,   93 /* Public */,
      30,    0,  318,    2, 0x02,   96 /* Public */,
      31,    2,  319,    2, 0x02,   97 /* Public */,
      32,    3,  324,    2, 0x02,  100 /* Public */,
      34,    3,  331,    2, 0x02,  104 /* Public */,
      35,    2,  338,    2, 0x02,  108 /* Public */,
      37,    2,  343,    2, 0x02,  111 /* Public */,
      39,    2,  348,    2, 0x02,  114 /* Public */,
      41,    2,  353,    2, 0x02,  117 /* Public */,
      43,    2,  358,    2, 0x02,  120 /* Public */,
      45,    2,  363,    2, 0x02,  123 /* Public */,
      47,    2,  368,    2, 0x02,  126 /* Public */,
      49,    2,  373,    2, 0x02,  129 /* Public */,
      50,    1,  378,    2, 0x02,  132 /* Public */,
      51,    2,  381,    2, 0x02,  134 /* Public */,
      54,    1,  386,    2, 0x102,  137 /* Public | MethodIsConst  */,
      55,    2,  389,    2, 0x02,  139 /* Public */,
      56,    1,  394,    2, 0x02,  142 /* Public */,
      58,    2,  397,    2, 0x02,  144 /* Public */,
      59,    0,  402,    2, 0x02,  147 /* Public */,
      60,    0,  403,    2, 0x02,  148 /* Public */,
      61,    0,  404,    2, 0x02,  149 /* Public */,
      62,    1,  405,    2, 0x02,  150 /* Public */,
      64,    0,  408,    2, 0x02,  152 /* Public */,
      65,    0,  409,    2, 0x02,  153 /* Public */,
      66,    0,  410,    2, 0x02,  154 /* Public */,
      67,    0,  411,    2, 0x102,  155 /* Public | MethodIsConst  */,
      68,    1,  412,    2, 0x02,  156 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,    7,
    QMetaType::Void, QMetaType::QVariantMap,    9,
    QMetaType::QVariantMap,
    QMetaType::QVariantList,
    QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,   13,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   22,   19,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   24,   25,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   27,   28,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   27,   28,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   27,   28,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double,   33,   27,   28,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double,   33,   27,   28,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   33,   36,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   33,   38,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   33,   40,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   33,   42,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   33,   44,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   33,   46,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   33,   48,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   33,   48,
    QMetaType::Void, QMetaType::Int,   33,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   52,   53,
    QMetaType::QVariantMap, QMetaType::Int,   33,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   27,   28,
    QMetaType::Void, QMetaType::Double,   57,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   27,   28,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,   63,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::QString,
    QMetaType::Bool, QMetaType::QString,   69,

 // properties: name, type, flags
      70, QMetaType::QString, 0x00015001, uint(0), 0,
      71, QMetaType::QString, 0x00015103, uint(0), 0,
       6, QMetaType::QString, 0x00015103, uint(0), 0,
      72, QMetaType::QString, 0x00015001, uint(0), 0,
      73, QMetaType::QString, 0x00015001, uint(0), 0,
      74, QMetaType::QString, 0x00015001, uint(0), 0,
      75, QMetaType::QString, 0x00015001, uint(0), 0,
      76, QMetaType::QString, 0x00015001, uint(0), 0,
      77, QMetaType::Int, 0x00015001, uint(0), 0,
      25, QMetaType::Int, 0x00015001, uint(0), 0,
      78, QMetaType::Bool, 0x00015001, uint(0), 0,
      79, QMetaType::Bool, 0x00015001, uint(0), 0,
      80, QMetaType::Bool, 0x00015001, uint(0), 0,
      38, QMetaType::Double, 0x00015103, uint(0), 0,
      36, QMetaType::Double, 0x00015103, uint(0), 0,
      81, QMetaType::Double, 0x00015103, uint(0), 0,
      82, QMetaType::Double, 0x00015103, uint(0), 0,
      83, QMetaType::Int, 0x00015103, uint(0), 0,
      84, QMetaType::Int, 0x00015103, uint(0), 0,
      57, QMetaType::Double, 0x00015103, uint(0), 0,
      85, QMetaType::Double, 0x00015103, uint(0), 0,
      86, QMetaType::Double, 0x00015103, uint(0), 0,
      87, QMetaType::Double, 0x00015103, uint(0), 0,
      88, QMetaType::Double, 0x00015103, uint(0), 0,
      89, QMetaType::Double, 0x00015103, uint(0), 0,
      90, QMetaType::Double, 0x00015103, uint(0), 0,
      91, QMetaType::Double, 0x00015103, uint(0), 0,
      92, QMetaType::QString, 0x00015103, uint(0), 0,
      93, QMetaType::QString, 0x00015103, uint(0), 0,
      94, QMetaType::QString, 0x00015103, uint(0), 0,
      95, QMetaType::QString, 0x00015103, uint(0), 0,
      96, QMetaType::QString, 0x00015103, uint(0), 0,
      97, QMetaType::QString, 0x00015103, uint(0), 0,
      98, QMetaType::QString, 0x00015103, uint(0), 0,
      99, QMetaType::QString, 0x00015103, uint(0), 0,
     100, QMetaType::QString, 0x00015103, uint(0), 0,
     101, QMetaType::QString, 0x00015103, uint(0), 0,
     102, QMetaType::QString, 0x00015103, uint(0), 0,
     103, QMetaType::QString, 0x00015103, uint(0), 0,
     104, QMetaType::QString, 0x00015103, uint(0), 0,
     105, QMetaType::QString, 0x00015103, uint(0), 0,
     106, QMetaType::QVariantList, 0x00015001, uint(0), 0,
     107, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
     108, QMetaType::Bool, 0x00015001, uint(0), 0,
     109, QMetaType::Double, 0x00015001, uint(0), 0,
     110, QMetaType::QVariantList, 0x00015001, uint(0), 0,
     111, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
     112, QMetaType::Double, 0x00015103, uint(0), 0,
     113, QMetaType::Double, 0x00015103, uint(0), 0,
     114, QMetaType::Double, 0x00015001, uint(0), 0,
     115, QMetaType::Double, 0x00015001, uint(0), 0,
     116, QMetaType::QString, 0x00015001, uint(0), 0,
     117, QMetaType::Int, 0x00015001, uint(1), 0,
     118, QMetaType::QVariantList, 0x00015001, uint(0), 0,
     119, QMetaType::QVariantList, 0x00015001, uint(1), 0,
     120, QMetaType::Bool, 0x00015001, uint(1), 0,
     121, QMetaType::Bool, 0x00015001, uint(1), 0,
     122, QMetaType::QString, 0x00015001, uint(1), 0,
     123, QMetaType::Bool, 0x00015001, uint(1), 0,
     124, QMetaType::Bool, 0x00015001, uint(1), 0,
     125, QMetaType::Bool, 0x00015001, uint(1), 0,
     126, QMetaType::QString, 0x00015001, uint(0), 0,
     127, QMetaType::QString, 0x00015001, uint(0), 0,
     128, QMetaType::QString, 0x00015001, uint(2), 0,

       0        // eod
};

void MissionPlanModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MissionPlanModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->planChanged(); break;
        case 1: _t->validationChanged(); break;
        case 2: _t->operationStatusChanged(); break;
        case 3: _t->createDraft((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->loadMission((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 5: { QVariantMap _r = _t->serializeForBackend();
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 6: { QVariantList _r = _t->serializeForMavsdkMission();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 7: { bool _r = _t->validateForUpload((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 8: _t->markUploading(); break;
        case 9: _t->markUploaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->markUploadFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->markExecuting(); break;
        case 12: _t->markCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 13: _t->setExecutionProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 14: _t->addWaypoint((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 15: _t->setTakeoffPoint((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 16: _t->clearTakeoffPoint(); break;
        case 17: _t->addPolygonVertex((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 18: _t->movePolygonVertex((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 19: _t->moveWaypoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 20: _t->setWaypointAltitude((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 21: _t->setWaypointSpeed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 22: _t->setWaypointHeading((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 23: _t->setWaypointGimbalPitch((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 24: _t->setWaypointAction((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 25: _t->setWaypointCameraMode((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 26: _t->setWaypointLoiterSeconds((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 27: _t->setWaypointHoverSeconds((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 28: _t->deleteWaypoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 29: _t->moveWaypointOrder((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 30: { QVariantMap _r = _t->waypointAt((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 31: _t->setPoi((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 32: _t->setPoiRadius((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 33: _t->setPrimaryCoordinate((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 34: _t->undoLastGeometry(); break;
        case 35: _t->clearDraftGeometry(); break;
        case 36: _t->validateMission(); break;
        case 37: _t->applyBackendValidation((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 38: _t->saveMission(); break;
        case 39: _t->saveLocal(); break;
        case 40: _t->resetGeometry(); break;
        case 41: { QString _r = _t->chooseImportFile();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 42: { bool _r = _t->importMissionFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MissionPlanModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionPlanModel::planChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MissionPlanModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionPlanModel::validationChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MissionPlanModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionPlanModel::operationStatusChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MissionPlanModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->missionId(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->name(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->missionType(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->selectedAircraftId(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->syncState(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->missionState(); break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->uploadState(); break;
        case 7: *reinterpret_cast< QString*>(_v) = _t->executionState(); break;
        case 8: *reinterpret_cast< int*>(_v) = _t->activeWaypointIndex(); break;
        case 9: *reinterpret_cast< int*>(_v) = _t->progressPercent(); break;
        case 10: *reinterpret_cast< bool*>(_v) = _t->createdLocally(); break;
        case 11: *reinterpret_cast< bool*>(_v) = _t->dirty(); break;
        case 12: *reinterpret_cast< bool*>(_v) = _t->hasPoi(); break;
        case 13: *reinterpret_cast< double*>(_v) = _t->speed(); break;
        case 14: *reinterpret_cast< double*>(_v) = _t->altitude(); break;
        case 15: *reinterpret_cast< double*>(_v) = _t->captureInterval(); break;
        case 16: *reinterpret_cast< double*>(_v) = _t->gsd(); break;
        case 17: *reinterpret_cast< int*>(_v) = _t->frontOverlap(); break;
        case 18: *reinterpret_cast< int*>(_v) = _t->sideOverlap(); break;
        case 19: *reinterpret_cast< double*>(_v) = _t->radius(); break;
        case 20: *reinterpret_cast< double*>(_v) = _t->buildingRadius(); break;
        case 21: *reinterpret_cast< double*>(_v) = _t->safeMargin(); break;
        case 22: *reinterpret_cast< double*>(_v) = _t->minAltitude(); break;
        case 23: *reinterpret_cast< double*>(_v) = _t->maxAltitude(); break;
        case 24: *reinterpret_cast< double*>(_v) = _t->courseAngle(); break;
        case 25: *reinterpret_cast< double*>(_v) = _t->margin(); break;
        case 26: *reinterpret_cast< double*>(_v) = _t->gimbalPitch(); break;
        case 27: *reinterpret_cast< QString*>(_v) = _t->cameraModel(); break;
        case 28: *reinterpret_cast< QString*>(_v) = _t->shootingAngle(); break;
        case 29: *reinterpret_cast< QString*>(_v) = _t->captureMode(); break;
        case 30: *reinterpret_cast< QString*>(_v) = _t->flightCourseMode(); break;
        case 31: *reinterpret_cast< QString*>(_v) = _t->insideMode(); break;
        case 32: *reinterpret_cast< QString*>(_v) = _t->headingMode(); break;
        case 33: *reinterpret_cast< QString*>(_v) = _t->aircraftRotation(); break;
        case 34: *reinterpret_cast< QString*>(_v) = _t->corneringStyle(); break;
        case 35: *reinterpret_cast< QString*>(_v) = _t->finishAction(); break;
        case 36: *reinterpret_cast< QString*>(_v) = _t->fenceShape(); break;
        case 37: *reinterpret_cast< QString*>(_v) = _t->boundaryType(); break;
        case 38: *reinterpret_cast< QString*>(_v) = _t->breachAction(); break;
        case 39: *reinterpret_cast< QString*>(_v) = _t->warningAction(); break;
        case 40: *reinterpret_cast< QString*>(_v) = _t->flightDirection(); break;
        case 41: *reinterpret_cast< QVariantList*>(_v) = _t->waypoints(); break;
        case 42: *reinterpret_cast< QVariantMap*>(_v) = _t->takeoffPoint(); break;
        case 43: *reinterpret_cast< bool*>(_v) = _t->hasTakeoffPoint(); break;
        case 44: *reinterpret_cast< double*>(_v) = _t->routeDistanceKm(); break;
        case 45: *reinterpret_cast< QVariantList*>(_v) = _t->polygon(); break;
        case 46: *reinterpret_cast< QVariantMap*>(_v) = _t->poi(); break;
        case 47: *reinterpret_cast< double*>(_v) = _t->primaryLatitude(); break;
        case 48: *reinterpret_cast< double*>(_v) = _t->primaryLongitude(); break;
        case 49: *reinterpret_cast< double*>(_v) = _t->missionAreaHa(); break;
        case 50: *reinterpret_cast< double*>(_v) = _t->estimatedBattery(); break;
        case 51: *reinterpret_cast< QString*>(_v) = _t->estimatedTime(); break;
        case 52: *reinterpret_cast< int*>(_v) = _t->missionReadiness(); break;
        case 53: *reinterpret_cast< QVariantList*>(_v) = _t->validationWarnings(); break;
        case 54: *reinterpret_cast< QVariantList*>(_v) = _t->validationChecks(); break;
        case 55: *reinterpret_cast< bool*>(_v) = _t->validationRunning(); break;
        case 56: *reinterpret_cast< bool*>(_v) = _t->missionReady(); break;
        case 57: *reinterpret_cast< QString*>(_v) = _t->backendValidationState(); break;
        case 58: *reinterpret_cast< bool*>(_v) = _t->backendSyncReady(); break;
        case 59: *reinterpret_cast< bool*>(_v) = _t->backendUploadEligible(); break;
        case 60: *reinterpret_cast< bool*>(_v) = _t->boundaryOnly(); break;
        case 61: *reinterpret_cast< QString*>(_v) = _t->importStatus(); break;
        case 62: *reinterpret_cast< QString*>(_v) = _t->importSummary(); break;
        case 63: *reinterpret_cast< QString*>(_v) = _t->operationStatus(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<MissionPlanModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setName(*reinterpret_cast< QString*>(_v)); break;
        case 2: _t->setMissionType(*reinterpret_cast< QString*>(_v)); break;
        case 13: _t->setSpeed(*reinterpret_cast< double*>(_v)); break;
        case 14: _t->setAltitude(*reinterpret_cast< double*>(_v)); break;
        case 15: _t->setCaptureInterval(*reinterpret_cast< double*>(_v)); break;
        case 16: _t->setGsd(*reinterpret_cast< double*>(_v)); break;
        case 17: _t->setFrontOverlap(*reinterpret_cast< int*>(_v)); break;
        case 18: _t->setSideOverlap(*reinterpret_cast< int*>(_v)); break;
        case 19: _t->setRadius(*reinterpret_cast< double*>(_v)); break;
        case 20: _t->setBuildingRadius(*reinterpret_cast< double*>(_v)); break;
        case 21: _t->setSafeMargin(*reinterpret_cast< double*>(_v)); break;
        case 22: _t->setMinAltitude(*reinterpret_cast< double*>(_v)); break;
        case 23: _t->setMaxAltitude(*reinterpret_cast< double*>(_v)); break;
        case 24: _t->setCourseAngle(*reinterpret_cast< double*>(_v)); break;
        case 25: _t->setMargin(*reinterpret_cast< double*>(_v)); break;
        case 26: _t->setGimbalPitch(*reinterpret_cast< double*>(_v)); break;
        case 27: _t->setCameraModel(*reinterpret_cast< QString*>(_v)); break;
        case 28: _t->setShootingAngle(*reinterpret_cast< QString*>(_v)); break;
        case 29: _t->setCaptureMode(*reinterpret_cast< QString*>(_v)); break;
        case 30: _t->setFlightCourseMode(*reinterpret_cast< QString*>(_v)); break;
        case 31: _t->setInsideMode(*reinterpret_cast< QString*>(_v)); break;
        case 32: _t->setHeadingMode(*reinterpret_cast< QString*>(_v)); break;
        case 33: _t->setAircraftRotation(*reinterpret_cast< QString*>(_v)); break;
        case 34: _t->setCorneringStyle(*reinterpret_cast< QString*>(_v)); break;
        case 35: _t->setFinishAction(*reinterpret_cast< QString*>(_v)); break;
        case 36: _t->setFenceShape(*reinterpret_cast< QString*>(_v)); break;
        case 37: _t->setBoundaryType(*reinterpret_cast< QString*>(_v)); break;
        case 38: _t->setBreachAction(*reinterpret_cast< QString*>(_v)); break;
        case 39: _t->setWarningAction(*reinterpret_cast< QString*>(_v)); break;
        case 40: _t->setFlightDirection(*reinterpret_cast< QString*>(_v)); break;
        case 47: _t->setPrimaryLatitude(*reinterpret_cast< double*>(_v)); break;
        case 48: _t->setPrimaryLongitude(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject MissionPlanModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MissionPlanModel.offsetsAndSize,
    qt_meta_data_MissionPlanModel,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MissionPlanModel_t
, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<MissionPlanModel, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>

>,
    nullptr
} };


const QMetaObject *MissionPlanModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MissionPlanModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MissionPlanModel.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MissionPlanModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 43)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 43;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 64;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MissionPlanModel::planChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MissionPlanModel::validationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MissionPlanModel::operationStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
