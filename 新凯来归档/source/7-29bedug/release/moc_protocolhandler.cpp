/****************************************************************************
** Meta object code from reading C++ file 'protocolhandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../protocolhandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'protocolhandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProtocolHandler_t {
    QByteArrayData data[20];
    char stringdata0[239];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProtocolHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProtocolHandler_t qt_meta_stringdata_ProtocolHandler = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ProtocolHandler"
QT_MOC_LITERAL(1, 16, 18), // "registerReadResult"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 8), // "uint32_t"
QT_MOC_LITERAL(4, 45, 7), // "regAddr"
QT_MOC_LITERAL(5, 53, 7), // "regData"
QT_MOC_LITERAL(6, 61, 17), // "powerDataReceived"
QT_MOC_LITERAL(7, 79, 20), // "QVector<QVariantMap>"
QT_MOC_LITERAL(8, 100, 9), // "powerData"
QT_MOC_LITERAL(9, 110, 15), // "ntcTempReceived"
QT_MOC_LITERAL(10, 126, 8), // "tempData"
QT_MOC_LITERAL(11, 135, 19), // "diffVoltageReceived"
QT_MOC_LITERAL(12, 155, 11), // "voltageData"
QT_MOC_LITERAL(13, 167, 19), // "digitalInputChanged"
QT_MOC_LITERAL(14, 187, 8), // "isGroupA"
QT_MOC_LITERAL(15, 196, 7), // "uint8_t"
QT_MOC_LITERAL(16, 204, 7), // "diValue"
QT_MOC_LITERAL(17, 212, 17), // "rawPacketReceived"
QT_MOC_LITERAL(18, 230, 3), // "cmd"
QT_MOC_LITERAL(19, 234, 4) // "data"

    },
    "ProtocolHandler\0registerReadResult\0\0"
    "uint32_t\0regAddr\0regData\0powerDataReceived\0"
    "QVector<QVariantMap>\0powerData\0"
    "ntcTempReceived\0tempData\0diffVoltageReceived\0"
    "voltageData\0digitalInputChanged\0"
    "isGroupA\0uint8_t\0diValue\0rawPacketReceived\0"
    "cmd\0data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProtocolHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,
       6,    1,   49,    2, 0x06 /* Public */,
       9,    1,   52,    2, 0x06 /* Public */,
      11,    1,   55,    2, 0x06 /* Public */,
      13,    2,   58,    2, 0x06 /* Public */,
      17,    2,   63,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,   10,
    QMetaType::Void, 0x80000000 | 7,   12,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 15,   14,   16,
    QMetaType::Void, QMetaType::Int, QMetaType::QByteArray,   18,   19,

       0        // eod
};

void ProtocolHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProtocolHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->registerReadResult((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 1: _t->powerDataReceived((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 2: _t->ntcTempReceived((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 3: _t->diffVoltageReceived((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 4: _t->digitalInputChanged((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< uint8_t(*)>(_a[2]))); break;
        case 5: _t->rawPacketReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVariantMap> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVariantMap> >(); break;
            }
            break;
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
            using _t = void (ProtocolHandler::*)(uint32_t , uint32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolHandler::registerReadResult)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProtocolHandler::*)(const QVector<QVariantMap> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolHandler::powerDataReceived)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProtocolHandler::*)(const QVector<QVariantMap> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolHandler::ntcTempReceived)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ProtocolHandler::*)(const QVector<QVariantMap> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolHandler::diffVoltageReceived)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ProtocolHandler::*)(bool , uint8_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolHandler::digitalInputChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ProtocolHandler::*)(int , const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolHandler::rawPacketReceived)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProtocolHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ProtocolHandler.data,
    qt_meta_data_ProtocolHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProtocolHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProtocolHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProtocolHandler.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ProtocolHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ProtocolHandler::registerReadResult(uint32_t _t1, uint32_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProtocolHandler::powerDataReceived(const QVector<QVariantMap> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProtocolHandler::ntcTempReceived(const QVector<QVariantMap> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ProtocolHandler::diffVoltageReceived(const QVector<QVariantMap> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ProtocolHandler::digitalInputChanged(bool _t1, uint8_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ProtocolHandler::rawPacketReceived(int _t1, const QByteArray & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
