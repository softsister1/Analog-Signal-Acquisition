#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QStatusBar>
#include <QRandomGenerator>
#include <QtMath>
#include <chrono>
#include "qcustomplot.h"
#pragma execution_character_set("UTF-8")
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    activeGroup(0),
    currentDisplayMode(0),
    currentCycleNumber(0),
    acquisitionInProgress(false),
    usbComm(nullptr),
    dataReady(false),
    connected(false),
    chartsInitialized(false)

{
    diStatus.resize(12);

        // A组默认状态：DI1-DI4,DI6为高，DI5为低
        diStatus[0] = true;  // DI1 - 高
        diStatus[1] = true;  // DI2 - 高
        diStatus[2] = true;  // DI3 - 高
        diStatus[3] = true;  // DI4 - 高
        diStatus[4] = false; // DI5 - 低
        diStatus[5] = true;  // DI6 - 高

        // B组默认状态：DI1-DI6全部为高
        diStatus[6] = true;  // DI1 - 高
        diStatus[7] = true;  // DI2 - 高
        diStatus[8] = true;  // DI3 - 高
        diStatus[9] = true;  // DI4 - 高
        diStatus[10] = true; // DI5 - 高
        diStatus[11] = true; // DI6 - 高

    // 初始化温度值
    tempValues.resize(4);
    for(int i = 0; i < 4; i++) {
        tempValues[i] = 0;
    }

    // 初始化电压数据存储
    voltageData.resize(2); // 两个组
    voltageData[0].resize(8); // A组8个通道
    voltageData[1].resize(4); // B组4个通道

    // 初始化温度时间戳数据存储
    timeStamp4Temp.resize(4); // 两个组
    for(int i = 0; i < 4; i++) {
        timeStamp4Temp[i] = 0;
    }
    // 初始化UI
    setupUi();

    // 创建USB通信模块
    usbComm = new UsbCommunication(this);

    // 连接信号
    connect(usbComm, &UsbCommunication::connectionChanged, this, &MainWindow::onConnectionChanged);
    connect(usbComm, &UsbCommunication::errorOccurred, this, &MainWindow::onErrorOccurred);
    connect(usbComm, &UsbCommunication::registerReadResult, this, &MainWindow::onRegisterDataReceived);
    connect(usbComm, &UsbCommunication::powerDataReceived, this, &MainWindow::onPowerDataReceived);
    connect(usbComm, &UsbCommunication::temperatureDataReceived, this, &MainWindow::onTemperatureDataReceived);
    connect(usbComm, &UsbCommunication::digitalInputChanged, this, &MainWindow::onDigitalInputChanged);
    connect(usbComm, &UsbCommunication::acquisitionCompleted, this, &MainWindow::onAcquisitionCompleted);
    connect(usbComm, &UsbCommunication::acquisitionProgress, this, &MainWindow::onAcquisitionProgress);
    connect(usbComm, &UsbCommunication::digitalInputChanged, this, &MainWindow::onDigitalInputChanged);
    // 设置定时器
    statusUpdateTimer = new QTimer(this);
    connect(statusUpdateTimer, &QTimer::timeout, this, &MainWindow::onStatusUpdateTimer);
    statusUpdateTimer->start(1000); // 每秒更新一次状态
    powerUpdateTimer = new QTimer(this);
        powerUpdateTimer->setSingleShot(false);
        powerUpdateTimer->setInterval(1000);
    connect(powerUpdateTimer, &QTimer::timeout, this, &MainWindow::updatePowerUI);
    diReadTimer = new QTimer(this);
       connect(diReadTimer, &QTimer::timeout, this, &MainWindow::onDIReadTimer);
       diReadTimer->setInterval(2000); // 每2秒读取一次DI状态

    // 设置状态栏
    statusBar()->showMessage("准备就绪");
    updateCharts();
    initializeRawDataStorage();
    //设置定时器
    _pTimer = std::make_shared<QTimer>(this);
    QObject::connect(_pTimer.get(),&QTimer::timeout,this,&MainWindow::onStartRecvTimer);

}
MainWindow::~MainWindow()
{
    // 停止定时器
    if (statusUpdateTimer) {
        statusUpdateTimer->stop();
    }
    if (powerUpdateTimer) {
        powerUpdateTimer->stop();
    }
    if (diReadTimer) {
        diReadTimer->stop();
    }
    // 断开设备连接
    if (usbComm) {
        if (usbComm->isConnected()) {
            usbComm->stopCommunication();
        }
        delete usbComm;
    }
    // 释放图表资源
    for (QCustomPlot* plot : voltageCharts) {
        if(plot)
            delete plot;
    }
    voltageCharts.clear();
    for(QGroupBox* group:voltageGroupBoxs)
    {
        if(group)
            delete group;
    }
    voltageGroupBoxs.clear();

    for (QCustomPlot* plot : tempCharts) {
        if(plot)
            delete plot;
    }
    tempCharts.clear();

    for(QGroupBox * group : tempGroupBoxs)
    {
        if(group)
            delete group;
    }
    tempGroupBoxs.clear();
}


void MainWindow::setupUi()
{
    // 设置窗口大小
    resize(1280, 800);
    setWindowTitle(" Analog Signal_V1.1.9");

    // 创建中央部件和主布局
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // 创建左右两个主面板
    QWidget *leftPanel = new QWidget;
    QWidget *rightPanel = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // 左上部分 (配置区域)
    QGroupBox *configGroupBox = new QGroupBox("采集配置");
    QGridLayout *configLayout = new QGridLayout(configGroupBox);

    // 组选择
    configLayout->addWidget(new QLabel("选择组:"), 0, 0);
        groupSelectionCombo = new QComboBox;
        groupSelectionCombo->addItem("A组");
        groupSelectionCombo->addItem("B组");
        configLayout->addWidget(groupSelectionCombo, 0, 1, 1, 2);

        // 数字输出控制 - 现在分为DO1和DO2
        do1CheckBox = new QCheckBox("5V_EN");
        do2CheckBox = new QCheckBox("3V3_CTRL");
        configLayout->addWidget(do1CheckBox, 1, 0, 1, 1);
        configLayout->addWidget(do2CheckBox, 1, 1, 1, 2);

    //
    powerPlusCheckBox = new QCheckBox("+5V");
    powerMinusCheckBox = new QCheckBox("-5V");
    configLayout->addWidget(powerPlusCheckBox, 2, 0, 1, 1);
    configLayout->addWidget(powerMinusCheckBox, 2, 1, 1, 2);

    // 延时配置
    configLayout->addWidget(new QLabel("ADC采样延时(us):"), 4, 0);
    adcDelaySpinBox = new QDoubleSpinBox;  // 使用QDoubleSpinBox
    adcDelaySpinBox->setRange(1.0, 16.0);
    adcDelaySpinBox->setValue(1.0);
    adcDelaySpinBox->setSingleStep(0.1);   // 设置小数步长
    adcDelaySpinBox->setDecimals(1);       // 显示1位小数
    configLayout->addWidget(adcDelaySpinBox, 4, 1, 1, 2);

    configLayout->addWidget(new QLabel("IO拉高延时(us):"), 5, 0);
    ioHighDelaySpinBox = new QDoubleSpinBox;  // 使用QDoubleSpinBox
    ioHighDelaySpinBox->setRange(6.0, 20.0);
    ioHighDelaySpinBox->setValue(16.0);
    ioHighDelaySpinBox->setSingleStep(0.1);   // 设置小数步长
    ioHighDelaySpinBox->setDecimals(1);       // 显示1位小数
    configLayout->addWidget(ioHighDelaySpinBox, 5, 1, 1, 2);

    configLayout->addWidget(new QLabel("IO拉低延时(us):"), 6, 0);
    ioLowDelaySpinBox = new QDoubleSpinBox;   // 使用QDoubleSpinBox
    ioLowDelaySpinBox->setRange(1.0, 13.0);
    ioLowDelaySpinBox->setValue(2.0);
    ioLowDelaySpinBox->setSingleStep(0.1);    // 设置小数步长
    ioLowDelaySpinBox->setDecimals(1);        // 显示1位小数
    configLayout->addWidget(ioLowDelaySpinBox, 6, 1, 1, 2);

    // 周期数
    configLayout->addWidget(new QLabel("采样周期数:"), 7, 0);
    cycleCountSpinBox = new QDoubleSpinBox;
    cycleCountSpinBox->setRange(1, 10000000);
    cycleCountSpinBox->setValue(1000);
    configLayout->addWidget(cycleCountSpinBox, 7, 1, 1, 2);

    configLayout->addWidget(new QLabel("ADC采样点数:"), 8, 0);
    cycleCountSpinBox1 = new QDoubleSpinBox;
    cycleCountSpinBox1->setRange(2, 20);
    cycleCountSpinBox1->setValue(20);
    configLayout->addWidget(cycleCountSpinBox1, 8, 1, 1, 2);

    // 触发模式
    QGroupBox *triggerGroupBox = new QGroupBox("触发模式");
    QHBoxLayout *triggerLayout = new QHBoxLayout(triggerGroupBox);

    triggerModeGroup = new QButtonGroup(this);
    internalTriggerRadio = new QRadioButton("内触发");
    externalTriggerRadio1 = new QRadioButton("外触发");
    //externalTriggerRadio2 = new QRadioButton("外触发2");
    internalTriggerRadio->setChecked(true);

    triggerModeGroup->addButton(internalTriggerRadio, 0);
    triggerModeGroup->addButton(externalTriggerRadio1, 1);
    //triggerModeGroup->addButton(externalTriggerRadio2, 2);

    triggerLayout->addWidget(internalTriggerRadio);
    triggerLayout->addWidget(externalTriggerRadio1);
    //triggerLayout->addWidget(externalTriggerRadio2);
    configLayout->addWidget(triggerGroupBox, 9, 0, 1, 3);

    // 控制按钮
    startAcquisitionButton = new QPushButton("开始采集");
    saveDataButton = new QPushButton("保存数据");
    saveDataButton->setEnabled(false);

    configLayout->addWidget(startAcquisitionButton, 10, 0, 1, 3);
    configLayout->addWidget(saveDataButton, 11, 0, 1, 3);

    // 连接控制区域
    QHBoxLayout *connectionLayout = new QHBoxLayout;
    connectionStatusLabel = new QLabel("未连接");
    connectionStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    connectButton = new QPushButton("连接设备");
    disconnectButton = new QPushButton("断开连接");
    disconnectButton->setEnabled(false);
   upgradeButton = new QPushButton("在线升级");
    upgradeButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; font-weight: bold; }");
    connectionLayout->addWidget(upgradeButton);

    connectionLayout->addWidget(new QLabel("设备状态:"));
    connectionLayout->addWidget(connectionStatusLabel);
    connectionLayout->addStretch();
    connectionLayout->addWidget(connectButton);
    connectionLayout->addWidget(disconnectButton);

    // 左下部分 (系统状态区域)
    QGroupBox *statusGroupBox = new QGroupBox("系统状态");

    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroupBox);
    diStatusTable = new QTableWidget(1, 7);
    diStatusTable->setHorizontalHeaderLabels(QStringList() << "参数" << "DI1" << "DI2" << "DI3" << "DI4" << "DI5" << "DI6");
    diStatusTable->verticalHeader()->setVisible(false);
    diStatusTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 设置第一列
    QTableWidgetItem *diLabelItem = new QTableWidgetItem("A组输入");
    diLabelItem->setFlags(diLabelItem->flags() & ~Qt::ItemIsEditable);
    diStatusTable->setItem(0, 0, diLabelItem);

    // 设置DI1-DI6状态
    for (int i = 1; i <= 6; i++) {
        QTableWidgetItem *diItem = new QTableWidgetItem("低");
        diItem->setFlags(diItem->flags() & ~Qt::ItemIsEditable);
        diStatusTable->setItem(0, i, diItem);
    }

    // 组状态表格
    groupStatusTable = new QTableWidget(1, 3);
    groupStatusTable->setHorizontalHeaderLabels(QStringList() << "参数" << "值" << "单位");
    groupStatusTable->verticalHeader()->setVisible(false);
    groupStatusTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 初始化A组的温度行
    QTableWidgetItem *tempParamItem = new QTableWidgetItem("温度CH0");
    tempParamItem->setFlags(tempParamItem->flags() & ~Qt::ItemIsEditable);
    groupStatusTable->setItem(0, 0, tempParamItem);

    QTableWidgetItem *tempValueItem = new QTableWidgetItem("--");
    tempValueItem->setFlags(tempValueItem->flags() & ~Qt::ItemIsEditable);
    groupStatusTable->setItem(0, 1, tempValueItem);

    QTableWidgetItem *tempUnitItem = new QTableWidgetItem("°C");
    tempUnitItem->setFlags(tempUnitItem->flags() & ~Qt::ItemIsEditable);
    groupStatusTable->setItem(0, 2, tempUnitItem);

