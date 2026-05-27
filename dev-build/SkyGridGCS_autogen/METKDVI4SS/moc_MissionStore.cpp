/****************************************************************************
** Meta object code from reading C++ file 'MissionStore.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/controllers/MissionStore.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MissionStore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MissionStore_t {
    const uint offsetsAndSize[22];
    char stringdata0[136];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MissionStore_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MissionStore_t qt_meta_stringdata_MissionStore = {
    {
QT_MOC_LITERAL(0, 12), // "MissionStore"
QT_MOC_LITERAL(13, 20), // "overlayInputsChanged"
QT_MOC_LITERAL(34, 0), // ""
QT_MOC_LITERAL(35, 10), // "startDraft"
QT_MOC_LITERAL(46, 11), // "missionType"
QT_MOC_LITERAL(58, 12), // "missionTypes"
QT_MOC_LITERAL(71, 17), // "MissionTypeModel*"
QT_MOC_LITERAL(89, 4), // "plan"
QT_MOC_LITERAL(94, 17), // "MissionPlanModel*"
QT_MOC_LITERAL(112, 14), // "missionHistory"
QT_MOC_LITERAL(127, 8) // "aircraft"

    },
    "MissionStore\0overlayInputsChanged\0\0"
    "startDraft\0missionType\0missionTypes\0"
    "MissionTypeModel*\0plan\0MissionPlanModel*\0"
    "missionHistory\0aircraft"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MissionStore[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       4,   30, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   26,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,   27,    2, 0x0a,    6 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,

 // properties: name, type, flags
       5, 0x80000000 | 6, 0x00015409, uint(-1), 0,
       7, 0x80000000 | 8, 0x00015409, uint(-1), 0,
       9, QMetaType::QVariantList, 0x00015401, uint(-1), 0,
      10, QMetaType::QVariantList, 0x00015401, uint(-1), 0,

       0        // eod
};

void MissionStore::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MissionStore *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->overlayInputsChanged(); break;
        case 1: _t->startDraft((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MissionStore::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MissionStore::overlayInputsChanged)) {
                *result = 0;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< MissionPlanModel* >(); break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< MissionTypeModel* >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MissionStore *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< MissionTypeModel**>(_v) = _t->missionTypes(); break;
        case 1: *reinterpret_cast< MissionPlanModel**>(_v) = _t->plan(); break;
        case 2: *reinterpret_cast< QVariantList*>(_v) = _t->missionHistory(); break;
        case 3: *reinterpret_cast< QVariantList*>(_v) = _t->aircraft(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject MissionStore::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MissionStore.offsetsAndSize,
    qt_meta_data_MissionStore,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MissionStore_t
, QtPrivate::TypeAndForceComplete<MissionTypeModel*, std::true_type>, QtPrivate::TypeAndForceComplete<MissionPlanModel*, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>, QtPrivate::TypeAndForceComplete<MissionStore, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>


>,
    nullptr
} };


const QMetaObject *MissionStore::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MissionStore::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MissionStore.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MissionStore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
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
void MissionStore::overlayInputsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
