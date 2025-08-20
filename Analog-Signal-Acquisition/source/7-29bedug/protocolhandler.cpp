#include "protocolhandler.h"
#include<math.h>
#include<QFile>
#include <QDateTime>
#include <QCoreApplication>
#pragma execution_character_set("UTF-8")
ProtocolHandler::ProtocolHandler(QObject *parent) : QObject(parent)
{
    _count = 0;
}

QByteArray ProtocolHandler::createPacketHeader(uint16_t cmd, uint16_t dataLength)
{
    QByteArray packet;

    // 添加包头标识符 (0xF0A5, 0x5A0F) - 使用大端序
    packet.append(static_cast<char>((PROTOCOL_HEADER_WORD1 >> 8) & 0xFF));
    packet.append(static_cast<char>(PROTOCOL_HEADER_WORD1 & 0xFF));
    packet.append(static_cast<char>((PROTOCOL_HEADER_WORD2 >> 8) & 0xFF));
    packet.append(static_cast<char>(PROTOCOL_HEADER_WORD2 & 0xFF));

    // 添加命令类型
    packet.append(static_cast<char>((cmd ) & 0xFF));
    packet.append(static_cast<char>((cmd>> 8) & 0xFF));

    // 添加数据长度
    packet.append(static_cast<char>(dataLength  & 0xFF));
    packet.append(static_cast<char>((dataLength>> 8) & 0xFF));

    // 计算并添加头部校验和
    uint16_t headerCrc = calculateHeaderCRC(cmd, dataLength);
    packet.append(static_cast<char>(headerCrc  & 0xFF));
    packet.append(static_cast<char>((headerCrc >> 8)& 0xFF));

    return packet;
}

QByteArray ProtocolHandler::createRegisterReadPacket(uint32_t regAddr)
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
    packet.append(static_cast<char>((headerCrc ) & 0xFF));
    packet.append(static_cast<char>((headerCrc>> 8) & 0xFF));

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
    packet.append(static_cast<char>((dataCrc>> 8) & 0xFF));

    return packet;
}

QByteArray ProtocolHandler::createRegisterWritePacket(uint32_t regAddr, uint32_t regData)
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

uint16_t ProtocolHandler::calculateHeaderCRC(uint16_t cmd, uint16_t length)
{
    uint8_t sum = (cmd + length) & 0xFF;
    return sum;
}

uint16_t ProtocolHandler::calculateDataCRC(const QByteArray &data, int offset, int length)
{
    uint16_t sum = 0;
    for (int i = offset; i < offset + length; i += 2) {
        if (i < data.size()) {
            sum = sum + bytesToUInt16_1(data,i);
        }
    }
    return sum;
}

