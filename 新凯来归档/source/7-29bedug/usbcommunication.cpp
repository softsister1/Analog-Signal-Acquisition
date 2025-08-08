#include "usbcommunication.h"
#include <QDebug>
#include <QtEndian>
#include <QtMath>
#include <QThread>
#include <QCoreApplication>
#include <chrono>
#include <QDateTime>
#include "windows.h"
int count1 = 0;
#pragma execution_character_set("UTF-8")
// UsbWorker 实现
UsbWorker::UsbWorker(QObject *parent) : QObject(parent),
    ftHandle(nullptr),
    isRunning(false),
    deviceConnected(false),
    deviceMode(CONFIGURATION_FIFO_MODE_245),
    channelConfig(CONFIGURATION_CHANNEL_CONFIG_1),
    sendThreadRunning(false),
    recvThreadRunning(false),
    processThreadRunning(false),
    continuousReadMode(false),
    sendThread(nullptr),
    recvThread(nullptr),
    processThread(nullptr),
    errorEmitted(false)
{
}

UsbWorker::~UsbWorker()
{
    cleanup();
}

void UsbWorker::setRunning(bool run)
{
    isRunning = run;
    sendThreadRunning = run;
    recvThreadRunning = run;
    processThreadRunning = run;

    if (!run) {
        continuousReadMode = false;
        sendCondition.wakeAll();
        processCondition.wakeAll();
        condition.wakeAll();
    }
}

bool UsbWorker::isConnected() const
{
    return deviceConnected;
}

void UsbWorker::errorEmittedInit()
{
    errorEmitted = false;
}

void UsbWorker::initialize()
{
    emit ftdiStatus("正在初始化设备...");

    // 重置错误状态
    errorEmitted = false;

    if (findAndOpenDevice()) {
        deviceConnected = true;
        emit connected();
        emit ftdiStatus("设备已连接，正在配置...");

        // 设置 FT600 配置
        if (!setupFt600Configuration()) {
            emit error("设备配置失败");
            closeDevice();
            deviceConnected = false;
            emit disconnected();
            return;
        }

        emit ftdiStatus("设备配置成功，正在发送同步序列...");

        // 发送初始同步序列
        if (!sendSyncPattern()) {
            emit ftdiStatus("同步序列发送失败，但将继续");
        } else {
            emit ftdiStatus("同步序列发送成功");
        }

        // 启动三个线程
        setRunning(true);

        qDebug() << "启动数据发送线程...";
        sendThread = new std::thread(&UsbWorker::sendDataThread, this);

        qDebug() << "启动数据接收线程...";
        recvThread = new std::thread(&UsbWorker::recvDataThread, this);

         //SetThreadPriority(recvThread, THREAD_PRIORITY_HIGHEST);

        qDebug() << "启动数据处理线程...";
        processThread = new std::thread(&UsbWorker::processDataThread, this);

        emit ftdiStatus("所有线程已启动，设备就绪");

        // 主循环等待直到停止
        while (isRunning && deviceConnected) {
            QThread::msleep(100);
            QCoreApplication::processEvents();
        }

        // 等待所有线程结束
        qDebug() << "等待线程结束...";
        if (sendThread && sendThread->joinable()) {
            sendThread->join();
            delete sendThread;
            sendThread = nullptr;
        }
        if (recvThread && recvThread->joinable()) {
            recvThread->join();
            delete recvThread;
            recvThread = nullptr;
        }
        if (processThread && processThread->joinable()) {
            processThread->join();
            delete processThread;
            processThread = nullptr;
        }

        closeDevice();
        deviceConnected = false;
        emit disconnected();
    } else {
        emit error("无法连接到USB设备");
    }
}
void UsbWorker::resetErrorState()
{
    errorEmitted = false;
    qDebug() << "错误状态已重置";
}
void UsbWorker::sendDataThread()
{
    qDebug() << "数据发送线程启动";

    while (isRunning && deviceConnected) {
        QByteArray dataToSendItem;

        // 从发送队列获取数据
        {
            QMutexLocker locker(&sendMutex);
            if (sendQueue.isEmpty()) {
                QThread::msleep(1); // 避免CPU占用过高
                continue;
            }
            dataToSendItem = sendQueue.dequeue();
        }

        // 发送数据
        if (!dataToSendItem.isEmpty()) {
            if (!writeToDevice(dataToSendItem)) {
                if (!errorEmitted) {
                    errorEmitted = true;
                    emit error("数据发送失败");
                    qDebug() << "设备通信失败，已发送错误信号";
                }

                break; // 退出发送循环
            } else {

                if (errorEmitted) {
                    errorEmitted = false;
                    qDebug() << "发送恢复成功，已重置错误状态";
                }
            }
        }
    }

    qDebug() << "数据发送线程退出";
}
void UsbWorker::handleSendFailure()
{
    qDebug() << "处理发送失败：自动断开连接";
    emit ftdiStatus("发送失败，正在断开连接...");

    // 停止运行标志，这会导致所有线程退出
    setRunning(false);

    // 关闭设备连接
    closeDevice();
    deviceConnected = false;

    // 发送断开信号
    emit disconnected();
    emit ftdiStatus("发送失败，连接已断开，请手动重新连接");

    qDebug() << "自动断开完成，等待用户手动重连";
}
void UsbWorker::recvDataThread()
{
    qDebug() << "数据接收线程启动 - 使用同步模式";

    // 确保设置流管道
    FT_STATUS ftStatus = FT_SetStreamPipe(ftHandle, FALSE, TRUE, FT600_READ_PIPE, FT600_BUFFER_SIZE);
    if (ftStatus != FT_OK) {
        qDebug() << "设置读管道流传输失败. 状态:" << ftStatus;
        return;
    }

    const int BUFFER_SIZE = FT600_BUFFER_SIZE * 16;
    QByteArray buffer(BUFFER_SIZE, 0);

    while (isRunning && deviceConnected) {
        DWORD bytesReceived = 0;
        ftStatus = FT_ReadPipe(ftHandle, FT600_READ_PIPE,
                              (PUCHAR)buffer.data(),
                              FT600_BUFFER_SIZE,
                              &bytesReceived, NULL);

        if (ftStatus == FT_OK && bytesReceived > 0) {
            QByteArray receivedData = buffer.left(bytesReceived);
            QMutexLocker locker(&processMutex);
            processQueue.enqueue(receivedData);
            processCondition.wakeAll();
        }
        else if (ftStatus != FT_TIMEOUT) {
            qDebug() << "读取失败. 状态:" << ftStatus;
            QThread::msleep(10); // 出错时短暂延迟
        }
        else {
            QThread::msleep(1); // 无数据时延迟，减少CPU占用
        }
    }
}

void UsbWorker::processDataThread()
{
    qDebug() << "数据处理线程启动";

    while (isRunning&&deviceConnected) {
        QByteArray dataToProcessItem;

        // 从处理队列获取数据
        {
            QMutexLocker locker(&processMutex);
            if (processQueue.isEmpty()) {
                processCondition.wait(&processMutex, 100);
                continue;
            }
            dataToProcessItem = processQueue.dequeue();
        }

        // 处理数据
        if (!dataToProcessItem.isEmpty()) {
            // 添加到读缓冲区
            {
                //QMutexLocker locker(&readBufferMutex);
                readBuffer.append(dataToProcessItem);
            }

            // 处理缓冲区中的完整数据包
            processBufferedData();
        }
    }

    qDebug() << "数据处理线程退出";
}

