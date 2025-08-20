#ifndef PROTOCOLHANDLER_H
#define PROTOCOLHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QVector>
#include <QVariantMap>
#include <QDebug>

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

class ProtocolHandler : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolHandler(QObject *parent = nullptr);

    // 创建寄存器操作数据包
    QByteArray createRegisterReadPacket(uint32_t regAddr);
    QByteArray createRegisterWritePacket(uint32_t regAddr, uint32_t regData);

    // 创建控制包 (高级操作，包装寄存器操作)
    QByteArray createPowerControlPacket(bool isGroupA, bool plusEnable, bool minusEnable);
    QByteArray createDigitalOutputPacket(bool isGroupA, bool do1Value, bool do2Value);
    QByteArray createTriggerModePacket(bool isGroupA, int mode); // 0=内触发, 1=外触发1, 2=外触发2
    QByteArray createDelayConfigPacket(bool isGroupA, uint32_t adcDelay, uint32_t ioHighDelay, uint32_t ioLowDelay);
    QByteArray createSamplingConfigPacket(bool isGroupA, uint32_t cycleCount, uint32_t pointsPerCycle);
    QByteArray createStartAcquisitionPacket(bool isGroupA);

    // 解析接收到的数据包
    bool parsePacket(const QByteArray &data);

    // 获取解析结果
    QVector<QVariantMap> getParsedData() const;
    void clearParsedData();

    void setCountClear();
    void printCount();
signals:
    // 解析到各种数据包的信号
    void registerReadResult(uint32_t regAddr, uint32_t regData);
    void powerDataReceived(const QVector<QVariantMap> &powerData);
    void ntcTempReceived(const QVector<QVariantMap> &tempData);
    void diffVoltageReceived(const QVector<QVariantMap> &voltageData);
    void digitalInputChanged(bool isGroupA, uint8_t diValue);
    void rawPacketReceived(int cmd, const QByteArray &data);

private:
    // 计算校验和
    uint16_t calculateHeaderCRC(uint16_t cmd, uint16_t length);
    uint16_t calculateDataCRC(const QByteArray &data, int offset, int length);
    int _processEventsCounter;
    // 辅助函数
    QByteArray createPacketHeader(uint16_t cmd, uint16_t dataLength);
    void appendDataWithCRC(QByteArray &packet, const QByteArray &data);
    void logMessage(const QString &message, bool isError = false);
    // 解析不同类型的数据包
    bool parseRegisterResponse(const QByteArray &data, int dataOffset, int dataLength);
    bool parsePowerData(const QByteArray &data, int dataOffset, int dataLength);
    bool parseNtcTemp(const QByteArray &data, int dataOffset, int dataLength);
    bool parseDiffVoltage(const QByteArray &data, int dataOffset, int dataLength);
    double convertRawToVoltage(uint32_t rawValue);
    double convertNtcToTemperature(uint32_t ntcValue);

    // 字节序转换辅助函数
    uint16_t bytesToUInt16(const QByteArray &data, int offset);
    uint16_t bytesToUInt16_1(const QByteArray &data, int offset);
    uint32_t bytesToUInt32(const QByteArray &data, int offset);
    uint64_t bytesToUInt64(const QByteArray &data, int offset);

    // 存储解析结果
    QVector<QVariantMap> parsedDataList;
    double convertResistanceToTemperature(double resistance);
    double convertRawToVoltage1(uint32_t rawValue, int channel);


    //统计收数据次数
    int _count;
};

#endif // PROTOCOLHANDLER_H
