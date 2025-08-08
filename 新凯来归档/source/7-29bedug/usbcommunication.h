#ifndef USBCOMMUNICATION_H
#define USBCOMMUNICATION_H

#include <QObject>
#include <QByteArray>
#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QTimer>
#include <QWaitCondition>
#include <QVector>
#include <QVariantMap>
#include <atomic>
#include <thread>
#include <windows.h>
#include "FTD3XX.h"
#include <QElapsedTimer>

// 协议头部常量
#define PROTOCOL_HEADER_WORD1 0xf0a5
#define PROTOCOL_HEADER_WORD2 0x5a0f

// 命令类型定义
#define CMD_RESERVED     0x00
#define CMD_READ_REG     0x01
#define CMD_WRITE_REG    0x02
#define CMD_POWER_DATA   0x03
#define CMD_NTC_TEMP     0x04
#define CMD_DIFF_VOLTAGE 0x05

// 寄存器地址定义
#define REG_VERSION            0x00000000  // FPGA逻辑版本号
#define REG_TEST               0x00000001  // 测试寄存器
#define REG_STATUS             0x00000002  // 状态寄存器
#define REG_UTCLOW             0x00000080  // utc时间戳低八位寄存器
#define REG_UTCHIGH            0x00000081  // utc时间戳高八位寄存器

// A组寄存器
#define REG_A_START_ACQUISITION 0x00000020 // A组开始采集控制
#define REG_A_ADC_DELAY         0x00000021 // A组ADC开始采样时刻相对于触发信号的延时
#define REG_A_IO_LOW_DELAY      0x00000022 // A组数字控制IO拉低相对于触发信号的延时
#define REG_A_IO_HIGH_DELAY     0x00000023 // A组数字控制IO拉高相对于触发信号的延时
#define REG_A_CYCLE_POINT_COUNT 0x00000024 // A组每个周期采样点数
#define REG_A_CYCLE_COUNT       0x00000025 // A组采样周期数
#define REG_A_TRIGGER_MODE      0x00000026 // A组触发模式选择
#define REG_A_POWER_PLUS        0x00000027 // A组+5V电源使能
#define REG_A_POWER_MINUS       0x00000028 // A组-5V电源使能
#define REG_A_DI                0x00000029 // A组数字输入
#define REG_A_DO1               0x0000002A // A组数字输出1
#define REG_A_DO2               0x0000002B // A组数字输出2

// B组寄存器
#define REG_B_START_ACQUISITION 0x00000040 // B组开始采集控制
#define REG_B_ADC_DELAY         0x00000041 // B组ADC开始采样时刻相对于触发信号的延时
#define REG_B_IO_LOW_DELAY      0x00000042 // B组数字控制IO拉低相对于触发信号的延时
#define REG_B_IO_HIGH_DELAY     0x00000043 // B组数字控制IO拉高相对于触发信号的延时
#define REG_B_CYCLE_POINT_COUNT 0x00000044 // B组每个周期采样点数
#define REG_B_CYCLE_COUNT       0x00000045 // B组采样周期数
#define REG_B_TRIGGER_MODE      0x00000046 // B组触发模式选择
#define REG_B_POWER_PLUS        0x00000047 // B组+5V电源使能
#define REG_B_POWER_MINUS       0x00000048 // B组-5V电源使能
#define REG_B_DI                0x00000049 // B组数字输入
#define REG_B_DO1               0x0000004A // B组数字输出1
#define REG_B_DO2               0x0000004B // B组数字输出2

// FT600 特定配置常量
#define FT600_WRITE_PIPE 0x02
#define FT600_READ_PIPE 0x82
#define FT600_BUFFER_SIZE 40960

// USB通信工作线程类
class UsbWorker : public QObject {
    Q_OBJECT
public:
    void processAccumulatedData(const QByteArray& data);
    explicit UsbWorker(QObject *parent = nullptr);
    ~UsbWorker();
    void flushReadPipe();
    void setRunning(bool run);
    bool isConnected() const;
    void errorEmittedInit();
    void resetErrorState();
    void handleSendFailure();
    void flushBuffers() {
        if (ftHandle) {
            // 清空读管道
            FT_FlushPipe(ftHandle, FT600_READ_PIPE);

            // 清空内部缓冲区
            QMutexLocker locker(&readBufferMutex);
            readBuffer.clear();

            // 清空处理队列
            QMutexLocker processLocker(&processMutex);
            processQueue.clear();
        }
    }
public slots:
    void initialize();
    void cleanup();
    void sendData(const QByteArray &data);

signals:
    void connected();
    void disconnected();
    void error(const QString &message);
    void dataReceived(const QByteArray &data);
    void ftdiStatus(const QString &status);

private:
    // 三个线程函数
    void sendDataThread();
    void recvDataThread();
    void processDataThread();

