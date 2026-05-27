/****************************************************************************
** Meta object code from reading C++ file 'WindTelemetryManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/telemetry/WindTelemetryManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WindTelemetryManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WindTelemetryManager_t {
    const uint offsetsAndSize[28];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_WindTelemetryManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_WindTelemetryManager_t qt_meta_stringdata_WindTelemetryManager = {
    {
QT_MOC_LITERAL(0, 20), // "WindTelemetryManager"
QT_MOC_LITERAL(21, 11), // "windChanged"
QT_MOC_LITERAL(33, 0), // ""
QT_MOC_LITERAL(34, 5), // "clear"
QT_MOC_LITERAL(40, 18), // "setDevelopmentWind"
QT_MOC_LITERAL(59, 8), // "speedMps"
QT_MOC_LITERAL(68, 12), // "directionDeg"
QT_MOC_LITERAL(81, 7), // "gustMps"
QT_MOC_LITERAL(89, 9), // "available"
QT_MOC_LITERAL(99, 9), // "windSpeed"
QT_MOC_LITERAL(109, 13), // "windDirection"
QT_MOC_LITERAL(123, 4), // "gust"
QT_MOC_LITERAL(128, 14), // "directionLabel"
QT_MOC_LITERAL(143, 6) // "status"

    },
    "WindTelemetryManager\0windChanged\0\0"
    "clear\0setDevelopmentWind\0speedMps\0"
    "directionDeg\0gustMps\0available\0windSpeed\0"
    "windDirection\0gust\0directionLabel\0"
    "status"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WindTelemetryManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       6,   41, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x06,    7 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   33,    2, 0x02,    8 /* Public */,
       4,    3,   34,    2, 0x02,    9 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double,    5,    6,    7,

 // properties: name, type, flags
       8, QMetaType::Bool, 0x00015001, uint(0), 0,
       9, QMetaType::Double, 0x00015001, uint(0), 0,
      10, QMetaType::Double, 0x00015001, uint(0), 0,
      11, QMetaType::Double, 0x00015001, uint(0), 0,
      12, QMetaType::QString, 0x00015001, uint(0), 0,
      13, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

void WindTelemetryManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WindTelemetryManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->windChanged(); break;
        case 1: _t->clear(); break;
        case 2: _t->setDevelopmentWind((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WindTelemetryManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WindTelemetryManager::windChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<WindTelemetryManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->available(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->windSpeed(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->windDirection(); break;
        case 3: *reinterpret_cast< double*>(_v) = _t->gust(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->directionLabel(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject WindTelemetryManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_WindTelemetryManager.offsetsAndSize,
    qt_meta_data_WindTelemetryManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_WindTelemetryManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<WindTelemetryManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>

>,
    nullptr
} };


const QMetaObject *WindTelemetryManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WindTelemetryManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WindTelemetryManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WindTelemetryManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void WindTelemetryManager::windChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
