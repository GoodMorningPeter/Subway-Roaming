#pragma once
#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <stdlib.h>
#include <vector>
#include <time.h>
#define PRECOLOR 20
#define BACKGROUNDCOLOR 255,255,255
class PaintWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaintWidget(QWidget *parent = 0);
    void paintpath(int x1,int y1,int x2,int y2,int zoom,int add);
    void clear();
    void resize(QSize size);
    void paintcircle(int x1,int y1,int zoom, bool startOrEnd);
public  slots:


protected:
    void paintEvent(QPaintEvent *);

signals:

private:
    QColor color[50];
    QPixmap image;

    QRgb backColor;
    QPoint lastPoint;
    QPoint endPoint;
    bool isDrawing;
    int colornum;
};
#endif