void UsbWorker::processBufferedData()
{
    QMutexLocker locker(&readBufferMutex);

    // 快速查找所有包头位置
    QVector<int> headerPositions;
    for (int i = 0; i < readBuffer.size() - 3; i++) {
        if ((static_cast<uint8_t>(readBuffer[i]) == 0xF0) &&
            (static_cast<uint8_t>(readBuffer[i+1]) == 0xA5) &&
            (static_cast<uint8_t>(readBuffer[i+2]) == 0x5A) &&
            (static_cast<uint8_t>(readBuffer[i+3]) == 0x0F)) {
            headerPositions.append(i);
        }
    }

    // 处理所有找到的包头
    int lastProcessedPos = 0;
    for (int pos : headerPositions) {
        if (pos + 12 > readBuffer.size()) {
            break; // 头部不完整
        }

        int packetSize = findCompletePacket(readBuffer, pos);
//        qDebug() << packetSize;
        if (packetSize > 0 && pos + packetSize <= readBuffer.size()) {
            QByteArray packet = readBuffer.mid(pos, packetSize);
            emit dataReceived(packet);
            lastProcessedPos = pos + packetSize;
        }
    }

    // 移除已处理数据
    if (lastProcessedPos > 0) {
        readBuffer.remove(0, lastProcessedPos);
    }

}

void UsbWorker::sendData(const QByteArray &data)
{
    QMutexLocker locker(&sendMutex);
    sendQueue.enqueue(data);

    sendCondition.wakeAll();
}

void UsbWorker::cleanup()
{
    qDebug() << "开始清理UsbWorker...";

    setRunning(false);

    // 等待所有线程结束并删除
    if (sendThread) {
        if (sendThread->joinable()) {
            sendThread->join();
        }
        delete sendThread;
        sendThread = nullptr;
    }

    if (recvThread) {
        if (recvThread->joinable()) {
            recvThread->join();
        }
        delete recvThread;
        recvThread = nullptr;
    }

    if (processThread) {
        if (processThread->joinable()) {
            processThread->join();
        }
        delete processThread;
        processThread = nullptr;
    }

    closeDevice();

    // 清空所有队列
    {
        QMutexLocker locker(&sendMutex);
        sendQueue.clear();
    }

    {
        QMutexLocker locker(&processMutex);
        processQueue.clear();
    }

    {
        QMutexLocker locker(&readBufferMutex);
        readBuffer.clear();
    }

    qDebug() << "UsbWorker清理完成";
}






bool UsbWorker::findAndOpenDevice()
{
    qDebug() << "查找 FT600 设备...";

    DWORD numDevs = 0;
    FT_STATUS ftStatus = FT_CreateDeviceInfoList(&numDevs);

    if (ftStatus != FT_OK || numDevs == 0) {
        qDebug() << "未找到 FT600 设备. 状态:" << ftStatus << ", 设备数量:" << numDevs;
        return false;
    }

    qDebug() << "找到" << numDevs << "个 FTDI 设备";

    // 查看设备信息
    FT_DEVICE_LIST_INFO_NODE *devInfo = new FT_DEVICE_LIST_INFO_NODE[numDevs];
    ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);

    if (ftStatus == FT_OK) {
        for (DWORD i = 0; i < numDevs; i++) {
            qDebug() << "设备" << i << ":";
            qDebug() << "  描述:" << devInfo[i].Description;
            qDebug() << "  序列号:" << devInfo[i].SerialNumber;
            qDebug() << "  ID:" << devInfo[i].ID;
            qDebug() << "  类型:" << devInfo[i].Type;
            qDebug() << "  标志:" << devInfo[i].Flags;
        }
    }

    delete[] devInfo;

    // 打开第一个设备
    ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
    if (ftStatus != FT_OK) {
        qDebug() << "无法打开 FT600 设备. 状态:" << ftStatus;
        return false;
    }

    qDebug() << "设备已成功打开";

    // 获取 VID/PID 以验证设备类型
    USHORT vid, pid;
    ftStatus = FT_GetVIDPID(ftHandle, &vid, &pid);
    if (ftStatus == FT_OK) {
        qDebug() << "设备 VID:" << QString("0x%1").arg(vid, 4, 16, QChar('0'))
                 << "PID:" << QString("0x%1").arg(pid, 4, 16, QChar('0'));

        // 确认是 FT600 设备
        if (pid != 0x601E) { // FT600 的 PID 是 0x601E
            qDebug() << "警告: 这可能不是 FT600 设备，PID 不匹配";
        }
    }

    // 设置管道超时
    ftStatus = FT_SetPipeTimeout(ftHandle, FT600_WRITE_PIPE, 3000); // 写管道超时 1 秒
    if (ftStatus != FT_OK) {
        qDebug() << "设置写管道超时失败. 状态:" << ftStatus;
        FT_Close(ftHandle);
        ftHandle = nullptr;
        return false;
    }

    ftStatus = FT_SetPipeTimeout(ftHandle, FT600_READ_PIPE, 3000); // 读管道超时 1 秒
    if (ftStatus != FT_OK) {
        qDebug() << "设置读管道超时失败. 状态:" << ftStatus;
        FT_Close(ftHandle);
        ftHandle = nullptr;
        return false;
    }

    return true;
}

bool UsbWorker::setupFt600Configuration()
{
    if (!ftHandle) return false;

    FT_60XCONFIGURATION chipConfig;
    memset(&chipConfig, 0, sizeof(FT_60XCONFIGURATION));

    // 获取当前配置
    FT_STATUS ftStatus = FT_GetChipConfiguration(ftHandle, &chipConfig);
    if (ftStatus != FT_OK) {
        qDebug() << "获取芯片配置失败. 状态:" << ftStatus;
        return false;
    }

    // 输出当前配置
    qDebug() << "当前配置:";
    qDebug() << "  FIFO 时钟:" << chipConfig.FIFOClock;
    qDebug() << "  FIFO 模式:" << chipConfig.FIFOMode;
    qDebug() << "  通道配置:" << chipConfig.ChannelConfig;

    // 检查是否需要更新配置
    bool configChanged = false;

    if (chipConfig.FIFOClock != CONFIGURATION_FIFO_CLK_100) {
        qDebug() << "更新 FIFO 时钟为 100MHz";
        chipConfig.FIFOClock = CONFIGURATION_FIFO_CLK_100;
        configChanged = true;
    }

    if (chipConfig.FIFOMode != deviceMode) {
        qDebug() << "更新 FIFO 模式为 245 模式";
        chipConfig.FIFOMode = deviceMode;
        configChanged = true;
    }

    if (chipConfig.ChannelConfig != channelConfig) {
        qDebug() << "更新通道配置为 1 通道";
        chipConfig.ChannelConfig = channelConfig;
        configChanged = true;
    }

    // 如果配置需要更改，更新它
    if (configChanged) {
        qDebug() << "更新芯片配置...";
        ftStatus = FT_SetChipConfiguration(ftHandle, &chipConfig);
        if (ftStatus != FT_OK) {
            qDebug() << "设置芯片配置失败. 状态:" << ftStatus;
            return false;
        }

        // 重新打开设备以应用新配置
        qDebug() << "重新打开设备以应用配置...";
        FT_Close(ftHandle);
        QThread::msleep(500);

        ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
        if (ftStatus != FT_OK) {
            qDebug() << "配置后重新打开设备失败. 状态:" << ftStatus;
            ftHandle = nullptr;
            return false;
        }

        qDebug() << "配置更改后设备已成功重新打开";
    }

    // 设置流传输参数
    ftStatus = FT_SetStreamPipe(ftHandle, FALSE, FALSE, FT600_WRITE_PIPE, FT600_BUFFER_SIZE);
    if (ftStatus != FT_OK) {
        qDebug() << "设置写管道流传输失败. 状态:" << ftStatus;
        return false;
    }

    ftStatus = FT_SetStreamPipe(ftHandle, FALSE, TRUE, FT600_READ_PIPE, FT600_BUFFER_SIZE);
    if (ftStatus != FT_OK) {
        qDebug() << "设置读管道流传输失败. 状态:" << ftStatus;
        return false;
    }

    qDebug() << "FT600 配置已成功设置!";
    return true;
}

