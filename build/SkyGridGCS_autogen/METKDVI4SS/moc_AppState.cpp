/****************************************************************************
** Meta object code from reading C++ file 'AppState.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/controllers/AppState.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppState.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AppState_t {
    const uint offsetsAndSize[50];
    char stringdata0[383];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_AppState_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_AppState_t qt_meta_stringdata_AppState = {
    {
QT_MOC_LITERAL(0, 8), // "AppState"
QT_MOC_LITERAL(9, 17), // "navigationChanged"
QT_MOC_LITERAL(27, 0), // ""
QT_MOC_LITERAL(28, 14), // "missionChanged"
QT_MOC_LITERAL(43, 22), // "operationalModeChanged"
QT_MOC_LITERAL(66, 11), // "toolChanged"
QT_MOC_LITERAL(78, 23), // "selectedWaypointChanged"
QT_MOC_LITERAL(102, 12), // "panelChanged"
QT_MOC_LITERAL(115, 14), // "missionStarted"
QT_MOC_LITERAL(130, 11), // "missionType"
QT_MOC_LITERAL(142, 6), // "goHome"
QT_MOC_LITERAL(149, 19), // "openMissionSelector"
QT_MOC_LITERAL(169, 20), // "closeMissionSelector"
QT_MOC_LITERAL(190, 12), // "startMission"
QT_MOC_LITERAL(203, 19), // "openExistingMission"
QT_MOC_LITERAL(223, 9), // "missionId"
QT_MOC_LITERAL(233, 14), // "startPilotMode"
QT_MOC_LITERAL(248, 12), // "missionTitle"
QT_MOC_LITERAL(261, 13), // "currentScreen"
QT_MOC_LITERAL(275, 18), // "currentMissionType"
QT_MOC_LITERAL(294, 15), // "operationalMode"
QT_MOC_LITERAL(310, 17), // "selectedMissionId"
QT_MOC_LITERAL(328, 12), // "selectedTool"
QT_MOC_LITERAL(341, 21), // "selectedWaypointIndex"
QT_MOC_LITERAL(363, 19) // "rightPanelCollapsed"

    },
    "AppState\0navigationChanged\0\0missionChanged\0"
    "operationalModeChanged\0toolChanged\0"
    "selectedWaypointChanged\0panelChanged\0"
    "missionStarted\0missionType\0goHome\0"
    "openMissionSelector\0closeMissionSelector\0"
    "startMission\0openExistingMission\0"
    "missionId\0startPilotMode\0missionTitle\0"
    "currentScreen\0currentMissionType\0"
    "operationalMode\0selectedMissionId\0"
    "selectedTool\0selectedWaypointIndex\0"
    "rightPanelCollapsed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AppState[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       7,  120, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   98,    2, 0x06,    8 /* Public */,
       3,    0,   99,    2, 0x06,    9 /* Public */,
       4,    0,  100,    2, 0x06,   10 /* Public */,
       5,    0,  101,    2, 0x06,   11 /* Public */,
       6,    0,  102,    2, 0x06,   12 /* Public */,
       7,    0,  103,    2, 0x06,   13 /* Public */,
       8,    1,  104,    2, 0x06,   14 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      10,    0,  107,    2, 0x02,   16 /* Public */,
      11,    0,  108,    2, 0x02,   17 /* Public */,
      12,    0,  109,    2, 0x02,   18 /* Public */,
      13,    1,  110,    2, 0x02,   19 /* Public */,
      14,    2,  113,    2, 0x02,   21 /* Public */,
      16,    0,  118,    2, 0x02,   24 /* Public */,
      17,    0,  119,    2, 0x102,   25 /* Public | MethodIsConst  */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   15,
    QMetaType::Void,
    QMetaType::QString,

 // properties: name, type, flags
      18, QMetaType::QString, 0x00015001, uint(0), 0,
      19, QMetaType::QString, 0x00015001, uint(1), 0,
      20, QMetaType::QString, 0x00015103, uint(2), 0,
      21, QMetaType::QString, 0x00015001, uint(1), 0,
      22, QMetaType::QString, 0x00015103, uint(3), 0,
      23, QMetaType::Int, 0x00015103, uint(4), 0,
      24, QMetaType::Bool, 0x00015103, uint(5), 0,

       0        // eod
};

void AppState::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AppState *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->navigationChanged(); break;
        case 1: _t->missionChanged(); break;
        case 2: _t->operationalModeChanged(); break;
        case 3: _t->toolChanged(); break;
        case 4: _t->selectedWaypointChanged(); break;
        case 5: _t->panelChanged(); break;
        case 6: _t->missionStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->goHome(); break;
        case 8: _t->openMissionSelector(); break;
        case 9: _t->closeMissionSelector(); break;
        case 10: _t->startMission((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->openExistingMission((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 12: _t->startPilotMode(); break;
        case 13: { QString _r = _t->missionTitle();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AppState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppState::navigationChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AppState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppState::missionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AppState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppState::operationalModeChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (AppState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppState::toolChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (AppState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppState::selectedWaypointChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (AppState::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppState::panelChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (AppState::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppState::missionStarted)) {
                *result = 6;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<AppState *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->currentScreen(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->currentMissionType(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->operationalMode(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->selectedMissionId(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->selectedTool(); break;
        case 5: *reinterpret_cast< int*>(_v) = _t->selectedWaypointIndex(); break;
        case 6: *reinterpret_cast< bool*>(_v) = _t->rightPanelCollapsed(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<AppState *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 2: _t->setOperationalMode(*reinterpret_cast< QString*>(_v)); break;
        case 4: _t->setSelectedTool(*reinterpret_cast< QString*>(_v)); break;
        case 5: _t->setSelectedWaypointIndex(*reinterpret_cast< int*>(_v)); break;
        case 6: _t->setRightPanelCollapsed(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject AppState::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AppState.offsetsAndSize,
    qt_meta_data_AppState,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_AppState_t
, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<AppState, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>

>,
    nullptr
} };


const QMetaObject *AppState::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppState::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AppState.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AppState::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
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
void AppState::navigationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AppState::missionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AppState::operationalModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AppState::toolChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AppState::selectedWaypointChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void AppState::panelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void AppState::missionStarted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
