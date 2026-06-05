/****************************************************************************
** Meta object code from reading C++ file 'HomePositionManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/vehicle/HomePositionManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HomePositionManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HomePositionManager_t {
    const uint offsetsAndSize[28];
    char stringdata0[171];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_HomePositionManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_HomePositionManager_t qt_meta_stringdata_HomePositionManager = {
    {
QT_MOC_LITERAL(0, 19), // "HomePositionManager"
QT_MOC_LITERAL(20, 11), // "homeChanged"
QT_MOC_LITERAL(32, 0), // ""
QT_MOC_LITERAL(33, 7), // "setHome"
QT_MOC_LITERAL(41, 8), // "latitude"
QT_MOC_LITERAL(50, 9), // "longitude"
QT_MOC_LITERAL(60, 6), // "source"
QT_MOC_LITERAL(67, 9), // "clearHome"
QT_MOC_LITERAL(77, 28), // "updateHomeToCurrentTelemetry"
QT_MOC_LITERAL(106, 22), // "distanceFromHomeMeters"
QT_MOC_LITERAL(129, 7), // "hasHome"
QT_MOC_LITERAL(137, 12), // "homeLatitude"
QT_MOC_LITERAL(150, 13), // "homeLongitude"
QT_MOC_LITERAL(164, 6) // "locked"

    },
    "HomePositionManager\0homeChanged\0\0"
    "setHome\0latitude\0longitude\0source\0"
    "clearHome\0updateHomeToCurrentTelemetry\0"
    "distanceFromHomeMeters\0hasHome\0"
    "homeLatitude\0homeLongitude\0locked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HomePositionManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       5,   79, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x06,    6 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    3,   57,    2, 0x02,    7 /* Public */,
       3,    2,   64,    2, 0x22,   11 /* Public | MethodCloned */,
       7,    0,   69,    2, 0x02,   14 /* Public */,
       8,    1,   70,    2, 0x02,   15 /* Public */,
       8,    0,   73,    2, 0x22,   17 /* Public | MethodCloned */,
       9,    2,   74,    2, 0x102,   18 /* Public | MethodIsConst  */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::QString,    4,    5,    6,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    4,    5,
    QMetaType::Void,
    QMetaType::Bool, QMetaType::QString,    6,
    QMetaType::Bool,
    QMetaType::Double, QMetaType::Double, QMetaType::Double,    4,    5,

 // properties: name, type, flags
      10, QMetaType::Bool, 0x00015001, uint(0), 0,
      11, QMetaType::Double, 0x00015001, uint(0), 0,
      12, QMetaType::Double, 0x00015001, uint(0), 0,
       6, QMetaType::QString, 0x00015001, uint(0), 0,
      13, QMetaType::Bool, 0x00015103, uint(0), 0,

       0        // eod
};

void HomePositionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HomePositionManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->homeChanged(); break;
        case 1: _t->setHome((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 2: _t->setHome((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 3: _t->clearHome(); break;
        case 4: { bool _r = _t->updateHomeToCurrentTelemetry((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 5: { bool _r = _t->updateHomeToCurrentTelemetry();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 6: { double _r = _t->distanceFromHomeMeters((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HomePositionManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HomePositionManager::homeChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<HomePositionManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->hasHome(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->homeLatitude(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->homeLongitude(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->source(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->locked(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<HomePositionManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 4: _t->setLocked(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject HomePositionManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_HomePositionManager.offsetsAndSize,
    qt_meta_data_HomePositionManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_HomePositionManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<HomePositionManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>

>,
    nullptr
} };


const QMetaObject *HomePositionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HomePositionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HomePositionManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HomePositionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
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
void HomePositionManager::homeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
