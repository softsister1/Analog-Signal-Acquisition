/****************************************************************************
** Meta object code from reading C++ file 'debugdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../debugdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'debugdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DebugDialog_t {
    QByteArrayData data[13];
    char stringdata0[200];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DebugDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DebugDialog_t qt_meta_stringdata_DebugDialog = {
    {
QT_MOC_LITERAL(0, 0, 11), // "DebugDialog"
QT_MOC_LITERAL(1, 12, 21), // "onReadRegisterClicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 22), // "onWriteRegisterClicked"
QT_MOC_LITERAL(4, 58, 22), // "onChannelEnableChanged"
QT_MOC_LITERAL(5, 81, 27), // "onApplyChannelEnableClicked"
QT_MOC_LITERAL(6, 109, 19), // "onClearDebugClicked"
QT_MOC_LITERAL(7, 129, 22), // "onRegisterDataReceived"
QT_MOC_LITERAL(8, 152, 8), // "uint32_t"
QT_MOC_LITERAL(9, 161, 7), // "regAddr"
QT_MOC_LITERAL(10, 169, 7), // "regData"
QT_MOC_LITERAL(11, 177, 17), // "onRawDataReceived"
QT_MOC_LITERAL(12, 195, 4) // "data"

    },
    "DebugDialog\0onReadRegisterClicked\0\0"
    "onWriteRegisterClicked\0onChannelEnableChanged\0"
    "onApplyChannelEnableClicked\0"
    "onClearDebugClicked\0onRegisterDataReceived\0"
    "uint32_t\0regAddr\0regData\0onRawDataReceived\0"
    "data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DebugDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    0,   50,    2, 0x0a /* Public */,
       4,    0,   51,    2, 0x0a /* Public */,
       5,    0,   52,    2, 0x0a /* Public */,
       6,    0,   53,    2, 0x0a /* Public */,
       7,    2,   54,    2, 0x0a /* Public */,
      11,    1,   59,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8, 0x80000000 | 8,    9,   10,
    QMetaType::Void, QMetaType::QByteArray,   12,

       0        // eod
};

void DebugDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DebugDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onReadRegisterClicked(); break;
        case 1: _t->onWriteRegisterClicked(); break;
        case 2: _t->onChannelEnableChanged(); break;
        case 3: _t->onApplyChannelEnableClicked(); break;
        case 4: _t->onClearDebugClicked(); break;
        case 5: _t->onRegisterDataReceived((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 6: _t->onRawDataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DebugDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_DebugDialog.data,
    qt_meta_data_DebugDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DebugDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DebugDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DebugDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DebugDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
