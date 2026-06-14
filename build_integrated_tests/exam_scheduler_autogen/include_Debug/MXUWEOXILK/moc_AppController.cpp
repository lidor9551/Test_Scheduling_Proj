/****************************************************************************
** Meta object code from reading C++ file 'AppController.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/gui/AppController.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppController.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13AppControllerE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN13AppControllerE = QtMocHelpers::stringData(
    "AppController",
    "coursesFilePathChanged",
    "",
    "examPeriodsFilePathChanged",
    "statusMessageChanged",
    "errorMessageChanged",
    "dataChanged",
    "selectedProgramsChanged",
    "setCoursesFilePath",
    "path",
    "setExamPeriodsFilePath",
    "replaceData",
    "appendData",
    "clearMessages",
    "getCoursesForProgram",
    "QVariantList",
    "programId",
    "year",
    "semester",
    "toggleProgram",
    "coursesFilePath",
    "examPeriodsFilePath",
    "statusMessage",
    "errorMessage",
    "courseCount",
    "examPeriodCount",
    "hasData",
    "courses",
    "examPeriods",
    "availablePrograms",
    "selectedPrograms",
    "programCourseModel",
    "ProgramCourseModel*"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN13AppControllerE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
      12,  137, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  104,    2, 0x06,   13 /* Public */,
       3,    0,  105,    2, 0x06,   14 /* Public */,
       4,    0,  106,    2, 0x06,   15 /* Public */,
       5,    0,  107,    2, 0x06,   16 /* Public */,
       6,    0,  108,    2, 0x06,   17 /* Public */,
       7,    0,  109,    2, 0x06,   18 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       8,    1,  110,    2, 0x02,   19 /* Public */,
      10,    1,  113,    2, 0x02,   21 /* Public */,
      11,    0,  116,    2, 0x02,   23 /* Public */,
      12,    0,  117,    2, 0x02,   24 /* Public */,
      13,    0,  118,    2, 0x02,   25 /* Public */,
      14,    3,  119,    2, 0x02,   26 /* Public */,
      14,    2,  126,    2, 0x22,   30 /* Public | MethodCloned */,
      14,    1,  131,    2, 0x22,   33 /* Public | MethodCloned */,
      19,    1,  134,    2, 0x02,   35 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    0x80000000 | 15, QMetaType::QString, QMetaType::Int, QMetaType::Int,   16,   17,   18,
    0x80000000 | 15, QMetaType::QString, QMetaType::Int,   16,   17,
    0x80000000 | 15, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,

 // properties: name, type, flags, notifyId, revision
      20, QMetaType::QString, 0x00015001, uint(0), 0,
      21, QMetaType::QString, 0x00015001, uint(1), 0,
      22, QMetaType::QString, 0x00015001, uint(2), 0,
      23, QMetaType::QString, 0x00015001, uint(3), 0,
      24, QMetaType::Int, 0x00015001, uint(4), 0,
      25, QMetaType::Int, 0x00015001, uint(4), 0,
      26, QMetaType::Bool, 0x00015001, uint(4), 0,
      27, 0x80000000 | 15, 0x00015009, uint(4), 0,
      28, 0x80000000 | 15, 0x00015009, uint(4), 0,
      29, QMetaType::QStringList, 0x00015401, uint(-1), 0,
      30, QMetaType::QStringList, 0x00015001, uint(5), 0,
      31, 0x80000000 | 32, 0x00015409, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject AppController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN13AppControllerE.offsetsAndSizes,
    qt_meta_data_ZN13AppControllerE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN13AppControllerE_t,
        // property 'coursesFilePath'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'examPeriodsFilePath'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'statusMessage'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'errorMessage'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'courseCount'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'examPeriodCount'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'hasData'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'courses'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'examPeriods'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'availablePrograms'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // property 'selectedPrograms'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // property 'programCourseModel'
        QtPrivate::TypeAndForceComplete<ProgramCourseModel*, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AppController, std::true_type>,
        // method 'coursesFilePathChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'examPeriodsFilePathChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'statusMessageChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'errorMessageChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'dataChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectedProgramsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setCoursesFilePath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'setExamPeriodsFilePath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'replaceData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'appendData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clearMessages'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'getCoursesForProgram'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'getCoursesForProgram'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'getCoursesForProgram'
        QtPrivate::TypeAndForceComplete<QVariantList, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'toggleProgram'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void AppController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AppController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->coursesFilePathChanged(); break;
        case 1: _t->examPeriodsFilePathChanged(); break;
        case 2: _t->statusMessageChanged(); break;
        case 3: _t->errorMessageChanged(); break;
        case 4: _t->dataChanged(); break;
        case 5: _t->selectedProgramsChanged(); break;
        case 6: _t->setCoursesFilePath((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->setExamPeriodsFilePath((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->replaceData(); break;
        case 9: _t->appendData(); break;
        case 10: _t->clearMessages(); break;
        case 11: { QVariantList _r = _t->getCoursesForProgram((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 12: { QVariantList _r = _t->getCoursesForProgram((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 13: { QVariantList _r = _t->getCoursesForProgram((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 14: _t->toggleProgram((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (AppController::*)();
            if (_q_method_type _q_method = &AppController::coursesFilePathChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (AppController::*)();
            if (_q_method_type _q_method = &AppController::examPeriodsFilePathChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (AppController::*)();
            if (_q_method_type _q_method = &AppController::statusMessageChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (AppController::*)();
            if (_q_method_type _q_method = &AppController::errorMessageChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (AppController::*)();
            if (_q_method_type _q_method = &AppController::dataChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (AppController::*)();
            if (_q_method_type _q_method = &AppController::selectedProgramsChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 11:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ProgramCourseModel* >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->coursesFilePath(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->examPeriodsFilePath(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->statusMessage(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->errorMessage(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->courseCount(); break;
        case 5: *reinterpret_cast< int*>(_v) = _t->examPeriodCount(); break;
        case 6: *reinterpret_cast< bool*>(_v) = _t->hasData(); break;
        case 7: *reinterpret_cast< QVariantList*>(_v) = _t->getCoursesVariant(); break;
        case 8: *reinterpret_cast< QVariantList*>(_v) = _t->getExamPeriodsVariant(); break;
        case 9: *reinterpret_cast< QStringList*>(_v) = _t->availablePrograms(); break;
        case 10: *reinterpret_cast< QStringList*>(_v) = _t->selectedPrograms(); break;
        case 11: *reinterpret_cast< ProgramCourseModel**>(_v) = _t->programCourseModel(); break;
        default: break;
        }
    }
}

const QMetaObject *AppController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN13AppControllerE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AppController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void AppController::coursesFilePathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AppController::examPeriodsFilePathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AppController::statusMessageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AppController::errorMessageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AppController::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void AppController::selectedProgramsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