void UsbWorker::closeDevice()
{
    if (ftHandle != nullptr) {
        FT_Close(ftHandle);
        ftHandle = nullptr;
        qDebug() << "设备已关闭";
    }
}

bool UsbWorker::writeToDevice(const QByteArray &data)
{
    if (!deviceConnected || ftHandle == nullptr) {
        qDebug() << "UsbWorker::writeToDevice - 错误: 设备未连接";
        return false;
    }

//    // 打印发送的数据内容
//    QString hexData;
//    for (int i = 0; i < qMin(data.size(), 32); i++) {
//        hexData += QString("%1 ").arg((unsigned char)data[i], 2, 16, QChar('0'));
//    }
//    if (data.size() > 32) hexData += "...";
//    qDebug() << "UsbWorker::writeToDevice - 发送" << data.size() << "字节:" << hexData;

    // 设置最大重试次数
    const int maxRetries = 3;
    bool success = false;

    for (int retry = 0; retry < maxRetries && !success; retry++) {
        if (retry > 0) {
            qDebug() << "UsbWorker::writeToDevice - 重试" << retry << "次...";
            QThread::msleep(100 * retry);
        }

        // 按 FT600 缓冲区大小分块发送数据
        DWORD dataSize = static_cast<DWORD>(data.size());
        DWORD totalBytesSent = 0;
        DWORD remainingBytes = dataSize;
        DWORD offset = 0;
        bool chunkSuccess = true;

        while (remainingBytes > 0 && chunkSuccess) {
            DWORD chunkSize = (remainingBytes > FT600_BUFFER_SIZE) ? FT600_BUFFER_SIZE : remainingBytes;
            DWORD bytesSent = 0;

            FT_STATUS ftStatus = FT_WritePipe(ftHandle, FT600_WRITE_PIPE,
                                             (PUCHAR)(data.data() + offset),
                                             chunkSize, &bytesSent, NULL);

            if (ftStatus != FT_OK) {
                qDebug() << "UsbWorker::writeToDevice - 写入失败. 状态:" << ftStatus
                         << "在偏移:" << offset << "重试:" << retry;
                chunkSuccess = false;
                break;
            }

            totalBytesSent += bytesSent;
            offset += bytesSent;
            remainingBytes -= bytesSent;

            // 如果这个块没有完全发送，重试一次
            if (bytesSent < chunkSize) {
                qDebug() << "UsbWorker::writeToDevice - 块部分写入: 已发送" << bytesSent
                         << "字节，应为" << chunkSize << "字节";
            }

            // 为 FPGA 处理提供时间
            if (remainingBytes > 0) {
                QThread::usleep(5000); // 微小延迟，避免 FPGA 缓冲区溢出
            }
        }

        if (chunkSuccess) {
            // 刷新管道确保数据发送
            FT_STATUS ftStatus = FT_FlushPipe(ftHandle, FT600_WRITE_PIPE);
            if (ftStatus != FT_OK) {
                //qDebug() << "UsbWorker::writeToDevice - 警告: 刷新写管道失败. 状态:" << ftStatus;
            }

            qDebug() << "UsbWorker::writeToDevice - 写入完成. 已发送:" << totalBytesSent
                     << "共" << dataSize << "字节";

            success = (totalBytesSent == dataSize);
            if (success) break; // 发送成功，跳出重试循环
        }
    }

    if (!success) {
        qDebug() << "UsbWorker::writeToDevice - 在" << maxRetries << "次尝试后发送失败";
    }

    // 发送后添加延时，给设备足够处理时间
    QThread::msleep(100);

    return success;
}


int UsbWorker::findCompletePacket(const QByteArray& buffer, int offset)
{
    if (offset + 12 > buffer.size()) {
        return -1; // 数据不足
    }

    // 检查包头标识符 (0xF0A5, 0x5A0F)
    if (static_cast<uint8_t>(buffer[offset]) != 0xF0 ||
        static_cast<uint8_t>(buffer[offset + 1]) != 0xA5 ||
        static_cast<uint8_t>(buffer[offset + 2]) != 0x5A ||
        static_cast<uint8_t>(buffer[offset + 3]) != 0x0F) {
        return -1; // 不是有效的包头
    }

    // 获取数据长度 (字节6-7，小端序)
    uint16_t dataLength = static_cast<uint16_t>(buffer[offset + 6]) |
                         (static_cast<uint16_t>(buffer[offset + 7]) << 8);

    // 计算完整数据包大小：包头(8字节) + 头部CRC(2字节) + 数据(dataLength*2字节) + 数据CRC(2字节)
    int packetSize = 8 + 2 + (dataLength * 2) + 2;

    if (offset + packetSize > buffer.size()) {
        return -1; // 数据包不完整
    }

    return packetSize;
}

bool UsbWorker::sendSyncPattern()
{
    // 创建同步序列
    QByteArray syncPattern;

    // 添加16字节的同步模式 (0xAA 0x55 交替)
    for (int i = 0; i < 8; i++) {
        syncPattern.append(static_cast<char>(0xAA));
        syncPattern.append(static_cast<char>(0x55));
    }

    qDebug() << "发送初始同步序列...";
    return writeToDevice(syncPattern);
}



QVector<QVector<QVector<double>>> UsbCommunication::g_voltageDataCache;
QVector<QVector<QVector<uint32_t>>> UsbCommunication::g_rawDataCache;
QVector<QVector<QVector<uint32_t>>> UsbCommunication::g_timestamp1Cache;
QVector<QVector<QVector<uint32_t>>> UsbCommunication::g_timestamp2Cache;
QMutex UsbCommunication::g_dataCacheMutex;
bool UsbCommunication::g_dataReady = false;


