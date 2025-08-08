#include "debugdialog.h"
#include "usbcommunication.h"
#include <QSplitter>
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>

DebugDialog::DebugDialog(UsbCommunication *usbComm, QWidget *parent)
    : QDialog(parent)
    , usbCommunication(usbComm)
    , scrollTimer(new QTimer(this))
{
    setWindowTitle("FPGA调试窗口");
    setModal(false);
    resize(1000, 700);

    // 初始化通道复选框
    channelCheckBoxes.resize(22);

    setupUi();
    connectSignals();

    // 设置自动滚动定时器
    scrollTimer->setSingleShot(true);
    scrollTimer->setInterval(100);
    connect(scrollTimer, &QTimer::timeout, [this]() {
        debugTextEdit->moveCursor(QTextCursor::End);
    });

    // 初始读取通道使能寄存器
    if (usbCommunication && usbCommunication->isConnected()) {
        usbCommunication->readRegister(REG_ACQ_CH_EN);
    }

    addDebugInfo("=== 调试窗口已打开 ===");
}

DebugDialog::~DebugDialog()
{
}

void DebugDialog::setupUi()
{
    // 创建主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    // 创建分割器
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    mainLayout->addWidget(splitter);

    // === 左侧面板：调试信息和寄存器操作 ===
    QWidget *leftWidget = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);

    // 调试信息组
    QGroupBox *debugGroup = new QGroupBox("调试信息");
    QVBoxLayout *debugLayout = new QVBoxLayout(debugGroup);

    debugTextEdit = new QTextEdit;
    debugTextEdit->setReadOnly(true);
    debugTextEdit->setFont(QFont("Consolas", 9));
    debugTextEdit->document()->setMaximumBlockCount(MAX_DEBUG_LINES);
    debugLayout->addWidget(debugTextEdit);

    // 清除按钮
    clearDebugButton = new QPushButton("清除调试信息");
    debugLayout->addWidget(clearDebugButton);

    leftLayout->addWidget(debugGroup, 3);

    // 寄存器操作组
    QGroupBox *regGroup = new QGroupBox("寄存器操作");
    QGridLayout *regLayout = new QGridLayout(regGroup);

    regLayout->addWidget(new QLabel("寄存器地址:"), 0, 0);
    regAddrEdit = new QLineEdit("0x00000000");
    regAddrEdit->setPlaceholderText("例如：0x00000005");
    regLayout->addWidget(regAddrEdit, 0, 1, 1, 2);

    regLayout->addWidget(new QLabel("写入数据:"), 1, 0);
    regDataEdit = new QLineEdit("0x00000000");
    regDataEdit->setPlaceholderText("例如：0x003FFFFF");
    regLayout->addWidget(regDataEdit, 1, 1, 1, 2);

    readRegButton = new QPushButton("读取寄存器");
    regLayout->addWidget(readRegButton, 2, 0);

    writeRegButton = new QPushButton("写入寄存器");
    regLayout->addWidget(writeRegButton, 2, 1);

    leftLayout->addWidget(regGroup, 1);

    splitter->addWidget(leftWidget);

    // === 右侧面板：通道使能控制 ===
    QWidget *rightWidget = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);

    QGroupBox *channelGroup = new QGroupBox("通道使能控制 (0x05寄存器)");
    QVBoxLayout *channelLayout = new QVBoxLayout(channelGroup);

    // 状态标签
    channelStatusLabel = new QLabel("当前通道使能状态: 0x000000");
    channelStatusLabel->setStyleSheet("font-weight: bold; color: blue;");
    channelLayout->addWidget(channelStatusLabel);

    // 全选/清空按钮
    QHBoxLayout *selectLayout = new QHBoxLayout;
    selectAllButton = new QPushButton("全选");
    clearAllButton = new QPushButton("清空");
    selectLayout->addWidget(selectAllButton);
    selectLayout->addWidget(clearAllButton);
    selectLayout->addStretch();
    channelLayout->addLayout(selectLayout);

    // 创建滚动区域
    channelScrollArea = new QScrollArea;
    channelScrollArea->setWidgetResizable(true);
    channelScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    channelScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *channelWidget = new QWidget;
    QGridLayout *channelGridLayout = new QGridLayout(channelWidget);

    // 创建22个通道复选框（每行4个）
    for (int i = 0; i < 22; i++) {
        channelCheckBoxes[i] = new QCheckBox(QString("CH%1").arg(i));
        channelCheckBoxes[i]->setStyleSheet("QCheckBox { font-size: 12px; }");

        int row = i / 4;
        int col = i % 4;
        channelGridLayout->addWidget(channelCheckBoxes[i], row, col);

        connect(channelCheckBoxes[i], &QCheckBox::toggled,
                this, &DebugDialog::onChannelEnableChanged);
    }

    channelScrollArea->setWidget(channelWidget);
    channelLayout->addWidget(channelScrollArea, 1);

    // 应用按钮
    applyChannelEnableButton = new QPushButton("应用通道使能设置");
    applyChannelEnableButton->setStyleSheet("QPushButton { font-weight: bold; background-color: #4CAF50; color: white; }");
    channelLayout->addWidget(applyChannelEnableButton);

    rightLayout->addWidget(channelGroup);
    splitter->addWidget(rightWidget);

    // 设置分割器比例
    splitter->setSizes({600, 400});
}

