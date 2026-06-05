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
    const uint offsetsAndSize[278];
    char stringdata0[1949];
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
QT_MOC_LITERAL(71, 15), // "geometryChanged"
QT_MOC_LITERAL(87, 11), // "createDraft"
QT_MOC_LITERAL(99, 11), // "missionType"
QT_MOC_LITERAL(111, 10), // "aircraftId"
QT_MOC_LITERAL(122, 11), // "loadMission"
QT_MOC_LITERAL(134, 7), // "mission"
QT_MOC_LITERAL(142, 19), // "serializeForBackend"
QT_MOC_LITERAL(162, 25), // "serializeForMavsdkMission"
QT_MOC_LITERAL(188, 17), // "validateForUpload"
QT_MOC_LITERAL(206, 17), // "aircraftConnected"
QT_MOC_LITERAL(224, 13), // "aircraftReady"
QT_MOC_LITERAL(238, 13), // "markUploading"
QT_MOC_LITERAL(252, 12), // "markUploaded"
QT_MOC_LITERAL(265, 6), // "result"
QT_MOC_LITERAL(272, 16), // "markUploadFailed"
QT_MOC_LITERAL(289, 6), // "reason"
QT_MOC_LITERAL(296, 13), // "markExecuting"
QT_MOC_LITERAL(310, 13), // "markCompleted"
QT_MOC_LITERAL(324, 7), // "success"
QT_MOC_LITERAL(332, 20), // "setExecutionProgress"
QT_MOC_LITERAL(353, 14), // "activeWaypoint"
QT_MOC_LITERAL(368, 15), // "progressPercent"
QT_MOC_LITERAL(384, 11), // "addWaypoint"
QT_MOC_LITERAL(396, 8), // "latitude"
QT_MOC_LITERAL(405, 9), // "longitude"
QT_MOC_LITERAL(415, 15), // "setTakeoffPoint"
QT_MOC_LITERAL(431, 17), // "clearTakeoffPoint"
QT_MOC_LITERAL(449, 16), // "addPolygonVertex"
QT_MOC_LITERAL(466, 19), // "insertPolygonVertex"
QT_MOC_LITERAL(486, 10), // "afterIndex"
QT_MOC_LITERAL(497, 17), // "movePolygonVertex"
QT_MOC_LITERAL(515, 5), // "index"
QT_MOC_LITERAL(521, 19), // "deletePolygonVertex"
QT_MOC_LITERAL(541, 12), // "moveWaypoint"
QT_MOC_LITERAL(554, 19), // "setWaypointAltitude"
QT_MOC_LITERAL(574, 8), // "altitude"
QT_MOC_LITERAL(583, 16), // "setWaypointSpeed"
QT_MOC_LITERAL(600, 5), // "speed"
QT_MOC_LITERAL(606, 18), // "setWaypointHeading"
QT_MOC_LITERAL(625, 7), // "heading"
QT_MOC_LITERAL(633, 22), // "setWaypointGimbalPitch"
QT_MOC_LITERAL(656, 5), // "pitch"
QT_MOC_LITERAL(662, 17), // "setWaypointAction"
QT_MOC_LITERAL(680, 6), // "action"
QT_MOC_LITERAL(687, 21), // "setWaypointCameraMode"
QT_MOC_LITERAL(709, 10), // "cameraMode"
QT_MOC_LITERAL(720, 24), // "setWaypointLoiterSeconds"
QT_MOC_LITERAL(745, 7), // "seconds"
QT_MOC_LITERAL(753, 23), // "setWaypointHoverSeconds"
QT_MOC_LITERAL(777, 14), // "deleteWaypoint"
QT_MOC_LITERAL(792, 17), // "moveWaypointOrder"
QT_MOC_LITERAL(810, 9), // "fromIndex"
QT_MOC_LITERAL(820, 7), // "toIndex"
QT_MOC_LITERAL(828, 10), // "waypointAt"
QT_MOC_LITERAL(839, 6), // "setPoi"
QT_MOC_LITERAL(846, 12), // "setPoiRadius"
QT_MOC_LITERAL(859, 6), // "radius"
QT_MOC_LITERAL(866, 20), // "setPrimaryCoordinate"
QT_MOC_LITERAL(887, 16), // "undoLastGeometry"
QT_MOC_LITERAL(904, 18), // "clearDraftGeometry"
QT_MOC_LITERAL(923, 15), // "validateMission"
QT_MOC_LITERAL(939, 22), // "applyBackendValidation"
QT_MOC_LITERAL(962, 10), // "validation"
QT_MOC_LITERAL(973, 11), // "saveMission"
QT_MOC_LITERAL(985, 9), // "saveLocal"
QT_MOC_LITERAL(995, 13), // "resetGeometry"
QT_MOC_LITERAL(1009, 16), // "chooseImportFile"
QT_MOC_LITERAL(1026, 17), // "importMissionFile"
QT_MOC_LITERAL(1044, 13), // "fileUrlOrPath"
QT_MOC_LITERAL(1058, 9), // "missionId"
QT_MOC_LITERAL(1068, 4), // "name"
QT_MOC_LITERAL(1073, 18), // "selectedAircraftId"
QT_MOC_LITERAL(1092, 9), // "syncState"
QT_MOC_LITERAL(1102, 12), // "missionState"
QT_MOC_LITERAL(1115, 11), // "uploadState"
QT_MOC_LITERAL(1127, 14), // "executionState"
QT_MOC_LITERAL(1142, 19), // "activeWaypointIndex"
QT_MOC_LITERAL(1162, 14), // "createdLocally"
QT_MOC_LITERAL(1177, 5), // "dirty"
QT_MOC_LITERAL(1183, 6), // "hasPoi"
QT_MOC_LITERAL(1190, 15), // "captureInterval"
QT_MOC_LITERAL(1206, 3), // "gsd"
QT_MOC_LITERAL(1210, 12), // "frontOverlap"
QT_MOC_LITERAL(1223, 11), // "sideOverlap"
QT_MOC_LITERAL(1235, 14), // "buildingRadius"
QT_MOC_LITERAL(1250, 10), // "safeMargin"
QT_MOC_LITERAL(1261, 11), // "minAltitude"
QT_MOC_LITERAL(1273, 11), // "maxAltitude"
QT_MOC_LITERAL(1285, 11), // "courseAngle"
QT_MOC_LITERAL(1297, 6), // "margin"
QT_MOC_LITERAL(1304, 11), // "gimbalPitch"
QT_MOC_LITERAL(1316, 11), // "cameraModel"
QT_MOC_LITERAL(1328, 13), // "shootingAngle"
QT_MOC_LITERAL(1342, 11), // "captureMode"
QT_MOC_LITERAL(1354, 16), // "flightCourseMode"
QT_MOC_LITERAL(1371, 10), // "insideMode"
QT_MOC_LITERAL(1382, 11), // "headingMode"
QT_MOC_LITERAL(1394, 16), // "aircraftRotation"
QT_MOC_LITERAL(1411, 14), // "corneringStyle"
QT_MOC_LITERAL(1426, 12), // "finishAction"
QT_MOC_LITERAL(1439, 10), // "fenceShape"
QT_MOC_LITERAL(1450, 12), // "boundaryType"
QT_MOC_LITERAL(1463, 12), // "breachAction"
QT_MOC_LITERAL(1476, 13), // "warningAction"
QT_MOC_LITERAL(1490, 15), // "flightDirection"
QT_MOC_LITERAL(1506, 9), // "waypoints"
QT_MOC_LITERAL(1516, 12), // "takeoffPoint"
QT_MOC_LITERAL(1529, 15), // "hasTakeoffPoint"
QT_MOC_LITERAL(1545, 15), // "routeDistanceKm"
QT_MOC_LITERAL(1561, 7), // "polygon"
QT_MOC_LITERAL(1569, 3), // "poi"
QT_MOC_LITERAL(1573, 15), // "primaryLatitude"
QT_MOC_LITERAL(1589, 16), // "primaryLongitude"
QT_MOC_LITERAL(1606, 13), // "missionAreaHa"
QT_MOC_LITERAL(1620, 16), // "estimatedBattery"
QT_MOC_LITERAL(1637, 13), // "estimatedTime"
QT_MOC_LITERAL(1651, 16), // "missionReadiness"
QT_MOC_LITERAL(1668, 18), // "validationWarnings"
QT_MOC_LITERAL(1687, 16), // "validationChecks"
QT_MOC_LITERAL(1704, 17), // "validationRunning"
QT_MOC_LITERAL(1722, 12), // "missionReady"
QT_MOC_LITERAL(1735, 22), // "backendValidationState"
QT_MOC_LITERAL(1758, 16), // "backendSyncReady"
QT_MOC_LITERAL(1775, 21), // "backendUploadEligible"
QT_MOC_LITERAL(1797, 12), // "boundaryOnly"
QT_MOC_LITERAL(1810, 12), // "importStatus"
QT_MOC_LITERAL(1823, 13), // "importSummary"
QT_MOC_LITERAL(1837, 15), // "operationStatus"
QT_MOC_LITERAL(1853, 14), // "generatedRoute"
QT_MOC_LITERAL(1868, 14), // "routeEstimates"
QT_MOC_LITERAL(1883, 13), // "cameraPreview"
QT_MOC_LITERAL(1897, 16), // "elevationProfile"
QT_MOC_LITERAL(1914, 15), // "boundaryPreview"
QT_MOC_LITERAL(1930, 18) // "generationRevision"

    },
    "MissionPlanModel\0planChanged\0\0"
    "validationChanged\0operationStatusChanged\0"
    "geometryChanged\0createDraft\0missionType\0"
    "aircraftId\0loadMission\0mission\0"
    "serializeForBackend\0serializeForMavsdkMission\0"
    "validateForUpload\0aircraftConnected\0"
    "aircraftReady\0markUploading\0markUploaded\0"
    "result\0markUploadFailed\0reason\0"
    "markExecuting\0markCompleted\0success\0"
    "setExecutionProgress\0activeWaypoint\0"
    "progressPercent\0addWaypoint\0latitude\0"
    "longitude\0setTakeoffPoint\0clearTakeoffPoint\0"
    "addPolygonVertex\0insertPolygonVertex\0"
    "afterIndex\0movePolygonVertex\0index\0"
    "deletePolygonVertex\0moveWaypoint\0"
    "setWaypointAltitude\0altitude\0"
    "setWaypointSpeed\0speed\0setWaypointHeading\0"
    "heading\0setWaypointGimbalPitch\0pitch\0"
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
    "importStatus\0importSummary\0operationStatus\0"
    "generatedRoute\0routeEstimates\0"
    "cameraPreview\0elevationProfile\0"
    "boundaryPreview\0generationRevision"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MissionPlanModel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      46,   14, // methods
      70,  444, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  290,    2, 0x06,   71 /* Public */,
       3,    0,  291,    2, 0x06,   72 /* Public */,
       4,    0,  292,    2, 0x06,   73 /* Public */,
       5,    0,  293,    2, 0x06,   74 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       6,    2,  294,    2, 0x02,   75 /* Public */,
       9,    1,  299,    2, 0x02,   78 /* Public */,
      11,    0,  302,    2, 0x102,   80 /* Public | MethodIsConst  */,
      12,    0,  303,    2, 0x102,   81 /* Public | MethodIsConst  */,
      13,    2,  304,    2, 0x02,   82 /* Public */,
      16,    0,  309,    2, 0x02,   85 /* Public */,
      17,    1,  310,    2, 0x02,   86 /* Public */,
      19,    1,  313,    2, 0x02,   88 /* Public */,
      21,    0,  316,    2, 0x02,   90 /* Public */,
      22,    2,  317,    2, 0x02,   91 /* Public */,
      24,    2,  322,    2, 0x02,   94 /* Public */,
      27,    2,  327,    2, 0x02,   97 /* Public */,
      30,    2,  332,    2, 0x02,  100 /* Public */,
      31,    0,  337,    2, 0x02,  103 /* Public */,
      32,    2,  338,    2, 0x02,  104 /* Public */,
      33,    3,  343,    2, 0x02,  107 /* Public */,
      35,    3,  350,    2, 0x02,  111 /* Public */,
      37,    1,  357,    2, 0x02,  115 /* Public */,
      38,    3,  360,    2, 0x02,  117 /* Public */,
      39,    2,  367,    2, 0x02,  121 /* Public */,
      41,    2,  372,    2, 0x02,  124 /* Public */,
      43,    2,  377,    2, 0x02,  127 /* Public */,
      45,    2,  382,    2, 0x02,  130 /* Public */,
      47,    2,  387,    2, 0x02,  133 /* Public */,
      49,    2,  392,    2, 0x02,  136 /* Public */,
      51,    2,  397,    2, 0x02,  139 /* Public */,
      53,    2,  402,    2, 0x02,  142 /* Public */,
      54,    1,  407,    2, 0x02,  145 /* Public */,
      55,    2,  410,    2, 0x02,  147 /* Public */,
      58,    1,  415,    2, 0x102,  150 /* Public | MethodIsConst  */,
      59,    2,  418,    2, 0x02,  152 /* Public */,
      60,    1,  423,    2, 0x02,  155 /* Public */,
      62,    2,  426,    2, 0x02,  157 /* Public */,
      63,    0,  431,    2, 0x02,  160 /* Public */,
      64,    0,  432,    2, 0x02,  161 /* Public */,
      65,    0,  433,    2, 0x02,  162 /* Public */,
      66,    1,  434,    2, 0x02,  163 /* Public */,
      68,    0,  437,    2, 0x02,  165 /* Public */,
      69,    0,  438,    2, 0x02,  166 /* Public */,
      70,    0,  439,    2, 0x02,  167 /* Public */,
      71,    0,  440,    2, 0x102,  168 /* Public | MethodIsConst  */,
      72,    1,  441,    2, 0x02,  169 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,    8,
    QMetaType::Void, QMetaType::QVariantMap,   10,
    QMetaType::QVariantMap,
    QMetaType::QVariantList,
    QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,   14,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   23,   20,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   25,   26,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   28,   29,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   28,   29,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   28,   29,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double,   34,   28,   29,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double,   36,   28,   29,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double,   36,   28,   29,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   36,   40,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   36,   42,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   36,   44,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   36,   46,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   36,   48,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   36,   50,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   36,   52,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   36,   52,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   56,   57,
    QMetaType::QVariantMap, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   28,   29,
    QMetaType::Void, QMetaType::Double,   61,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   28,   29,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,   67,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::QString,
    QMetaType::Bool, QMetaType::QString,   73,

 // properties: name, type, flags
      74, QMetaType::QString, 0x00015001, uint(0), 0,
      75, QMetaType::QString, 0x00015103, uint(0), 0,
       7, QMetaType::QString, 0x00015103, uint(0), 0,
      76, QMetaType::QString, 0x00015001, uint(0), 0,
      77, QMetaType::QString, 0x00015001, uint(0), 0,
      78, QMetaType::QString, 0x00015001, uint(0), 0,
      79, QMetaType::QString, 0x00015001, uint(0), 0,
      80, QMetaType::QString, 0x00015001, uint(0), 0,
      81, QMetaType::Int, 0x00015001, uint(0), 0,
      26, QMetaType::Int, 0x00015001, uint(0), 0,
      82, QMetaType::Bool, 0x00015001, uint(0), 0,
      83, QMetaType::Bool, 0x00015001, uint(0), 0,
      84, QMetaType::Bool, 0x00015001, uint(0), 0,
      42, QMetaType::Double, 0x00015103, uint(0), 0,
      40, QMetaType::Double, 0x00015103, uint(0), 0,
      85, QMetaType::Double, 0x00015103, uint(0), 0,
      86, QMetaType::Double, 0x00015103, uint(0), 0,
      87, QMetaType::Int, 0x00015103, uint(0), 0,
      88, QMetaType::Int, 0x00015103, uint(0), 0,
      61, QMetaType::Double, 0x00015103, uint(0), 0,
      89, QMetaType::Double, 0x00015103, uint(0), 0,
      90, QMetaType::Double, 0x00015103, uint(0), 0,
      91, QMetaType::Double, 0x00015103, uint(0), 0,
      92, QMetaType::Double, 0x00015103, uint(0), 0,
      93, QMetaType::Double, 0x00015103, uint(0), 0,
      94, QMetaType::Double, 0x00015103, uint(0), 0,
      95, QMetaType::Double, 0x00015103, uint(0), 0,
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
     106, QMetaType::QString, 0x00015103, uint(0), 0,
     107, QMetaType::QString, 0x00015103, uint(0), 0,
     108, QMetaType::QString, 0x00015103, uint(0), 0,
     109, QMetaType::QString, 0x00015103, uint(0), 0,
     110, QMetaType::QVariantList, 0x00015001, uint(0), 0,
     111, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
     112, QMetaType::Bool, 0x00015001, uint(0), 0,
     113, QMetaType::Double, 0x00015001, uint(0), 0,
     114, QMetaType::QVariantList, 0x00015001, uint(0), 0,
     115, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
     116, QMetaType::Double, 0x00015103, uint(0), 0,
     117, QMetaType::Double, 0x00015103, uint(0), 0,
     118, QMetaType::Double, 0x00015001, uint(0), 0,
     119, QMetaType::Double, 0x00015001, uint(0), 0,
     120, QMetaType::QString, 0x00015001, uint(0), 0,
     121, QMetaType::Int, 0x00015001, uint(1), 0,
     122, QMetaType::QVariantList, 0x00015001, uint(0), 0,
     123, QMetaType::QVariantList, 0x00015001, uint(1), 0,
     124, QMetaType::Bool, 0x00015001, uint(1), 0,
     125, QMetaType::Bool, 0x00015001, uint(1), 0,
     126, QMetaType::QString, 0x00015001, uint(1), 0,
     127, QMetaType::Bool, 0x00015001, uint(1), 0,
     128, QMetaType::Bool, 0x00015001, uint(1), 0,
     129, QMetaType::Bool, 0x00015001, uint(1), 0,
     130, QMetaType::QString, 0x00015001, uint(0), 0,
     131, QMetaType::QString, 0x00015001, uint(0), 0,
     132, QMetaType::QString, 0x00015001, uint(2), 0,
     133, QMetaType::QVariantList, 0x00015001, uint(3), 0,
     134, QMetaType::QVariantMap, 0x00015001, uint(3), 0,
     135, QMetaType::QVariantMap, 0x00015001, uint(3), 0,
     136, QMetaType::QVariantList, 0x00015001, uint(3), 0,
     137, QMetaType::QVariantList, 0x00015001, uint(3), 0,
     138, QMetaType::Int, 0x00015001, uint(3), 0,

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
        case 3: _t->geometryChanged(); break;
        case 4: _t->createDraft((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->loadMission((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 6: { QVariantMap _r = _t->serializeForBackend();
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 7: { QVariantList _r = _t->serializeForMavsdkMission();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 8: { bool _r = _t->validateForUpload((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 9: _t->markUploading(); break;
        case 10: _t->markUploaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->markUploadFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->markExecuting(); break;
        case 13: _t->markCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 14: _t->setExecutionProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 15: _t->addWaypoint((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 16: _t->setTakeoffPoint((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 17: _t->clearTakeoffPoint(); break;
        case 18: _t->addPolygonVertex((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 19: _t->insertPolygonVertex((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 20: _t->movePolygonVertex((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 21: _t->deletePolygonVertex((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 22: _t->moveWaypoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 23: _t->setWaypointAltitude((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 24: _t->setWaypointSpeed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 25: _t->setWaypointHeading((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 26: _t->setWaypointGimbalPitch((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 27: _t->setWaypointAction((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 28: _t->setWaypointCameraMode((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 29: _t->setWaypointLoiterSeconds((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 30: _t->setWaypointHoverSeconds((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 31: _t->deleteWaypoint((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 32: _t->moveWaypointOrder((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 33: { QVariantMap _r = _t->waypointAt((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 34: _t->setPoi((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 35: _t->setPoiRadius((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 36: _t->setPrimaryCoordinate((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 37: _t->undoLastGeometry(); break;
        case 38: _t->clearDraftGeometry(); break;
        case 39: _t->validateMission(); break;
        case 40: _t->applyBackendValidation((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 41: _t->saveMission(); break;
        case 42: _t->saveLocal(); break;
        case 43: _t->resetGeometry(); break;
        case 44: { QString _r = _t->chooseImportFile();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 45: { bool _r = _t->importMissionFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
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
        {
            using _t = void (MissionPlanModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionPlanModel::geometryChanged)) {
                *result = 3;
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
        case 64: *reinterpret_cast< QVariantList*>(_v) = _t->generatedRoute(); break;
        case 65: *reinterpret_cast< QVariantMap*>(_v) = _t->routeEstimates(); break;
        case 66: *reinterpret_cast< QVariantMap*>(_v) = _t->cameraPreview(); break;
        case 67: *reinterpret_cast< QVariantList*>(_v) = _t->elevationProfile(); break;
        case 68: *reinterpret_cast< QVariantList*>(_v) = _t->boundaryPreview(); break;
        case 69: *reinterpret_cast< int*>(_v) = _t->generationRevision(); break;
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
, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<MissionPlanModel, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>

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
        if (_id < 46)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 46;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 46)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 46;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 70;
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

// SIGNAL 3
void MissionPlanModel::geometryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
