/****************************************************************************
** Meta object code from reading C++ file 'MissionExecutionManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/vehicle/MissionExecutionManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MissionExecutionManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MissionExecutionManager_t {
    const uint offsetsAndSize[26];
    char stringdata0[161];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MissionExecutionManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MissionExecutionManager_t qt_meta_stringdata_MissionExecutionManager = {
    {
QT_MOC_LITERAL(0, 23), // "MissionExecutionManager"
QT_MOC_LITERAL(24, 16), // "executionChanged"
QT_MOC_LITERAL(41, 0), // ""
QT_MOC_LITERAL(42, 14), // "missionStarted"
QT_MOC_LITERAL(57, 18), // "missionStartFailed"
QT_MOC_LITERAL(76, 7), // "message"
QT_MOC_LITERAL(84, 15), // "missionFinished"
QT_MOC_LITERAL(100, 6), // "result"
QT_MOC_LITERAL(107, 12), // "startMission"
QT_MOC_LITERAL(120, 9), // "executing"
QT_MOC_LITERAL(130, 14), // "activeWaypoint"
QT_MOC_LITERAL(145, 8), // "progress"
QT_MOC_LITERAL(154, 6) // "status"

    },
    "MissionExecutionManager\0executionChanged\0"
    "\0missionStarted\0missionStartFailed\0"
    "message\0missionFinished\0result\0"
    "startMission\0executing\0activeWaypoint\0"
    "progress\0status"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MissionExecutionManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       4,   53, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x06,    5 /* Public */,
       3,    0,   45,    2, 0x06,    6 /* Public */,
       4,    1,   46,    2, 0x06,    7 /* Public */,
       6,    1,   49,    2, 0x06,    9 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,   52,    2, 0x02,   11 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QVariantMap,    7,

 // methods: parameters
    QMetaType::Void,

 // properties: name, type, flags
       9, QMetaType::Bool, 0x00015001, uint(0), 0,
      10, QMetaType::Int, 0x00015001, uint(0), 0,
      11, QMetaType::Int, 0x00015001, uint(0), 0,
      12, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

void MissionExecutionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MissionExecutionManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->executionChanged(); break;
        case 1: _t->missionStarted(); break;
        case 2: _t->missionStartFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->missionFinished((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 4: _t->startMission(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MissionExecutionManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionExecutionManager::executionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MissionExecutionManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionExecutionManager::missionStarted)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MissionExecutionManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionExecutionManager::missionStartFailed)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MissionExecutionManager::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionExecutionManager::missionFinished)) {
                *result = 3;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MissionExecutionManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->executing(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->activeWaypoint(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->progress(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject MissionExecutionManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MissionExecutionManager.offsetsAndSize,
    qt_meta_data_MissionExecutionManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MissionExecutionManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<MissionExecutionManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>

>,
    nullptr
} };


const QMetaObject *MissionExecutionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MissionExecutionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MissionExecutionManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MissionExecutionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MissionExecutionManager::executionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MissionExecutionManager::missionStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MissionExecutionManager::missionStartFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MissionExecutionManager::missionFinished(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