UsbCommunication::UsbCommunication(QObject *parent) : QObject(parent),
    worker(nullptr), connected(false), isGroupA(true), _count(0)
{
    worker = new UsbWorker();
    worker->moveToThread(&workerThread);

    // 连接信号和槽
    connect(&workerThread, &QThread::started, worker, &UsbWorker::initialize);
    connect(&workerThread, &QThread::finished, worker, &UsbWorker::cleanup);

    connect(worker, &UsbWorker::connected, this, &UsbCommunication::handleWorkerConnected);
    connect(worker, &UsbWorker::disconnected, this, &UsbCommunication::handleWorkerDisconnected);
    connect(worker, &UsbWorker::error, this, &UsbCommunication::handleWorkerError);
    connect(worker, &UsbWorker::dataReceived, this, &UsbCommunication::handleWorkerDataReceived);
    connect(worker, &UsbWorker::ftdiStatus, this, &UsbCommunication::handleFtdiStatus);
}

UsbCommunication::~UsbCommunication()
{
    stopCommunication();
    delete worker;
}

bool UsbCommunication::isConnected() const
{
    return connected;
}

void UsbCommunication::startCommunication()
{
    worker->errorEmittedInit();
    if (!workerThread.isRunning())
    {
        workerThread.start();
    }
}

void UsbCommunication::stopCommunication()
{
    if (workerThread.isRunning()) {
        worker->setRunning(false);
        workerThread.quit();
        workerThread.wait();
    }
}

bool UsbCommunication::sendData(const QByteArray &data)
{
    if (!connected) {
        return false;
    }

    worker->sendData(data);
    return true;
}

void UsbCommunication::handleWorkerConnected()
{
    connected = true;
    emit connectionChanged(true);
}

void UsbCommunication::handleWorkerDisconnected()
{
    connected = false;
    emit connectionChanged(false);
}

void UsbCommunication::handleWorkerError(const QString &message)
{
    emit errorOccurred(message);
}

void UsbCommunication::handleFtdiStatus(const QString &status)
{
    emit ftdiStatusChanged(status);
}

void UsbCommunication::handleWorkerDataReceived(const QByteArray &data)
{
    parsePacket(data);
}

bool UsbCommunication::readRegister(uint32_t regAddr)
{
    if (!connected) {
        return false;
    }

    QByteArray packet = createRegisterReadPacket(regAddr);
    return sendData(packet);
}

bool UsbCommunication::writeRegister(uint32_t regAddr, uint32_t regData)
{
    if (!connected) {
        return false;
    }

    QByteArray packet = createRegisterWritePacket(regAddr, regData);
    return sendData(packet);
}

bool UsbCommunication::setPowerControl(bool isGroupA, bool plusEnable, bool minusEnable)
{
    if (!connected) {
        return false;
    }

    // 直接创建并发送+5V电源使能数据包
    uint32_t plusRegAddr = isGroupA ? REG_A_POWER_PLUS : REG_B_POWER_PLUS;
    QByteArray plusPacket = createRegisterWritePacket(plusRegAddr, plusEnable ? 1 : 0);
    bool success = sendData(plusPacket);

    if (success) {
        QThread::msleep(50); // 短暂延迟，避免数据包冲突

        // 直接创建并发送-5V电源使能数据包
        uint32_t minusRegAddr = isGroupA ? REG_A_POWER_MINUS : REG_B_POWER_MINUS;
        QByteArray minusPacket = createRegisterWritePacket(minusRegAddr, minusEnable ? 1 : 0);
        success = sendData(minusPacket);
    }

    return success;
}

bool UsbCommunication::setDigitalOutput(bool isGroupA, bool do1Value, bool do2Value)
{
    if (!connected) {
        return false;
    }

    // 直接创建并发送DO1数据包
    uint32_t do1RegAddr = isGroupA ? REG_A_DO1 : REG_B_DO1;
    QByteArray do1Packet = createRegisterWritePacket(do1RegAddr, do1Value ? 1 : 0);
    bool success = sendData(do1Packet);

    if (success) {
        QThread::msleep(50); // 短暂延迟，避免数据包冲突

        // 直接创建并发送DO2数据包
        uint32_t do2RegAddr = isGroupA ? REG_A_DO2 : REG_B_DO2;
        QByteArray do2Packet = createRegisterWritePacket(do2RegAddr, do2Value ? 1 : 0);
        success = sendData(do2Packet);
    }

    return success;
}

bool UsbCommunication::setTriggerMode(int mode)
{
    if (!connected) {
        return false;
    }

    // 创建触发模式数据包
    QByteArray packet = createRegisterWritePacket(
        isGroupA ? REG_A_TRIGGER_MODE : REG_B_TRIGGER_MODE,
        mode);
    return sendData(packet);
}

bool UsbCommunication::setDelayConfig(uint32_t adcDelay, uint32_t ioHighDelay, uint32_t ioLowDelay)
{
    if (!connected) {
        return false;
    }

    // 发送ADC延时
    QByteArray packet1 = createRegisterWritePacket(
        isGroupA ? REG_A_ADC_DELAY : REG_B_ADC_DELAY,
        adcDelay);
    bool success = sendData(packet1);

    if (success) {
        QThread::msleep(50); // 短暂延迟，避免数据包冲突

        // 发送IO高电平延时
        QByteArray packet2 = createRegisterWritePacket(
            isGroupA ? REG_A_IO_HIGH_DELAY : REG_B_IO_HIGH_DELAY,
            ioHighDelay);
        success = sendData(packet2);
    }

    if (success) {
        QThread::msleep(50);

        // 发送IO低电平延时
        QByteArray packet3 = createRegisterWritePacket(
            isGroupA ? REG_A_IO_LOW_DELAY : REG_B_IO_LOW_DELAY,
            ioLowDelay);
        success = sendData(packet3);
    }

    return success;
}

bool UsbCommunication::setSamplingConfig(uint32_t cycleCount, uint32_t pointsPerCycle)
{
    if (!connected) {
        return false;
    }

    // 发送采样周期数
    QByteArray packet1 = createRegisterWritePacket(
        isGroupA ? REG_A_CYCLE_COUNT : REG_B_CYCLE_COUNT,
        cycleCount);
    bool success = sendData(packet1);

    if (success) {
        QThread::msleep(50); // 短暂延迟，避免数据包冲突

        // 发送每周期采样点数
        QByteArray packet2 = createRegisterWritePacket(
            isGroupA ? REG_A_CYCLE_POINT_COUNT : REG_B_CYCLE_POINT_COUNT,
            pointsPerCycle);
        success = sendData(packet2);
    }

    return success;
}