    void requestReconnect();

    // 设备操作
    bool findAndOpenDevice();
    void closeDevice();
    bool writeToDevice(const QByteArray &data);
    QByteArray readFromDevice(ULONG64 size);
    bool setupFt600Configuration();
    bool sendSyncPattern();
    bool writeGpio(uint32_t mask, uint32_t value);
    bool readGpio(uint32_t *pValue);

    // 数据处理
    void processBufferedData();
    int findCompletePacket(const QByteArray& buffer, int offset);

    // 基本状态
    FT_HANDLE ftHandle;
    std::atomic<bool> isRunning;
    std::atomic<bool> deviceConnected;
    UCHAR deviceMode;
    UCHAR channelConfig;

    // 线程控制
    std::atomic<bool> sendThreadRunning;
    std::atomic<bool> recvThreadRunning;
    std::atomic<bool> processThreadRunning;
    std::atomic<bool> continuousReadMode;

    // 数据队列和缓冲区
    QQueue<QByteArray> sendQueue;        // 发送队列
    QQueue<QByteArray> processQueue;     // 处理队列
    QByteArray readBuffer;               // 接收缓冲区

    // 线程同步
    QMutex sendMutex;          // 发送队列互斥锁
    QMutex processMutex;       // 处理队列互斥锁
    QMutex readBufferMutex;    // 读缓冲区互斥锁

    QWaitCondition sendCondition;     // 发送等待条件
    QWaitCondition processCondition;  // 处理等待条件

    // C++ 标准线程指针
    std::thread *sendThread;
    std::thread *recvThread;
    std::thread *processThread;

    // 常量
    const int MAX_BUFFER_SIZE = 2 * 1024 * 1024;
    const int MAX_SEND_QUEUE_SIZE = 10;
    const int MAX_PROCESS_QUEUE_SIZE = 100;

    // 兼容性（保持原有接口）
    QMutex mutex;
    QQueue<QByteArray> dataToSend;
    QWaitCondition condition;
    void processReceivedData();
    void continuousReadLoop();

    bool errorEmitted;
    bool reconnectInProgress;  // 新增：重连进行中标志
    int reconnectAttempts;     // 新增：重连尝试次数
    static const int MAX_RECONNECT_ATTEMPTS = 3;  // 最大重连次数
};

// 整合了ProtocolHandler功能的UsbCommunication类
class UsbCommunication : public QObject {
    Q_OBJECT
public:
    static void getAllGroupData(int group,
                                   QVector<QVector<double>>& voltageData,
                                   QVector<QVector<uint32_t>>& rawData,
                                   QVector<QVector<uint32_t>>& ts1Data,
                                   QVector<QVector<uint32_t>>& ts2Data);
    // 设置采集参数
        void setAcquisitionParams(int cycles, int group, int pointsPerCycle);

        // 获取数据状态
        static bool isDataReady() { return g_dataReady; }
        static void clearDataCache();
        static void getVoltageData(int group, int channel, QVector<double>& data);
        static void getRawData(int group, int channel, QVector<uint32_t>& data);
        static void getTimestampData(int group, int channel, QVector<uint32_t>& ts1, QVector<uint32_t>& ts2);

    explicit UsbCommunication(QObject *parent = nullptr);
    ~UsbCommunication();
    void setActiveGroup(bool isGroupA) { this->isGroupA = isGroupA; }
    bool getActiveGroup() const { return isGroupA; }
    bool isConnected() const;
    void startCommunication();
    void stopCommunication();

    // 发送原始数据
    bool sendData(const QByteArray &data);

    // 中止管道传输
    bool abortTransfer(bool isReadPipe);

    // 协议接口 - 寄存器操作
    bool readRegister(uint32_t regAddr);
    bool writeRegister(uint32_t regAddr, uint32_t regData);

    // 协议接口 - 控制功能
    bool setPowerControl(bool isGroupA, bool plusEnable, bool minusEnable);
    bool setDigitalOutput(bool isGroupA, bool do1Value, bool do2Value);
    bool setTriggerMode(int mode); // 0=内触发, 1=外触发1, 2=外触发2
    bool setDelayConfig(uint32_t adcDelay, uint32_t ioHighDelay, uint32_t ioLowDelay);
    bool setSamplingConfig(uint32_t cycleCount, uint32_t pointsPerCycle);
    bool startAcquisition();