//        QStringList paramNames;
//        paramNames << "电压" << "电流" << "温度"; // 初始只有一个温度参数

//        for (int i = 0; i < paramNames.size(); i++) {
//            QTableWidgetItem *item = new QTableWidgetItem(paramNames[i]);
//            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
//            groupStatusTable->setItem(i, 0, item);

//            QTableWidgetItem *valueItem = new QTableWidgetItem("--");
//            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
//            groupStatusTable->setItem(i, 1, valueItem);

//            QTableWidgetItem *unitItem = new QTableWidgetItem(i == 0 ? "V" : (i == 1 ? "A" : "°C"));
//            unitItem->setFlags(unitItem->flags() & ~Qt::ItemIsEditable);
//            groupStatusTable->setItem(i, 2, unitItem);
//        }

    statusLayout->addWidget(diStatusTable);
    statusLayout->addWidget(groupStatusTable);

    // 添加到左侧面板
    leftLayout->addWidget(configGroupBox, 3);
    leftLayout->addLayout(connectionLayout);
    leftLayout->addWidget(statusGroupBox, 2);


    // 右上部分 (数据可视化区域)
    QGroupBox *dataVisGroupBox = new QGroupBox("数据可视化");
    QVBoxLayout *dataVisLayout = new QVBoxLayout(dataVisGroupBox);

    // 显示模式选择
    QHBoxLayout *displayModeLayout = new QHBoxLayout;
    displayModeGroup = new QButtonGroup(this);
    averageDisplayRadio = new QRadioButton("显示多周期均值");
    singleCycleDisplayRadio = new QRadioButton("显示单周期数据");
    averageDisplayRadio->setChecked(true);

    displayModeGroup->addButton(averageDisplayRadio, 0);
    displayModeGroup->addButton(singleCycleDisplayRadio, 1);

    displayModeLayout->addWidget(averageDisplayRadio);
    displayModeLayout->addWidget(singleCycleDisplayRadio);

    cycleNumberSpinBox = new QDoubleSpinBox;
    cycleNumberSpinBox->setRange(0, 999999999);
    cycleNumberSpinBox->setValue(0);
    cycleNumberSpinBox->setEnabled(false);

    displayModeLayout->addWidget(new QLabel("周期编号:"));
    displayModeLayout->addWidget(cycleNumberSpinBox);
    displayModeLayout->addStretch();

    dataVisLayout->addLayout(displayModeLayout);

    // 创建滚动区域用于包含多个图表
    chartsScrollArea = new QScrollArea;
    chartsScrollArea->setWidgetResizable(true);
    chartsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    chartsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    dataVisLayout->addWidget(chartsScrollArea, 1);

    QGroupBox *sysInfoGroupBox = new QGroupBox();
    QHBoxLayout *sysInfoMainLayout = new QHBoxLayout(sysInfoGroupBox);
    sysInfoMainLayout->setSpacing(10); // 设置列间距

    // 第一列：板卡基本信息
    QGroupBox *boardInfoBox = new QGroupBox("板卡信息");
    QFormLayout *boardInfoLayout = new QFormLayout(boardInfoBox);
    boardInfoLayout->setContentsMargins(5, 5, 5, 5);

    boardVoltageLabel = new QLabel("--");
    boardCurrentLabel = new QLabel("--");
    boardPowerLabel = new QLabel("--");
    firmwareVersionLabel = new QLabel("--");

    boardInfoLayout->addRow("板卡电压:", boardVoltageLabel);
    boardInfoLayout->addRow("板卡电流:", boardCurrentLabel);
    boardInfoLayout->addRow("板卡功耗:", boardPowerLabel);
    boardInfoLayout->addRow("固件版本:", firmwareVersionLabel);

    // 第二列：A组信息
    QGroupBox *aGroupBox = new QGroupBox("+5V");
    QFormLayout *aGroupLayout = new QFormLayout(aGroupBox);
    aGroupLayout->setContentsMargins(5, 5, 5, 5);

    aVoltageLabel = new QLabel("--");
    aCurrentLabel = new QLabel("--");
    aPowerLabel = new QLabel("--");

    aGroupLayout->addRow("电压:", aVoltageLabel);
    aGroupLayout->addRow("电流:", aCurrentLabel);
    aGroupLayout->addRow("功耗:", aPowerLabel);
    aGroupLayout->addRow("", new QLabel(""));

    // 第三列：B组信息
    QGroupBox *bGroupBox = new QGroupBox("-5V");
    QFormLayout *bGroupLayout = new QFormLayout(bGroupBox);
    bGroupLayout->setContentsMargins(5, 5, 5, 5);

    bVoltageLabel = new QLabel("--");
    bCurrentLabel = new QLabel("--");
    bPowerLabel = new QLabel("--");

    bGroupLayout->addRow("电压:", bVoltageLabel);
    bGroupLayout->addRow("电流:", bCurrentLabel);
    bGroupLayout->addRow("功耗:", bPowerLabel);
    // 添加一个空行以保持高度一致
    bGroupLayout->addRow("", new QLabel(""));

    // 将三列添加到主布局，使用相同的拉伸因子
    sysInfoMainLayout->addWidget(boardInfoBox, 1);
    sysInfoMainLayout->addWidget(aGroupBox, 1);
    sysInfoMainLayout->addWidget(bGroupBox, 1);

    // 确保所有列的高度相同
    boardInfoBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    aGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    boardInfoBox->setMinimumHeight(120);
    aGroupBox->setMinimumHeight(120);
    bGroupBox->setMinimumHeight(120);


        rightLayout->addWidget(dataVisGroupBox, 4);  // 数据可视化区域
        rightLayout->addWidget(sysInfoGroupBox, 1);  // 系统信息区域

        // 设置主布局比例
        mainLayout->addWidget(leftPanel, 2);
        mainLayout->addWidget(rightPanel, 3);

    // 连接信号和槽
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWindow::onDisconnectButtonClicked);
    connect(groupSelectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onGroupSelectionChanged);
    connect(startAcquisitionButton, &QPushButton::clicked, this, &MainWindow::onStartAcquisitionClicked);
    connect(saveDataButton, &QPushButton::clicked, this, &MainWindow::onSaveDataClicked);
    connect(triggerModeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &MainWindow::onTriggerModeChanged);
    connect(displayModeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &MainWindow::onDisplayModeChanged);
    connect(cycleNumberSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCycleNumberChanged);
    connect(adcDelaySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onAdcDelayChanged);
    connect(ioHighDelaySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onIoHighDelayChanged);
    connect(ioLowDelaySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onIoLowDelayChanged);
    connect(do1CheckBox, &QCheckBox::toggled, this, &MainWindow::onDO1Changed);
    connect(do2CheckBox, &QCheckBox::toggled, this, &MainWindow::onDO2Changed);
    connect(powerPlusCheckBox, &QCheckBox::toggled, this, &MainWindow::onPowerPlusChanged);
    connect(powerMinusCheckBox, &QCheckBox::toggled, this, &MainWindow::onPowerMinusChanged);
    connect(cycleCountSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onCycleCountChanged);
    connect(cycleCountSpinBox1, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onCyclePointsPerCycleChanged);
    connect(upgradeButton, &QPushButton::clicked, this, &MainWindow::onUpgradeOnlineClicked);
}
void MainWindow::clearAllData()
{
    QMutexLocker locker(&voltageDataMutex);

    qDebug() << "开始清理数据...";

    for (int group = 0; group < voltageData.size(); group++) {
            for (int ch = 0; ch < voltageData[group].size(); ch++) {
                voltageData[group][ch].clear();
                rawVoltageData[group][ch].clear();
                // 新增：清理时间戳数据
                timestamp1Data[group][ch].clear();
                timestamp2Data[group][ch].clear();
            }
        }


    // 清理功率历史数据
    aPowerHistory.clear();
    bPowerHistory.clear();
    aPowerHistory.squeeze();
    bPowerHistory.squeeze();

    qDebug() << "数据清理完成";
}
// 自动连接设备
void MainWindow::autoConnect()
{
    onConnectButtonClicked();
}

void MainWindow::onUpgradeOnlineClicked()
{
    // 先禁止功率包发送
    if (connected && usbComm) {
        usbComm->writeRegister(REG_POWER_DATA_ENABLE, 0);
        QThread::msleep(200);
    }

    // 断开设备连接
    if (connected) {
        onDisconnectButtonClicked();
        QThread::msleep(500);
    }

    // 启动升级程序
    QProcess::startDetached("upgradeTool1.0.exe");
}


// 添加更新组状态表格的函数
//void MainWindow::updateGroupStatusTable()
//{


//    // 获取当前温度传感器数量
//    int tempSensorCount = (activeGroup == 0) ? 1 : 3; // A组1个温度传感器，B组3个温度传感器

//    // 计算表格总行数：2个基本参数(电压、电流) + 温度传感器数量
//    int totalRows = 2 + tempSensorCount;

//    // 确保表格有足够的行
//    if (groupStatusTable->rowCount() != totalRows) {
//        groupStatusTable->setRowCount(totalRows);
//    }

//    // 设置基本参数（电压、电流）
//    QStringList baseParams;
//    baseParams << "电压" << "电流";

//    for (int i = 0; i < baseParams.size(); i++) {
//        // 确保单元格存在
//        QTableWidgetItem *paramItem = groupStatusTable->item(i, 0);
//        if (!paramItem) {
//            paramItem = new QTableWidgetItem(baseParams[i]);
//            paramItem->setFlags(paramItem->flags() & ~Qt::ItemIsEditable);
//            groupStatusTable->setItem(i, 0, paramItem);
//        } else {
//            paramItem->setText(baseParams[i]);
//        }

//        // 确保值单元格存在
//        QTableWidgetItem *valueItem = groupStatusTable->item(i, 1);
//        if (!valueItem) {
//            valueItem = new QTableWidgetItem("--");
//            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
//            groupStatusTable->setItem(i, 1, valueItem);
//        }

//        // 确保单位单元格存在
//        QTableWidgetItem *unitItem = groupStatusTable->item(i, 2);
//        if (!unitItem) {
//            unitItem = new QTableWidgetItem(i == 0 ? "V" : "A");
//            unitItem->setFlags(unitItem->flags() & ~Qt::ItemIsEditable);
//            groupStatusTable->setItem(i, 2, unitItem);
//        } else {
//            unitItem->setText(i == 0 ? "V" : "A");
//        }
//    }

//    // 设置温度参数
//    for (int i = 0; i < tempSensorCount; i++) {
//        int row = 2 + i; // 温度行从第3行开始

//        // 温度参数名称
//        QString tempName = (activeGroup == 0) ?
//                         "温度" :
//                         QString("温度CH%1").arg(i);

//        // 确保单元格存在
//        QTableWidgetItem *paramItem = groupStatusTable->item(row, 0);
//        if (!paramItem) {
//            paramItem = new QTableWidgetItem(tempName);
//            paramItem->setFlags(paramItem->flags() & ~Qt::ItemIsEditable);
//            groupStatusTable->setItem(row, 0, paramItem);
//        } else {
//            paramItem->setText(tempName);
//        }

