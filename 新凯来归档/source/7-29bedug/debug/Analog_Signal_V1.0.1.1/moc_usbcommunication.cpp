/****************************************************************************
** Meta object code from reading C++ file 'usbcommunication.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../5_22/usbcommunication.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'usbcommunication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UsbWorker_t {
    QByteArrayData data[18];
    char stringdata0[174];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UsbWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UsbWorker_t qt_meta_stringdata_UsbWorker = {
    {
QT_MOC_LITERAL(0, 0, 9), // "UsbWorker"
QT_MOC_LITERAL(1, 10, 9), // "connected"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 12), // "disconnected"
QT_MOC_LITERAL(4, 34, 5), // "error"
QT_MOC_LITERAL(5, 40, 7), // "message"
QT_MOC_LITERAL(6, 48, 12), // "dataReceived"
QT_MOC_LITERAL(7, 61, 4), // "data"
QT_MOC_LITERAL(8, 66, 10), // "ftdiStatus"
QT_MOC_LITERAL(9, 77, 6), // "status"
QT_MOC_LITERAL(10, 84, 10), // "initialize"
QT_MOC_LITERAL(11, 95, 7), // "cleanup"
QT_MOC_LITERAL(12, 103, 8), // "sendData"
QT_MOC_LITERAL(13, 112, 9), // "abortPipe"
QT_MOC_LITERAL(14, 122, 5), // "UCHAR"
QT_MOC_LITERAL(15, 128, 6), // "pipeID"
QT_MOC_LITERAL(16, 135, 19), // "startContinuousRead"
QT_MOC_LITERAL(17, 155, 18) // "stopContinuousRead"

    },
    "UsbWorker\0connected\0\0disconnected\0"
    "error\0message\0dataReceived\0data\0"
    "ftdiStatus\0status\0initialize\0cleanup\0"
    "sendData\0abortPipe\0UCHAR\0pipeID\0"
    "startContinuousRead\0stopContinuousRead"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UsbWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    0,   70,    2, 0x06 /* Public */,
       4,    1,   71,    2, 0x06 /* Public */,
       6,    1,   74,    2, 0x06 /* Public */,
       8,    1,   77,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   80,    2, 0x0a /* Public */,
      11,    0,   81,    2, 0x0a /* Public */,
      12,    1,   82,    2, 0x0a /* Public */,
      13,    1,   85,    2, 0x0a /* Public */,
      16,    0,   88,    2, 0x0a /* Public */,
      17,    0,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QByteArray,    7,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    7,
    QMetaType::Bool, 0x80000000 | 14,   15,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UsbWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UsbWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->error((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->dataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 4: _t->ftdiStatus((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->initialize(); break;
        case 6: _t->cleanup(); break;
        case 7: _t->sendData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 8: { bool _r = _t->abortPipe((*reinterpret_cast< UCHAR(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 9: _t->startContinuousRead(); break;
        case 10: _t->stopContinuousRead(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UsbWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbWorker::connected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UsbWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbWorker::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (UsbWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbWorker::error)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (UsbWorker::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbWorker::dataReceived)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (UsbWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbWorker::ftdiStatus)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UsbWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_UsbWorker.data,
    qt_meta_data_UsbWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UsbWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UsbWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UsbWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UsbWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void UsbWorker::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void UsbWorker::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void UsbWorker::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void UsbWorker::dataReceived(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void UsbWorker::ftdiStatus(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
struct qt_meta_stringdata_UsbCommunication_t {
    QByteArrayData data[31];
    char stringdata0[439];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UsbCommunication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UsbCommunication_t qt_meta_stringdata_UsbCommunication = {
    {
QT_MOC_LITERAL(0, 0, 16), // "UsbCommunication"
QT_MOC_LITERAL(1, 17, 17), // "connectionChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 9), // "connected"
QT_MOC_LITERAL(4, 46, 13), // "errorOccurred"
QT_MOC_LITERAL(5, 60, 7), // "message"
QT_MOC_LITERAL(6, 68, 12), // "dataReceived"
QT_MOC_LITERAL(7, 81, 4), // "data"
QT_MOC_LITERAL(8, 86, 17), // "ftdiStatusChanged"
QT_MOC_LITERAL(9, 104, 6), // "status"
QT_MOC_LITERAL(10, 111, 15), // "rawDataReceived"
QT_MOC_LITERAL(11, 127, 20), // "registerDataReceived"
QT_MOC_LITERAL(12, 148, 8), // "uint32_t"
QT_MOC_LITERAL(13, 157, 7), // "regAddr"
QT_MOC_LITERAL(14, 165, 7), // "regData"
QT_MOC_LITERAL(15, 173, 17), // "powerDataReceived"
QT_MOC_LITERAL(16, 191, 20), // "QVector<QVariantMap>"
QT_MOC_LITERAL(17, 212, 9), // "powerData"
QT_MOC_LITERAL(18, 222, 23), // "temperatureDataReceived"
QT_MOC_LITERAL(19, 246, 8), // "tempData"
QT_MOC_LITERAL(20, 255, 19), // "voltageDataReceived"
QT_MOC_LITERAL(21, 275, 11), // "voltageData"
QT_MOC_LITERAL(22, 287, 19), // "digitalInputChanged"
QT_MOC_LITERAL(23, 307, 8), // "isGroupA"
QT_MOC_LITERAL(24, 316, 7), // "uint8_t"
QT_MOC_LITERAL(25, 324, 7), // "diValue"
QT_MOC_LITERAL(26, 332, 21), // "handleWorkerConnected"
QT_MOC_LITERAL(27, 354, 24), // "handleWorkerDisconnected"
QT_MOC_LITERAL(28, 379, 17), // "handleWorkerError"
QT_MOC_LITERAL(29, 397, 24), // "handleWorkerDataReceived"
QT_MOC_LITERAL(30, 422, 16) // "handleFtdiStatus"

    },
    "UsbCommunication\0connectionChanged\0\0"
    "connected\0errorOccurred\0message\0"
    "dataReceived\0data\0ftdiStatusChanged\0"
    "status\0rawDataReceived\0registerDataReceived\0"
    "uint32_t\0regAddr\0regData\0powerDataReceived\0"
    "QVector<QVariantMap>\0powerData\0"
    "temperatureDataReceived\0tempData\0"
    "voltageDataReceived\0voltageData\0"
    "digitalInputChanged\0isGroupA\0uint8_t\0"
    "diValue\0handleWorkerConnected\0"
    "handleWorkerDisconnected\0handleWorkerError\0"
    "handleWorkerDataReceived\0handleFtdiStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UsbCommunication[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x06 /* Public */,
       4,    1,   92,    2, 0x06 /* Public */,
       6,    1,   95,    2, 0x06 /* Public */,
       8,    1,   98,    2, 0x06 /* Public */,
      10,    1,  101,    2, 0x06 /* Public */,
      11,    2,  104,    2, 0x06 /* Public */,
      15,    1,  109,    2, 0x06 /* Public */,
      18,    1,  112,    2, 0x06 /* Public */,
      20,    1,  115,    2, 0x06 /* Public */,
      22,    2,  118,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      26,    0,  123,    2, 0x08 /* Private */,
      27,    0,  124,    2, 0x08 /* Private */,
      28,    1,  125,    2, 0x08 /* Private */,
      29,    1,  128,    2, 0x08 /* Private */,
      30,    1,  131,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QByteArray,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QByteArray,    7,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12,   13,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16,   19,
    QMetaType::Void, 0x80000000 | 16,   21,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 24,   23,   25,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QByteArray,    7,
    QMetaType::Void, QMetaType::QString,    9,

       0        // eod
};

void UsbCommunication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UsbCommunication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connectionChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->dataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 3: _t->ftdiStatusChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->rawDataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 5: _t->registerDataReceived((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 6: _t->powerDataReceived((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 7: _t->temperatureDataReceived((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 8: _t->voltageDataReceived((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 9: _t->digitalInputChanged((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< uint8_t(*)>(_a[2]))); break;
        case 10: _t->handleWorkerConnected(); break;
        case 11: _t->handleWorkerDisconnected(); break;
        case 12: _t->handleWorkerError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->handleWorkerDataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 14: _t->handleFtdiStatus((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVariantMap> >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVariantMap> >(); break;
            }
            break;
        case 8:
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
            using _t = void (UsbCommunication::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::connectionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UsbCommunication::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::errorOccurred)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (UsbCommunication::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::dataReceived)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (UsbCommunication::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::ftdiStatusChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (UsbCommunication::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::rawDataReceived)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (UsbCommunication::*)(uint32_t , uint32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::registerDataReceived)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (UsbCommunication::*)(const QVector<QVariantMap> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::powerDataReceived)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (UsbCommunication::*)(const QVector<QVariantMap> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::temperatureDataReceived)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (UsbCommunication::*)(const QVector<QVariantMap> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::voltageDataReceived)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (UsbCommunication::*)(bool , uint8_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UsbCommunication::digitalInputChanged)) {
                *result = 9;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UsbCommunication::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_UsbCommunication.data,
    qt_meta_data_UsbCommunication,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UsbCommunication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UsbCommunication::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UsbCommunication.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UsbCommunication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void UsbCommunication::connectionChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UsbCommunication::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void UsbCommunication::dataReceived(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void UsbCommunication::ftdiStatusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void UsbCommunication::rawDataReceived(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void UsbCommunication::registerDataReceived(uint32_t _t1, uint32_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void UsbCommunication::powerDataReceived(const QVector<QVariantMap> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void UsbCommunication::temperatureDataReceived(const QVector<QVariantMap> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void UsbCommunication::voltageDataReceived(const QVector<QVariantMap> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void UsbCommunication::digitalInputChanged(bool _t1, uint8_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
