/****************************************************************************
** Meta object code from reading C++ file 'OperatorStateManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/profile/OperatorStateManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OperatorStateManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OperatorStateManager_t {
    const uint offsetsAndSize[22];
    char stringdata0[157];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_OperatorStateManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_OperatorStateManager_t qt_meta_stringdata_OperatorStateManager = {
    {
QT_MOC_LITERAL(0, 20), // "OperatorStateManager"
QT_MOC_LITERAL(21, 20), // "operatorStateChanged"
QT_MOC_LITERAL(42, 0), // ""
QT_MOC_LITERAL(43, 14), // "applyBootstrap"
QT_MOC_LITERAL(58, 9), // "bootstrap"
QT_MOC_LITERAL(68, 17), // "setActiveAircraft"
QT_MOC_LITERAL(86, 8), // "aircraft"
QT_MOC_LITERAL(95, 18), // "activeAircraftName"
QT_MOC_LITERAL(114, 16), // "activeAircraftId"
QT_MOC_LITERAL(131, 13), // "workspaceMode"
QT_MOC_LITERAL(145, 11) // "layoutState"

    },
    "OperatorStateManager\0operatorStateChanged\0"
    "\0applyBootstrap\0bootstrap\0setActiveAircraft\0"
    "aircraft\0activeAircraftName\0"
    "activeAircraftId\0workspaceMode\0"
    "layoutState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OperatorStateManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       4,   39, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x06,    5 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,   33,    2, 0x02,    6 /* Public */,
       5,    1,   36,    2, 0x02,    8 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QVariantMap,    4,
    QMetaType::Void, QMetaType::QVariantMap,    6,

 // properties: name, type, flags
       7, QMetaType::QString, 0x00015001, uint(0), 0,
       8, QMetaType::QString, 0x00015001, uint(0), 0,
       9, QMetaType::QString, 0x00015001, uint(0), 0,
      10, QMetaType::QVariantMap, 0x00015001, uint(0), 0,

       0        // eod
};

void OperatorStateManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OperatorStateManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->operatorStateChanged(); break;
        case 1: _t->applyBootstrap((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 2: _t->setActiveAircraft((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OperatorStateManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OperatorStateManager::operatorStateChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<OperatorStateManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->activeAircraftName(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->activeAircraftId(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->workspaceMode(); break;
        case 3: *reinterpret_cast< QVariantMap*>(_v) = _t->layoutState(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject OperatorStateManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_OperatorStateManager.offsetsAndSize,
    qt_meta_data_OperatorStateManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_OperatorStateManager_t
, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<OperatorStateManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>

>,
    nullptr
} };


const QMetaObject *OperatorStateManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OperatorStateManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OperatorStateManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int OperatorStateManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void OperatorStateManager::operatorStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