bool ProtocolHandler::parsePacket(const QByteArray &data)
{
    uint16_t cmd = bytesToUInt16_1(data, 4);
    if(cmd == CMD_DIFF_VOLTAGE)
    {
        _count++;
    }

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

bool ProtocolHandler::parseRegisterResponse(const QByteArray &data, int dataOffset, int dataLength)
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

bool ProtocolHandler::parsePowerData(const QByteArray &data, int dataOffset, int dataLength)
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
            case 0: // 单板电流检测（保持原有计算）
                // 参考电压 5V, 10bit(1024)
                // 电流值(A) = (上位机接收数据/1024)*5/20/0.1
                realValue = (adcValue / 1024.0) * 5.0 / 20.0 / 0.1;
                sample["unit"] = "A";
                break;

            case 1: // 单板电压检测（保持原有计算）
                // 参考电压 5V, 10bit(1024)
                // 电压值(V) = (上位机接收数据/1024)*5*9
                realValue = (adcValue / 1024.0) * 5.0 * 9.0;
                sample["unit"] = "V";
                break;

            case 2: // A组电流检测（更新计算公式）
                // 参考电压 2.5V, 10bit(1024)
                // 电流值(A) = (上位机接收数据/1024)*2.5/20/0.47
                realValue = (adcValue / 1024.0) * 2.5 / 20.0 / 0.47;
                sample["unit"] = "A";
                break;

            case 3: // A组电压检测（更新计算公式）
                // 参考电压 2.5V, 10bit(1024)
                // 电压值(V) = (上位机接收数据/1024)*2.5*5/2
                realValue = (adcValue / 1024.0) * 2.5 * 5.0 / 2.0;
                sample["unit"] = "V";
                break;

            case 4: // B组电流检测（更新计算公式）
                // 参考电压 2.5V, 10bit(1024)
                // 电流值(A) = (上位机接收数据/1024)*2.5/20/0.47
                realValue = (adcValue / 1024.0) * 2.5 / 20.0 / 0.47;
                sample["unit"] = "A";
                break;

            case 5: // B组电压检测（更新计算公式）
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
bool ProtocolHandler::parseNtcTemp(const QByteArray &data, int dataOffset, int dataLength)
{
    if (dataLength < 5) { // 至少需要通道(1字) + 时间戳(4字)
        qDebug() << "温度数据长度不足";
        return false;
    }

    // 通道(1字) + 时间戳(4字)
    uint16_t channel = bytesToUInt16_1(data, dataOffset);
    uint64_t timestamp = bytesToUInt64(data, dataOffset + 2);

//    qDebug() << "=== 温度数据包解析 ===";
//    qDebug() << "通道:" << channel << "时间戳:" << timestamp << "数据长度(字):" << dataLength;
//    qDebug() << "数据包总长度:" << data.size() << "字节";

    // 输出原始数据进行调试
//    QString hexData;
//    for (int i = dataOffset; i < qMin(data.size(), dataOffset + 60); i++) {
//        hexData += QString("%1 ").arg(static_cast<uint8_t>(data[i]), 2, 16, QChar('0'));
//    }
    //qDebug() << "原始数据前60字节:" << hexData;

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

//    qDebug() << "声明采样数:" << declaredSamples << "最大可能采样数:" << maxPossibleSamples
//             << "实际处理采样数:" << samplesCount;

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
        double voltage = convertRawToVoltage1(sampleValue,channel);
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

   // qDebug() << "温度解析完成，生成" << tempDataList.size() << "个数据项";

    if (!tempDataList.isEmpty()) {
        emit ntcTempReceived(tempDataList);
        return true;
    }

    return false;
}

double ProtocolHandler::convertRawToVoltage1(uint32_t rawValue, int channel)
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

void ProtocolHandler::setCountClear()
{
    _count = 0;
}

void ProtocolHandler::printCount()
{
    qDebug()<<"收数据次数:"<<_count;
}
double ProtocolHandler::convertResistanceToTemperature(double resistance)
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


double ProtocolHandler::convertNtcToTemperature(uint32_t ntcValue)
{

    const double CONSTANT_CURRENT = 50e-6; // 50µA
    double resistance = ntcValue / CONSTANT_CURRENT;
    return convertResistanceToTemperature(resistance);
}


bool ProtocolHandler::parseDiffVoltage(const QByteArray &data, int dataOffset, int dataLength)
{


    if (dataLength < 10) {
        qDebug() << "差分电压数据长度不足";
        return false;
    }

    uint16_t channel = bytesToUInt16_1(data, dataOffset);
    uint32_t timestamp2 = bytesToUInt32(data, dataOffset + 2);
    uint32_t timestamp1 = bytesToUInt32(data, dataOffset + 6);

    // 计算采样数量
    int samplesCount = (dataLength - 5) / 2;
    int dataSamplesOffset = dataOffset + 10;

    // 使用更高效的数据结构
    struct VoltageData {
        uint16_t channel;
        uint32_t timestamp1;
        uint32_t timestamp2;
        uint32_t rawValue;
        float voltage;
    };

    // 预分配向量，避免动态增长
    QVector<VoltageData> voltageDataBatch;
    voltageDataBatch.reserve(samplesCount);

    // 批量处理所有采样
    for (int i = 0; i < samplesCount; i++) {
        int byteOffset = dataSamplesOffset + i * 4;

        if (byteOffset + 3 >= data.size()) {
            break;
        }

        uint32_t sampleValue = bytesToUInt32(data, byteOffset);
        float voltage = convertRawToVoltage(sampleValue);

        voltageDataBatch.append({channel, timestamp1, timestamp2, sampleValue, voltage});
    }

    // 转换为QVariantMap格式（为了兼容现有接口）
    if (!voltageDataBatch.isEmpty()) {
        QVector<QVariantMap> voltageDataList;
        voltageDataList.reserve(voltageDataBatch.size());

        for (const auto& vd : voltageDataBatch) {
            QVariantMap sample;
            sample["type"] = "voltage";
            sample["channel"] = vd.channel;
            sample["timestamp1"] = vd.timestamp1;
            sample["timestamp2"] = vd.timestamp2;
            sample["rawValue"] = vd.rawValue;
            sample["voltage"] = vd.voltage;

            voltageDataList.append(sample);
        }

        emit diffVoltageReceived(voltageDataList);
    }

    return true;
}

double ProtocolHandler::convertRawToVoltage(uint32_t rawValue)
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

    // 计算电压值
    // 最大正值 0x7FFFF = 524287 对应 +5V
    // 最小负值 0x80000 = -524288 对应 -5V
    // 因此，电压换算公式为：voltage = signedValue * 5.0 / 524288
    float voltage = static_cast<double>(signedValue) * 5.0 / 524288.0;

    return voltage;
}


uint16_t ProtocolHandler::bytesToUInt16_1(const QByteArray &data, int offset)
{
    if (offset + 1 >= data.size()) {
        return 0;
    }


    return (static_cast<uint16_t>(static_cast<uint8_t>(data[offset])) ) |
            static_cast<uint16_t>(static_cast<uint8_t>(data[offset + 1])<< 8);
}

uint32_t ProtocolHandler::bytesToUInt32(const QByteArray &data, int offset)
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

uint64_t ProtocolHandler::bytesToUInt64(const QByteArray &data, int offset)
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
