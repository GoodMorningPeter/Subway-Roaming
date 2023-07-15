#-------------------------------------------------
#
# Project created by QtCreator 2023-07-04T10:28:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = subway
TEMPLATE = app


SOURCES += code/main.cpp\
        code/mainwindow.cpp \
    code/config.cpp \
    code/paintwidget.cpp \
    code/subway_map.cpp \
    code/test.cpp \
    code/sha236.cpp


HEADERS  += code/mainwindow.h \
           code/paintwidget.h \
           code/utils.h


FORMS    += \
    code/mainwindow.ui

DISTFILES += \
    subway.pro.user

RESOURCES += \
    images/image.qrc \
    data/subway_info.txt \
    data/subway_info_less.txt \
    data/subway_all_cost0.cache \
    data/subway_all_cost3.cache \
    data/config.txt \
    data/config_less.txt \
    data/tmp.txt


CONFIG += console
