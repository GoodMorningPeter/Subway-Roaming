#include "mainwindow.h"
#include <QApplication>
#include <QThread>
#include <iostream>
#include <cstring>

int main(int argc, char **argv) {
    system("chcp 936");
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(":/button/bit.jpg"));
    w.setWindowTitle("BIT Subway Roaming Platfrom");
    w.show();
    return a.exec();
}