    // 查收数据次数
    void printCount();
    void setCountClear();

    // 关闭连接
    void closeConnect();

    // 从ProtocolHandler移植的协议包创建函数
    QByteArray createPacketHeader(uint16_t cmd, uint16_t dataLength);
    QByteArray createRegisterReadPacket(uint32_t regAddr);
    QByteArray createRegisterWritePacket(uint32_t regAddr, uint32_t regData);
    QByteArray createPowerControlPacket(bool isGroupA, bool plusEnable, bool minusEnable);
    QByteArray createDigitalOutputPacket(bool isGroupA, bool do1Value, bool do2Value);
    QByteArray createTriggerModePacket(bool isGroupA, int mode);
    QByteArray createDelayConfigPacket(bool isGroupA, uint32_t adcDelay, uint32_t ioHighDelay, uint32_t ioLowDelay);
    QByteArray createStartAcquisitionPacket(bool isGroupA);
    // 从ProtocolHandler移植的数据包解析函数
    bool parsePacket(const QByteArray &data);

    // 解析不同类型的数据包
    bool parseRegisterResponse(const QByteArray &data, int dataOffset, int dataLength);
    bool parsePowerData(const QByteArray &data, int dataOffset, int dataLength);
    bool parseNtcTemp(const QByteArray &data, int dataOffset, int dataLength);
    bool parseDiffVoltage(const QByteArray &data, int dataOffset, int dataLength);

    // 辅助函数
    uint16_t calculateHeaderCRC(uint16_t cmd, uint16_t length);
    uint16_t calculateDataCRC(const QByteArray &data, int offset, int length);
    void appendDataWithCRC(QByteArray &packet, const QByteArray &data);
    void logMessage(const QString &message, bool isError = false);
    QVector<QVariantMap> getParsedData() const;
    void clearParsedData();

    // 转换函数
    double convertRawToVoltage(uint32_t rawValue);
    double convertRawToVoltage1(uint32_t rawValue, int channel);
    double convertNtcToTemperature(uint32_t ntcValue);
    double convertResistanceToTemperature(double resistance);

    // 字节序转换辅助函数
    uint16_t bytesToUInt16(const QByteArray &data, int offset);
    uint16_t bytesToUInt16_1(const QByteArray &data, int offset);
    uint32_t bytesToUInt32(const QByteArray &data, int offset);
    uint64_t bytesToUInt64(const QByteArray &data, int offset);

signals:
    void connectionChanged(bool connected);
    void errorOccurred(const QString &message);
    void dataReceived(const QByteArray &data);
    void ftdiStatusChanged(const QString &status);
    void rawDataReceived(const QByteArray& data);
    // 协议相关信号
    void registerReadResult(uint32_t regAddr, uint32_t regData);
    void powerDataReceived(const QVector<QVariantMap> &powerData);
    void temperatureDataReceived(const QVector<QVariantMap> &tempData);
    void voltageDataReceived(const QVector<QVariantMap> &voltageData);
    void digitalInputChanged(bool isGroupA, uint8_t diValue);
    void acquisitionCompleted(int totalPackets); // 只在完成时发送一次信号
    void acquisitionProgress(int current, int total); // 进度信号，降低频率
private slots:
    void handleWorkerConnected();
    void handleWorkerDisconnected();
    void handleWorkerError(const QString &message);
    void handleWorkerDataReceived(const QByteArray &data);
    void handleFtdiStatus(const QString &status);

private:
    // 添加数据收集相关成员
      int _count = 0;
      int _expectedTotalPackets = 0;
      int _activeGroup = 0;
      bool _acquisitionInProgress = false;

      // 全局数据缓存，避免信号槽传输
      static QVector<QVector<QVector<double>>> g_voltageDataCache;  // [group][channel][data]
      static QVector<QVector<QVector<uint32_t>>> g_rawDataCache;
      static QVector<QVector<QVector<uint32_t>>> g_timestamp1Cache;
      static QVector<QVector<QVector<uint32_t>>> g_timestamp2Cache;
      static QMutex g_dataCacheMutex;
      static bool g_dataReady;
    QThread workerThread;
    UsbWorker *worker;
    bool connected;
    bool isGroupA;

    // 原ProtocolHandler中的成员变量
    QVector<QVariantMap> parsedDataList;

    int _processEventsCounter;
};

#endif // USBCOMMUNICATION_H