//        // 确保值单元格存在
//        QTableWidgetItem *valueItem = groupStatusTable->item(row, 1);
//        if (!valueItem) {
//            valueItem = new QTableWidgetItem(QString::number(tempValues[i == 0 ? 0 : i], 'f', 1));
//            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
//            groupStatusTable->setItem(row, 1, valueItem);
//        } else {
//            valueItem->setText(QString::number(tempValues[i == 0 ? 0 : i], 'f', 1));
//        }

//        // 确保单位单元格存在
//        QTableWidgetItem *unitItem = groupStatusTable->item(row, 2);
//        if (!unitItem) {
//            unitItem = new QTableWidgetItem("°C");
//            unitItem->setFlags(unitItem->flags() & ~Qt::ItemIsEditable);
//            groupStatusTable->setItem(row, 2, unitItem);
//        } else {
//            unitItem->setText("°C");
//        }
//    }
//}
void MainWindow::updateGroupStatusTable()
{
    // 获取当前温度传感器数量
    int tempSensorCount = (activeGroup == 0) ? 1 : 3; // A组1个温度传感器，B组3个温度传感器

    // 设置表格行数为温度传感器数量
    groupStatusTable->setRowCount(tempSensorCount);

    // 设置温度参数
    for (int i = 0; i < tempSensorCount; i++) {
        // 温度参数名称
        QString tempName = (activeGroup == 0) ?
                         "温度" :
                         QString("温度CH%1").arg(i);

        // 参数名称列
        QTableWidgetItem *paramItem = groupStatusTable->item(i, 0);
        if (!paramItem) {
            paramItem = new QTableWidgetItem(tempName);
            paramItem->setFlags(paramItem->flags() & ~Qt::ItemIsEditable);
            groupStatusTable->setItem(i, 0, paramItem);
        } else {
            paramItem->setText(tempName);
        }

        // 温度值列
        QTableWidgetItem *valueItem = groupStatusTable->item(i, 1);
        double tempValue = 0.0;

        // 根据组和传感器索引获取正确的温度值
        if (activeGroup == 0) {
            // A组只有一个温度传感器，使用tempValues[0]
            tempValue = tempValues[0];
        } else {
            // B组有3个温度传感器，使用tempValues[1], tempValues[2], tempValues[3]
            tempValue = tempValues[i + 1];
        }

        if (!valueItem) {
            valueItem = new QTableWidgetItem(QString::number(tempValue, 'f', 1));
            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
            groupStatusTable->setItem(i, 1, valueItem);
        } else {
            valueItem->setText(QString::number(tempValue, 'f', 1));
        }

        // 单位列
        QTableWidgetItem *unitItem = groupStatusTable->item(i, 2);
        if (!unitItem) {
            unitItem = new QTableWidgetItem("°C");
            unitItem->setFlags(unitItem->flags() & ~Qt::ItemIsEditable);
            groupStatusTable->setItem(i, 2, unitItem);
        } else {
            unitItem->setText("°C");
        }
    }
}

void MainWindow::onConnectButtonClicked()
{

    usbComm->startCommunication();
}
void MainWindow::onDisconnectButtonClicked()
{
    // 停止通信
    usbComm->writeRegister(REG_POWER_DATA_ENABLE, 0);
    usbComm->stopCommunication();
}

void MainWindow::onGroupSelectionChanged(int index)
{

    activeGroup = index;
    statusBar()->showMessage("已选择: " + (index == 0 ? QString("A组") : QString("B组")));
    if (usbComm) {
        usbComm->setActiveGroup(index == 0);
    }

    if (connected && usbComm) {
        // 阻塞信号，避免在更新UI时触发信号
        adcDelaySpinBox->blockSignals(true);
        ioHighDelaySpinBox->blockSignals(true);
        ioLowDelaySpinBox->blockSignals(true);
        cycleCountSpinBox->blockSignals(true);
        cycleCountSpinBox1->blockSignals(true);
        powerPlusCheckBox->blockSignals(true);
        powerMinusCheckBox->blockSignals(true);
        do1CheckBox->blockSignals(true);
        do2CheckBox->blockSignals(true);

        // 根据选择的组读取对应的寄存器配置
        if (index == 0) { // A组
            // 读取A组的所有配置寄存器
            usbComm->readRegister(REG_A_ADC_DELAY);
            QThread::msleep(50);
            usbComm->readRegister(REG_A_IO_HIGH_DELAY);
            QThread::msleep(50);
            usbComm->readRegister(REG_A_IO_LOW_DELAY);
            QThread::msleep(50);
            usbComm->readRegister(REG_A_CYCLE_COUNT);
            QThread::msleep(50);
            usbComm->readRegister(REG_A_CYCLE_POINT_COUNT);
            QThread::msleep(50);
            usbComm->readRegister(REG_A_POWER_PLUS);
            QThread::msleep(50);
            usbComm->readRegister(REG_A_POWER_MINUS);
            QThread::msleep(50);
            usbComm->readRegister(REG_A_DO1);
            QThread::msleep(50);
            usbComm->readRegister(REG_A_DO2);
            QThread::msleep(50);
            usbComm->readRegister(REG_A_TRIGGER_MODE);
        } else { // B组
            // 读取B组的所有配置寄存器
            usbComm->readRegister(REG_B_ADC_DELAY);
            QThread::msleep(50);
            usbComm->readRegister(REG_B_IO_HIGH_DELAY);
            QThread::msleep(50);
            usbComm->readRegister(REG_B_IO_LOW_DELAY);
            QThread::msleep(50);
            usbComm->readRegister(REG_B_CYCLE_COUNT);
            QThread::msleep(50);
            usbComm->readRegister(REG_B_CYCLE_POINT_COUNT);
            QThread::msleep(50);
            usbComm->readRegister(REG_B_POWER_PLUS);
            QThread::msleep(50);
            usbComm->readRegister(REG_B_POWER_MINUS);
            QThread::msleep(50);
            usbComm->readRegister(REG_B_DO1);
            QThread::msleep(50);
            usbComm->readRegister(REG_B_DO2);
            QThread::msleep(50);
            usbComm->readRegister(REG_B_TRIGGER_MODE);
        }

        // 读取数字输入状态
        usbComm->readRegister(activeGroup == 0 ? REG_A_DI : REG_B_DI);

        // 延迟后恢复信号（给寄存器读取足够时间）
        QTimer::singleShot(1000, this, [this]() {
            // 恢复信号
            adcDelaySpinBox->blockSignals(false);
            ioHighDelaySpinBox->blockSignals(false);
            ioLowDelaySpinBox->blockSignals(false);
            cycleCountSpinBox->blockSignals(false);
            cycleCountSpinBox1->blockSignals(false);
            powerPlusCheckBox->blockSignals(false);
            powerMinusCheckBox->blockSignals(false);
            do1CheckBox->blockSignals(false);
            do2CheckBox->blockSignals(false);
        });
    }

    // 更新组状态表格
    updateGroupStatusTable();

    // 强制重新创建图表
    for (QCustomPlot* plot : voltageCharts) {
        if(plot)
            delete plot;
    }
    voltageCharts.clear();

    for(QGroupBox *group : voltageGroupBoxs) {
        if(group)
            delete group;
    }
    voltageGroupBoxs.clear();

    for (QCustomPlot* plot : tempCharts) {
        if(plot)
            delete plot;
    }
    tempCharts.clear();

    for (QGroupBox* group : tempGroupBoxs) {
        if(group)
            delete group;
    }
    tempGroupBoxs.clear();

    updateCharts();
    updateDIStatus();

    // 更新状态表格标题
    if (diStatusTable) {
        QTableWidgetItem *groupName = new QTableWidgetItem(index == 0 ? "A组" : "B组");
        if (groupName) {
            groupName->setFlags(groupName->flags() & ~Qt::ItemIsEditable);
            diStatusTable->setHorizontalHeaderItem(0, groupName);
        }
    }
}

void MainWindow::onStartAcquisitionClicked()
{
    if (!usbComm || !usbComm->isConnected()) {
        return;
    }

    bool isExternalTrigger = externalTriggerRadio1->isChecked();

    if (acquisitionInProgress) {
        // 停止采集的逻辑保持不变
        statusBar()->showMessage("用户手动停止采集...");
        acquisitionInProgress = false;
        startAcquisitionButton->setEnabled(true);
        startAcquisitionButton->setText("开始采集");

        if (_pTimer->isActive()) {
            _pTimer->stop();
        }

        // 清空全局缓存
        UsbCommunication::clearDataCache();

        uint32_t regAddr = (activeGroup == 0) ? REG_A_START_ACQUISITION : REG_B_START_ACQUISITION;
        usbComm->writeRegister(regAddr, 0);

        if (diReadTimer && !diReadTimer->isActive()) {
            diReadTimer->start();
            qDebug() << "手动停止采集：已重启DI读取定时器";
        }

        // 检查是否有数据
        if (UsbCommunication::isDataReady()) {
            saveDataButton->setEnabled(true);
            dataReady = true;
            statusBar()->showMessage("采集已停止");
            updateChartsFromCache();
        } else {
            statusBar()->showMessage("采集已停止，未收到数据");
        }

        return;
    }

    // 开始新的采集
    acquisitionCompleted.store(false);

    // 设置采集参数到通信模块
    int cycles = static_cast<int>(cycleCountSpinBox->value());
    int pointsPerCycle = static_cast<int>(cycleCountSpinBox1->value());
    usbComm->setAcquisitionParams(cycles, activeGroup, pointsPerCycle);

    // 清空原有数据结构
    clearAllData();
    aPowerHistory.clear();
    bPowerHistory.clear();
    currentVoltageDataIndex = 0;

    statusBar()->showMessage("正在采集...");

    if (diReadTimer && diReadTimer->isActive()) {
        diReadTimer->stop();
        qDebug() << "采集开始：已停止DI读取定时器";
    }

    QThread::msleep(100);

    if (isExternalTrigger) {
        startAcquisitionButton->setText("停止采集");
        startAcquisitionButton->setEnabled(true);
        statusBar()->showMessage("外触发模式：等待外部触发信号...");
    } else {
        startAcquisitionButton->setEnabled(false);
        startAcquisitionButton->setText("开始采集");
    }

    acquisitionInProgress = true;
    usbComm->startAcquisition();

    if (!isExternalTrigger) {
        int estimatedTimeMs = cycles * pointsPerCycle * 0.1;
        int timeoutMs = qMax(estimatedTimeMs, 10000);
        qDebug() << "内触发模式 - 预期采集时间:" << estimatedTimeMs << "ms，设置超时:" << timeoutMs << "ms";
        _pTimer->start(timeoutMs);
    } else {
        qDebug() << "外触发模式 - 不设置超时，等待用户手动停止或触发完成";
        statusBar()->showMessage("外触发模式：等待外部触发信号");
    }
}



void MainWindow::onDisplayModeChanged(int id)
{
    currentDisplayMode = id;
    cycleNumberSpinBox->setEnabled(id == 1);

    // 如果切换到单周期模式，确保周期编号在有效范围内
    if (id == 1 && connected) {
        // 获取当前数据的周期数
        int pointsPerCycle = static_cast<int>(cycleCountSpinBox1->value());
        if (pointsPerCycle <= 0) pointsPerCycle = 20;

        int totalPoints = 0;
        if (activeGroup < voltageData.size() && !voltageData[activeGroup].empty() &&
            !voltageData[activeGroup][0].isEmpty()) {
            totalPoints = voltageData[activeGroup][0].size();
        }

        int fullCycles = totalPoints / pointsPerCycle;
        int maxCycle = fullCycles > 0 ? fullCycles - 1 : 0;

        // 更新周期编号范围
        cycleNumberSpinBox->setRange(0, maxCycle);

        // 如果当前值超出范围，调整它
        if (cycleNumberSpinBox->value() > maxCycle) {
            cycleNumberSpinBox->setValue(maxCycle);
        }
    }

    statusBar()->showMessage("显示模式已切换为: " + (id == 0 ? QString("多周期均值") : QString("单周期数据")));

    // 立即更新图表数据以反映新的显示模式
    updateChartData();
}
void MainWindow::onCycleNumberChanged(int value)
{
    currentCycleNumber = value;
    statusBar()->showMessage("当前显示周期: " + QString::number(value));

    // 如果当前是单周期显示模式，立即更新图表
    if (currentDisplayMode == 1) {
        updateChartData();
    }
}


