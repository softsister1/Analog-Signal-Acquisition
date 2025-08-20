#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QTableWidget>
#include <QScrollArea>
#include <QTimer>
#include <QVector>
#include <QMap>
#include <QFileDialog>
#include <QMessageBox>
#include <memory>
#include "qcustomplot.h"
#include "usbcommunication.h"

const uint32_t REG_POWER_DATA_ENABLE = 0x04;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 连接控制
    void onAcquisitionCompleted(int totalPackets);
        void onAcquisitionProgress(int current, int total);
    void onConnectButtonClicked();
    void onDisconnectButtonClicked();
    void onConnectionChanged(bool connected);
    void updatePowerUI();
    // 组和通道选择
    void onGroupSelectionChanged(int index);
    // 电源控制
    void onPowerPlusChanged(bool checked);
    void onPowerMinusChanged(bool checked);
    void onDIReadTimer();
    // 数字输出控制
    void onDO1Changed(bool checked);
    void onDO2Changed(bool checked);
    //打开升级工具
    void onUpgradeOnlineClicked();
    // 延时配置
    void onAdcDelayChanged(double value);
    void onIoHighDelayChanged(double value);
    void onIoLowDelayChanged(double value);

    // 采样配置
    void onCycleCountChanged(double value);
    void onCyclePointsPerCycleChanged(double value);

    // 触发模式
    void onTriggerModeChanged(int id);

    // 采集控制
    void onStartAcquisitionClicked();
    void onSaveDataClicked();

    // 显示控制
    void onDisplayModeChanged(int id);
    void onCycleNumberChanged(int value);

    // 数据接收
    void onRegisterDataReceived(uint32_t regAddr, uint32_t regData);
    void onPowerDataReceived(const QVector<QVariantMap> &powerData);
    void onTemperatureDataReceived(const QVector<QVariantMap> &tempData);
    void onDigitalInputChanged(bool isGroupA, uint8_t diValue);
    void onErrorOccurred(const QString &message);

    // 定时器处理
    void onStatusUpdateTimer();

    // 开始采集定时处理
    void onStartRecvTimer();

private:
    std::atomic<bool> acquisitionCompleted{false};
    // UI初始化
    QVector<QVector<QVector<uint32_t>>> timestamp1Data; // [group][channel][sample]
    QVector<QVector<QVector<uint32_t>>> timestamp2Data; // [group][channel][sample]
    void setupUi();
    void clearAllData();
        void limitDataSize();
        QMutex voltageDataMutex;
    bool chartsInitialized;
    QVector<QVector<QVector<uint32_t>>> rawVoltageData;
    void initializeRawDataStorage();
    // 预分配相关
        bool isDataPreallocated;
        int expectedTotalPoints;
        int currentDataIndex[2][8]; // 记录每个通道当前写入位置

        void preallocateMemory();
        void resetDataIndices();
        QLabel *aVoltageLabel;
            QLabel *aCurrentLabel;
            QLabel *aPowerLabel;
            QLabel *bVoltageLabel;
            QLabel *bCurrentLabel;
            QLabel *bPowerLabel;

        // 新增的图表初始化和更新方法
        void initializeCharts();
        void updateVoltageChartData();
        void updateTemperatureChartData();
        void stopSimulation();
        void startSimulation();
        void updatePowerAndDigitalStatus();
        void updatePowerDisplays(const QMap<int, double> &channelValues);


    // 更新函数
    void updateChannelComboBox();
    void updateGroupStatusTable();
    void updateCharts();
    void updateDIStatus();
    // 连接设备
    void autoConnect();

    void completeAcquisition(int currentPoints);
    void checkAcquisitionCompletion();
    struct PowerDataPoint {
            qint64 timestamp;       // 时间戳
            double voltage;         // 电压值
            double current;         // 电流值
            double power;           // 功率值
            int dataIndex;          // 对应的数据索引（用于匹配差分电压数据）
        };

        QVector<PowerDataPoint> aPowerHistory;  // A组功率历史
        QVector<PowerDataPoint> bPowerHistory;  // B组功率历史

        int currentVoltageDataIndex;

        void clearPowerHistory();
        void savePowerDataToHistory();
        void updateChartsFromCache();
    // 发送配置到设备
    void sendAllConfigsToDevice();
    void updateChartData();
    QString formatTimespan(qint64 microseconds);

    // USB通信模块
    UsbCommunication *usbComm;

    // 状态变量
    int activeGroup;         // 当前激活的组 (0=A组, 1=B组)
    //onChannelSelectionChangedint activeChannel;       // 当前激活的通道
    int currentDisplayMode;  // 当前显示模式 (0=均值, 1=单周期)
    int currentCycleNumber;  // 当前显示的周期编号
    bool acquisitionInProgress; // 采集是否正在进行
    bool dataReady;          // 数据是否已准备好
    bool connected;          // 设备是否已连接

    // 数据存储
    QVector<bool> diStatus;  // 数字输入状态
    QVector<double> tempValues; // 温度值
    QVector<QVector<QVector<double>>> voltageData; // [组][通道][采样点]
    QVector<QVector<QVector<qint64>>> timeStamp4Voltage;
    QVector<qint64> timeStamp4Temp;
    // UI控件 - 连接区域
    QLabel *connectionStatusLabel;
    QPushButton *connectButton;
    QPushButton *disconnectButton;

    // UI控件 - 配置区域
    QComboBox *groupSelectionCombo;
    //QComboBox *channelSelectionCombo;
    QCheckBox *do1CheckBox;
    QCheckBox *do2CheckBox;
    QCheckBox *powerPlusCheckBox;
    QCheckBox *powerMinusCheckBox;
    QDoubleSpinBox *adcDelaySpinBox;
    QDoubleSpinBox *ioHighDelaySpinBox;
    QDoubleSpinBox *ioLowDelaySpinBox;
    QDoubleSpinBox *cycleCountSpinBox;
    QDoubleSpinBox *cycleCountSpinBox1;

    // UI控件 - 触发模式
    QButtonGroup *triggerModeGroup;
    QRadioButton *internalTriggerRadio;
    QRadioButton *externalTriggerRadio1;
    QRadioButton *externalTriggerRadio2;

    // UI控件 - 采集控制
    QPushButton *startAcquisitionButton;
    QPushButton *saveDataButton;

    // UI控件 - 状态显示
    QTableWidget *diStatusTable;
    QTableWidget *groupStatusTable;
    //在线升级
    QPushButton *upgradeButton;

    // UI控件 - 数据显示
    QButtonGroup *displayModeGroup;
    QRadioButton *averageDisplayRadio;
    QRadioButton *singleCycleDisplayRadio;
    QDoubleSpinBox *cycleNumberSpinBox;
    QScrollArea *chartsScrollArea;

    // UI控件 - 板卡信息
    QLabel *boardVoltageLabel;
    QLabel *boardCurrentLabel;
    QLabel *boardPowerLabel;
    QLabel *boardTemperatureLabel;
    QLabel *firmwareVersionLabel;

    // 图表容器
    QVector<QCustomPlot*> voltageCharts;
    QVector<QGroupBox*> voltageGroupBoxs;
    QVector<QCustomPlot*> tempCharts;
    QVector<QGroupBox*> tempGroupBoxs;
    // 定时器
    QTimer *statusUpdateTimer;
        QTimer* powerUpdateTimer;                   // 功率数据更新定时器
            QMap<int, double> latestPowerValues;        // 缓存最新的功率数据值
            QMutex powerDataMutex;
            QTimer *diReadTimer;
            std::shared_ptr<QTimer> _pTimer;


};

#endif // MAINWINDOW_H
