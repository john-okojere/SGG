/****************************************************************************
** Meta object code from reading C++ file 'MissionSyncManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/sync/MissionSyncManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MissionSyncManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MissionSyncManager_t {
    const uint offsetsAndSize[54];
    char stringdata0[360];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MissionSyncManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MissionSyncManager_t qt_meta_stringdata_MissionSyncManager = {
    {
QT_MOC_LITERAL(0, 18), // "MissionSyncManager"
QT_MOC_LITERAL(19, 11), // "syncChanged"
QT_MOC_LITERAL(31, 0), // ""
QT_MOC_LITERAL(32, 17), // "bootstrapReceived"
QT_MOC_LITERAL(50, 9), // "bootstrap"
QT_MOC_LITERAL(60, 13), // "missionOpened"
QT_MOC_LITERAL(74, 11), // "missionType"
QT_MOC_LITERAL(86, 9), // "missionId"
QT_MOC_LITERAL(96, 18), // "activeMissionSaved"
QT_MOC_LITERAL(115, 7), // "success"
QT_MOC_LITERAL(123, 7), // "message"
QT_MOC_LITERAL(131, 22), // "activeMissionValidated"
QT_MOC_LITERAL(154, 12), // "syncMissions"
QT_MOC_LITERAL(167, 17), // "saveActiveMission"
QT_MOC_LITERAL(185, 21), // "validateActiveMission"
QT_MOC_LITERAL(207, 11), // "openMission"
QT_MOC_LITERAL(219, 7), // "mission"
QT_MOC_LITERAL(227, 7), // "syncing"
QT_MOC_LITERAL(235, 6), // "status"
QT_MOC_LITERAL(242, 12), // "organization"
QT_MOC_LITERAL(255, 12), // "pilotProfile"
QT_MOC_LITERAL(268, 13), // "deviceSummary"
QT_MOC_LITERAL(282, 13), // "sessionStatus"
QT_MOC_LITERAL(296, 16), // "assignedAircraft"
QT_MOC_LITERAL(313, 16), // "approvedMissions"
QT_MOC_LITERAL(330, 14), // "activeMissions"
QT_MOC_LITERAL(345, 14) // "missionHistory"

    },
    "MissionSyncManager\0syncChanged\0\0"
    "bootstrapReceived\0bootstrap\0missionOpened\0"
    "missionType\0missionId\0activeMissionSaved\0"
    "success\0message\0activeMissionValidated\0"
    "syncMissions\0saveActiveMission\0"
    "validateActiveMission\0openMission\0"
    "mission\0syncing\0status\0organization\0"
    "pilotProfile\0deviceSummary\0sessionStatus\0"
    "assignedAircraft\0approvedMissions\0"
    "activeMissions\0missionHistory"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MissionSyncManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
      10,  100, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,   11 /* Public */,
       3,    1,   75,    2, 0x06,   12 /* Public */,
       5,    2,   78,    2, 0x06,   14 /* Public */,
       8,    2,   83,    2, 0x06,   17 /* Public */,
      11,    2,   88,    2, 0x06,   20 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   93,    2, 0x02,   23 /* Public */,
      12,    0,   94,    2, 0x02,   24 /* Public */,
      13,    0,   95,    2, 0x02,   25 /* Public */,
      14,    0,   96,    2, 0x02,   26 /* Public */,
      15,    1,   97,    2, 0x02,   27 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,    7,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    9,   10,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    9,   10,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,   16,

 // properties: name, type, flags
      17, QMetaType::Bool, 0x00015001, uint(0), 0,
      18, QMetaType::QString, 0x00015001, uint(0), 0,
      19, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
      20, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
      21, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
      22, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
      23, QMetaType::QVariantList, 0x00015001, uint(0), 0,
      24, QMetaType::QVariantList, 0x00015001, uint(0), 0,
      25, QMetaType::QVariantList, 0x00015001, uint(0), 0,
      26, QMetaType::QVariantList, 0x00015001, uint(0), 0,

       0        // eod
};

void MissionSyncManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MissionSyncManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->syncChanged(); break;
        case 1: _t->bootstrapReceived((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 2: _t->missionOpened((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->activeMissionSaved((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->activeMissionValidated((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->bootstrap(); break;
        case 6: _t->syncMissions(); break;
        case 7: _t->saveActiveMission(); break;
        case 8: _t->validateActiveMission(); break;
        case 9: _t->openMission((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MissionSyncManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionSyncManager::syncChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MissionSyncManager::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionSyncManager::bootstrapReceived)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MissionSyncManager::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionSyncManager::missionOpened)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MissionSyncManager::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionSyncManager::activeMissionSaved)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MissionSyncManager::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionSyncManager::activeMissionValidated)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MissionSyncManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->syncing(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        case 2: *reinterpret_cast< QVariantMap*>(_v) = _t->organization(); break;
        case 3: *reinterpret_cast< QVariantMap*>(_v) = _t->pilotProfile(); break;
        case 4: *reinterpret_cast< QVariantMap*>(_v) = _t->deviceSummary(); break;
        case 5: *reinterpret_cast< QVariantMap*>(_v) = _t->sessionStatus(); break;
        case 6: *reinterpret_cast< QVariantList*>(_v) = _t->assignedAircraft(); break;
        case 7: *reinterpret_cast< QVariantList*>(_v) = _t->approvedMissions(); break;
        case 8: *reinterpret_cast< QVariantList*>(_v) = _t->activeMissions(); break;
        case 9: *reinterpret_cast< QVariantList*>(_v) = _t->missionHistory(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject MissionSyncManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MissionSyncManager.offsetsAndSize,
    qt_meta_data_MissionSyncManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MissionSyncManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<MissionSyncManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>

>,
    nullptr
} };


const QMetaObject *MissionSyncManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MissionSyncManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MissionSyncManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MissionSyncManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MissionSyncManager::syncChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MissionSyncManager::bootstrapReceived(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MissionSyncManager::missionOpened(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MissionSyncManager::activeMissionSaved(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MissionSyncManager::activeMissionValidated(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