void MainWindow::onAdcDelayChanged(double value)
{
    if (connected && usbComm) {
        // 根据当前选中的组选择正确的寄存器地址
        uint32_t regAddr = (activeGroup == 0) ? REG_A_ADC_DELAY : REG_B_ADC_DELAY;
        usbComm->writeRegister(regAddr, static_cast<uint32_t>(value*100));
    }
    statusBar()->showMessage("ADC延时已设为: " + QString::number(value, 'f', 1) + " us");
}


void MainWindow::onIoHighDelayChanged(double value)
{
    if (connected && usbComm) {
        // 根据当前选中的组选择正确的寄存器地址
        uint32_t regAddr = (activeGroup == 0) ? REG_A_IO_HIGH_DELAY : REG_B_IO_HIGH_DELAY;
        usbComm->writeRegister(regAddr, static_cast<uint32_t>(value*100));
    }
    statusBar()->showMessage("IO拉高延时已设为: " + QString::number(value, 'f', 1) + " us");
}

void MainWindow::onIoLowDelayChanged(double value)
{
    if (connected && usbComm) {
        // 根据当前选中的组选择正确的寄存器地址
        uint32_t regAddr = (activeGroup == 0) ? REG_A_IO_LOW_DELAY : REG_B_IO_LOW_DELAY;
        usbComm->writeRegister(regAddr, static_cast<uint32_t>(value*100));
    }
    statusBar()->showMessage("IO拉低延时已设为: " + QString::number(value, 'f', 1) + " us");
}
void MainWindow::onCycleCountChanged(double value)  // 注意参数类型
{
    if (connected && usbComm) {
        // 根据当前选中的组选择正确的寄存器地址
        uint32_t regAddr = (activeGroup == 0) ? REG_A_CYCLE_COUNT : REG_B_CYCLE_COUNT;
        usbComm->writeRegister(regAddr, static_cast<uint32_t>(value));
    }
    statusBar()->showMessage("采样周期数已设为: " + QString::number(static_cast<int>(value)));
}

// 5. 修改MainWindow.cpp中的onCyclePointsPerCycleChanged函数

void MainWindow::onCyclePointsPerCycleChanged(double value)  // 注意参数类型
{
    if (connected && usbComm) {
        // 根据当前选中的组选择正确的寄存器地址
        uint32_t regAddr = (activeGroup == 0) ? REG_A_CYCLE_POINT_COUNT : REG_B_CYCLE_POINT_COUNT;
        usbComm->writeRegister(regAddr, static_cast<uint32_t>(value));
    }
    statusBar()->showMessage("每周期采样点数已设为: " + QString::number(static_cast<int>(value)));
}

// 修改触发模式函数
void MainWindow::onTriggerModeChanged(int id)
{
    if (connected && usbComm) {
        usbComm->setTriggerMode(id);
    }

    QString modeName;
    switch (id) {
        case 0: modeName = "内触发"; break;
        case 1: modeName = "外触发"; break;
        default: modeName = "未知触发模式";
    }

    statusBar()->showMessage("触发模式已切换为: " + modeName);

    // 如果正在采集且切换了触发模式，给用户提示
    if (acquisitionInProgress) {
        QMessageBox::information(this, "触发模式变更",
                               "触发模式已更改为: " + modeName + "\n"
                               "当前采集将继续使用之前的触发模式。\n"
                               "新的触发模式将在下次采集时生效。");
    }
}


void MainWindow::onSaveDataClicked()
{
    if (!dataReady || !UsbCommunication::isDataReady()) {
        QMessageBox::warning(this, "无数据", "尚未采集数据或数据无效，请先开始采集。");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "保存数据", "", "CSV文件 (*.csv)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        int pointsPerCycle = static_cast<int>(cycleCountSpinBox1->value());
        if (pointsPerCycle <= 0) pointsPerCycle = 20;

        int channelCount = (activeGroup == 0) ? 8 : 4;

        // 直接从全局缓存获取数据
        QVector<QVector<double>> cacheVoltageData;
        QVector<QVector<uint32_t>> cacheRawData;
        QVector<QVector<uint32_t>> cacheTs1Data;
        QVector<QVector<uint32_t>> cacheTs2Data;

        UsbCommunication::getAllGroupData(activeGroup, cacheVoltageData, cacheRawData, cacheTs1Data, cacheTs2Data);

        if (cacheRawData.isEmpty()) {
            QMessageBox::warning(this, "无数据", "缓存中没有找到数据，请重新采集。");
            return;
        }

        // 计算实际数据量
        int maxSamples = 0;
        for (int ch = 0; ch < channelCount && ch < cacheRawData.size(); ch++) {
            if (cacheRawData[ch].size() > maxSamples) {
                maxSamples = cacheRawData[ch].size();
            }
        }

        if (maxSamples == 0) {
            QMessageBox::warning(this, "无数据", "没有找到有效的采集数据。");
            return;
        }

        int fullCycles = maxSamples / pointsPerCycle;

        // 选择对应组的功率历史数据
        const QVector<PowerDataPoint> &powerHistory = (activeGroup == 0) ? aPowerHistory : bPowerHistory;

        // 根据当前组确定温度传感器数量
        int tempSensorCount = (activeGroup == 0) ? 1 : 3;

        // 写入标题行
        out << "No,";
        for (int ch = 0; ch < channelCount; ch++) {
            QString channelPrefix = QString("%1%2").arg(activeGroup == 0 ? "A" : "B").arg(ch);
            for (int point = 1; point <= pointsPerCycle; point++) {
                out << channelPrefix << point << ",";
            }
        }
        out << "Timestamp1(count),Timestamp2(us),,No.,";

        // 根据组写入温度标题
        if (activeGroup == 0) {
            out << "TEMP_CH0,,";
        } else {
            out << "TEMP_CH0,TEMP_CH1,TEMP_CH2,,";
        }

        out << "No.,U,I,P,\n";

        // 功率数据连续放置在前面
        int powerEntries = powerHistory.size();
        for (int cycle = 0; cycle < fullCycles; cycle++) {
            // 数据包编号
            out << QString::number(cycle + 1) << ",";

            // 差分电压原始数据 - 转换为10进制有符号数
            for (int ch = 0; ch < channelCount; ch++) {
                for (int point = 0; point < pointsPerCycle; point++) {
                    int dataIndex = cycle * pointsPerCycle + point;
                    if (ch < cacheRawData.size() && dataIndex < cacheRawData[ch].size()) {
                        uint32_t rawValue = cacheRawData[ch][dataIndex];

                        // 转换为20位有符号数
                        int32_t signedValue;
                        if (rawValue & 0x80000) {
                            // 负数 - 补码转换，扩展符号位
                            signedValue = static_cast<int32_t>(rawValue | 0xFFF00000);
                        } else {
                            // 正数 - 保留低19位
                            signedValue = static_cast<int32_t>(rawValue & 0x7FFFF);
                        }

                        // 输出为10进制有符号数
                        out << signedValue << ",";
                    } else {
                        out << ",";
                    }
                }
            }

            // 输出时间戳
            uint32_t ts1 = 0, ts2 = 0;
            int dataIndex = cycle * pointsPerCycle;

            if (!cacheTs1Data.isEmpty() &&
                !cacheTs1Data[0].isEmpty() &&
                dataIndex < cacheTs1Data[0].size()) {
                ts1 = cacheTs1Data[0][dataIndex];
                ts2 = cacheTs2Data[0][dataIndex];
            }

            out << ts1 << "," << ts2 << ",,";

            // 温度数据（前5行）- 修改为支持多个温度传感器
            if (cycle < 5) {
                out << QString::number(cycle + 1) << ",";

                if (activeGroup == 0) {
                    // A组：只有一个温度传感器
                    if (0 < tempValues.size()) {
                        out << QString::number(tempValues[0], 'f', 1) << ",";
                    } else {
                        out << ",";
                    }
                } else {
                    // B组：有3个温度传感器
                    for (int tempIdx = 0; tempIdx < 3; tempIdx++) {
                        int tempValueIndex = tempIdx + 1; // B组使用tempValues[1], tempValues[2], tempValues[3]
                        if (tempValueIndex < tempValues.size()) {
                            out << QString::number(tempValues[tempValueIndex], 'f', 1) << ",";
                        } else {
                            out << ",";
                        }
                    }
                }
            } else {
                // 非前5行的空白填充
                if (activeGroup == 0) {
                    out << ",,"; // A组：一个温度列的空白
                } else {
                    out << ",,,,"; // B组：三个温度列的空白
                }
            }

            out << ",";

            // 功率数据 - 在前面连续排列
            if (cycle < powerEntries) {  // 将功率数据连续排列在前面
                out << QString::number(cycle + 1) << ",";

                // 从功率历史中获取对应的数据
                if (cycle < powerHistory.size()) {
                    double voltage = powerHistory[cycle].voltage;
                    double current = powerHistory[cycle].current;
                    double power = powerHistory[cycle].power;

                    out << QString::number(voltage, 'f', 2) << ","
                        << QString::number(current, 'f', 3) << ","
                        << QString::number(power, 'f', 3) << ",";
                } else {
                    out << ",,,,";
                }
            } else if (cycle % 1000 == 0) {
                // 对于后面的部分，保留原有的每1000个周期显示一次功率数据的逻辑
                int powerIndex = cycle / 1000;
                if (powerIndex < powerHistory.size()) {
                    out << QString::number(powerIndex + 1) << ",";
                    double voltage = powerHistory[powerIndex].voltage;
                    double current = powerHistory[powerIndex].current;
                    double power = powerHistory[powerIndex].power;

                    out << QString::number(voltage, 'f', 2) << ","
                        << QString::number(current, 'f', 3) << ","
                        << QString::number(power, 'f', 3) << ",";
                } else {
                    out << ",,,,";
                }
            } else {
                out << ",,,,";
            }

            out << "\n";
        }

        file.close();

        qDebug() << "保存完成 - 最大数据点数:" << maxSamples
                 << "，完整周期数:" << fullCycles
                 << "，功率数据点数:" << powerHistory.size();

        statusBar()->showMessage("数据已保存到: " + fileName);
        QMessageBox::information(this, "保存成功",
            QString("数据已成功保存到: %1\n")
            .arg(fileName)
            );
    } else {
        QMessageBox::critical(this, "保存失败", "无法打开文件 " + fileName + " 进行写入");
    }
}
void MainWindow::updateCharts()
{
   // qDebug() << "updateCharts 开始执行";

    try {
        // 1. 清理现有图表
        for (QCustomPlot* plot : voltageCharts) {
            if (plot) delete plot;
        }
        voltageCharts.clear();

        for(QGroupBox *group : voltageGroupBoxs)
        {
            if(group)
                delete group;
        }
        voltageGroupBoxs.clear();

        for (QCustomPlot* plot : tempCharts) {
            if (plot) delete plot;
        }
        tempCharts.clear();

        for(QGroupBox * group : tempGroupBoxs)
        {
            if(group)
                delete group;
        }
        tempGroupBoxs.clear();

        // 2. 检查并获取容器
        if (!chartsScrollArea) {
            qDebug() << "错误: chartsScrollArea 为 nullptr";
            return;
        }

        QWidget *container = chartsScrollArea->widget();
        if (!container) {
            container = new QWidget;
            chartsScrollArea->setWidget(container);
        }

        // 3. 清理旧布局
        QLayout *oldLayout = container->layout();
        if (oldLayout) {
            QLayoutItem *item;
            while ((item = oldLayout->takeAt(0)) != nullptr) {
                if (item->widget()) {
                    item->widget()->hide();
                }
                delete item;
            }
            delete oldLayout;
        }

        // 4. 创建新布局
        QVBoxLayout *chartsLayout = new QVBoxLayout(container);
        chartsLayout->setSpacing(10);
        chartsLayout->setContentsMargins(10, 10, 10, 10);

        // 5. 确定要创建的图表数量
        int voltageChannelCount = (activeGroup == 0) ? 8 : 4;
        int tempSensorCount = (activeGroup == 0) ? 1 : 3;

        // 6. 创建电压图表
        for (int ch = 0; ch < voltageChannelCount; ch++) {
            try {
                QString title = activeGroup == 0 ?
                               QString("A组通道%1电压").arg(ch) :
                               QString("B组通道%1电压").arg(ch);

                QGroupBox *chartBox = new QGroupBox(title);
                chartBox->setMinimumHeight(180);
                chartBox->setMaximumHeight(180);
                QVBoxLayout *chartLayout = new QVBoxLayout(chartBox);
                chartLayout->setContentsMargins(5, 5, 5, 5);

                QCustomPlot *plot = new QCustomPlot(chartBox);
                plot->setMinimumHeight(140);
                plot->setMaximumHeight(140);
                plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

                // 添加零电平参考线
                QCPItemStraightLine *zeroLine = new QCPItemStraightLine(plot);
                zeroLine->setPen(QPen(QColor(128, 128, 128, 180), 1, Qt::DashLine));
                zeroLine->point1->setCoords(0, 0);
                zeroLine->point2->setCoords(20, 0);  // 固定20us的范围

                // 基本设置
                plot->addGraph();

                // 颜色设置
                QColor color;
                switch (ch % 8) {
                    case 0: color = QColor(0, 114, 189); break;
                    case 1: color = QColor(217, 83, 25); break;
                    case 2: color = QColor(237, 177, 32); break;
                    case 3: color = QColor(126, 47, 142); break;
                    case 4: color = QColor(119, 172, 48); break;
                    case 5: color = QColor(77, 190, 238); break;
                    case 6: color = QColor(162, 20, 47); break;
                    case 7: color = QColor(0, 128, 128); break;
                }

                QPen pen(color);
                pen.setWidth(2);
                plot->graph(0)->setPen(pen);

                // 设置坐标轴
                plot->xAxis->setLabel("时间(us)");
                plot->yAxis->setLabel("电压(V)");

                // 固定x轴范围，不随数据变化
                plot->xAxis->setRange(0, 20);
                plot->yAxis->setRange(-2.5, 2.5);

                // 设置空数据，等待实际数据
                QVector<double> x, y;
                plot->graph(0)->setData(x, y);

                plot->replot();

                chartLayout->addWidget(plot);
                chartsLayout->addWidget(chartBox);
                voltageCharts.append(plot);
                voltageGroupBoxs.append(chartBox);
            } catch (const std::exception& e) {
                qDebug() << "创建电压图表" << ch << "时出错:" << e.what();
                continue;
            }
        }

        // 7. 创建温度图表
        for (int i = 0; i < tempSensorCount; i++) {
                try {
                    QString title = (activeGroup == 0) ?
                                   "A组温度CH0" :
                                   QString("B组温度CH%1").arg(i);

                    QGroupBox *chartBox = new QGroupBox(title);
                    chartBox->setMinimumHeight(180);
                    chartBox->setMaximumHeight(180);
                    QVBoxLayout *chartLayout = new QVBoxLayout(chartBox);
                    chartLayout->setContentsMargins(5, 5, 5, 5);

                    QCustomPlot *plot = new QCustomPlot(chartBox);
                    plot->setMinimumHeight(140);
                    plot->setMaximumHeight(140);
                    plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

                    // 设置坐标轴
                    plot->xAxis->setLabel("时间(s)");
                    plot->yAxis->setLabel("温度(°C)");

                    // 添加温度曲线
                    plot->addGraph();
                    QPen tempPen(Qt::red);
                    tempPen.setWidth(2);
                    plot->graph(0)->setPen(tempPen);

                    // 【关键修改】固定温度图表的坐标轴范围 - 改为0到1秒
                    plot->xAxis->setRange(0, 1.0);  // 修改：从(0, 60)改为(0, 1.0)
                    plot->yAxis->setRange(20, 30);

                    // 设置空数据，等待实际温度数据
                    QVector<double> x, y;
                    plot->graph(0)->setData(x, y);

                    plot->replot();

                    chartLayout->addWidget(plot);
                    chartsLayout->addWidget(chartBox);
                    tempCharts.append(plot);
                    tempGroupBoxs.append(chartBox);
                } catch (const std::exception& e) {
                    qDebug() << "创建温度图表" << i << "时出错:" << e.what();
                    continue;
                }
            }


        // 8. 完成布局设置
        chartsLayout->addStretch();
        container->setLayout(chartsLayout);

        // 9. 强制更新显示
        container->setVisible(true);
        container->updateGeometry();
        container->update();

        // 10. 滚动到顶部
        chartsScrollArea->setVisible(true);
        chartsScrollArea->updateGeometry();
        chartsScrollArea->update();
        chartsScrollArea->verticalScrollBar()->setValue(0);

       // qDebug() << "updateCharts 完成，创建了" << voltageCharts.size() << "个电压图表和" << tempCharts.size() << "个温度图表";


    } catch (const std::exception& e) {
        qDebug() << "updateCharts 发生异常:" << e.what();
    }
}

