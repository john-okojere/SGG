/****************************************************************************
** Meta object code from reading C++ file 'PreflightChecklistManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/flight/PreflightChecklistManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PreflightChecklistManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PreflightChecklistManager_t {
    const uint offsetsAndSize[42];
    char stringdata0[260];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_PreflightChecklistManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_PreflightChecklistManager_t qt_meta_stringdata_PreflightChecklistManager = {
    {
QT_MOC_LITERAL(0, 25), // "PreflightChecklistManager"
QT_MOC_LITERAL(26, 16), // "checklistChanged"
QT_MOC_LITERAL(43, 0), // ""
QT_MOC_LITERAL(44, 18), // "checklistCompleted"
QT_MOC_LITERAL(63, 7), // "success"
QT_MOC_LITERAL(71, 7), // "message"
QT_MOC_LITERAL(79, 12), // "runChecklist"
QT_MOC_LITERAL(92, 9), // "logEvents"
QT_MOC_LITERAL(102, 16), // "acknowledgeCheck"
QT_MOC_LITERAL(119, 7), // "checkId"
QT_MOC_LITERAL(127, 22), // "acknowledgeAllCautions"
QT_MOC_LITERAL(150, 11), // "blockReason"
QT_MOC_LITERAL(162, 6), // "checks"
QT_MOC_LITERAL(169, 16), // "readinessPercent"
QT_MOC_LITERAL(186, 7), // "blocked"
QT_MOC_LITERAL(194, 15), // "cautionRequired"
QT_MOC_LITERAL(210, 9), // "canUpload"
QT_MOC_LITERAL(220, 6), // "canArm"
QT_MOC_LITERAL(227, 15), // "canStartMission"
QT_MOC_LITERAL(243, 6), // "status"
QT_MOC_LITERAL(250, 9) // "lastRunAt"

    },
    "PreflightChecklistManager\0checklistChanged\0"
    "\0checklistCompleted\0success\0message\0"
    "runChecklist\0logEvents\0acknowledgeCheck\0"
    "checkId\0acknowledgeAllCautions\0"
    "blockReason\0checks\0readinessPercent\0"
    "blocked\0cautionRequired\0canUpload\0"
    "canArm\0canStartMission\0status\0lastRunAt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PreflightChecklistManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       9,   71, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x06,   10 /* Public */,
       3,    2,   57,    2, 0x06,   11 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       6,    1,   62,    2, 0x02,   14 /* Public */,
       6,    0,   65,    2, 0x22,   16 /* Public | MethodCloned */,
       8,    1,   66,    2, 0x02,   17 /* Public */,
      10,    0,   69,    2, 0x02,   19 /* Public */,
      11,    0,   70,    2, 0x102,   20 /* Public | MethodIsConst  */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    4,    5,

 // methods: parameters
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::QString,

 // properties: name, type, flags
      12, QMetaType::QVariantList, 0x00015001, uint(0), 0,
      13, QMetaType::Int, 0x00015001, uint(0), 0,
      14, QMetaType::Bool, 0x00015001, uint(0), 0,
      15, QMetaType::Bool, 0x00015001, uint(0), 0,
      16, QMetaType::Bool, 0x00015001, uint(0), 0,
      17, QMetaType::Bool, 0x00015001, uint(0), 0,
      18, QMetaType::Bool, 0x00015001, uint(0), 0,
      19, QMetaType::QString, 0x00015001, uint(0), 0,
      20, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

void PreflightChecklistManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PreflightChecklistManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->checklistChanged(); break;
        case 1: _t->checklistCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->runChecklist((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->runChecklist(); break;
        case 4: _t->acknowledgeCheck((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->acknowledgeAllCautions(); break;
        case 6: { QString _r = _t->blockReason();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PreflightChecklistManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PreflightChecklistManager::checklistChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PreflightChecklistManager::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PreflightChecklistManager::checklistCompleted)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<PreflightChecklistManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariantList*>(_v) = _t->checks(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->readinessPercent(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->blocked(); break;
        case 3: *reinterpret_cast< bool*>(_v) = _t->cautionRequired(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->canUpload(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->canArm(); break;
        case 6: *reinterpret_cast< bool*>(_v) = _t->canStartMission(); break;
        case 7: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        case 8: *reinterpret_cast< QString*>(_v) = _t->lastRunAt(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject PreflightChecklistManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PreflightChecklistManager.offsetsAndSize,
    qt_meta_data_PreflightChecklistManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_PreflightChecklistManager_t
, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<PreflightChecklistManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>

>,
    nullptr
} };


const QMetaObject *PreflightChecklistManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PreflightChecklistManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PreflightChecklistManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PreflightChecklistManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void PreflightChecklistManager::checklistChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PreflightChecklistManager::checklistCompleted(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
