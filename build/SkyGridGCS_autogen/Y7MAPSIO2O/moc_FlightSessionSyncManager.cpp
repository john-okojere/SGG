/****************************************************************************
** Meta object code from reading C++ file 'FlightSessionSyncManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/sync/FlightSessionSyncManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FlightSessionSyncManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FlightSessionSyncManager_t {
    const uint offsetsAndSize[40];
    char stringdata0[247];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_FlightSessionSyncManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_FlightSessionSyncManager_t qt_meta_stringdata_FlightSessionSyncManager = {
    {
QT_MOC_LITERAL(0, 24), // "FlightSessionSyncManager"
QT_MOC_LITERAL(25, 14), // "sessionChanged"
QT_MOC_LITERAL(40, 0), // ""
QT_MOC_LITERAL(41, 14), // "applyBootstrap"
QT_MOC_LITERAL(56, 9), // "bootstrap"
QT_MOC_LITERAL(66, 17), // "beginPilotSession"
QT_MOC_LITERAL(84, 15), // "clientSessionId"
QT_MOC_LITERAL(100, 19), // "beginMissionSession"
QT_MOC_LITERAL(120, 9), // "missionId"
QT_MOC_LITERAL(130, 16), // "endActiveSession"
QT_MOC_LITERAL(147, 9), // "endStatus"
QT_MOC_LITERAL(157, 6), // "reason"
QT_MOC_LITERAL(164, 17), // "recordPilotAction"
QT_MOC_LITERAL(182, 10), // "actionType"
QT_MOC_LITERAL(193, 10), // "actionData"
QT_MOC_LITERAL(204, 7), // "message"
QT_MOC_LITERAL(212, 6), // "active"
QT_MOC_LITERAL(219, 15), // "serverSessionId"
QT_MOC_LITERAL(235, 4), // "mode"
QT_MOC_LITERAL(240, 6) // "status"

    },
    "FlightSessionSyncManager\0sessionChanged\0"
    "\0applyBootstrap\0bootstrap\0beginPilotSession\0"
    "clientSessionId\0beginMissionSession\0"
    "missionId\0endActiveSession\0endStatus\0"
    "reason\0recordPilotAction\0actionType\0"
    "actionData\0message\0active\0serverSessionId\0"
    "mode\0status"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FlightSessionSyncManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       5,  110, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    6 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,   75,    2, 0x02,    7 /* Public */,
       5,    1,   78,    2, 0x02,    9 /* Public */,
       7,    2,   81,    2, 0x02,   11 /* Public */,
       9,    2,   86,    2, 0x02,   14 /* Public */,
       9,    1,   91,    2, 0x22,   17 /* Public | MethodCloned */,
       9,    0,   94,    2, 0x22,   19 /* Public | MethodCloned */,
      12,    3,   95,    2, 0x02,   20 /* Public */,
      12,    2,  102,    2, 0x22,   24 /* Public | MethodCloned */,
      12,    1,  107,    2, 0x22,   27 /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QVariantMap,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   10,   11,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QJsonObject, QMetaType::QString,   13,   14,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::QJsonObject,   13,   14,
    QMetaType::Void, QMetaType::QString,   13,

 // properties: name, type, flags
      16, QMetaType::Bool, 0x00015001, uint(0), 0,
      17, QMetaType::QString, 0x00015001, uint(0), 0,
       6, QMetaType::QString, 0x00015001, uint(0), 0,
      18, QMetaType::QString, 0x00015001, uint(0), 0,
      19, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

void FlightSessionSyncManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FlightSessionSyncManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sessionChanged(); break;
        case 1: _t->applyBootstrap((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 2: _t->beginPilotSession((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->beginMissionSession((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->endActiveSession((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->endActiveSession((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->endActiveSession(); break;
        case 7: _t->recordPilotAction((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 8: _t->recordPilotAction((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 9: _t->recordPilotAction((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FlightSessionSyncManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FlightSessionSyncManager::sessionChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<FlightSessionSyncManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->active(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->serverSessionId(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->clientSessionId(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->mode(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject FlightSessionSyncManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FlightSessionSyncManager.offsetsAndSize,
    qt_meta_data_FlightSessionSyncManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_FlightSessionSyncManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<FlightSessionSyncManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>

>,
    nullptr
} };


const QMetaObject *FlightSessionSyncManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FlightSessionSyncManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FlightSessionSyncManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FlightSessionSyncManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void FlightSessionSyncManager::sessionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