void MainWindow::updateChartData()
{
    // 检查图表是否存在
    if (voltageCharts.isEmpty() && tempCharts.isEmpty()) {
        return;
    }

    // 静态变量记录上次的状态，避免重复打印
    static QMap<QString, int> lastChannelPoints;
    static QMap<QString, int> lastChannelCycles;

    // 更新电压图表数据
    int voltageChannelCount = (activeGroup == 0) ? 8 : 4;

        for (int ch = 0; ch < qMin(voltageChannelCount, voltageCharts.size()); ch++) {
            QCustomPlot *plot = voltageCharts[ch];
            if (!plot) continue;

            // 获取当前通道的数据
            QVector<double> rawData;
            if (connected && !voltageData[activeGroup][ch].isEmpty()) {
                rawData = voltageData[activeGroup][ch];
            } else {
                continue;
            }

            // 准备用于显示的数据
            QVector<double> x, y;
            int pointsPerCycle = static_cast<int>(cycleCountSpinBox1->value());
            if (pointsPerCycle <= 0) pointsPerCycle = 20;

            int totalPoints = rawData.size();
            int fullCycles = totalPoints / pointsPerCycle;

            if (currentDisplayMode == 0) {  // 多周期均值模式
                int cyclesToAverage = fullCycles;
                int startCycle = fullCycles - cyclesToAverage;
                if (startCycle < 0) startCycle = 0;

                if (cyclesToAverage > 0) {
                    x.resize(pointsPerCycle);
                    y.resize(pointsPerCycle);

                    // 初始化y值为0
                    for (int i = 0; i < pointsPerCycle; i++) {
                        y[i] = 0.0;
                    }

                    // 累加所选周期的值
                    for (int cycle = startCycle; cycle < fullCycles; cycle++) {
                        for (int i = 0; i < pointsPerCycle; i++) {
                            int index = cycle * pointsPerCycle + i;
                            if (index < totalPoints) {
                                y[i] += rawData[index];
                            }
                        }
                    }

                    // 计算平均值
                    for (int i = 0; i < pointsPerCycle; i++) {
                        y[i] /= cyclesToAverage;
                        x[i] = i * 1.0; // 时间轴单位为us
                    }
                }
            } else {  // 单周期数据模式
                int cycleIndex = cycleNumberSpinBox->value();
                int totalPoints = rawData.size();
                int fullCycles = totalPoints / pointsPerCycle;

                if (cycleNumberSpinBox->maximum() != fullCycles - 1) {
                    cycleNumberSpinBox->setMaximum(qMax(0, fullCycles - 1));
                }

                if (cycleIndex >= fullCycles) {
                    cycleIndex = fullCycles - 1;
                    if (cycleIndex < 0) cycleIndex = 0;
                    cycleNumberSpinBox->blockSignals(true);
                    cycleNumberSpinBox->setValue(cycleIndex);
                    cycleNumberSpinBox->blockSignals(false);
                }

                // 计算选定周期的起始位置
                int startIdx = cycleIndex * pointsPerCycle;
                if (startIdx + pointsPerCycle <= totalPoints) {
                    x.resize(pointsPerCycle);
                    y.resize(pointsPerCycle);

                    for (int i = 0; i < pointsPerCycle; i++) {
                        x[i] = i * 1.0; // 时间轴单位为us
                        y[i] = rawData[startIdx + i];
                    }
                }
            }

            // 如果有数据，更新图表
            if (!x.isEmpty() && !y.isEmpty()) {
                plot->clearGraphs();

                // 添加线条图
                plot->addGraph();
                QColor lineColor;
                switch (ch % 8) {
                    case 0: lineColor = QColor(0, 114, 189); break;
                    case 1: lineColor = QColor(217, 83, 25); break;
                    case 2: lineColor = QColor(237, 177, 32); break;
                    case 3: lineColor = QColor(126, 47, 142); break;
                    case 4: lineColor = QColor(119, 172, 48); break;
                    case 5: lineColor = QColor(77, 190, 238); break;
                    case 6: lineColor = QColor(162, 20, 47); break;
                    case 7: lineColor = QColor(0, 128, 128); break;
                }

                QPen linePen(lineColor);
                linePen.setWidth(2);
                plot->graph(0)->setPen(linePen);
                plot->graph(0)->setData(x, y);

                // *** 新增：添加数据点标记（实心圆点） ***
                plot->addGraph();
                plot->graph(1)->setPen(QPen(lineColor));
                plot->graph(1)->setBrush(QBrush(lineColor)); // 实心填充
                plot->graph(1)->setLineStyle(QCPGraph::lsNone); // 不显示连线
                plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, lineColor, lineColor, 6)); // 实心圆点，大小6
                plot->graph(1)->setData(x, y); // 使用相同的数据点

                // 自动计算Y轴范围
                double minY = *std::min_element(y.begin(), y.end());
                double maxY = *std::max_element(y.begin(), y.end());

//                if (minY > 0) minY = 0;
//                if (maxY < 0) maxY = 0;

                double rangeY = maxY - minY;

                double paddingY = rangeY * 0.15;
                double lowerY = minY - paddingY;
                double upperY = maxY + paddingY;

                double currentMinY = plot->yAxis->range().lower;
                double currentMaxY = plot->yAxis->range().upper;

                if (std::abs(lowerY - currentMinY) > rangeY * 0.2 ||
                    std::abs(upperY - currentMaxY) > rangeY * 0.2) {
                    plot->yAxis->setRange(lowerY, upperY);
                }

                double maxX = x.isEmpty() ? pointsPerCycle : (pointsPerCycle - 1);
                plot->xAxis->setRange(0, maxX);

                plot->replot(QCustomPlot::rpQueuedReplot);
            }
        }

        int tempSensorCount = (activeGroup == 0) ? 1 : 3;

            for (int i = 0; i < qMin(tempSensorCount, tempCharts.size()); i++) {
                QCustomPlot *plot = tempCharts[i];
                if (!plot) continue;

                // 获取当前温度值
                double currentTemp;
                int tempIndex = (activeGroup == 0) ? 0 : i+1;

                if (tempIndex < tempValues.size()) {
                    currentTemp = tempValues[tempIndex];
                }

                // 温度数据处理
                QVector<double> x, y;

                // 检查图表是否已经有数据
                if (plot->graph(0) && plot->graph(0)->data()->size() > 0) {
                    const int dataSize = plot->graph(0)->data()->size();
                    x.resize(dataSize);
                    y.resize(dataSize);

                    for (int j = 0; j < dataSize; j++) {
                        QCPGraphData dataPoint = *plot->graph(0)->data()->at(j);
                        x[j] = dataPoint.key;
                        y[j] = dataPoint.value;
                    }

                    const int maxDataPoints = 5;
                    if (x.size() >= maxDataPoints) {
                        for (int t = 0; t < x.size() - 1; t++) {
                            y[t] = y[t+1];
                        }
                        y[y.size() - 1] = currentTemp;

                        for (int t = 0; t < x.size(); t++) {
                            x[t] = t * 0.2;
                        }
                    } else {
                        x.append(x.size() * 0.2);
                        y.append(currentTemp);
                    }
                } else {
                    x.resize(1);
                    y.resize(1);
                    x[0] = 0.0;
                    y[0] = currentTemp;
                }
                plot->clearGraphs();

                // 添加温度曲线
                plot->addGraph();
                QPen tempPen(Qt::red);
                tempPen.setWidth(2);
                plot->graph(0)->setPen(tempPen);
                plot->graph(0)->setData(x, y);


                plot->addGraph();
                plot->graph(1)->setPen(QPen(Qt::red));
                plot->graph(1)->setBrush(QBrush(Qt::red)); // 实心填充
                plot->graph(1)->setLineStyle(QCPGraph::lsNone); // 不显示连线
                plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::red, 8)); // 实心圆点，大小8
                plot->graph(1)->setData(x, y);

                // 更新Y轴范围
                if (!y.isEmpty()) {
                    double minY = *std::min_element(y.begin(), y.end());
                    double maxY = *std::max_element(y.begin(), y.end());
                    double rangeY = maxY - minY;

                    double currentMinY = plot->yAxis->range().lower;
                    double currentMaxY = plot->yAxis->range().upper;

                    if (std::abs(minY - currentMinY) > rangeY * 0.2 ||
                        std::abs(maxY - currentMaxY) > rangeY * 0.2) {
                        plot->yAxis->setRange(minY - rangeY * 0.1, maxY + rangeY * 0.1);
                    }
                }

                plot->xAxis->setRange(0, 0.8);
                plot->replot(QCustomPlot::rpQueuedReplot);
            }
        }

