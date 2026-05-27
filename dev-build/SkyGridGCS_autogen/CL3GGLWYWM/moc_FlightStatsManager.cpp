/****************************************************************************
** Meta object code from reading C++ file 'FlightStatsManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/flight/FlightStatsManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FlightStatsManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FlightStatsManager_t {
    const uint offsetsAndSize[36];
    char stringdata0[227];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_FlightStatsManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_FlightStatsManager_t qt_meta_stringdata_FlightStatsManager = {
    {
QT_MOC_LITERAL(0, 18), // "FlightStatsManager"
QT_MOC_LITERAL(19, 12), // "statsChanged"
QT_MOC_LITERAL(32, 0), // ""
QT_MOC_LITERAL(33, 12), // "startSession"
QT_MOC_LITERAL(46, 10), // "endSession"
QT_MOC_LITERAL(57, 5), // "reset"
QT_MOC_LITERAL(63, 6), // "active"
QT_MOC_LITERAL(70, 15), // "flightSessionId"
QT_MOC_LITERAL(86, 9), // "startedAt"
QT_MOC_LITERAL(96, 21), // "flightDurationSeconds"
QT_MOC_LITERAL(118, 14), // "flightTimeText"
QT_MOC_LITERAL(133, 14), // "distanceMeters"
QT_MOC_LITERAL(148, 10), // "distanceKm"
QT_MOC_LITERAL(159, 11), // "maxAltitude"
QT_MOC_LITERAL(171, 19), // "batteryStartPercent"
QT_MOC_LITERAL(191, 17), // "batteryEndPercent"
QT_MOC_LITERAL(209, 10), // "flightPath"
QT_MOC_LITERAL(220, 6) // "status"

    },
    "FlightStatsManager\0statsChanged\0\0"
    "startSession\0endSession\0reset\0active\0"
    "flightSessionId\0startedAt\0"
    "flightDurationSeconds\0flightTimeText\0"
    "distanceMeters\0distanceKm\0maxAltitude\0"
    "batteryStartPercent\0batteryEndPercent\0"
    "flightPath\0status"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FlightStatsManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
      12,   42, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x06,   13 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   39,    2, 0x02,   14 /* Public */,
       4,    0,   40,    2, 0x02,   15 /* Public */,
       5,    0,   41,    2, 0x02,   16 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       6, QMetaType::Bool, 0x00015001, uint(0), 0,
       7, QMetaType::QString, 0x00015001, uint(0), 0,
       8, QMetaType::QString, 0x00015001, uint(0), 0,
       9, QMetaType::Int, 0x00015001, uint(0), 0,
      10, QMetaType::QString, 0x00015001, uint(0), 0,
      11, QMetaType::Double, 0x00015001, uint(0), 0,
      12, QMetaType::Double, 0x00015001, uint(0), 0,
      13, QMetaType::Double, 0x00015001, uint(0), 0,
      14, QMetaType::Double, 0x00015001, uint(0), 0,
      15, QMetaType::Double, 0x00015001, uint(0), 0,
      16, QMetaType::QVariantList, 0x00015001, uint(0), 0,
      17, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

void FlightStatsManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FlightStatsManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->statsChanged(); break;
        case 1: _t->startSession(); break;
        case 2: _t->endSession(); break;
        case 3: _t->reset(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FlightStatsManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FlightStatsManager::statsChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<FlightStatsManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->active(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->flightSessionId(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->startedAt(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->flightDurationSeconds(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->flightTimeText(); break;
        case 5: *reinterpret_cast< double*>(_v) = _t->distanceMeters(); break;
        case 6: *reinterpret_cast< double*>(_v) = _t->distanceKm(); break;
        case 7: *reinterpret_cast< double*>(_v) = _t->maxAltitude(); break;
        case 8: *reinterpret_cast< double*>(_v) = _t->batteryStartPercent(); break;
        case 9: *reinterpret_cast< double*>(_v) = _t->batteryEndPercent(); break;
        case 10: *reinterpret_cast< QVariantList*>(_v) = _t->flightPath(); break;
        case 11: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

const QMetaObject FlightStatsManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FlightStatsManager.offsetsAndSize,
    qt_meta_data_FlightStatsManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_FlightStatsManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<FlightStatsManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

>,
    nullptr
} };


const QMetaObject *FlightStatsManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FlightStatsManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FlightStatsManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FlightStatsManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void FlightStatsManager::statsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
