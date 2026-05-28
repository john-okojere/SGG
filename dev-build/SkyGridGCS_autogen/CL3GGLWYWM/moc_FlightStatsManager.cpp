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
    const uint offsetsAndSize[46];
    char stringdata0[281];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_FlightStatsManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_FlightStatsManager_t qt_meta_stringdata_FlightStatsManager = {
    {
QT_MOC_LITERAL(0, 18), // "FlightStatsManager"
QT_MOC_LITERAL(19, 12), // "statsChanged"
QT_MOC_LITERAL(32, 0), // ""
QT_MOC_LITERAL(33, 11), // "flightEnded"
QT_MOC_LITERAL(45, 6), // "record"
QT_MOC_LITERAL(52, 12), // "startSession"
QT_MOC_LITERAL(65, 10), // "endSession"
QT_MOC_LITERAL(76, 5), // "reset"
QT_MOC_LITERAL(82, 6), // "active"
QT_MOC_LITERAL(89, 15), // "flightSessionId"
QT_MOC_LITERAL(105, 9), // "startedAt"
QT_MOC_LITERAL(115, 21), // "flightDurationSeconds"
QT_MOC_LITERAL(137, 14), // "flightTimeText"
QT_MOC_LITERAL(152, 14), // "distanceMeters"
QT_MOC_LITERAL(167, 10), // "distanceKm"
QT_MOC_LITERAL(178, 11), // "maxAltitude"
QT_MOC_LITERAL(190, 19), // "batteryStartPercent"
QT_MOC_LITERAL(210, 17), // "batteryEndPercent"
QT_MOC_LITERAL(228, 15), // "averageSpeedMps"
QT_MOC_LITERAL(244, 7), // "endedAt"
QT_MOC_LITERAL(252, 10), // "lastRecord"
QT_MOC_LITERAL(263, 10), // "flightPath"
QT_MOC_LITERAL(274, 6) // "status"

    },
    "FlightStatsManager\0statsChanged\0\0"
    "flightEnded\0record\0startSession\0"
    "endSession\0reset\0active\0flightSessionId\0"
    "startedAt\0flightDurationSeconds\0"
    "flightTimeText\0distanceMeters\0distanceKm\0"
    "maxAltitude\0batteryStartPercent\0"
    "batteryEndPercent\0averageSpeedMps\0"
    "endedAt\0lastRecord\0flightPath\0status"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FlightStatsManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
      15,   51, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x06,   16 /* Public */,
       3,    1,   45,    2, 0x06,   17 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       5,    0,   48,    2, 0x02,   19 /* Public */,
       6,    0,   49,    2, 0x02,   20 /* Public */,
       7,    0,   50,    2, 0x02,   21 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,    4,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       8, QMetaType::Bool, 0x00015001, uint(0), 0,
       9, QMetaType::QString, 0x00015001, uint(0), 0,
      10, QMetaType::QString, 0x00015001, uint(0), 0,
      11, QMetaType::Int, 0x00015001, uint(0), 0,
      12, QMetaType::QString, 0x00015001, uint(0), 0,
      13, QMetaType::Double, 0x00015001, uint(0), 0,
      14, QMetaType::Double, 0x00015001, uint(0), 0,
      15, QMetaType::Double, 0x00015001, uint(0), 0,
      16, QMetaType::Double, 0x00015001, uint(0), 0,
      17, QMetaType::Double, 0x00015001, uint(0), 0,
      18, QMetaType::Double, 0x00015001, uint(0), 0,
      19, QMetaType::QString, 0x00015001, uint(0), 0,
      20, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
      21, QMetaType::QVariantList, 0x00015001, uint(0), 0,
      22, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

void FlightStatsManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FlightStatsManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->statsChanged(); break;
        case 1: _t->flightEnded((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 2: _t->startSession(); break;
        case 3: _t->endSession(); break;
        case 4: _t->reset(); break;
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
        {
            using _t = void (FlightStatsManager::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FlightStatsManager::flightEnded)) {
                *result = 1;
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
        case 10: *reinterpret_cast< double*>(_v) = _t->averageSpeedMps(); break;
        case 11: *reinterpret_cast< QString*>(_v) = _t->endedAt(); break;
        case 12: *reinterpret_cast< QVariantMap*>(_v) = _t->lastRecord(); break;
        case 13: *reinterpret_cast< QVariantList*>(_v) = _t->flightPath(); break;
        case 14: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject FlightStatsManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FlightStatsManager.offsetsAndSize,
    qt_meta_data_FlightStatsManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_FlightStatsManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<FlightStatsManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>

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
        _id -= 15;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void FlightStatsManager::statsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FlightStatsManager::flightEnded(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