void MainWindow::updateDIStatus()
{
    if (diStatus.size() < 12) {
        diStatus.resize(12);
    }

    // 根据当前选择的组更新DI1-DI6的状态
    int startIdx = activeGroup * 6; // 0或6，分别对应A组或B组

    if (!diStatusTable) {
        qDebug() << "错误: diStatusTable 为 nullptr";
        return;
    }

    // 更新表格标题以显示当前组
    QString groupName = (activeGroup == 0) ? "A组" : "B组";
    QTableWidgetItem *groupItem = diStatusTable->item(0, 0);
    if (!groupItem) {
        groupItem = new QTableWidgetItem(groupName + "输入");
        groupItem->setFlags(groupItem->flags() & ~Qt::ItemIsEditable);
        diStatusTable->setItem(0, 0, groupItem);
    } else {
        groupItem->setText(groupName + "输入");
    }

    // 更新DI1-DI6状态显示
    //qDebug() << "更新" << groupName << "DI状态显示:";
    for (int i = 0; i < 6; i++) {
        QTableWidgetItem *item = diStatusTable->item(0, i+1);
        if (!item) {
            item = new QTableWidgetItem("低");
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            diStatusTable->setItem(0, i+1, item);
        }

        if (item) {
            bool diState = diStatus[startIdx + i];
            item->setText(diState ? "高" : "低");

            // 为高低电平添加不同的背景色
            if (diState) {
                item->setBackground(QColor(144, 238, 144)); // 浅绿色表示高电平
                item->setForeground(QColor(0, 100, 0));     // 深绿色文字
            } else {
                item->setBackground(QColor(255, 182, 193)); // 浅红色表示低电平
                item->setForeground(QColor(139, 0, 0));     // 深红色文字
            }

           // qDebug() << "  DI" << (i+1) << ":" << (diState ? "高" : "低");
        }
    }

    // 强制表格刷新显示
    diStatusTable->viewport()->update();
   // qDebug() << groupName << "DI状态显示更新完成";
}

void MainWindow::onStatusUpdateTimer()
{
    updateDIStatus();
}



void MainWindow::onConnectionChanged(bool isConnected)
{
    connected = isConnected;

    if (isConnected) {
        setWindowTitle(" Analog Signal_V1.1.9");
        connectionStatusLabel->setText("已连接");
        connectionStatusLabel->setStyleSheet("color: green; font-weight: bold;");
        connectButton->setEnabled(false);
        disconnectButton->setEnabled(true);
        startAcquisitionButton->setEnabled(true);

        // 只有在非采集状态下才启动DI读取定时器
        if (!acquisitionInProgress && diReadTimer && !diReadTimer->isActive()) {
            diReadTimer->start();
            qDebug() << "设备连接：已启动DI读取定时器";
        }

        // 启用控件
        groupSelectionCombo->setEnabled(true);
        do1CheckBox->setEnabled(true);
        do2CheckBox->setEnabled(true);
        powerPlusCheckBox->setEnabled(true);
        powerMinusCheckBox->setEnabled(true);
        adcDelaySpinBox->setEnabled(true);
        ioHighDelaySpinBox->setEnabled(true);
        ioLowDelaySpinBox->setEnabled(true);
        cycleCountSpinBox->setEnabled(true);
        cycleCountSpinBox1->setEnabled(true);
        internalTriggerRadio->setEnabled(true);
        externalTriggerRadio1->setEnabled(true);
        //externalTriggerRadio2->setEnabled(true);
        powerUpdateTimer->start();
        // 读取固件版本
        usbComm->readRegister(REG_VERSION);

        // 发送当前配置到设备
        sendAllConfigsToDevice();
        QTimer::singleShot(1000, this, [this]() {
            if (connected && usbComm) {
                qDebug() << "连接成功1秒后，启用功率数据包发送";
                usbComm->writeRegister(REG_POWER_DATA_ENABLE, 1);
                statusBar()->showMessage("功率数据包发送已启用");
            }
        });

        statusBar()->showMessage("设备已连接");


    } else {

        powerUpdateTimer->stop();

        // 断开连接时停止DI读取定时器
        if (diReadTimer && diReadTimer->isActive()) {
            diReadTimer->stop();
            qDebug() << "设备断开：已停止DI读取定时器";
        }

        {
            QMutexLocker locker(&powerDataMutex);
            latestPowerValues.clear();
        }

        // 重置UI显示
        boardVoltageLabel->setText("--");
        boardCurrentLabel->setText("--");
        boardPowerLabel->setText("--");


        connectionStatusLabel->setText("未连接");
        connectionStatusLabel->setStyleSheet("color: red; font-weight: bold;");
        connectButton->setEnabled(true);
        disconnectButton->setEnabled(false);

        // 禁用控件
        groupSelectionCombo->setEnabled(false);
        do1CheckBox->setEnabled(false);
        do2CheckBox->setEnabled(false);
        powerPlusCheckBox->setEnabled(false);
        powerMinusCheckBox->setEnabled(false);
        adcDelaySpinBox->setEnabled(false);
        ioHighDelaySpinBox->setEnabled(false);
        ioLowDelaySpinBox->setEnabled(false);
        cycleCountSpinBox->setEnabled(false);
        cycleCountSpinBox1->setEnabled(false);
        internalTriggerRadio->setEnabled(false);
        externalTriggerRadio1->setEnabled(false);
        //externalTriggerRadio2->setEnabled(false);
        startAcquisitionButton->setEnabled(false);
        saveDataButton->setEnabled(false);

        setWindowTitle("模拟信号采集板 [未连接]");
        statusBar()->showMessage("设备已断开连接");


    }
}


void MainWindow::resetDataIndices()
{
    // 重置所有通道的数据写入索引
    for (int group = 0; group < 2; group++) {
        for (int ch = 0; ch < 8; ch++) {
            currentDataIndex[group][ch] = 0;
        }
    }
}

void MainWindow::sendAllConfigsToDevice()
{
    if (!usbComm || !usbComm->isConnected()) {
        return;
    }

    // 获取当前选中的组
    bool isGroupA = (activeGroup == 0);

    // 1. 发送电源配置
    usbComm->setPowerControl(isGroupA, powerPlusCheckBox->isChecked(), powerMinusCheckBox->isChecked());
    QThread::msleep(50);

    // 2. 发送数字输出配置
    usbComm->setDigitalOutput(isGroupA, do1CheckBox->isChecked(), do2CheckBox->isChecked());
    QThread::msleep(50);

    // 3. 发送触发模式
    int triggerMode = triggerModeGroup->checkedId();
    usbComm->setTriggerMode(triggerMode);
    QThread::msleep(50);

    // 4. 发送延时配置
    uint32_t adcDelayValue = static_cast<uint32_t>(adcDelaySpinBox->value() * 100);
    uint32_t ioHighDelayValue = static_cast<uint32_t>(ioHighDelaySpinBox->value() * 100);
    uint32_t ioLowDelayValue = static_cast<uint32_t>(ioLowDelaySpinBox->value() * 100);

    usbComm->setDelayConfig(adcDelayValue, ioHighDelayValue, ioLowDelayValue);
    QThread::msleep(100);

    // 5. 发送采样配置
    uint32_t cycleCount = static_cast<uint32_t>(cycleCountSpinBox->value());
    uint32_t pointsPerCycle = static_cast<uint32_t>(cycleCountSpinBox1->value());

    qDebug() << "发送采样配置: 周期数=" << cycleCount << ", 每周期点数=" << pointsPerCycle;

    usbComm->setSamplingConfig(cycleCount, pointsPerCycle);
    QThread::msleep(100);

    statusBar()->showMessage("配置已发送到设备");
}