bool UsbCommunication::startAcquisition()
{
    qDebug()<<"启动startAcquisition函数";
    setCountClear();
    printCount();
    worker->flushBuffers();
    qDebug() << "UsbCommunication::startAcquisition开始执行";

    if (!connected) {
        qDebug() << "UsbCommunication::startAcquisition - 错误: 未连接到设备";
        return false;
    }

    // 当前活动组
    qDebug() << "当前活动组: " << (isGroupA ? "A组" : "B组");

    QThread::msleep(10); // 减少等待时间

    try {
        // 先发送停止命令，确保FPGA处于初始状态
        uint32_t regAddr = isGroupA ? REG_A_START_ACQUISITION : REG_B_START_ACQUISITION;
        writeRegister(regAddr, 0);
        QThread::msleep(50); // 减少等待时间

        qDebug() << "使用寄存器地址: 0x" << QString::number(regAddr, 16);

        // 创建并发送数据包
        QByteArray packet = createRegisterWritePacket(regAddr, 1);
        qDebug() << "发送开始采集数据包...";
        bool success = sendData(packet);
        qDebug() << "开始采集命令发送结果: " << (success ? "成功" : "失败");

        return success;
    }
    catch (const std::exception& e) {
        qDebug() << "UsbCommunication::startAcquisition - 异常: " << e.what();
        return false;
    }
    catch (...) {
        qDebug() << "UsbCommunication::startAcquisition - 未知异常";
        return false;
    }
}

void UsbCommunication::printCount()
{
    qDebug() << "收数据次数:" << _count;
}

void UsbCommunication::setCountClear()
{
    _count = 0;
}

void UsbCommunication::closeConnect()
{
    worker->handleSendFailure();
}




uint16_t UsbCommunication::calculateHeaderCRC(uint16_t cmd, uint16_t length)
{
    uint8_t sum = (cmd + length) & 0xFF;
    return sum;
}

uint16_t UsbCommunication::calculateDataCRC(const QByteArray &data, int offset, int length)
{
    uint16_t sum = 0;
    for (int i = offset; i < offset + length; i += 2) {
        if (i < data.size()) {
            sum = sum + bytesToUInt16_1(data, i);
        }
    }
    return sum;
}

bool UsbCommunication::parsePacket(const QByteArray &data)
{
    uint16_t cmd = bytesToUInt16_1(data, 4);
    // 检查数据长度是否足够一个最小数据包
    if (data.size() < 12) { // 头部(8字节) + 头部CRC(2字节) + 数据CRC(2字节)
        qDebug() << "数据包长度不足";
       // return false;
    }

    // 检查包头
    if ((static_cast<uint8_t>(data[0]) != 0xF0) ||
        (static_cast<uint8_t>(data[1]) != 0xA5) ||
        (static_cast<uint8_t>(data[2]) != 0x5A) ||
        (static_cast<uint8_t>(data[3]) != 0x0F)) {

        QString invalidHeader = QString("%1%2 %3%4")
                .arg(static_cast<uint8_t>(data[0]), 2, 16, QChar('0'))
                .arg(static_cast<uint8_t>(data[1]), 2, 16, QChar('0'))
                .arg(static_cast<uint8_t>(data[2]), 2, 16, QChar('0'))
                .arg(static_cast<uint8_t>(data[3]), 2, 16, QChar('0'));

        qDebug() << "无效的包头:" << invalidHeader;
      //  return false;
    }

    // 获取命令类型和数据长度

    uint16_t length = bytesToUInt16_1(data, 6);
    uint16_t headerCrc = bytesToUInt16_1(data, 8);
    uint16_t calculatedHeaderCrc = calculateHeaderCRC(cmd, length);

    if (headerCrc != calculatedHeaderCrc) {
        qDebug() << "头部校验和错误: 接收=" << QString::number(headerCrc, 16)
                 << " 计算=" << QString::number(calculatedHeaderCrc, 16)
                 << " 命令=" << QString::number(cmd, 16)
                 << " 长度=" << QString::number(length, 16);
      //  return false;
    }

    // 检查数据长度
    if (data.size() < 10 + length * 2 + 2) {
        qDebug() << "数据长度不符合预期";
        qDebug() << " 长度=" << QString::number(length, 16);
       // return false;
    }

    // 验证数据校验和
    uint16_t dataCrc = bytesToUInt16_1(data, 10 + length * 2);
    QByteArray dataSection = data.mid(10, length * 2);

    uint16_t calculatedDataCrc = calculateDataCRC(dataSection, 0, dataSection.size());

    if (dataCrc != calculatedDataCrc) {
        qDebug() << "数据校验和错误: 接收=" << QString::number(dataCrc, 16)
                 << " 计算=" << QString::number(calculatedDataCrc, 16);

        // return false;
    }

    // 根据命令类型解析数据
    bool parseResult = false;
    switch (cmd) {
        case CMD_READ_REG:
        case CMD_WRITE_REG:
            parseResult = parseRegisterResponse(data, 10, length * 2);
            break;
        case CMD_POWER_DATA:
            parseResult = parsePowerData(data, 10, length * 2);
            break;
        case CMD_NTC_TEMP:
            parseResult = parseNtcTemp(data, 10, length * 2);
            break;
        case CMD_DIFF_VOLTAGE:
            parseResult = parseDiffVoltage(data, 10, length * 2);
            break;
        default:
            qDebug() << "未知命令类型:" << cmd;
            return false;
    }

    return parseResult;
}

bool UsbCommunication::parseRegisterResponse(const QByteArray &data, int dataOffset, int dataLength)
{
    if (dataLength < 8) { // 至少需要32位地址+32位数据
        return false;
    }

    uint32_t regAddr = bytesToUInt32(data, dataOffset);
    uint32_t regData = bytesToUInt32(data, dataOffset + 4);

    QVariantMap result;
    result["type"] = "register";
    result["address"] = regAddr;
    result["data"] = regData;

    parsedDataList.append(result);

    // 检查是否是数字输入寄存器
    if (regAddr == REG_A_DI) {  // A组数字输入寄存器
        emit digitalInputChanged(true, static_cast<uint8_t>(regData & 0x3F)); // 6位DI
    } else if (regAddr == REG_B_DI) {  // B组数字输入寄存器
        emit digitalInputChanged(false, static_cast<uint8_t>(regData & 0x3F)); // 6位DI
    }

    emit registerReadResult(regAddr, regData);

    return true;
}

