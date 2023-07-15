#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFileDialog>
#include <stdio.h>
//#include <errorhandle.h>
#include <QPushButton>
#include <QPainter>
#include "paintwidget.h"
#include "subway_map.cpp"
//#include "config.cpp"
//#include <city.h>
#include <QBitmap>
#include <thread>
#include <QTextCodec>
#include <QDockWidget>
#include <QLineEdit>
#include <sys/types.h>
#include <unistd.h>
#include <processthreadsapi.h>
#include <QThread>
#include <QPixmapCache>
#include <QReadWriteLock>
#include <QLabel>
using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void printPath();
    void repaint();

    ~MainWindow();
signals:
    void send();
public slots:
    void got();
    void Import_clicked();
    void Zoom_in();
    void Zoom_out();
    void Button_clicked();
    void ShortestPath();
    void ShortestPathWithTime();
protected:
    void wheelEvent(QWheelEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *size);
private slots:
//    void on_pushButton_clicked();


    void on_butZoomIn_clicked();

    void on_butZoomOut_clicked();

    void on_stationOrigin_textChanged(const QString &arg1);

    void on_stationDestination_textChanged(const QString &arg1);

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_butFindWay_clicked();

    void on_LineName_activated(const QString &arg1);

    void on_radioButton_2_toggled(bool checked);

private:
    Ui::MainWindow *ui;
//    ErrorHandle * handle;
    vector<QPushButton*>list;//using for save station button
    vector<QLabel*>labellist;
    vector<int>nolist;//using for save station No 与list相对应，at相同位置记录list那个站的序号
    vector<int>pointlist;//using for save path.
    PaintWidget *area;
    QPoint lastpoint;//using for catch mouse
    QPoint endpoint;//
    QReadWriteLock lock;
    QDockWidget *dock;
//    City *city;
    Graph* graph;
    Config* config;
    DWORD ThreadID;
    QPixmap buttonimage;

    int xy[100][2];
    int xylen;
    int zoom;//using for zoom
    int zoom_last;
    int positionx,positiony;//using for align
    int start,end;//using for print
    //using for leading the path
    int nowpos;
    int lastpos;
    QPushButton *nowbutton;
    vector <int >path;
    bool LenthOrTransfer = true;
    bool moveflag = false;
    vector<QPushButton*>stressLineButton;
    int stressLine = -1; //当前强调的线路
    typedef std::vector<std::string> sv;
    std::pair<sv, sv> pathResults;
    std::pair<std::pair<sv, sv>, sv> pathResultsWithTimes;
    QTextCodec *codec = QTextCodec::codecForName("GBK");
};

#endif // MAINWINDOW_H