void MainWindow::onRegisterDataReceived(uint32_t regAddr, uint32_t regData)
{
    // 处理不同寄存器的值
    switch (regAddr) {
        case 0x0000: // 版本寄存器
            // 版本号
        firmwareVersionLabel->setText(QString("v%1.%2.%3.%4")
                                         .arg((regData >> 24) & 0xFF)   // 主版本
                                         .arg((regData >> 16) & 0xFF)   // 次版本
                                         .arg((regData >> 8) & 0xFF)    // 修订版本
                                         .arg(regData & 0xFF));         // 构建版本

            break;
    case REG_A_ADC_DELAY:
                if (activeGroup == 0) {
                    adcDelaySpinBox->blockSignals(true);
                    adcDelaySpinBox->setValue(regData / 100.0); // 转换为小数
                    adcDelaySpinBox->blockSignals(false);
                }
                break;

            case REG_A_IO_HIGH_DELAY:
                if (activeGroup == 0) {
                    ioHighDelaySpinBox->blockSignals(true);
                    ioHighDelaySpinBox->setValue(regData / 100.0);
                    ioHighDelaySpinBox->blockSignals(false);
                }
                break;

            case REG_A_IO_LOW_DELAY:
                if (activeGroup == 0) {
                    ioLowDelaySpinBox->blockSignals(true);
                    ioLowDelaySpinBox->setValue(regData / 100.0);
                    ioLowDelaySpinBox->blockSignals(false);
                }
                break;

            case REG_A_CYCLE_COUNT:
                if (activeGroup == 0) {
                    cycleCountSpinBox->blockSignals(true);
                    cycleCountSpinBox->setValue(regData);
                    cycleCountSpinBox->blockSignals(false);
                }
                break;

            case REG_A_CYCLE_POINT_COUNT:
                if (activeGroup == 0) {
                    cycleCountSpinBox1->blockSignals(true);
                    cycleCountSpinBox1->setValue(regData);
                    cycleCountSpinBox1->blockSignals(false);
                }
                break;

            // B组配置寄存器
            case REG_B_ADC_DELAY:
                if (activeGroup == 1) {
                    adcDelaySpinBox->blockSignals(true);
                    adcDelaySpinBox->setValue(regData / 100.0);
                    adcDelaySpinBox->blockSignals(false);
                }
                break;

            case REG_B_IO_HIGH_DELAY:
                if (activeGroup == 1) {
                    ioHighDelaySpinBox->blockSignals(true);
                    ioHighDelaySpinBox->setValue(regData / 100.0);
                    ioHighDelaySpinBox->blockSignals(false);
                }
                break;

            case REG_B_IO_LOW_DELAY:
                if (activeGroup == 1) {
                    ioLowDelaySpinBox->blockSignals(true);
                    ioLowDelaySpinBox->setValue(regData / 100.0);
                    ioLowDelaySpinBox->blockSignals(false);
                }
                break;

            case REG_B_CYCLE_COUNT:
                if (activeGroup == 1) {
                    cycleCountSpinBox->blockSignals(true);
                    cycleCountSpinBox->setValue(regData);
                    cycleCountSpinBox->blockSignals(false);
                }
                break;

            case REG_B_CYCLE_POINT_COUNT:
                if (activeGroup == 1) {
                    cycleCountSpinBox1->blockSignals(true);
                    cycleCountSpinBox1->setValue(regData);
                    cycleCountSpinBox1->blockSignals(false);
                }
                break;

            // 触发模式处理
            case REG_A_TRIGGER_MODE:
                if (activeGroup == 0) {
                    triggerModeGroup->blockSignals(true);
                    switch (regData) {
                        case 0: internalTriggerRadio->setChecked(true); break;
                        case 1: externalTriggerRadio1->setChecked(true); break;
                    }
                    triggerModeGroup->blockSignals(false);
                }
                break;

            case REG_B_TRIGGER_MODE:
                if (activeGroup == 1) {
                    triggerModeGroup->blockSignals(true);
                    switch (regData) {
                        case 0: internalTriggerRadio->setChecked(true); break;
                        case 1: externalTriggerRadio1->setChecked(true); break;
                    }
                    triggerModeGroup->blockSignals(false);
                }
                break;

        // A组寄存器
        case 0x0020: // A组开始采集控制
            if (regData == 1) {
                statusBar()->showMessage("A组采集开始");
            } else {
                statusBar()->showMessage("A组采集停止");
                // 如果A组采集完成
                if (activeGroup == 0) {
                    startAcquisitionButton->setEnabled(true);
                    acquisitionInProgress = false;
                    saveDataButton->setEnabled(true);
                    dataReady = true;
                }
            }
            break;



    case 0x002C: // A组采集完成标志
                qDebug() << "收到A组完成标志: " << regData;
                qDebug() << "--------------" ;
                if (regData == 1 && activeGroup == 0 && acquisitionInProgress) {
                    qDebug() << "A组采集完成标志确认，开始处理完成逻辑";

                }
                break;

            case 0x004C: // B组采集完成标志（假设B组对应寄存器）
                qDebug() << "收到B组完成标志: " << regData;
                if (regData == 1 && activeGroup == 1 && acquisitionInProgress) {
                    qDebug() << "B组采集完成标志确认，开始处理完成逻辑";

                }
                break;
        // 新增：A组数字输入寄存器处理
        case REG_A_DI: // A组数字输入寄存器 (0x0029)
            {
                uint8_t diValue = static_cast<uint8_t>(regData & 0x3F); // 取低6位
                //qDebug() << "接收到A组DI数据: 0x" << QString::number(regData, 16)
                  //       << "解析后的DI值: 0x" << QString::number(diValue, 16);

                // 调用数字输入变化处理函数
                onDigitalInputChanged(true, diValue);  // true表示A组
                //statusBar()->showMessage(QString("A组DI状态更新: 0x%1").arg(diValue, 2, 16, QChar('0')));
            }
            break;

        case REG_A_POWER_PLUS: // A组+5V电源
             if (activeGroup == 0) { // 只在当前选中A组时更新UI
                 powerPlusCheckBox->blockSignals(true);
                 powerPlusCheckBox->setChecked(regData == 1);
                 powerPlusCheckBox->blockSignals(false);
             }
             break;

         case REG_A_POWER_MINUS: // A组-5V电源
             if (activeGroup == 0) { // 只在当前选中A组时更新UI
                 powerMinusCheckBox->blockSignals(true);
                 powerMinusCheckBox->setChecked(regData == 1);
                 powerMinusCheckBox->blockSignals(false);
             }
             break;

         case REG_A_DO1: // A组DO1
             if (activeGroup == 0) { // 只在当前选中A组时更新UI
                 do1CheckBox->blockSignals(true);
                 do1CheckBox->setChecked(regData == 1);
                 do1CheckBox->blockSignals(false);
             }
             break;

         case REG_A_DO2: // A组DO2
             if (activeGroup == 0) { // 只在当前选中A组时更新UI
                 do2CheckBox->blockSignals(true);
                 do2CheckBox->setChecked(regData == 1);
                 do2CheckBox->blockSignals(false);
             }
             break;

        // B组寄存器
        case 0x0040: // B组开始采集控制
            if (regData == 1) {
                statusBar()->showMessage("B组采集开始");
            } else {
                statusBar()->showMessage("B组采集停止");
                // 如果B组采集完成
                if (activeGroup == 1) {
                    startAcquisitionButton->setEnabled(true);
                    acquisitionInProgress = false;
                    saveDataButton->setEnabled(true);
                    dataReady = true;
                }
            }
            break;


        case REG_B_DI: // B组数字输入寄存器 (0x0049)
            {
                uint8_t diValue = static_cast<uint8_t>(regData & 0x3F); // 取低6位
                onDigitalInputChanged(false, diValue);  // false表示B组
               // statusBar()->showMessage(QString("B组DI状态更新: 0x%1").arg(diValue, 2, 16, QChar('0')));
            }
            break;

        case REG_B_POWER_PLUS: // B组+5V电源
            if (activeGroup == 1) { // 只在当前选中B组时更新UI
                powerPlusCheckBox->blockSignals(true);
                powerPlusCheckBox->setChecked(regData == 1);
                powerPlusCheckBox->blockSignals(false);
            }
            break;

        case REG_B_POWER_MINUS: // B组-5V电源
            if (activeGroup == 1) { // 只在当前选中B组时更新UI
                powerMinusCheckBox->blockSignals(true);
                powerMinusCheckBox->setChecked(regData == 1);
                powerMinusCheckBox->blockSignals(false);
            }
            break;

        case REG_B_DO1: // B组DO1
            if (activeGroup == 1) { // 只在当前选中B组时更新UI
                do1CheckBox->blockSignals(true);
                do1CheckBox->setChecked(regData == 1);
                do1CheckBox->blockSignals(false);
            }
            break;

        case REG_B_DO2: // B组DO2
            if (activeGroup == 1) { // 只在当前选中B组时更新UI
                do2CheckBox->blockSignals(true);
                do2CheckBox->setChecked(regData == 1);
                do2CheckBox->blockSignals(false);
            }
            break;

        default:
            // 记录未知寄存器，但不显示为错误（减少日志噪音）
            qDebug() << "接收到未处理的寄存器数据: 地址=0x" << QString::number(regAddr, 16)
                     << "值=0x" << QString::number(regData, 16);
            break;
    }
}



// 处理温度数据接收
void MainWindow::onTemperatureDataReceived(const QVector<QVariantMap> &tempData)
{
    if (tempData.isEmpty()) {
        qDebug() << "MainWindow::onTemperatureDataReceived - 接收到空温度数据";
        return;
    }

    // 更新板卡温度和温度表格
    for (const QVariantMap &data : tempData) {
        int channel = data["channel"].toInt();
        double temperature = data["temperature"].toDouble();
        QString channelName = data["channelName"].toString();
        qint64 timestamp = data["timestamp"].toULongLong();
//        qDebug() << "处理温度数据 - 通道:" << channel << "名称:" << channelName
//                 << "温度:" << temperature << "°C";

        // 保存到温度值数组
        if (channel < tempValues.size()) {
            tempValues[channel] = temperature;
            //qDebug() << "已更新tempValues[" << channel << "] = " << temperature;
        }

        if(channel < timeStamp4Temp.size())
        {
            timeStamp4Temp[channel] = timestamp;
        }

    }
    updateGroupStatusTable();



    updateChartData();

}

void MainWindow::onDigitalInputChanged(bool isGroupA, uint8_t diValue)
{


    // 更新diStatus数组
    int startIdx = isGroupA ? 0 : 6; // A组从0开始，B组从6开始

    // 修正：不反转逻辑，1=高电平，0=低电平
    bool needInvert = false;  // 改为false，使用正常逻辑

    // 解析DI值的每一位（0-5位对应DI1-DI6）
   // qDebug() << "  DI位详细状态 (正常逻辑):";
    for (int i = 0; i < 6; i++) {
        bool rawState = (diValue & (1 << i)) != 0;
        bool finalState = needInvert ? !rawState : rawState;  // 不反转
        diStatus[startIdx + i] = finalState;


    }

    // 验证A组的预期状态（DI5应为低，其他应为高）
    if (isGroupA) {
        QString statusStr = QString("A组DI状态: ");
        for (int i = 0; i < 6; i++) {
            statusStr += QString("DI%1=%2 ").arg(i+1).arg(diStatus[i] ? "H" : "L");
        }

    }

    // 验证B组的预期状态（全部应为高）
    if (!isGroupA) {
        QString statusStr = QString("B组DI状态: ");
        for (int i = 0; i < 6; i++) {
            statusStr += QString("DI%1=%2 ").arg(i+1).arg(diStatus[6+i] ? "H" : "L");
        }
     //   qDebug() << statusStr;

        // 检查是否全部为高
        bool allHigh = true;
        for (int i = 6; i < 12; i++) {
            if (!diStatus[i]) {
                allHigh = false;
                break;
            }
        }
      //  qDebug() << "B组是否全部为高:" << (allHigh ? "是" : "否");
    }

    // 如果当前显示的组与更新的组匹配，立即更新UI
    if ((isGroupA && activeGroup == 0) || (!isGroupA && activeGroup == 1)) {
//        qDebug() << "更新当前组的DI显示";
        updateDIStatus();
    }



}

// 处理错误
void MainWindow::onErrorOccurred(const QString &message)
{
    onDisconnectButtonClicked();
    usbComm->closeConnect();
    statusBar()->showMessage("错误: " + message);
    QMessageBox::critical(this, "通信错误", message);

}

void MainWindow::onDO1Changed(bool checked)
{
    if (connected && usbComm) {
        uint32_t regAddr = activeGroup == 0 ? REG_A_DO1 : REG_B_DO1;
        usbComm->writeRegister(regAddr, checked ? 1 : 0);
    }
    statusBar()->showMessage(QString("5V_EN已设为: %2").arg(checked ? "使能" : "禁用"));
}

void MainWindow::onDO2Changed(bool checked)
{
    if (connected && usbComm) {
        uint32_t regAddr = activeGroup == 0 ? REG_A_DO2 : REG_B_DO2;
        usbComm->writeRegister(regAddr, checked ? 1 : 0);
    }
    statusBar()->showMessage(QString("3V3_CTRL已设为: %2").arg(checked ? "使能" : "禁用"));
}

void MainWindow::onPowerPlusChanged(bool checked)
{
    if (connected && usbComm) {
        uint32_t regAddr = activeGroup == 0 ? REG_A_POWER_PLUS : REG_B_POWER_PLUS;
        usbComm->writeRegister(regAddr, checked ? 1 : 0);
    }
    statusBar()->showMessage(QString("%1 +5V已%2").arg(activeGroup == 0 ? "A组" : "B组").arg(checked ? "使能" : "禁用"));
}

void MainWindow::onPowerMinusChanged(bool checked)
{
    if (connected && usbComm) {
        uint32_t regAddr = activeGroup == 0 ? REG_A_POWER_MINUS : REG_B_POWER_MINUS;
        usbComm->writeRegister(regAddr, checked ? 1 : 0);
    }
    statusBar()->showMessage(QString("%1 -5V已%2").arg(activeGroup == 0 ? "A组" : "B组").arg(checked ? "使能" : "禁用"));
}
// 在mainwindow.cpp中修改onVoltageDataReceived函数



