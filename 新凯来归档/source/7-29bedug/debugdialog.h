#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QScrollArea>
#include <QDateTime>
#include <QTimer>

class UsbCommunication;

class DebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugDialog(UsbCommunication *usbComm, QWidget *parent = nullptr);
    ~DebugDialog();

    // 添加调试信息到窗口
    void addDebugInfo(const QString &info);

    // 更新通道使能状态
    void updateChannelEnable(uint32_t regValue);

public slots:
    // 寄存器操作槽函数
    void onReadRegisterClicked();
    void onWriteRegisterClicked();
    void onChannelEnableChanged();
    void onApplyChannelEnableClicked();
    void onClearDebugClicked();

    // 数据接收槽函数
    void onRegisterDataReceived(uint32_t regAddr, uint32_t regData);
    void onRawDataReceived(const QByteArray &data);

private:
    void setupUi();
    void connectSignals();
    QString formatHexData(const QByteArray &data);
    void logDebugInfo(const QString &info);

private:
    UsbCommunication *usbCommunication;

    // UI控件
    QTextEdit *debugTextEdit;
    QScrollArea *channelScrollArea;

    // 寄存器操作控件
    QLineEdit *regAddrEdit;
    QLineEdit *regDataEdit;
    QPushButton *readRegButton;
    QPushButton *writeRegButton;
    QPushButton *clearDebugButton;

    // 通道使能控件
    QVector<QCheckBox*> channelCheckBoxes;
    QPushButton *applyChannelEnableButton;
    QPushButton *selectAllButton;
    QPushButton *clearAllButton;

    // 状态标签
    QLabel *channelStatusLabel;

    // 定时器用于自动滚动
    QTimer *scrollTimer;

    // 常量
    static const uint32_t REG_ACQ_CH_EN = 0x00000005;
    static const int MAX_DEBUG_LINES = 1000;

};

#endif // DEBUGDIALOG_H
