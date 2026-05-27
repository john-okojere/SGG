/****************************************************************************
** Meta object code from reading C++ file 'MavsdkVehicleManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/vehicle/MavsdkVehicleManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MavsdkVehicleManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MavsdkVehicleManager_t {
    const uint offsetsAndSize[36];
    char stringdata0[202];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MavsdkVehicleManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MavsdkVehicleManager_t qt_meta_stringdata_MavsdkVehicleManager = {
    {
QT_MOC_LITERAL(0, 20), // "MavsdkVehicleManager"
QT_MOC_LITERAL(21, 14), // "vehicleChanged"
QT_MOC_LITERAL(36, 0), // ""
QT_MOC_LITERAL(37, 11), // "systemReady"
QT_MOC_LITERAL(49, 14), // "startDiscovery"
QT_MOC_LITERAL(64, 13), // "stopDiscovery"
QT_MOC_LITERAL(78, 12), // "connectRetry"
QT_MOC_LITERAL(91, 14), // "refreshSystems"
QT_MOC_LITERAL(106, 9), // "connected"
QT_MOC_LITERAL(116, 15), // "discoveryActive"
QT_MOC_LITERAL(132, 6), // "status"
QT_MOC_LITERAL(139, 13), // "connectionUrl"
QT_MOC_LITERAL(153, 8), // "systemId"
QT_MOC_LITERAL(162, 9), // "autopilot"
QT_MOC_LITERAL(172, 5), // "armed"
QT_MOC_LITERAL(178, 5), // "inAir"
QT_MOC_LITERAL(184, 10), // "flightMode"
QT_MOC_LITERAL(195, 6) // "health"

    },
    "MavsdkVehicleManager\0vehicleChanged\0"
    "\0systemReady\0startDiscovery\0stopDiscovery\0"
    "connectRetry\0refreshSystems\0connected\0"
    "discoveryActive\0status\0connectionUrl\0"
    "systemId\0autopilot\0armed\0inAir\0"
    "flightMode\0health"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MavsdkVehicleManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
      10,   56, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x06,   11 /* Public */,
       3,    0,   51,    2, 0x06,   12 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   52,    2, 0x02,   13 /* Public */,
       5,    0,   53,    2, 0x02,   14 /* Public */,
       6,    0,   54,    2, 0x02,   15 /* Public */,
       7,    0,   55,    2, 0x02,   16 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       8, QMetaType::Bool, 0x00015001, uint(0), 0,
       9, QMetaType::Bool, 0x00015001, uint(0), 0,
      10, QMetaType::QString, 0x00015001, uint(0), 0,
      11, QMetaType::QString, 0x00015001, uint(0), 0,
      12, QMetaType::QString, 0x00015001, uint(0), 0,
      13, QMetaType::QString, 0x00015001, uint(0), 0,
      14, QMetaType::Bool, 0x00015001, uint(0), 0,
      15, QMetaType::Bool, 0x00015001, uint(0), 0,
      16, QMetaType::QString, 0x00015001, uint(0), 0,
      17, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

void MavsdkVehicleManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MavsdkVehicleManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->vehicleChanged(); break;
        case 1: _t->systemReady(); break;
        case 2: _t->startDiscovery(); break;
        case 3: _t->stopDiscovery(); break;
        case 4: _t->connectRetry(); break;
        case 5: _t->refreshSystems(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MavsdkVehicleManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MavsdkVehicleManager::vehicleChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MavsdkVehicleManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MavsdkVehicleManager::systemReady)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MavsdkVehicleManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->connected(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->discoveryActive(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->connectionUrl(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->systemId(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->autopilot(); break;
        case 6: *reinterpret_cast< bool*>(_v) = _t->armed(); break;
        case 7: *reinterpret_cast< bool*>(_v) = _t->inAir(); break;
        case 8: *reinterpret_cast< QString*>(_v) = _t->flightMode(); break;
        case 9: *reinterpret_cast< QString*>(_v) = _t->health(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

const QMetaObject MavsdkVehicleManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MavsdkVehicleManager.offsetsAndSize,
    qt_meta_data_MavsdkVehicleManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MavsdkVehicleManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<MavsdkVehicleManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

>,
    nullptr
} };


const QMetaObject *MavsdkVehicleManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MavsdkVehicleManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MavsdkVehicleManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MavsdkVehicleManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
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
void MavsdkVehicleManager::vehicleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MavsdkVehicleManager::systemReady()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
