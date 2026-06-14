/****************************************************************************
** Meta object code from reading C++ file 'CalendarManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/gui/CalendarManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CalendarManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN15CalendarManagerE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN15CalendarManagerE = QtMocHelpers::stringData(
    "CalendarManager",
    "daysChanged",
    "",
    "semesterChanged",
    "setData",
    "QVariantList",
    "periods",
    "courses",
    "getPeriodTree",
    "toggleDay",
    "dateStr",
    "shiftPeriod",
    "semester",
    "newStartStr",
    "newEndStr",
    "selectPeriod",
    "index",
    "nextSemester",
    "previousSemester",
    "saveChanges",
    "days",
    "currentSemester",
    "currentStartDate",
    "currentEndDate",
    "semesterList"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN15CalendarManagerE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       5,   98, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    6 /* Public */,
       3,    0,   75,    2, 0x06,    7 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       4,    2,   76,    2, 0x02,    8 /* Public */,
       8,    0,   81,    2, 0x102,   11 /* Public | MethodIsConst  */,
       9,    1,   82,    2, 0x02,   12 /* Public */,
      11,    3,   85,    2, 0x02,   14 /* Public */,
      15,    1,   92,    2, 0x02,   18 /* Public */,
      17,    0,   95,    2, 0x02,   20 /* Public */,
      18,    0,   96,    2, 0x02,   21 /* Public */,
      19,    0,   97,    2, 0x02,   22 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 5,    6,    7,
    0x80000000 | 5,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   12,   13,   14,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags, notifyId, revision
      20, 0x80000000 | 5, 0x00015009, uint(0), 0,
      21, QMetaType::QString, 0x00015001, uint(1), 0,
      22, QMetaType::QString, 0x00015001, uint(1), 0,
      23, QMetaType::QString, 0x00015001, uint(1), 0,
      24, 0x80000000 | 5, 0x00015009, uint(1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject CalendarManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN15CalendarManagerE.offsetsAndSizes,
    qt_meta_data_ZN15CalendarManagerE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN15CalendarManagerE_t,
        // property 'days'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'currentSemester'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'currentStartDate'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'currentEndDate'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'semesterList'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CalendarManager, std::true_type>,
        // method 'daysChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'semesterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        // method 'getPeriodTree'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        // method 'toggleDay'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'shiftPeriod'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'selectPeriod'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'nextSemester'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'previousSemester'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveChanges'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void CalendarManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CalendarManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->daysChanged(); break;
        case 1: _t->semesterChanged(); break;
        case 2: _t->setData((*reinterpret_cast< std::add_pointer_t<QVariantList>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantList>>(_a[2]))); break;
        case 3: { QVariantList _r = _t->getPeriodTree();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->toggleDay((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->shiftPeriod((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 6: _t->selectPeriod((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->nextSemester(); break;
        case 8: _t->previousSemester(); break;
        case 9: _t->saveChanges(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (CalendarManager::*)();
            if (_q_method_type _q_method = &CalendarManager::daysChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (CalendarManager::*)();
            if (_q_method_type _q_method = &CalendarManager::semesterChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariantList*>(_v) = _t->getDays(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->getCurrentSemester(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->getCurrentStartDate(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->getCurrentEndDate(); break;
        case 4: *reinterpret_cast< QVariantList*>(_v) = _t->getSemesterList(); break;
        default: break;
        }
    }
}

const QMetaObject *CalendarManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalendarManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN15CalendarManagerE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CalendarManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CalendarManager::daysChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CalendarManager::semesterChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
