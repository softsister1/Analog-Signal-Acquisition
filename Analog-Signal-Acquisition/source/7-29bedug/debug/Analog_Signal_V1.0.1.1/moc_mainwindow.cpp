/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../5_22/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[49];
    char stringdata0[786];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 22), // "onConnectButtonClicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 25), // "onDisconnectButtonClicked"
QT_MOC_LITERAL(4, 61, 19), // "onConnectionChanged"
QT_MOC_LITERAL(5, 81, 9), // "connected"
QT_MOC_LITERAL(6, 91, 13), // "updatePowerUI"
QT_MOC_LITERAL(7, 105, 23), // "onGroupSelectionChanged"
QT_MOC_LITERAL(8, 129, 5), // "index"
QT_MOC_LITERAL(9, 135, 20), // "onDebugButtonClicked"
QT_MOC_LITERAL(10, 156, 18), // "onPowerPlusChanged"
QT_MOC_LITERAL(11, 175, 7), // "checked"
QT_MOC_LITERAL(12, 183, 19), // "onPowerMinusChanged"
QT_MOC_LITERAL(13, 203, 13), // "onDIReadTimer"
QT_MOC_LITERAL(14, 217, 12), // "onDO1Changed"
QT_MOC_LITERAL(15, 230, 12), // "onDO2Changed"
QT_MOC_LITERAL(16, 243, 17), // "onAdcDelayChanged"
QT_MOC_LITERAL(17, 261, 5), // "value"
QT_MOC_LITERAL(18, 267, 20), // "onIoHighDelayChanged"
QT_MOC_LITERAL(19, 288, 19), // "onIoLowDelayChanged"
QT_MOC_LITERAL(20, 308, 19), // "onCycleCountChanged"
QT_MOC_LITERAL(21, 328, 28), // "onCyclePointsPerCycleChanged"
QT_MOC_LITERAL(22, 357, 20), // "onTriggerModeChanged"
QT_MOC_LITERAL(23, 378, 2), // "id"
QT_MOC_LITERAL(24, 381, 25), // "onStartAcquisitionClicked"
QT_MOC_LITERAL(25, 407, 17), // "onSaveDataClicked"
QT_MOC_LITERAL(26, 425, 20), // "onDisplayModeChanged"
QT_MOC_LITERAL(27, 446, 20), // "onCycleNumberChanged"
QT_MOC_LITERAL(28, 467, 22), // "onRegisterDataReceived"
QT_MOC_LITERAL(29, 490, 8), // "uint32_t"
QT_MOC_LITERAL(30, 499, 7), // "regAddr"
QT_MOC_LITERAL(31, 507, 7), // "regData"
QT_MOC_LITERAL(32, 515, 19), // "onPowerDataReceived"
QT_MOC_LITERAL(33, 535, 20), // "QVector<QVariantMap>"
QT_MOC_LITERAL(34, 556, 9), // "powerData"
QT_MOC_LITERAL(35, 566, 25), // "onTemperatureDataReceived"
QT_MOC_LITERAL(36, 592, 8), // "tempData"
QT_MOC_LITERAL(37, 601, 21), // "onVoltageDataReceived"
QT_MOC_LITERAL(38, 623, 11), // "voltageData"
QT_MOC_LITERAL(39, 635, 21), // "onDigitalInputChanged"
QT_MOC_LITERAL(40, 657, 8), // "isGroupA"
QT_MOC_LITERAL(41, 666, 7), // "uint8_t"
QT_MOC_LITERAL(42, 674, 7), // "diValue"
QT_MOC_LITERAL(43, 682, 15), // "onErrorOccurred"
QT_MOC_LITERAL(44, 698, 7), // "message"
QT_MOC_LITERAL(45, 706, 19), // "onStatusUpdateTimer"
QT_MOC_LITERAL(46, 726, 16), // "onStartRecvTimer"
QT_MOC_LITERAL(47, 743, 29), // "convertMicrosecondsToDateTime"
QT_MOC_LITERAL(48, 773, 12) // "timestamp_us"

    },
    "MainWindow\0onConnectButtonClicked\0\0"
    "onDisconnectButtonClicked\0onConnectionChanged\0"
    "connected\0updatePowerUI\0onGroupSelectionChanged\0"
    "index\0onDebugButtonClicked\0"
    "onPowerPlusChanged\0checked\0"
    "onPowerMinusChanged\0onDIReadTimer\0"
    "onDO1Changed\0onDO2Changed\0onAdcDelayChanged\0"
    "value\0onIoHighDelayChanged\0"
    "onIoLowDelayChanged\0onCycleCountChanged\0"
    "onCyclePointsPerCycleChanged\0"
    "onTriggerModeChanged\0id\0"
    "onStartAcquisitionClicked\0onSaveDataClicked\0"
    "onDisplayModeChanged\0onCycleNumberChanged\0"
    "onRegisterDataReceived\0uint32_t\0regAddr\0"
    "regData\0onPowerDataReceived\0"
    "QVector<QVariantMap>\0powerData\0"
    "onTemperatureDataReceived\0tempData\0"
    "onVoltageDataReceived\0voltageData\0"
    "onDigitalInputChanged\0isGroupA\0uint8_t\0"
    "diValue\0onErrorOccurred\0message\0"
    "onStatusUpdateTimer\0onStartRecvTimer\0"
    "convertMicrosecondsToDateTime\0"
    "timestamp_us"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  164,    2, 0x08 /* Private */,
       3,    0,  165,    2, 0x08 /* Private */,
       4,    1,  166,    2, 0x08 /* Private */,
       6,    0,  169,    2, 0x08 /* Private */,
       7,    1,  170,    2, 0x08 /* Private */,
       9,    0,  173,    2, 0x08 /* Private */,
      10,    1,  174,    2, 0x08 /* Private */,
      12,    1,  177,    2, 0x08 /* Private */,
      13,    0,  180,    2, 0x08 /* Private */,
      14,    1,  181,    2, 0x08 /* Private */,
      15,    1,  184,    2, 0x08 /* Private */,
      16,    1,  187,    2, 0x08 /* Private */,
      18,    1,  190,    2, 0x08 /* Private */,
      19,    1,  193,    2, 0x08 /* Private */,
      20,    1,  196,    2, 0x08 /* Private */,
      21,    1,  199,    2, 0x08 /* Private */,
      22,    1,  202,    2, 0x08 /* Private */,
      24,    0,  205,    2, 0x08 /* Private */,
      25,    0,  206,    2, 0x08 /* Private */,
      26,    1,  207,    2, 0x08 /* Private */,
      27,    1,  210,    2, 0x08 /* Private */,
      28,    2,  213,    2, 0x08 /* Private */,
      32,    1,  218,    2, 0x08 /* Private */,
      35,    1,  221,    2, 0x08 /* Private */,
      37,    1,  224,    2, 0x08 /* Private */,
      39,    2,  227,    2, 0x08 /* Private */,
      43,    1,  232,    2, 0x08 /* Private */,
      45,    0,  235,    2, 0x08 /* Private */,
      46,    0,  236,    2, 0x08 /* Private */,
      47,    1,  237,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void, QMetaType::Double,   17,
    QMetaType::Void, QMetaType::Double,   17,
    QMetaType::Void, QMetaType::Double,   17,
    QMetaType::Void, QMetaType::Double,   17,
    QMetaType::Void, QMetaType::Double,   17,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, 0x80000000 | 29, 0x80000000 | 29,   30,   31,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void, 0x80000000 | 33,   36,
    QMetaType::Void, 0x80000000 | 33,   38,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 41,   40,   42,
    QMetaType::Void, QMetaType::QString,   44,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::QString, QMetaType::LongLong,   48,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onConnectButtonClicked(); break;
        case 1: _t->onDisconnectButtonClicked(); break;
        case 2: _t->onConnectionChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->updatePowerUI(); break;
        case 4: _t->onGroupSelectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->onDebugButtonClicked(); break;
        case 6: _t->onPowerPlusChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->onPowerMinusChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->onDIReadTimer(); break;
        case 9: _t->onDO1Changed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->onDO2Changed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->onAdcDelayChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: _t->onIoHighDelayChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: _t->onIoLowDelayChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: _t->onCycleCountChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->onCyclePointsPerCycleChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->onTriggerModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->onStartAcquisitionClicked(); break;
        case 18: _t->onSaveDataClicked(); break;
        case 19: _t->onDisplayModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->onCycleNumberChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->onRegisterDataReceived((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 22: _t->onPowerDataReceived((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 23: _t->onTemperatureDataReceived((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 24: _t->onVoltageDataReceived((*reinterpret_cast< const QVector<QVariantMap>(*)>(_a[1]))); break;
        case 25: _t->onDigitalInputChanged((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< uint8_t(*)>(_a[2]))); break;
        case 26: _t->onErrorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 27: _t->onStatusUpdateTimer(); break;
        case 28: _t->onStartRecvTimer(); break;
        case 29: { QString _r = _t->convertMicrosecondsToDateTime((*reinterpret_cast< qint64(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 22:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVariantMap> >(); break;
            }
            break;
        case 23:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVariantMap> >(); break;
            }
            break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVariantMap> >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
