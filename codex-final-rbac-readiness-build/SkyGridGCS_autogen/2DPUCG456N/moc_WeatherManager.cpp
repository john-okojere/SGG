/****************************************************************************
** Meta object code from reading C++ file 'WeatherManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/telemetry/WeatherManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WeatherManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WeatherManager_t {
    const uint offsetsAndSize[34];
    char stringdata0[208];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_WeatherManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_WeatherManager_t qt_meta_stringdata_WeatherManager = {
    {
QT_MOC_LITERAL(0, 14), // "WeatherManager"
QT_MOC_LITERAL(15, 14), // "weatherChanged"
QT_MOC_LITERAL(30, 0), // ""
QT_MOC_LITERAL(31, 15), // "markUnavailable"
QT_MOC_LITERAL(47, 6), // "reason"
QT_MOC_LITERAL(54, 12), // "applyWeather"
QT_MOC_LITERAL(67, 7), // "weather"
QT_MOC_LITERAL(75, 9), // "available"
QT_MOC_LITERAL(85, 11), // "sourceState"
QT_MOC_LITERAL(97, 6), // "status"
QT_MOC_LITERAL(104, 12), // "windSpeedMps"
QT_MOC_LITERAL(117, 16), // "windDirectionDeg"
QT_MOC_LITERAL(134, 11), // "windGustMps"
QT_MOC_LITERAL(146, 12), // "visibilityKm"
QT_MOC_LITERAL(159, 24), // "precipitationProbability"
QT_MOC_LITERAL(184, 10), // "cloudCover"
QT_MOC_LITERAL(195, 12) // "temperatureC"

    },
    "WeatherManager\0weatherChanged\0\0"
    "markUnavailable\0reason\0applyWeather\0"
    "weather\0available\0sourceState\0status\0"
    "windSpeedMps\0windDirectionDeg\0windGustMps\0"
    "visibilityKm\0precipitationProbability\0"
    "cloudCover\0temperatureC"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WeatherManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
      10,   46, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x06,   11 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,   39,    2, 0x02,   12 /* Public */,
       3,    0,   42,    2, 0x22,   14 /* Public | MethodCloned */,
       5,    1,   43,    2, 0x02,   15 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,    6,

 // properties: name, type, flags
       7, QMetaType::Bool, 0x00015001, uint(0), 0,
       8, QMetaType::QString, 0x00015001, uint(0), 0,
       9, QMetaType::QString, 0x00015001, uint(0), 0,
      10, QMetaType::Double, 0x00015001, uint(0), 0,
      11, QMetaType::Double, 0x00015001, uint(0), 0,
      12, QMetaType::Double, 0x00015001, uint(0), 0,
      13, QMetaType::Double, 0x00015001, uint(0), 0,
      14, QMetaType::Double, 0x00015001, uint(0), 0,
      15, QMetaType::Double, 0x00015001, uint(0), 0,
      16, QMetaType::Double, 0x00015001, uint(0), 0,

       0        // eod
};

void WeatherManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WeatherManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->weatherChanged(); break;
        case 1: _t->markUnavailable((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->markUnavailable(); break;
        case 3: _t->applyWeather((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WeatherManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeatherManager::weatherChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<WeatherManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->available(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->sourceState(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        case 3: *reinterpret_cast< double*>(_v) = _t->windSpeedMps(); break;
        case 4: *reinterpret_cast< double*>(_v) = _t->windDirectionDeg(); break;
        case 5: *reinterpret_cast< double*>(_v) = _t->windGustMps(); break;
        case 6: *reinterpret_cast< double*>(_v) = _t->visibilityKm(); break;
        case 7: *reinterpret_cast< double*>(_v) = _t->precipitationProbability(); break;
        case 8: *reinterpret_cast< double*>(_v) = _t->cloudCover(); break;
        case 9: *reinterpret_cast< double*>(_v) = _t->temperatureC(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject WeatherManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_WeatherManager.offsetsAndSize,
    qt_meta_data_WeatherManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_WeatherManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<WeatherManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>

>,
    nullptr
} };


const QMetaObject *WeatherManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WeatherManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WeatherManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WeatherManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void WeatherManager::weatherChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
