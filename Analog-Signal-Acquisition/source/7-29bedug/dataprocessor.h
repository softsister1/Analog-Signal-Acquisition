#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include <QVector>
#include <QVariantMap>
#include <QMutex>
#include <stdint.h>

class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(QObject *parent = nullptr);

    void setActiveGroup(int group) { activeGroup = group; }
    void setExpectedPoints(int points) { expectedTotalPoints = points; }

    // 获取处理后的数据
    QVector<double> getVoltageData(int group, int channel);
    QVector<uint32_t> getRawVoltageData(int group, int channel);
    int getChannelDataSize(int group, int channel);
        int getMaxDataSize(int group);
        QVector<QVector<double>> getAllVoltageData(int group);
        QVector<QVector<uint32_t>> getAllRawVoltageData(int group);

public slots:
    void processData(const QVector<QVariantMap>& data);
    void clearData();

signals:
    void dataProcessed(int channelCount, int currentPoints);
    void acquisitionComplete(int totalPoints);
    void updateProgress(int current, int total);

private:
    QMutex dataMutex;
    int activeGroup;
    int expectedTotalPoints;
    QVector<QVector<QVector<double>>> voltageData;
    QVector<QVector<QVector<uint32_t>>> rawVoltageData;
};

#endif // DATAPROCESSOR_H
