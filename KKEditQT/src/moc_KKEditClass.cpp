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
    QByteArrayData data[22];
    char stringdata0[279];
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
QT_MOC_LITERAL(7, 84, 19), // "doBookmarkMenuItems"
QT_MOC_LITERAL(8, 104, 15), // "doHelpMenuItems"
QT_MOC_LITERAL(9, 120, 16), // "doToolsMenuItems"
QT_MOC_LITERAL(10, 137, 18), // "doDoubleClickPrefs"
QT_MOC_LITERAL(11, 156, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(12, 173, 4), // "item"
QT_MOC_LITERAL(13, 178, 14), // "setPreferences"
QT_MOC_LITERAL(14, 193, 11), // "setBMColour"
QT_MOC_LITERAL(15, 205, 13), // "setLineColour"
QT_MOC_LITERAL(16, 219, 7), // "setFont"
QT_MOC_LITERAL(17, 227, 12), // "addToToolBar"
QT_MOC_LITERAL(18, 240, 10), // "switchPage"
QT_MOC_LITERAL(19, 251, 5), // "index"
QT_MOC_LITERAL(20, 257, 8), // "closeTab"
QT_MOC_LITERAL(21, 266, 12) // "closeAllTabs"

    },
    "KKEditClass\0doTimer\0\0doFileMenuItems\0"
    "doEditMenuItems\0doViewMenuItems\0"
    "doNavMenuItems\0doBookmarkMenuItems\0"
    "doHelpMenuItems\0doToolsMenuItems\0"
    "doDoubleClickPrefs\0QListWidgetItem*\0"
    "item\0setPreferences\0setBMColour\0"
    "setLineColour\0setFont\0addToToolBar\0"
    "switchPage\0index\0closeTab\0closeAllTabs"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KKEditClass[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x0a /* Public */,
       3,    0,  100,    2, 0x0a /* Public */,
       4,    0,  101,    2, 0x0a /* Public */,
       5,    0,  102,    2, 0x0a /* Public */,
       6,    0,  103,    2, 0x0a /* Public */,
       7,    0,  104,    2, 0x0a /* Public */,
       8,    0,  105,    2, 0x0a /* Public */,
       9,    0,  106,    2, 0x0a /* Public */,
      10,    1,  107,    2, 0x0a /* Public */,
      13,    0,  110,    2, 0x0a /* Public */,
      14,    0,  111,    2, 0x0a /* Public */,
      15,    0,  112,    2, 0x0a /* Public */,
      16,    0,  113,    2, 0x0a /* Public */,
      17,    0,  114,    2, 0x0a /* Public */,
      18,    1,  115,    2, 0x0a /* Public */,
      20,    1,  118,    2, 0x0a /* Public */,
      21,    0,  121,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Bool, QMetaType::Int,   19,
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
        case 5: _t->doBookmarkMenuItems(); break;
        case 6: _t->doHelpMenuItems(); break;
        case 7: _t->doToolsMenuItems(); break;
        case 8: _t->doDoubleClickPrefs((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 9: _t->setPreferences(); break;
        case 10: _t->setBMColour(); break;
        case 11: _t->setLineColour(); break;
        case 12: _t->setFont(); break;
        case 13: _t->addToToolBar(); break;
        case 14: _t->switchPage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: { bool _r = _t->closeTab((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 16: _t->closeAllTabs(); break;
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
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