void DebugDialog::connectSignals()
{
    // 按钮连接
    connect(readRegButton, &QPushButton::clicked, this, &DebugDialog::onReadRegisterClicked);
    connect(writeRegButton, &QPushButton::clicked, this, &DebugDialog::onWriteRegisterClicked);
    connect(clearDebugButton, &QPushButton::clicked, this, &DebugDialog::onClearDebugClicked);
    connect(applyChannelEnableButton, &QPushButton::clicked, this, &DebugDialog::onApplyChannelEnableClicked);
    connect(selectAllButton, &QPushButton::clicked, [this]() {
        for (auto *checkbox : channelCheckBoxes) {
            checkbox->setChecked(true);
        }
    });
    connect(clearAllButton, &QPushButton::clicked, [this]() {
        for (auto *checkbox : channelCheckBoxes) {
            checkbox->setChecked(false);
        }
    });

    // 通信信号连接
    if (usbCommunication) {
//        connect(usbCommunication, &UsbCommunication::registerDataReceived,
//                this, &DebugDialog::onRegisterDataReceived);
//        connect(usbCommunication, &UsbCommunication::dataReceived,
//                this, &DebugDialog::onRawDataReceived);
    }
}

void DebugDialog::addDebugInfo(const QString &info)
{
    logDebugInfo(info);
}

void DebugDialog::logDebugInfo(const QString &info)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString logLine = QString("[%1] %2").arg(timestamp, info);

    debugTextEdit->append(logLine);

    // 启动滚动定时器
    scrollTimer->start();
}

void DebugDialog::onReadRegisterClicked()
{
    if (!usbCommunication || !usbCommunication->isConnected()) {
        QMessageBox::warning(this, "错误", "设备未连接");
        return;
    }

    QString addrStr = regAddrEdit->text().trimmed();
    bool ok;
    uint32_t regAddr = addrStr.toUInt(&ok, 16);

    if (!ok) {
        QMessageBox::warning(this, "错误", "无效的寄存器地址格式");
        return;
    }

    addDebugInfo(QString("发送读取寄存器请求: 地址=0x%1").arg(regAddr, 8, 16, QChar('0')));
    usbCommunication->readRegister(regAddr);
}

void DebugDialog::onWriteRegisterClicked()
{
    if (!usbCommunication || !usbCommunication->isConnected()) {
        QMessageBox::warning(this, "错误", "设备未连接");
        return;
    }

    QString addrStr = regAddrEdit->text().trimmed();
    QString dataStr = regDataEdit->text().trimmed();
    bool ok1, ok2;
    uint32_t regAddr = addrStr.toUInt(&ok1, 16);
    uint32_t regData = dataStr.toUInt(&ok2, 16);

    if (!ok1 || !ok2) {
        QMessageBox::warning(this, "错误", "无效的地址或数据格式");
        return;
    }

    addDebugInfo(QString("发送写入寄存器请求: 地址=0x%1, 数据=0x%2")
                 .arg(regAddr, 8, 16, QChar('0'))
                 .arg(regData, 8, 16, QChar('0')));
    usbCommunication->writeRegister(regAddr, regData);
}

void DebugDialog::onChannelEnableChanged()
{
    // 实时更新状态显示
    uint32_t currentValue = 0;
    for (int i = 0; i < 22; i++) {
        if (channelCheckBoxes[i]->isChecked()) {
            currentValue |= (1 << i);
        }
    }

    channelStatusLabel->setText(QString("当前通道使能状态: 0x%1")
                               .arg(currentValue, 6, 16, QChar('0')).toUpper());
}

