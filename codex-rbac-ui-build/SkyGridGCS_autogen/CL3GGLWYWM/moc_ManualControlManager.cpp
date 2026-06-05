/****************************************************************************
** Meta object code from reading C++ file 'ManualControlManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/flight/ManualControlManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ManualControlManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ManualControlManager_t {
    const uint offsetsAndSize[36];
    char stringdata0[198];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ManualControlManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ManualControlManager_t qt_meta_stringdata_ManualControlManager = {
    {
QT_MOC_LITERAL(0, 20), // "ManualControlManager"
QT_MOC_LITERAL(21, 20), // "manualControlChanged"
QT_MOC_LITERAL(42, 0), // ""
QT_MOC_LITERAL(43, 18), // "startManualControl"
QT_MOC_LITERAL(62, 17), // "stopManualControl"
QT_MOC_LITERAL(80, 8), // "setInput"
QT_MOC_LITERAL(89, 7), // "forward"
QT_MOC_LITERAL(97, 7), // "lateral"
QT_MOC_LITERAL(105, 8), // "vertical"
QT_MOC_LITERAL(114, 3), // "yaw"
QT_MOC_LITERAL(118, 7), // "neutral"
QT_MOC_LITERAL(126, 14), // "goToCoordinate"
QT_MOC_LITERAL(141, 8), // "latitude"
QT_MOC_LITERAL(150, 9), // "longitude"
QT_MOC_LITERAL(160, 14), // "altitudeMeters"
QT_MOC_LITERAL(175, 8), // "speedMps"
QT_MOC_LITERAL(184, 6), // "active"
QT_MOC_LITERAL(191, 6) // "status"

    },
    "ManualControlManager\0manualControlChanged\0"
    "\0startManualControl\0stopManualControl\0"
    "setInput\0forward\0lateral\0vertical\0yaw\0"
    "neutral\0goToCoordinate\0latitude\0"
    "longitude\0altitudeMeters\0speedMps\0"
    "active\0status"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ManualControlManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       6,   72, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x06,    7 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   51,    2, 0x02,    8 /* Public */,
       4,    0,   52,    2, 0x02,    9 /* Public */,
       5,    4,   53,    2, 0x02,   10 /* Public */,
      10,    0,   62,    2, 0x02,   15 /* Public */,
      11,    4,   63,    2, 0x02,   16 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double,    6,    7,    8,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double,   12,   13,   14,   15,

 // properties: name, type, flags
      16, QMetaType::Bool, 0x00015001, uint(0), 0,
      17, QMetaType::QString, 0x00015001, uint(0), 0,
       6, QMetaType::Double, 0x00015001, uint(0), 0,
       7, QMetaType::Double, 0x00015001, uint(0), 0,
       8, QMetaType::Double, 0x00015001, uint(0), 0,
       9, QMetaType::Double, 0x00015001, uint(0), 0,

       0        // eod
};

void ManualControlManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ManualControlManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->manualControlChanged(); break;
        case 1: _t->startManualControl(); break;
        case 2: _t->stopManualControl(); break;
        case 3: _t->setInput((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[4]))); break;
        case 4: _t->neutral(); break;
        case 5: _t->goToCoordinate((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ManualControlManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ManualControlManager::manualControlChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ManualControlManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->active(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->forward(); break;
        case 3: *reinterpret_cast< double*>(_v) = _t->lateral(); break;
        case 4: *reinterpret_cast< double*>(_v) = _t->vertical(); break;
        case 5: *reinterpret_cast< double*>(_v) = _t->yaw(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject ManualControlManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ManualControlManager.offsetsAndSize,
    qt_meta_data_ManualControlManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ManualControlManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<ManualControlManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>

>,
    nullptr
} };


const QMetaObject *ManualControlManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ManualControlManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ManualControlManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ManualControlManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ManualControlManager::manualControlChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
