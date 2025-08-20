#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<QVector<QVariantMap>>("QVector<QVariantMap>");
    MainWindow w;
    w.show();
    return a.exec();
}
