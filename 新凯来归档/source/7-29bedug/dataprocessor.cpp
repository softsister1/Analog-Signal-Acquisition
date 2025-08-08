#include "dataprocessor.h"
#include <QMutexLocker>
#include <QtMath>
#include <QDebug>

DataProcessor::DataProcessor(QObject *parent) : QObject(parent),
    activeGroup(0),
    expectedTotalPoints(0)
{
    // 初始化数据存储结构
    voltageData.resize(2);
    voltageData[0].resize(8);  // A组8个通道
    voltageData[1].resize(4);  // B组4个通道

    rawVoltageData.resize(2);
    rawVoltageData[0].resize(8);
    rawVoltageData[1].resize(4);
}

void DataProcessor::processData(const QVector<QVariantMap>& data)
{
    QMutexLocker locker(&dataMutex);

    for (const auto& sample : data) {
        int channel = sample["channel"].toInt();
        float voltage = sample["voltage"].toDouble();
        uint32_t rawValue = sample["rawValue"].toUInt();

        if (channel < 8 && activeGroup == 0) {
            voltageData[0][channel].append(voltage);
            rawVoltageData[0][channel].append(rawValue);
        } else if (channel >= 8 && channel < 12 && activeGroup == 1) {
            int bGroupChannel = channel - 8;
            voltageData[1][bGroupChannel].append(voltage);
            rawVoltageData[1][bGroupChannel].append(rawValue);
        }
    }

    // 检查进度
    int currentPoints = 0;
    int channelCount = (activeGroup == 0) ? 8 : 4;

    for (int ch = 0; ch < channelCount; ch++) {
        currentPoints = qMax(currentPoints, voltageData[activeGroup][ch].size());
    }

    // 发送进度更新
    if (currentPoints % 1000 == 0) {
        emit updateProgress(currentPoints, expectedTotalPoints);
    }

    // 检查是否完成
    if (currentPoints >= expectedTotalPoints) {
        emit acquisitionComplete(currentPoints);
    }

    // 发送处理完成信号
    emit dataProcessed(channelCount, currentPoints);
}

void DataProcessor::clearData()
{
    QMutexLocker locker(&dataMutex);

    // 清空所有通道数据
    for (int group = 0; group < 2; group++) {
        for (int ch = 0; ch < voltageData[group].size(); ch++) {
            voltageData[group][ch].clear();
            rawVoltageData[group][ch].clear();
        }
    }
}

QVector<double> DataProcessor::getVoltageData(int group, int channel)
{
    QMutexLocker locker(&dataMutex);

    if (group < 0 || group >= voltageData.size() ||
        channel < 0 || channel >= voltageData[group].size()) {
        return QVector<double>();
    }

    return voltageData[group][channel];
}

QVector<uint32_t> DataProcessor::getRawVoltageData(int group, int channel)
{
    QMutexLocker locker(&dataMutex);

    if (group < 0 || group >= rawVoltageData.size() ||
        channel < 0 || channel >= rawVoltageData[group].size()) {
        return QVector<uint32_t>();
    }

    return rawVoltageData[group][channel];
}
int DataProcessor::getChannelDataSize(int group, int channel)
{
    QMutexLocker locker(&dataMutex);

    if (group < 0 || group >= voltageData.size() ||
        channel < 0 || channel >= voltageData[group].size()) {
        return 0;
    }

    return voltageData[group][channel].size();
}

int DataProcessor::getMaxDataSize(int group)
{
    QMutexLocker locker(&dataMutex);

    if (group < 0 || group >= voltageData.size()) {
        return 0;
    }

    int maxSize = 0;
    for (int ch = 0; ch < voltageData[group].size(); ch++) {
        maxSize = qMax(maxSize, voltageData[group][ch].size());
    }

    return maxSize;
}

QVector<QVector<double>> DataProcessor::getAllVoltageData(int group)
{
    QMutexLocker locker(&dataMutex);

    if (group < 0 || group >= voltageData.size()) {
        return QVector<QVector<double>>();
    }

    return voltageData[group];
}

QVector<QVector<uint32_t>> DataProcessor::getAllRawVoltageData(int group)
{
    QMutexLocker locker(&dataMutex);

    if (group < 0 || group >= rawVoltageData.size()) {
        return QVector<QVector<uint32_t>>();
    }

    return rawVoltageData[group];
}