bool UsbCommunication::parsePowerData(const QByteArray &data, int dataOffset, int dataLength)
{
    if (dataLength < 5) { // 至少需要通道(1字) + 时间戳(4字) = 5字
        qDebug() << "功耗数据长度不足";
        return false;
    }

    uint16_t channel = bytesToUInt16_1(data, dataOffset);

    // 时间戳：接下来4个字(64位)
    uint64_t timestamp = bytesToUInt64(data, dataOffset + 2);

    QVector<QVariantMap> powerDataList;

    // 处理剩余的采样数据
    // 通道(1字) + 时间戳(4字) = 5字 = 10字节
    int dataSamplesOffset = dataOffset + 10;

    // 剩余的数据长度(以字为单位)
    int remainingWords = dataLength - 10;

    // 每个采样数据是32位，即2个字(4字节)
    int samplesCount = remainingWords / 4;

    for (int i = 0; i < samplesCount; i++) {
        int byteOffset = dataSamplesOffset + i * 4;

        // 确保不越界
        if (byteOffset + 3 >= data.size()) {
            break;
        }

        // 按32位读取采样值
        uint32_t sampleValue = bytesToUInt32(data, byteOffset);

        QVariantMap sample;
        sample["type"] = "power";
        sample["timestamp"] = timestamp + i;
        sample["channel"] = channel;
        sample["value"] = sampleValue;

        double adcValue = sampleValue;
        double realValue = 0.0;

        // 根据新的规格更新计算公式
        switch (channel) {
            case 0: // 单板电流检测
                // 参考电压 5V, 10bit(1024)
                // 电流值(A) = (上位机接收数据/1024)*5/20/0.1
                realValue = (adcValue / 1024.0) * 5.0 / 20.0 / 0.1;
                sample["unit"] = "A";
                break;

            case 1: // 单板电压检测
                // 参考电压 5V, 10bit(1024)
                // 电压值(V) = (上位机接收数据/1024)*5*9
                realValue = (adcValue / 1024.0) * 5.0 * 9.0;
                sample["unit"] = "V";
                break;

            case 2: // A组电流检测
                // 参考电压 2.5V, 10bit(1024)
                // 电流值(A) = (上位机接收数据/1024)*2.5/20/0.47
                realValue = (adcValue / 1024.0) * 2.5 / 20.0 / 0.47;
                sample["unit"] = "A";
                break;

            case 3: // A组电压检测
                // 参考电压 2.5V, 10bit(1024)
                // 电压值(V) = (上位机接收数据/1024)*2.5*5/2
                realValue = (adcValue / 1024.0) * 2.5 * 5.0 / 2.0;
                sample["unit"] = "V";
                break;

            case 4: // B组电流检测
                // 参考电压 2.5V, 10bit(1024)
                // 电流值(A) = (上位机接收数据/1024)*2.5/20/0.47
                realValue = (adcValue / 1024.0) * 2.5 / 20.0 / 0.47;
                sample["unit"] = "A";
                break;

            case 5: // B组电压检测
                // 参考电压 2.5V, 10bit(1024)
                // 电压值(V) = (上位机接收数据/1024)*2.5*5/2
                realValue = (adcValue / 1024.0) * 2.5 * 5.0 / 2.0;
                sample["unit"] = "V";
                break;

            default:
                realValue = adcValue;
                sample["unit"] = "raw";
        }

        sample["realValue"] = realValue;
        powerDataList.append(sample);
    }

    if (!powerDataList.isEmpty()) {
        emit powerDataReceived(powerDataList);
        return true;
    }

    return false;
}

bool UsbCommunication::parseNtcTemp(const QByteArray &data, int dataOffset, int dataLength)
{
    if (dataLength < 5) { // 至少需要通道(1字) + 时间戳(4字)
        qDebug() << "温度数据长度不足";
        return false;
    }

    // 通道(1字) + 时间戳(4字)
    uint16_t channel = bytesToUInt16_1(data, dataOffset);
    uint64_t timestamp = bytesToUInt64(data, dataOffset + 2);

    QVector<QVariantMap> tempDataList;

    // 处理剩余的采样数据
    int dataSamplesOffset = dataOffset + 10; // 通道(1字) + 时间戳(4字) = 10字节

    // 检查实际可用的数据量
    int availableBytes = data.size() - dataSamplesOffset;
    int maxPossibleSamples = availableBytes / 4; // 每个样本4字节

    // 声明的样本数
    int declaredSamples = (dataLength - 5) / 2;

    // 使用实际可用的采样数，防止越界
    int samplesCount = qMin(declaredSamples, maxPossibleSamples);

    QString channelName;
    switch (channel) {
        case 0: channelName = "A组CH1温度"; break;
        case 1: channelName = "B组CH1温度"; break;
        case 2: channelName = "B组CH2温度"; break;
        case 3: channelName = "B组CH3温度"; break;
        default: channelName = QString("未知温度通道(%1)").arg(channel);
    }

    // 重新处理所有数据
    for (int i = 0; i < samplesCount; i++) {
        int byteOffset = dataSamplesOffset + i * 4;

        if (byteOffset + 3 >= data.size()) {
            break;
        }

        uint32_t sampleValue = bytesToUInt32(data, byteOffset);

        QVariantMap sample;
        sample["type"] = "temperature";
        sample["timestamp"] = static_cast<qint64>(timestamp + i);
        sample["channel"] = channel;
        sample["channelName"] = channelName;
        sample["rawValue"] = sampleValue;
        // 1. 原始值 -> 电压值
        double voltage = convertRawToVoltage1(sampleValue, channel);
        sample["voltage"] = voltage;

        // 2. 电压值 -> 电阻值（使用50µA恒流源）
        const double CONSTANT_CURRENT = 50e-6; // 50µA
        double resistance = voltage / CONSTANT_CURRENT;
        sample["resistance"] = resistance;

        // 3. 电阻值 -> 温度值
        double temperature = convertResistanceToTemperature(resistance);

        sample["temperature"] = temperature;
        tempDataList.append(sample);
    }

    if (!tempDataList.isEmpty()) {
        emit temperatureDataReceived(tempDataList);
        return true;
    }

    return false;
}

double UsbCommunication::convertRawToVoltage1(uint32_t rawValue, int channel)
{
    const double GAIN = 1.0;                  // 增益
    const double V_REF = 1.35;                // 参考电压 1.35V
    const double ADC_OFFSET = 8388608.0;      // 2^(N-1) = 2^23 = 8388608

    double voltage;

    if (channel == 0) {
        // A组公式：Code = 2^(N-1) × (V_IN × Gain/V_REF)
        // 解出 V_IN = Code × V_REF / (2^(N-1) × Gain)
        voltage = (double)rawValue * V_REF / (ADC_OFFSET * GAIN);
    } else {
        // B组公式：Code = 2^(N-1) × ((V_IN × Gain/V_REF) + 1)
        // 解出 V_IN = (Code / 2^(N-1) - 1) × V_REF / Gain
        voltage = ((double)rawValue / ADC_OFFSET - 1.0) * V_REF / GAIN;
    }

    return voltage;
}

double UsbCommunication::convertResistanceToTemperature(double resistance)
{
    // NTC热敏电阻温度计算 - 基于B方程
    // T = 1/(ln(Rt/R0)/B + 1/T0)
    // 其中:
    // - Rt: 当前温度下的电阻值
    // - R0: 标称电阻值 (通常是25°C时的阻值)
    // - T0: 标称温度 (25°C = 298.15K)
    // - B: NTC的B值

    const double B = 3380.0;         // B常数，单位K (需要根据具体NTC型号调整)
    const double R0 = 10000.0;       // 标称阻值 10KΩ @ 25°C (需要根据具体NTC型号调整)
    const double T0 = 273.15 + 25.0; // 参考温度，单位K (25°C)

    // 计算温度
    double ln_ratio = log(resistance / R0);
    double T_kelvin = 1.0 / (ln_ratio / B + 1.0 / T0);
    double temperature_celsius = T_kelvin - 273.15;

    return temperature_celsius;
}

double UsbCommunication::convertNtcToTemperature(uint32_t ntcValue)
{
    const double CONSTANT_CURRENT = 50e-6; // 50µA
    double resistance = ntcValue / CONSTANT_CURRENT;
    return convertResistanceToTemperature(resistance);
}

//bool UsbCommunication::parseDiffVoltage(const QByteArray &data, int dataOffset, int dataLength)
//{
//    if (dataLength < 10) {
//        qDebug() << "差分电压数据长度不足";
//        return false;
//    }

