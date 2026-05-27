/****************************************************************************
** Meta object code from reading C++ file 'ProfileManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/profile/ProfileManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProfileManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProfileManager_t {
    const uint offsetsAndSize[28];
    char stringdata0[183];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ProfileManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ProfileManager_t qt_meta_stringdata_ProfileManager = {
    {
QT_MOC_LITERAL(0, 14), // "ProfileManager"
QT_MOC_LITERAL(15, 14), // "profileChanged"
QT_MOC_LITERAL(30, 0), // ""
QT_MOC_LITERAL(31, 17), // "loadCachedProfile"
QT_MOC_LITERAL(49, 14), // "applyBootstrap"
QT_MOC_LITERAL(64, 9), // "bootstrap"
QT_MOC_LITERAL(74, 11), // "displayName"
QT_MOC_LITERAL(86, 8), // "username"
QT_MOC_LITERAL(95, 9), // "avatarUrl"
QT_MOC_LITERAL(105, 16), // "organizationName"
QT_MOC_LITERAL(122, 14), // "organizationId"
QT_MOC_LITERAL(137, 16), // "analyticsSummary"
QT_MOC_LITERAL(154, 13), // "notifications"
QT_MOC_LITERAL(168, 14) // "recentActivity"

    },
    "ProfileManager\0profileChanged\0\0"
    "loadCachedProfile\0applyBootstrap\0"
    "bootstrap\0displayName\0username\0avatarUrl\0"
    "organizationName\0organizationId\0"
    "analyticsSummary\0notifications\0"
    "recentActivity"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProfileManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       8,   37, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x06,    9 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   33,    2, 0x02,   10 /* Public */,
       4,    1,   34,    2, 0x02,   11 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,    5,

 // properties: name, type, flags
       6, QMetaType::QString, 0x00015001, uint(0), 0,
       7, QMetaType::QString, 0x00015001, uint(0), 0,
       8, QMetaType::QString, 0x00015001, uint(0), 0,
       9, QMetaType::QString, 0x00015001, uint(0), 0,
      10, QMetaType::Int, 0x00015001, uint(0), 0,
      11, QMetaType::QVariantMap, 0x00015001, uint(0), 0,
      12, QMetaType::QVariantList, 0x00015001, uint(0), 0,
      13, QMetaType::QVariantList, 0x00015001, uint(0), 0,

       0        // eod
};

void ProfileManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProfileManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->profileChanged(); break;
        case 1: _t->loadCachedProfile(); break;
        case 2: _t->applyBootstrap((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProfileManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProfileManager::profileChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ProfileManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->displayName(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->username(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->avatarUrl(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->organizationName(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->organizationId(); break;
        case 5: *reinterpret_cast< QVariantMap*>(_v) = _t->analyticsSummary(); break;
        case 6: *reinterpret_cast< QVariantList*>(_v) = _t->notifications(); break;
        case 7: *reinterpret_cast< QVariantList*>(_v) = _t->recentActivity(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject ProfileManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ProfileManager.offsetsAndSize,
    qt_meta_data_ProfileManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ProfileManager_t
, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<int, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantMap, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<ProfileManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>

, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QVariantMap &, std::false_type>

>,
    nullptr
} };


const QMetaObject *ProfileManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProfileManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProfileManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ProfileManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ProfileManager::profileChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
