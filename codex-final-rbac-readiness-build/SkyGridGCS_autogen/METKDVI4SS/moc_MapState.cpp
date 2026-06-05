/****************************************************************************
** Meta object code from reading C++ file 'MapState.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/controllers/MapState.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MapState.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MapState_t {
    const uint offsetsAndSize[38];
    char stringdata0[236];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MapState_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MapState_t qt_meta_stringdata_MapState = {
    {
QT_MOC_LITERAL(0, 8), // "MapState"
QT_MOC_LITERAL(9, 13), // "centerChanged"
QT_MOC_LITERAL(23, 0), // ""
QT_MOC_LITERAL(24, 11), // "zoomChanged"
QT_MOC_LITERAL(36, 12), // "layerChanged"
QT_MOC_LITERAL(49, 21), // "followAircraftChanged"
QT_MOC_LITERAL(71, 14), // "overlayChanged"
QT_MOC_LITERAL(86, 6), // "zoomIn"
QT_MOC_LITERAL(93, 7), // "zoomOut"
QT_MOC_LITERAL(101, 14), // "setOverlayMode"
QT_MOC_LITERAL(116, 4), // "mode"
QT_MOC_LITERAL(121, 14), // "refreshOverlay"
QT_MOC_LITERAL(136, 14), // "centerLatitude"
QT_MOC_LITERAL(151, 15), // "centerLongitude"
QT_MOC_LITERAL(167, 9), // "zoomLevel"
QT_MOC_LITERAL(177, 9), // "layerMode"
QT_MOC_LITERAL(187, 14), // "followAircraft"
QT_MOC_LITERAL(202, 17), // "activeOverlayMode"
QT_MOC_LITERAL(220, 15) // "overlayRevision"

    },
    "MapState\0centerChanged\0\0zoomChanged\0"
    "layerChanged\0followAircraftChanged\0"
    "overlayChanged\0zoomIn\0zoomOut\0"
    "setOverlayMode\0mode\0refreshOverlay\0"
    "centerLatitude\0centerLongitude\0zoomLevel\0"
    "layerMode\0followAircraft\0activeOverlayMode\0"
    "overlayRevision"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MapState[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       7,   79, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   68,    2, 0x06,    8 /* Public */,
       3,    0,   69,    2, 0x06,    9 /* Public */,
       4,    0,   70,    2, 0x06,   10 /* Public */,
       5,    0,   71,    2, 0x06,   11 /* Public */,
       6,    0,   72,    2, 0x06,   12 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       7,    0,   73,    2, 0x02,   13 /* Public */,
       8,    0,   74,    2, 0x02,   14 /* Public */,
       9,    1,   75,    2, 0x02,   15 /* Public */,
      11,    0,   78,    2, 0x02,   17 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,

 // properties: name, type, flags
      12, QMetaType::Double, 0x00015103, uint(0), 0,
      13, QMetaType::Double, 0x00015103, uint(0), 0,
      14, QMetaType::Double, 0x00015103, uint(1), 0,
      15, QMetaType::QString, 0x00015103, uint(2), 0,
      16, QMetaType::Bool, 0x00015103, uint(3), 0,
      17, QMetaType::QString, 0x00015001, uint(4), 0,
      18, QMetaType::Int, 0x00015001, uint(4), 0,

       0        // eod
};

void MapState::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MapState *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->centerChanged(); break;
        case 1: _t->zoomChanged(); break;
        case 2: _t->layerChanged(); break;
        case 3: _t->followAircraftChanged(); break;
        case 4: _t->overlayChanged(); break;
        case 5: _t->zoomIn(); break;
        case 6: _t->zoomOut(); break;
        case 7: _t->setOverlayMode((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->refreshOverlay(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MapState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapState::centerChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MapState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapState::zoomChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MapState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapState::layerChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MapState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapState::followAircraftChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MapState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapState::overlayChanged)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MapState *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->centerLatitude(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->centerLongitude(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->zoomLevel(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->layerMode(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->followAircraft(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->activeOverlayMode(); break;
        case 6: *reinterpret_cast< int*>(_v) = _t->overlayRevision(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<MapState *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setCenterLatitude(*reinterpret_cast< double*>(_v)); break;
        case 1: _t->setCenterLongitude(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setZoomLevel(*reinterpret_cast< double*>(_v)); break;
        case 3: _t->setLayerMode(*reinterpret_cast< QString*>(_v)); break;
        case 4: _t->setFollowAircraft(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject MapState::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MapState.offsetsAndSize,
    qt_meta_data_MapState,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MapState_t
, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<double, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<MapState, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

>,
    nullptr
} };


const QMetaObject *MapState::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MapState::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MapState.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MapState::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MapState::centerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MapState::zoomChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MapState::layerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void MapState::followAircraftChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MapState::overlayChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