//    uint16_t channel = bytesToUInt16_1(data, dataOffset);
//    uint32_t timestamp2 = bytesToUInt32(data, dataOffset + 2);
//    uint32_t timestamp1 = bytesToUInt32(data, dataOffset + 6);

//    // 计算采样数量
//    int samplesCount = (dataLength - 5) / 2;
//    int dataSamplesOffset = dataOffset + 10;

//    // 使用更高效的数据结构
//    struct VoltageData {
//        uint16_t channel;
//        uint32_t timestamp1;
//        uint32_t timestamp2;
//        uint32_t rawValue;
//        float voltage;
//    };

//    // 预分配向量，避免动态增长
//    QVector<VoltageData> voltageDataBatch;
//    voltageDataBatch.reserve(samplesCount);

//    // 批量处理所有采样
//    for (int i = 0; i < samplesCount; i++) {
//        int byteOffset = dataSamplesOffset + i * 4;

//        if (byteOffset + 3 >= data.size()) {
//            break;
//        }

//        uint32_t sampleValue = bytesToUInt32(data, byteOffset);
//        float voltage = convertRawToVoltage(sampleValue);

//        voltageDataBatch.append({channel, timestamp1, timestamp2, sampleValue, voltage});
//    }

//    // 转换为QVariantMap格式（为了兼容现有接口）
//    if (!voltageDataBatch.isEmpty()) {
//        QVector<QVariantMap> voltageDataList;
//        voltageDataList.reserve(voltageDataBatch.size());

//        for (const auto& vd : voltageDataBatch) {
//            QVariantMap sample;
//            sample["type"] = "voltage";
//            sample["channel"] = vd.channel;
//            sample["timestamp1"] = vd.timestamp1;
//            sample["timestamp2"] = vd.timestamp2;
//            sample["rawValue"] = vd.rawValue;
//            sample["voltage"] = vd.voltage;

//            voltageDataList.append(sample);
//        }

//        emit voltageDataReceived(voltageDataList);
//    }

//    return true;
//}

bool UsbCommunication::parseDiffVoltage(const QByteArray &data, int dataOffset, int dataLength)
{
    if (dataLength < 10) {
        qDebug() << "差分电压数据长度不足";
        return false;
    }

    uint16_t channel = bytesToUInt16_1(data, dataOffset);
    uint32_t timestamp2 = bytesToUInt32(data, dataOffset + 2);
    uint32_t timestamp1 = bytesToUInt32(data, dataOffset + 6);

    // 检查是否是当前活动组的数据
    bool isValidChannel = false;
    int targetGroup = -1;
    int targetChannel = -1;

    if (channel < 8 && _activeGroup == 0) {
        isValidChannel = true;
        targetGroup = 0;
        targetChannel = channel;
    } else if (channel >= 8 && channel < 12 && _activeGroup == 1) {
        isValidChannel = true;
        targetGroup = 1;
        targetChannel = channel - 8;
    }

    if (!isValidChannel || !_acquisitionInProgress) {
        return false;
    }

    int dataSamplesOffset = dataOffset + 10;
    int remainingWords = dataLength - 5;
    int samplesCount = remainingWords / 2;

    // 直接存储到全局缓存，避免创建临时对象
    {
        QMutexLocker locker(&g_dataCacheMutex);

        for (int i = 0; i < samplesCount; i++) {
            int byteOffset = dataSamplesOffset + i * 4;
            if (byteOffset + 3 >= data.size()) {
                break;
            }

            uint32_t sampleValue = bytesToUInt32(data, byteOffset);
            double voltage = convertRawToVoltage(sampleValue);

            // 直接存储到全局缓存
            g_voltageDataCache[targetGroup][targetChannel].append(voltage);
            g_rawDataCache[targetGroup][targetChannel].append(sampleValue);
            g_timestamp1Cache[targetGroup][targetChannel].append(timestamp1);
            g_timestamp2Cache[targetGroup][targetChannel].append(timestamp2);
        }
    }

    // 更新计数并检查完成状态
    if (_acquisitionInProgress) {
        _count++;

        // 降低进度更新频率 - 每100个包更新一次
        if (_count % 100 == 0 || _count >= _expectedTotalPackets) {
            emit acquisitionProgress(_count, _expectedTotalPackets);
        }

        // 检查是否完成
        if (_count >= _expectedTotalPackets) {
            _acquisitionInProgress = false;
            g_dataReady = true;
            qDebug() << "数据采集完成，共收到" << _count << "个包";
            emit acquisitionCompleted(_count);
        }
    }

    return true;
}
double UsbCommunication::convertRawToVoltage(uint32_t rawValue)
{
    // 根据您提供的ADC特性:
    // 20位ADC，范围为-5V到+5V
    // 0x7FFFF = +4.99999046V
    // 0x00000 = 0V
    // 0x80000 = -5V

    // 处理有符号数 (将20位有符号数转换为int32_t)
    int32_t signedValue;
    if (rawValue & 0x80000) {
        // 负数 - 补码转换
        signedValue = static_cast<int32_t>(rawValue | 0xFFF00000); // 扩展符号位
    } else {
        // 正数
        signedValue = static_cast<int32_t>(rawValue & 0x7FFFF);
    }

    double voltage = static_cast<double>(signedValue) * 5.0 / 524288.0;

    return voltage;
}

uint16_t UsbCommunication::bytesToUInt16_1(const QByteArray &data, int offset)
{
    if (offset + 1 >= data.size()) {
        return 0;
    }

    return (static_cast<uint16_t>(static_cast<uint8_t>(data[offset])) ) |
            static_cast<uint16_t>(static_cast<uint8_t>(data[offset + 1])<< 8);
}

uint32_t UsbCommunication::bytesToUInt32(const QByteArray &data, int offset)
{
    if (offset + 3 >= data.size()) {
        return 0;
    }

    // 从示例看，这个是大端序，但字节顺序是从低字节到高字节
    return (static_cast<uint32_t>(static_cast<uint8_t>(data[offset]))) |
           (static_cast<uint32_t>(static_cast<uint8_t>(data[offset + 1])) << 8) |
           (static_cast<uint32_t>(static_cast<uint8_t>(data[offset + 2])) << 16) |
           (static_cast<uint32_t>(static_cast<uint8_t>(data[offset + 3])) << 24);
}

uint64_t UsbCommunication::bytesToUInt64(const QByteArray &data, int offset)
{
    if (offset + 7 >= data.size()) {
        return 0;
    }

    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result |= (static_cast<uint64_t>(static_cast<uint8_t>(data[offset + i])) << (i * 8));
    }
    return result;
}


