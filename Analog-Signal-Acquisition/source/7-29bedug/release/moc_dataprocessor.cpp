/****************************************************************************
** Meta object code from reading C++ file 'dataprocessor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../dataprocessor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dataprocessor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DataProcessor_t {
    QByteArrayData data[14];
    char stringdata0[165];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DataProcessor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DataProcessor_t qt_meta_stringdata_DataProcessor = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DataProcessor"
QT_MOC_LITERAL(1, 14, 13), // "dataProcessed"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 12), // "channelCount"
QT_MOC_LITERAL(4, 42, 13), // "currentPoints"
QT_MOC_LITERAL(5, 56, 19), // "acquisitionComplete"
QT_MOC_LITERAL(6, 76, 11), // "totalPoints"
QT_MOC_LITERAL(7, 88, 14), // "updateProgress"
QT_MOC_LITERAL(8, 103, 7), // "current"
QT_MOC_LITERAL(9, 111, 5), // "total"
QT_MOC_LITERAL(10, 117, 11), // "processData"
QT_MOC_LITERAL(11, 129, 20), // "QVector<QVariantMap>"
QT_MOC_LITERAL(12, 150, 4), // "data"
QT_MOC_LITERAL(13, 155, 9) // "clearData"

    },
    "DataProcessor\0dataProcessed\0\0channelCount\0"
    "currentPoints\0acquisitionComplete\0"
    "totalPoints\0updateProgress\0current\0"
    "total\0processData\0QVector<QVariantMap>\0"
    "data\0clearData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DataProcessor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       5,    1,   44,    2, 0x06 /* Public */,
       7,    2,   47,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   52,    2, 0x0a /* Public */,
      13,    0,   55,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void,

       0        // eod
};

void DataProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataProcessor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dataProcessed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->acquisitionComplete((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->updateProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->processData((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 4: _t->clearData(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVariantMap> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataProcessor::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataProcessor::dataProcessed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DataProcessor::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataProcessor::acquisitionComplete)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DataProcessor::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataProcessor::updateProgress)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DataProcessor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DataProcessor.data,
    qt_meta_data_DataProcessor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DataProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DataProcessor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DataProcessor::dataProcessed(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DataProcessor::acquisitionComplete(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DataProcessor::updateProgress(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
