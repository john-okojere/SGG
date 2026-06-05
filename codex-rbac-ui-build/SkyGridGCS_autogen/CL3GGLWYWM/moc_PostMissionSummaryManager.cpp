/****************************************************************************
** Meta object code from reading C++ file 'PostMissionSummaryManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/flight/PostMissionSummaryManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PostMissionSummaryManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PostMissionSummaryManager_t {
    const uint offsetsAndSize[40];
    char stringdata0[276];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_PostMissionSummaryManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_PostMissionSummaryManager_t qt_meta_stringdata_PostMissionSummaryManager = {
    {
QT_MOC_LITERAL(0, 25), // "PostMissionSummaryManager"
QT_MOC_LITERAL(26, 14), // "summaryChanged"
QT_MOC_LITERAL(41, 0), // ""
QT_MOC_LITERAL(42, 26), // "returnToDashboardRequested"
QT_MOC_LITERAL(69, 24), // "startNewMissionRequested"
QT_MOC_LITERAL(94, 21), // "handleMissionFinished"
QT_MOC_LITERAL(116, 6), // "result"
QT_MOC_LITERAL(123, 17), // "handleFlightEnded"
QT_MOC_LITERAL(141, 6), // "record"
QT_MOC_LITERAL(148, 7), // "dismiss"
QT_MOC_LITERAL(156, 7), // "syncNow"
QT_MOC_LITERAL(164, 12), // "exportReport"
QT_MOC_LITERAL(177, 11), // "viewFullLog"
QT_MOC_LITERAL(189, 17), // "returnToDashboard"
QT_MOC_LITERAL(207, 15), // "startNewMission"
QT_MOC_LITERAL(223, 7), // "visible"
QT_MOC_LITERAL(231, 6), // "status"
QT_MOC_LITERAL(238, 10), // "syncStatus"
QT_MOC_LITERAL(249, 11), // "lastSummary"
QT_MOC_LITERAL(261, 14) // "fullLogVisible"

    },
    "PostMissionSummaryManager\0summaryChanged\0"
    "\0returnToDashboardRequested\0"
    "startNewMissionRequested\0handleMissionFinished\0"
    "result\0handleFlightEnded\0record\0dismiss\0"
    "syncNow\0exportReport\0viewFullLog\0"
    "returnToDashboard\0startNewMission\0"
    "visible\0status\0syncStatus\0lastSummary\0"
    "fullLogVisible"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PostMissionSummaryManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       5,   95, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    6 /* Public */,
       3,    0,   81,    2, 0x06,    7 /* Public */,
       4,    0,   82,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,   83,    2, 0x0a,    9 /* Public */,
       7,    1,   86,    2, 0x0a,   11 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,   89,    2, 0x02,   13 /* Public */,
      10,    0,   90,    2, 0x02,   14 /* Public */,
      11,    0,   91,    2, 0x02,   15 /* Public */,
      12,    0,   92,    2, 0x02,   16 /* Public */,
      13,    0,   93,    2, 0x02,   17 /* Public */,
      14,    0,   94,    2, 0x02,   18 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QVariantMap,    6,
    QMetaType::Void, QMetaType::QVariantMap,    8,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      15, QMetaType::Bool, 0x00015001, uint(0), 0,
      16, QMetaType::QString, 0x00015001, uint(0), 0,
      17, QMetaType::QString, 0x00015001, uint(0), 0,
      18, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
      19, QMetaType::Bool, 0x00015103, uint(0), 0,

       0        // eod
};

void PostMissionSummaryManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PostMissionSummaryManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->summaryChanged(); break;
        case 1: _t->returnToDashboardRequested(); break;
        case 2: _t->startNewMissionRequested(); break;
        case 3: _t->handleMissionFinished((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 4: _t->handleFlightEnded((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 5: _t->dismiss(); break;
        case 6: _t->syncNow(); break;
        case 7: _t->exportReport(); break;
        case 8: _t->viewFullLog(); break;
        case 9: _t->returnToDashboard(); break;
        case 10: _t->startNewMission(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PostMissionSummaryManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PostMissionSummaryManager::summaryChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PostMissionSummaryManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PostMissionSummaryManager::returnToDashboardRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PostMissionSummaryManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PostMissionSummaryManager::startNewMissionRequested)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<PostMissionSummaryManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->visible(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->syncStatus(); break;
        case 3: *reinterpret_cast< QVariantMap*>(_v) = _t->lastSummary(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->fullLogVisible(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<PostMissionSummaryManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 4: _t->setFullLogVisible(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject PostMissionSummaryManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PostMissionSummaryManager.offsetsAndSize,
    qt_meta_data_PostMissionSummaryManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_PostMissionSummaryManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<PostMissionSummaryManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

>,
    nullptr
} };


const QMetaObject *PostMissionSummaryManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PostMissionSummaryManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PostMissionSummaryManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PostMissionSummaryManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
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
void PostMissionSummaryManager::summaryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PostMissionSummaryManager::returnToDashboardRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PostMissionSummaryManager::startNewMissionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