void MainWindow::onStartRecvTimer()
{
    if (acquisitionInProgress) {
        // 检查实际收到的数据
        int currentPoints = 0;
        if (activeGroup == 0) {
            for (int ch = 0; ch < voltageData[0].size(); ch++) {
                if (!voltageData[0][ch].empty()) {
                    currentPoints = qMax(currentPoints, voltageData[0][ch].size());
                }
            }
        } else {
            for (int ch = 0; ch < voltageData[1].size(); ch++) {
                if (!voltageData[1][ch].empty()) {
                    currentPoints = qMax(currentPoints, voltageData[1][ch].size());
                }
            }
        }

        // 超时完成处理
        acquisitionInProgress = false;
        startAcquisitionButton->setEnabled(true);
        startAcquisitionButton->setText("开始采集");
        usbComm->readRegister(0x002C);


        if (diReadTimer && !diReadTimer->isActive()) {
            diReadTimer->start();
            qDebug() << "采集超时完成：已重启DI读取定时器";
        }

        if (currentPoints > 0) {
            saveDataButton->setEnabled(true);
            dataReady = true;
          //  statusBar()->showMessage(QString("内触发采集超时，但收到了 %1 个数据点").arg(currentPoints));
            statusBar()->showMessage(QString("内触发采集超时"));

            QTimer::singleShot(100, this, [this]() {
                updateChartData();
            });
        } else {
            statusBar()->showMessage("采集超时，未收到数据");
        }

        usbComm->printCount();
    }
}
void MainWindow::onPowerDataReceived(const QVector<QVariantMap> &powerData)
{
    if (powerData.isEmpty()) return;

    // 原有的UI更新逻辑保持不变
    QMutexLocker locker(&powerDataMutex);
    for (const QVariantMap &data : powerData) {
        int channel = data["channel"].toInt();
        double value = data["realValue"].toDouble();
        if (channel >= 0 && channel <= 5) {
            latestPowerValues[channel] = value;
        }
    }

    // 新增：在采集过程中保存功率数据历史
    if (acquisitionInProgress) {
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

        // 获取当前差分电压数据的索引位置
        int voltageDataCount = 0;
        if (activeGroup == 0) {
            for (int ch = 0; ch < voltageData[0].size(); ch++) {
                voltageDataCount = qMax(voltageDataCount, voltageData[0][ch].size());
            }
        } else {
            for (int ch = 0; ch < voltageData[1].size(); ch++) {
                voltageDataCount = qMax(voltageDataCount, voltageData[1][ch].size());
            }
        }

        // 为A组和B组分别保存功率数据
        double aVoltage = latestPowerValues.value(3, 0.0);  // A组电压
        double aCurrent = latestPowerValues.value(2, 0.0);  // A组电流
        double bVoltage = latestPowerValues.value(5, 0.0);  // B组电压
        double bCurrent = latestPowerValues.value(4, 0.0);  // B组电流

        // A组功率历史
        PowerDataPoint aPoint;
        aPoint.timestamp = currentTime;
        aPoint.voltage = aVoltage;
        aPoint.current = aCurrent;
        aPoint.power = aVoltage * aCurrent;
        aPoint.dataIndex = voltageDataCount;
        aPowerHistory.append(aPoint);

        // B组功率历史
        PowerDataPoint bPoint;
        bPoint.timestamp = currentTime;
        bPoint.voltage = bVoltage;
        bPoint.current = bCurrent;
        bPoint.power = bVoltage * bCurrent;
        bPoint.dataIndex = voltageDataCount;
        bPowerHistory.append(bPoint);

        // 限制历史数据大小
        if (aPowerHistory.size() > 20000) {
            aPowerHistory.removeFirst();
        }
        if (bPowerHistory.size() > 20000) {
            bPowerHistory.removeFirst();
        }
    }

    // 确保定时器在接收数据时启动
    if (!powerUpdateTimer->isActive()) {
        powerUpdateTimer->start();
    }
}
void MainWindow::preallocateMemory()
{
    if (!connected) return;

    // 计算期望的总数据点数
    int cycles = static_cast<int>(cycleCountSpinBox->value());
    int pointsPerCycle = static_cast<int>(cycleCountSpinBox1->value());
    expectedTotalPoints = cycles * pointsPerCycle;


    // 为A组和B组预分配内存
    int voltageChannelCount = (activeGroup == 0) ? 8 : 4;

    // 清空现有数据
    for (int ch = 0; ch < voltageData[activeGroup].size(); ch++) {
        voltageData[activeGroup][ch].clear();
        // 预分配内存，多分配10%作为缓冲
        int reserveSize = expectedTotalPoints + (expectedTotalPoints / 10);
        voltageData[activeGroup][ch].reserve(reserveSize);

        qDebug() << "通道" << ch << "预分配" << reserveSize << "个数据点";
    }

    // 重置数据索引
    resetDataIndices();
    isDataPreallocated = true;

    statusBar()->showMessage(QString("内存预分配完成，每通道%1个数据点").arg(expectedTotalPoints));
}
void MainWindow::initializeRawDataStorage()
{
    // 原有代码保持不变
    rawVoltageData.resize(2);
    rawVoltageData[0].resize(8);  // A组
    rawVoltageData[1].resize(4);  // B组

    // 新增：初始化时间戳存储
    timestamp1Data.resize(2);
    timestamp1Data[0].resize(8);  // A组
    timestamp1Data[1].resize(4);  // B组

    timestamp2Data.resize(2);
    timestamp2Data[0].resize(8);  // A组
    timestamp2Data[1].resize(4);  // B组

    for (int ch = 0; ch < 8; ch++) {
        rawVoltageData[0][ch].clear();
        timestamp1Data[0][ch].clear();
        timestamp2Data[0][ch].clear();
    }
    for (int ch = 0; ch < 4; ch++) {
        rawVoltageData[1][ch].clear();
        timestamp1Data[1][ch].clear();
        timestamp2Data[1][ch].clear();
    }
}
void MainWindow::updatePowerUI()
{
    QMutexLocker locker(&powerDataMutex);

    // 获取各通道最新值
    double boardVoltage = latestPowerValues.value(1, -1.0);    // 通道1：单板电压
    double boardCurrent = latestPowerValues.value(0, -1.0);    // 通道0：单板电流

    // A组数据
    double aVoltage = latestPowerValues.value(3, -1.0);  // 通道3：A组电压
    double aCurrent = latestPowerValues.value(2, -1.0);  // 通道2：A组电流

    // B组数据
    double bVoltage = latestPowerValues.value(5, -1.0);  // 通道5：B组电压
    double bCurrent = latestPowerValues.value(4, -1.0);  // 通道4：B组电流
    if (activeGroup == 0) {
    // 更新板卡信息
    if (boardVoltage >= 0) {
        boardVoltageLabel->setText(QString("%1 V").arg(boardVoltage, 0, 'f', 2));
    }

    if (boardCurrent >= 0) {
        boardCurrentLabel->setText(QString("%1 A").arg(boardCurrent, 0, 'f', 3));
    }

    // 计算并更新板卡功耗
    if (boardVoltage >= 0 && boardCurrent >= 0) {
        double boardPower = boardVoltage * boardCurrent;
        boardPowerLabel->setText(QString("%1 W").arg(boardPower, 0, 'f', 2));
    }


    if (aVoltage >= 0) {
        aVoltageLabel->setText(QString("%1 V").arg(aVoltage, 0, 'f', 2));
    } else {
        aVoltageLabel->setText("--");
    }

    if (aCurrent >= 0) {
        aCurrentLabel->setText(QString("%1 A").arg(aCurrent, 0, 'f', 3));
    } else {
        aCurrentLabel->setText("--");
    }

    if (aVoltage >= 0 && aCurrent >= 0) {
        double aPower = aVoltage * aCurrent;
        aPowerLabel->setText(QString("%1 W").arg(aPower, 0, 'f', 3));
    } else {
        aPowerLabel->setText("--");
    }

    } else {
    if (bVoltage >= 0) {
        aVoltageLabel->setText(QString("%1 V").arg(bVoltage, 0, 'f', 2));
    } else {
        aVoltageLabel->setText("--");
    }

    if (bCurrent >= 0) {
        aCurrentLabel->setText(QString("%1 A").arg(bCurrent, 0, 'f', 3));
    } else {
        aCurrentLabel->setText("--");
    }

    if (bVoltage >= 0 && bCurrent >= 0) {
        double bPower = bVoltage * bCurrent;
        aPowerLabel->setText(QString("%1 W").arg(bPower, 0, 'f', 3));
    } else {
        aPowerLabel->setText("--");
    }
    }

//    // 继续更新当前选择组的状态表格（保持原有逻辑）
//    if (activeGroup == 0) {
//        // A组：更新组状态表格
//        if (aVoltage >= 0 && groupStatusTable->item(0, 1)) {
//            groupStatusTable->item(0, 1)->setText(QString::number(aVoltage, 'f', 2));
//        }
//        if (aCurrent >= 0 && groupStatusTable->item(1, 1)) {
//            groupStatusTable->item(1, 1)->setText(QString::number(aCurrent, 'f', 3));
//        }
//    } else {
//        // B组：更新组状态表格
//        if (bVoltage >= 0 && groupStatusTable->item(0, 1)) {
//            groupStatusTable->item(0, 1)->setText(QString::number(bVoltage, 'f', 2));
//        }
//        if (bCurrent >= 0 && groupStatusTable->item(1, 1)) {
//            groupStatusTable->item(1, 1)->setText(QString::number(bCurrent, 'f', 3));
//        }
//    }


}
void MainWindow::onDIReadTimer()
{
    if (!connected || !usbComm) {
        return;
    }

    // 读取A组DI状态（DI1-DI6）
    usbComm->readRegister(REG_A_DI);

    // 稍微延时后读取B组DI状态
    QTimer::singleShot(100, [this]() {
        if (connected && usbComm) {
            usbComm->readRegister(REG_B_DI);
        }
    });
}
void MainWindow::onAcquisitionCompleted(int totalPackets)
{
    // 停止定时器
    if (_pTimer && _pTimer->isActive()) {
        _pTimer->stop();
    }

    acquisitionInProgress = false;
    startAcquisitionButton->setEnabled(true);
    startAcquisitionButton->setText("开始采集");
    saveDataButton->setEnabled(true);
    dataReady = true;

    usbComm->readRegister(0x002C);

    // 重新启动DI读取定时器
    if (diReadTimer && !diReadTimer->isActive()) {
        diReadTimer->start();
        qDebug() << "采集完成：已重启DI读取定时器";
    }

    bool isExternalTrigger = externalTriggerRadio1->isChecked();
    if (isExternalTrigger) {
        statusBar()->showMessage("外触发采集完成");
    } else {
        statusBar()->showMessage("内触发采集完成");
    }

    qDebug() << "采集完成，收到" << totalPackets << "个数据包";

    // 延迟更新图表，避免阻塞
    QTimer::singleShot(100, this, &MainWindow::updateChartsFromCache);
}
void MainWindow::onAcquisitionProgress(int current, int total)
{
    // 更新进度，但不要过于频繁更新UI
    QString progressMsg = QString("采集进度: %1/%2 (%3%)")
                         .arg(current)
                         .arg(total)
                         .arg(static_cast<int>(100.0 * current / total));
    statusBar()->showMessage(progressMsg);
}

void MainWindow::updateChartsFromCache()
{
    if (!UsbCommunication::isDataReady()) {
        return;
    }

    // 从全局缓存获取数据并更新图表
    int channelCount = (activeGroup == 0) ? 8 : 4;

    for (int ch = 0; ch < channelCount; ch++) {
        QVector<double> voltageData;
        UsbCommunication::getVoltageData(activeGroup, ch, voltageData);

        if (!voltageData.isEmpty()) {
            // 更新到原有的数据结构中，以兼容现有的图表更新逻辑
            if (activeGroup < this->voltageData.size() && ch < this->voltageData[activeGroup].size()) {
                this->voltageData[activeGroup][ch] = voltageData;
            }
        }
    }

    // 调用原有的图表更新方法
    updateChartData();
}
