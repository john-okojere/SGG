/****************************************************************************
** Meta object code from reading C++ file 'AccessManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/security/AccessManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AccessManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AccessManager_t {
    const uint offsetsAndSize[84];
    char stringdata0[484];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_AccessManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_AccessManager_t qt_meta_stringdata_AccessManager = {
    {
QT_MOC_LITERAL(0, 13), // "AccessManager"
QT_MOC_LITERAL(14, 13), // "accessChanged"
QT_MOC_LITERAL(28, 0), // ""
QT_MOC_LITERAL(29, 13), // "accessRevoked"
QT_MOC_LITERAL(43, 6), // "reason"
QT_MOC_LITERAL(50, 14), // "applyBootstrap"
QT_MOC_LITERAL(65, 9), // "bootstrap"
QT_MOC_LITERAL(75, 11), // "clearAccess"
QT_MOC_LITERAL(87, 15), // "setSessionState"
QT_MOC_LITERAL(103, 7), // "trusted"
QT_MOC_LITERAL(111, 9), // "reachable"
QT_MOC_LITERAL(121, 3), // "can"
QT_MOC_LITERAL(125, 10), // "permission"
QT_MOC_LITERAL(136, 6), // "canAny"
QT_MOC_LITERAL(143, 11), // "permissions"
QT_MOC_LITERAL(155, 9), // "canModule"
QT_MOC_LITERAL(165, 6), // "module"
QT_MOC_LITERAL(172, 10), // "canPerform"
QT_MOC_LITERAL(183, 6), // "action"
QT_MOC_LITERAL(190, 17), // "canAccessAircraft"
QT_MOC_LITERAL(208, 10), // "aircraftId"
QT_MOC_LITERAL(219, 16), // "canAccessMission"
QT_MOC_LITERAL(236, 9), // "missionId"
QT_MOC_LITERAL(246, 14), // "filterAircraft"
QT_MOC_LITERAL(261, 8), // "aircraft"
QT_MOC_LITERAL(270, 14), // "filterMissions"
QT_MOC_LITERAL(285, 8), // "missions"
QT_MOC_LITERAL(294, 15), // "authorizeAction"
QT_MOC_LITERAL(310, 7), // "context"
QT_MOC_LITERAL(318, 7), // "message"
QT_MOC_LITERAL(326, 13), // "recordAllowed"
QT_MOC_LITERAL(340, 13), // "recordBlocked"
QT_MOC_LITERAL(354, 12), // "accessLoaded"
QT_MOC_LITERAL(367, 14), // "sessionTrusted"
QT_MOC_LITERAL(382, 25), // "offlineAuthorizationValid"
QT_MOC_LITERAL(408, 4), // "role"
QT_MOC_LITERAL(413, 5), // "roles"
QT_MOC_LITERAL(419, 14), // "allowedModules"
QT_MOC_LITERAL(434, 14), // "organizationId"
QT_MOC_LITERAL(449, 13), // "sessionStatus"
QT_MOC_LITERAL(463, 13), // "deviceSummary"
QT_MOC_LITERAL(477, 6) // "status"

    },
    "AccessManager\0accessChanged\0\0accessRevoked\0"
    "reason\0applyBootstrap\0bootstrap\0"
    "clearAccess\0setSessionState\0trusted\0"
    "reachable\0can\0permission\0canAny\0"
    "permissions\0canModule\0module\0canPerform\0"
    "action\0canAccessAircraft\0aircraftId\0"
    "canAccessMission\0missionId\0filterAircraft\0"
    "aircraft\0filterMissions\0missions\0"
    "authorizeAction\0context\0message\0"
    "recordAllowed\0recordBlocked\0accessLoaded\0"
    "sessionTrusted\0offlineAuthorizationValid\0"
    "role\0roles\0allowedModules\0organizationId\0"
    "sessionStatus\0deviceSummary\0status"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AccessManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
      11,  228, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  146,    2, 0x06,   12 /* Public */,
       3,    1,  147,    2, 0x06,   13 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,  150,    2, 0x02,   15 /* Public */,
       7,    1,  153,    2, 0x02,   17 /* Public */,
       7,    0,  156,    2, 0x22,   19 /* Public | MethodCloned */,
       8,    3,  157,    2, 0x02,   20 /* Public */,
       8,    2,  164,    2, 0x22,   24 /* Public | MethodCloned */,
      11,    1,  169,    2, 0x102,   27 /* Public | MethodIsConst  */,
      13,    1,  172,    2, 0x102,   29 /* Public | MethodIsConst  */,
      15,    1,  175,    2, 0x102,   31 /* Public | MethodIsConst  */,
      17,    1,  178,    2, 0x102,   33 /* Public | MethodIsConst  */,
      19,    1,  181,    2, 0x102,   35 /* Public | MethodIsConst  */,
      21,    1,  184,    2, 0x102,   37 /* Public | MethodIsConst  */,
      23,    1,  187,    2, 0x102,   39 /* Public | MethodIsConst  */,
      25,    1,  190,    2, 0x102,   41 /* Public | MethodIsConst  */,
      27,    3,  193,    2, 0x02,   43 /* Public */,
      27,    2,  200,    2, 0x22,   47 /* Public | MethodCloned */,
      27,    1,  205,    2, 0x22,   50 /* Public | MethodCloned */,
      30,    2,  208,    2, 0x02,   52 /* Public */,
      30,    1,  213,    2, 0x22,   55 /* Public | MethodCloned */,
      31,    3,  216,    2, 0x02,   57 /* Public */,
      31,    2,  223,    2, 0x22,   61 /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,

 // methods: parameters
    QMetaType::Void, QMetaType::QVariantMap,    6,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool, QMetaType::QString,    9,   10,    4,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,    9,   10,
    QMetaType::Bool, QMetaType::QString,   12,
    QMetaType::Bool, QMetaType::QStringList,   14,
    QMetaType::Bool, QMetaType::QString,   16,
    QMetaType::Bool, QMetaType::QString,   18,
    QMetaType::Bool, QMetaType::QVariant,   20,
    QMetaType::Bool, QMetaType::QVariant,   22,
    QMetaType::QVariantList, QMetaType::QVariantList,   24,
    QMetaType::QVariantList, QMetaType::QVariantList,   26,
    QMetaType::Bool, QMetaType::QString, QMetaType::QVariantMap, QMetaType::QString,   18,   28,   29,
    QMetaType::Bool, QMetaType::QString, QMetaType::QVariantMap,   18,   28,
    QMetaType::Bool, QMetaType::QString,   18,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariantMap,   18,   28,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QVariantMap,   18,    4,   28,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   18,    4,

 // properties: name, type, flags
      32, QMetaType::Bool, 0x00015001, uint(0), 0,
      33, QMetaType::Bool, 0x00015001, uint(0), 0,
      34, QMetaType::Bool, 0x00015001, uint(0), 0,
      35, QMetaType::QString, 0x00015001, uint(0), 0,
      36, QMetaType::QStringList, 0x00015001, uint(0), 0,
      14, QMetaType::QStringList, 0x00015001, uint(0), 0,
      37, QMetaType::QStringList, 0x00015001, uint(0), 0,
      38, QMetaType::Int, 0x00015001, uint(0), 0,
      39, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
      40, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
      41, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

void AccessManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AccessManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->accessChanged(); break;
        case 1: _t->accessRevoked((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->applyBootstrap((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 3: _t->clearAccess((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->clearAccess(); break;
        case 5: _t->setSessionState((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 6: _t->setSessionState((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 7: { bool _r = _t->can((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 8: { bool _r = _t->canAny((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 9: { bool _r = _t->canModule((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 10: { bool _r = _t->canPerform((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { bool _r = _t->canAccessAircraft((*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 12: { bool _r = _t->canAccessMission((*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 13: { QVariantList _r = _t->filterAircraft((*reinterpret_cast< std::add_pointer_t<QVariantList>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 14: { QVariantList _r = _t->filterMissions((*reinterpret_cast< std::add_pointer_t<QVariantList>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 15: { bool _r = _t->authorizeAction((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 16: { bool _r = _t->authorizeAction((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 17: { bool _r = _t->authorizeAction((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 18: _t->recordAllowed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 19: _t->recordAllowed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->recordBlocked((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[3]))); break;
        case 21: _t->recordBlocked((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AccessManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AccessManager::accessChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AccessManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AccessManager::accessRevoked)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<AccessManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->accessLoaded(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->sessionTrusted(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->offlineAuthorizationValid(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->role(); break;
        case 4: *reinterpret_cast< QStringList*>(_v) = _t->roles(); break;
        case 5: *reinterpret_cast< QStringList*>(_v) = _t->permissions(); break;
        case 6: *reinterpret_cast< QStringList*>(_v) = _t->allowedModules(); break;
        case 7: *reinterpret_cast< int*>(_v) = _t->organizationId(); break;
        case 8: *reinterpret_cast< QVariantMap*>(_v) = _t->sessionStatus(); break;
        case 9: *reinterpret_cast< QVariantMap*>(_v) = _t->deviceSummary(); break;
        case 10: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject AccessManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AccessManager.offsetsAndSize,
    qt_meta_data_AccessManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_AccessManager_t
, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QStringList, std::true_type>, QtPrivate::TypeAndForceComplete<QStringList, std::true_type>, QtPrivate::TypeAndForceComplete<QStringList, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<AccessManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantList &, std::false_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantList &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>

>,
    nullptr
} };


const QMetaObject *AccessManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AccessManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AccessManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AccessManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 22;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void AccessManager::accessChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AccessManager::accessRevoked(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
