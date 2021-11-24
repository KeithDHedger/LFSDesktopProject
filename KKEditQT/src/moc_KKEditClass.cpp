/****************************************************************************
** Meta object code from reading C++ file 'KKEditClass.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "KKEditClass.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KKEditClass.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KKEditClass_t {
    QByteArrayData data[15];
    char stringdata0[187];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KKEditClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KKEditClass_t qt_meta_stringdata_KKEditClass = {
    {
QT_MOC_LITERAL(0, 0, 11), // "KKEditClass"
QT_MOC_LITERAL(1, 12, 7), // "doTimer"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 15), // "doFileMenuItems"
QT_MOC_LITERAL(4, 37, 15), // "doEditMenuItems"
QT_MOC_LITERAL(5, 53, 15), // "doViewMenuItems"
QT_MOC_LITERAL(6, 69, 14), // "doNavMenuItems"
QT_MOC_LITERAL(7, 84, 18), // "doDoubleClickPrefs"
QT_MOC_LITERAL(8, 103, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(9, 120, 4), // "item"
QT_MOC_LITERAL(10, 125, 14), // "setPreferences"
QT_MOC_LITERAL(11, 140, 11), // "setBMColour"
QT_MOC_LITERAL(12, 152, 13), // "setLineColour"
QT_MOC_LITERAL(13, 166, 7), // "setFont"
QT_MOC_LITERAL(14, 174, 12) // "addToToolBar"

    },
    "KKEditClass\0doTimer\0\0doFileMenuItems\0"
    "doEditMenuItems\0doViewMenuItems\0"
    "doNavMenuItems\0doDoubleClickPrefs\0"
    "QListWidgetItem*\0item\0setPreferences\0"
    "setBMColour\0setLineColour\0setFont\0"
    "addToToolBar"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KKEditClass[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    0,   70,    2, 0x0a /* Public */,
       4,    0,   71,    2, 0x0a /* Public */,
       5,    0,   72,    2, 0x0a /* Public */,
       6,    0,   73,    2, 0x0a /* Public */,
       7,    1,   74,    2, 0x0a /* Public */,
      10,    0,   77,    2, 0x0a /* Public */,
      11,    0,   78,    2, 0x0a /* Public */,
      12,    0,   79,    2, 0x0a /* Public */,
      13,    0,   80,    2, 0x0a /* Public */,
      14,    0,   81,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KKEditClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KKEditClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->doTimer(); break;
        case 1: _t->doFileMenuItems(); break;
        case 2: _t->doEditMenuItems(); break;
        case 3: _t->doViewMenuItems(); break;
        case 4: _t->doNavMenuItems(); break;
        case 5: _t->doDoubleClickPrefs((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 6: _t->setPreferences(); break;
        case 7: _t->setBMColour(); break;
        case 8: _t->setLineColour(); break;
        case 9: _t->setFont(); break;
        case 10: _t->addToToolBar(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KKEditClass::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KKEditClass.data,
    qt_meta_data_KKEditClass,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KKEditClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KKEditClass::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KKEditClass.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KKEditClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