void DebugDialog::onApplyChannelEnableClicked()
{
    if (!usbCommunication || !usbCommunication->isConnected()) {
        QMessageBox::warning(this, "错误", "设备未连接");
        return;
    }

    // 计算通道使能值
    uint32_t channelEnableValue = 0;
    for (int i = 0; i < 22; i++) {
        if (channelCheckBoxes[i]->isChecked()) {
            channelEnableValue |= (1 << i);
        }
    }

    addDebugInfo(QString("=== 开始通道使能设置操作 ==="));
    addDebugInfo(QString("目标值: 0x%1 (二进制: %2)")
                 .arg(channelEnableValue, 6, 16, QChar('0')).toUpper()
                 .arg(channelEnableValue, 22, 2, QChar('0')));

    // 先读取当前值
    addDebugInfo("步骤1: 读取当前值");
    usbCommunication->readRegister(REG_ACQ_CH_EN);

    // 延迟写入
    QTimer::singleShot(200, [this, channelEnableValue]() {
        addDebugInfo("步骤2: 写入新值");
        bool writeSuccess = usbCommunication->writeRegister(REG_ACQ_CH_EN, channelEnableValue);
        if (writeSuccess) {
            addDebugInfo("写入命令已发送");
        } else {
            addDebugInfo("ERROR: 写入命令发送失败");
        }

        // 延迟读取验证
        QTimer::singleShot(200, [this]() {
            addDebugInfo("步骤3: 读取验证");
            usbCommunication->readRegister(REG_ACQ_CH_EN);
        });
    });
}

void DebugDialog::onClearDebugClicked()
{
    debugTextEdit->clear();
    addDebugInfo("=== 调试信息已清除 ===");
}

void DebugDialog::onRegisterDataReceived(uint32_t regAddr, uint32_t regData)
{
    QString regName = "未知寄存器";
    QString interpretation = "";

    // 解析寄存器含义
    switch (regAddr) {
        case 0x00000000:
            regName = "FPGA版本号";
            interpretation = QString("版本: v%1.%2.%3")
                           .arg((regData >> 24) & 0xFF)
                           .arg((regData >> 16) & 0xFF)
                           .arg(regData & 0xFFFF);
            break;

        case REG_ACQ_CH_EN:
            regName = "通道使能控制";
            interpretation = QString("使能通道: ");
            for (int i = 0; i < 22; i++) {
                if (regData & (1 << i)) {
                    interpretation += QString("CH%1 ").arg(i);
                }
            }
            if (interpretation.endsWith(": ")) {
                interpretation += "无";
            }
            // 更新UI显示
            updateChannelEnable(regData);
            break;

        default:
            if (regAddr >= 0x20 && regAddr <= 0x2B) {
                regName = "A组寄存器";
            } else if (regAddr >= 0x40 && regAddr <= 0x4B) {
                regName = "B组寄存器";
            }
            break;
    }

    addDebugInfo(QString("接收寄存器数据: [%1] 地址=0x%2, 原始数据=0x%3 (%4), %5")
                 .arg(regName)
                 .arg(regAddr, 8, 16, QChar('0'))
                 .arg(regData, 8, 16, QChar('0'))
                 .arg(regData)
                 .arg(interpretation));
}

void DebugDialog::onRawDataReceived(const QByteArray &data)
{
    if (data.size() < 10) return; // 忽略太短的数据包

    QString hexData = formatHexData(data);

    // 检查是否是写寄存器的响应包
    if (data.size() >= 10) {
        // 解析命令类型
        uint16_t cmd = (static_cast<uint8_t>(data[4])) |
                      (static_cast<uint8_t>(data[5]) << 8);

        if (cmd == 0x02) { // 写寄存器命令
            // 解析地址
            uint32_t addr = (static_cast<uint8_t>(data[10])) |
                           (static_cast<uint8_t>(data[11]) << 8) |
                           (static_cast<uint8_t>(data[12]) << 16) |
                           (static_cast<uint8_t>(data[13]) << 24);

            // 解析数据
            uint32_t wdata = (static_cast<uint8_t>(data[14])) |
                            (static_cast<uint8_t>(data[15]) << 8) |
                            (static_cast<uint8_t>(data[16]) << 16) |
                            (static_cast<uint8_t>(data[17]) << 24);

            if (addr == REG_ACQ_CH_EN) {
                addDebugInfo(QString("发送写入数据包: 地址=0x%1, 数据=0x%2")
                            .arg(addr, 8, 16, QChar('0'))
                            .arg(wdata, 8, 16, QChar('0')));
            }
        }
    }

    addDebugInfo(QString("接收原始数据 (%1字节): %2").arg(data.size()).arg(hexData));
}

void DebugDialog::updateChannelEnable(uint32_t regValue)
{
    for (int i = 0; i < 22; i++) {
        bool enabled = (regValue & (1 << i)) != 0;
        channelCheckBoxes[i]->blockSignals(true);
        channelCheckBoxes[i]->setChecked(enabled);
        channelCheckBoxes[i]->blockSignals(false);
    }

    channelStatusLabel->setText(QString("当前通道使能状态: 0x%1")
                               .arg(regValue, 6, 16, QChar('0')).toUpper());
}

QString DebugDialog::formatHexData(const QByteArray &data)
{
    QString result;
    for (int i = 0; i < data.size(); i++) {
        result += QString("%1 ").arg(static_cast<uint8_t>(data[i]), 2, 16, QChar('0'));
    }

    return result.toUpper();
}