QByteArray UsbCommunication::createRegisterReadPacket(uint32_t regAddr)
{
    QByteArray packet;

    // 添加包头标识符 (0xA5F0, 0x0F5A) - 使用大端序
    packet.append(static_cast<char>(0xF0));
    packet.append(static_cast<char>(0xA5));
    packet.append(static_cast<char>(0x5A));
    packet.append(static_cast<char>(0x0F));

    // 添加命令类型: 读寄存器 - 使用大端序
    packet.append(static_cast<char>(0x01));
    packet.append(static_cast<char>(0x00));

    // 添加数据长度: 4个16位字 - 使用大端序
    packet.append(static_cast<char>(0x04));
    packet.append(static_cast<char>(0x00));

    // 计算并添加头部校验和 - 使用大端序
    uint16_t headerCrc = calculateHeaderCRC(0x01, 0x04);
    packet.append(static_cast<char>((headerCrc) & 0xFF));
    packet.append(static_cast<char>((headerCrc >> 8) & 0xFF));

    // 添加32位地址 (按照示例，对于0x01，正确格式是01 00 00 00)
    packet.append(static_cast<char>((regAddr >> 0) & 0xFF));  // 最低字节放最前面
    packet.append(static_cast<char>((regAddr >> 8) & 0xFF));
    packet.append(static_cast<char>((regAddr >> 16) & 0xFF));
    packet.append(static_cast<char>((regAddr >> 24) & 0xFF)); // 最高字节放最后面

    // 添加32位数据 (读取时为0)
    packet.append(static_cast<char>(0x00));
    packet.append(static_cast<char>(0x00));
    packet.append(static_cast<char>(0x00));
    packet.append(static_cast<char>(0x00));

    // 计算并添加数据校验和
    uint16_t dataCrc = calculateDataCRC(packet, 10, 8);
    packet.append(static_cast<char>((dataCrc) & 0xFF));
    packet.append(static_cast<char>((dataCrc >> 8) & 0xFF));

    return packet;
}

QByteArray UsbCommunication::createRegisterWritePacket(uint32_t regAddr, uint32_t regData)
{
    QByteArray packet;

    // 添加包头标识符 (0xA5F0, 0x0F5A) - 使用大端序
    packet.append(static_cast<char>(0xF0));
    packet.append(static_cast<char>(0xA5));
    packet.append(static_cast<char>(0x5A));
    packet.append(static_cast<char>(0x0F));

    // 添加命令类型: 写寄存器 - 使用大端序
    packet.append(static_cast<char>(0x02));
    packet.append(static_cast<char>(0x00));

    // 添加数据长度: 4个16位字 - 使用大端序
    packet.append(static_cast<char>(0x04));
    packet.append(static_cast<char>(0x00));

    // 计算并添加头部校验和
    uint16_t headerCrc = calculateHeaderCRC(0x02, 0x04);
    packet.append(static_cast<char>((headerCrc ) & 0xFF));
    packet.append(static_cast<char>((headerCrc>> 8) & 0xFF));

    // 添加32位地址 (按照示例格式)
    packet.append(static_cast<char>((regAddr >> 0) & 0xFF));
    packet.append(static_cast<char>((regAddr >> 8) & 0xFF));
    packet.append(static_cast<char>((regAddr >> 16) & 0xFF));
    packet.append(static_cast<char>((regAddr >> 24) & 0xFF));

    // 添加32位数据 (按照相同格式)
    packet.append(static_cast<char>((regData >> 0) & 0xFF));
    packet.append(static_cast<char>((regData >> 8) & 0xFF));
    packet.append(static_cast<char>((regData >> 16) & 0xFF));
    packet.append(static_cast<char>((regData >> 24) & 0xFF));

    // 计算并添加数据校验和
    uint16_t dataCrc = calculateDataCRC(packet, 10, 8);
    packet.append(static_cast<char>((dataCrc) & 0xFF));
    packet.append(static_cast<char>((dataCrc>> 8) & 0xFF));

    return packet;
}
void UsbCommunication::setAcquisitionParams(int cycles, int group, int pointsPerCycle)
{
    _activeGroup = group;
    int channelCount = (group == 0) ? 8 : 4;
    _expectedTotalPackets = cycles * channelCount;
    _count = 0;
    _acquisitionInProgress = true;

    // 预分配全局缓存内存
    clearDataCache();
    QMutexLocker locker(&g_dataCacheMutex);

    g_voltageDataCache.resize(2);
    g_rawDataCache.resize(2);
    g_timestamp1Cache.resize(2);
    g_timestamp2Cache.resize(2);

    for (int g = 0; g < 2; g++) {
        int channels = (g == 0) ? 8 : 4;
        g_voltageDataCache[g].resize(channels);
        g_rawDataCache[g].resize(channels);
        g_timestamp1Cache[g].resize(channels);
        g_timestamp2Cache[g].resize(channels);

        for (int ch = 0; ch < channels; ch++) {
            int reserveSize = cycles * pointsPerCycle + 1000; // 预留缓冲
            g_voltageDataCache[g][ch].reserve(reserveSize);
            g_rawDataCache[g][ch].reserve(reserveSize);
            g_timestamp1Cache[g][ch].reserve(reserveSize);
            g_timestamp2Cache[g][ch].reserve(reserveSize);
        }
    }

    g_dataReady = false;
    qDebug() << "设置采集参数: 组" << group << ", 预期包数:" << _expectedTotalPackets;
}

void UsbCommunication::clearDataCache()
{
    QMutexLocker locker(&g_dataCacheMutex);
    g_voltageDataCache.clear();
    g_rawDataCache.clear();
    g_timestamp1Cache.clear();
    g_timestamp2Cache.clear();
    g_dataReady = false;
}

void UsbCommunication::getVoltageData(int group, int channel, QVector<double>& data)
{
    QMutexLocker locker(&g_dataCacheMutex);
    if (group < g_voltageDataCache.size() && channel < g_voltageDataCache[group].size()) {
        data = g_voltageDataCache[group][channel];
    }
}

void UsbCommunication::getRawData(int group, int channel, QVector<uint32_t>& data)
{
    QMutexLocker locker(&g_dataCacheMutex);
    if (group < g_rawDataCache.size() && channel < g_rawDataCache[group].size()) {
        data = g_rawDataCache[group][channel];
    }
}

void UsbCommunication::getTimestampData(int group, int channel, QVector<uint32_t>& ts1, QVector<uint32_t>& ts2)
{
    QMutexLocker locker(&g_dataCacheMutex);
    if (group < g_timestamp1Cache.size() && channel < g_timestamp1Cache[group].size()) {
        ts1 = g_timestamp1Cache[group][channel];
        ts2 = g_timestamp2Cache[group][channel];
    }
}
void UsbCommunication::getAllGroupData(int group,
                                      QVector<QVector<double>>& voltageData,
                                      QVector<QVector<uint32_t>>& rawData,
                                      QVector<QVector<uint32_t>>& ts1Data,
                                      QVector<QVector<uint32_t>>& ts2Data)
{
    QMutexLocker locker(&g_dataCacheMutex);

    if (group >= g_voltageDataCache.size()) {
        return;
    }

    int channelCount = g_voltageDataCache[group].size();

    voltageData.resize(channelCount);
    rawData.resize(channelCount);
    ts1Data.resize(channelCount);
    ts2Data.resize(channelCount);

    for (int ch = 0; ch < channelCount; ch++) {
        voltageData[ch] = g_voltageDataCache[group][ch];
        rawData[ch] = g_rawDataCache[group][ch];
        ts1Data[ch] = g_timestamp1Cache[group][ch];
        ts2Data[ch] = g_timestamp2Cache[group][ch];
    }
}

