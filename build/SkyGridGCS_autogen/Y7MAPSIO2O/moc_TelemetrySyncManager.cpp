/****************************************************************************
** Meta object code from reading C++ file 'TelemetrySyncManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/sync/TelemetrySyncManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TelemetrySyncManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TelemetrySyncManager_t {
    const uint offsetsAndSize[24];
    char stringdata0[130];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_TelemetrySyncManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_TelemetrySyncManager_t qt_meta_stringdata_TelemetrySyncManager = {
    {
QT_MOC_LITERAL(0, 20), // "TelemetrySyncManager"
QT_MOC_LITERAL(21, 20), // "telemetrySyncChanged"
QT_MOC_LITERAL(42, 0), // ""
QT_MOC_LITERAL(43, 5), // "start"
QT_MOC_LITERAL(49, 4), // "stop"
QT_MOC_LITERAL(54, 9), // "uploadNow"
QT_MOC_LITERAL(64, 9), // "uploading"
QT_MOC_LITERAL(74, 10), // "gazeboMode"
QT_MOC_LITERAL(85, 6), // "status"
QT_MOC_LITERAL(92, 9), // "syncState"
QT_MOC_LITERAL(102, 9), // "lastAckAt"
QT_MOC_LITERAL(112, 17) // "pendingQueueCount"

    },
    "TelemetrySyncManager\0telemetrySyncChanged\0"
    "\0start\0stop\0uploadNow\0uploading\0"
    "gazeboMode\0status\0syncState\0lastAckAt\0"
    "pendingQueueCount"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TelemetrySyncManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       6,   42, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x06,    7 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   39,    2, 0x02,    8 /* Public */,
       4,    0,   40,    2, 0x02,    9 /* Public */,
       5,    0,   41,    2, 0x02,   10 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       6, QMetaType::Bool, 0x00015001, uint(0), 0,
       7, QMetaType::Bool, 0x00015103, uint(0), 0,
       8, QMetaType::QString, 0x00015001, uint(0), 0,
       9, QMetaType::QString, 0x00015001, uint(0), 0,
      10, QMetaType::QString, 0x00015001, uint(0), 0,
      11, QMetaType::Int, 0x00015001, uint(0), 0,

       0        // eod
};

void TelemetrySyncManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TelemetrySyncManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->telemetrySyncChanged(); break;
        case 1: _t->start(); break;
        case 2: _t->stop(); break;
        case 3: _t->uploadNow(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TelemetrySyncManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TelemetrySyncManager::telemetrySyncChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<TelemetrySyncManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->uploading(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->gazeboMode(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->syncState(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->lastAckAt(); break;
        case 5: *reinterpret_cast< int*>(_v) = _t->pendingQueueCount(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<TelemetrySyncManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setGazeboMode(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

const QMetaObject TelemetrySyncManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TelemetrySyncManager.offsetsAndSize,
    qt_meta_data_TelemetrySyncManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_TelemetrySyncManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<TelemetrySyncManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

>,
    nullptr
} };


const QMetaObject *TelemetrySyncManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TelemetrySyncManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TelemetrySyncManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TelemetrySyncManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void TelemetrySyncManager::telemetrySyncChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
