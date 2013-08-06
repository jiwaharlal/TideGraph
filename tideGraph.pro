#-------------------------------------------------
#
# Project created by QtCreator 2013-08-02T19:20:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tideGraph
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    maingraphicswidget.cpp \
    tidegraphwidget.cpp \
    mainview.cpp \
    tidedatacache.cpp \
    mainscene.cpp

HEADERS  += mainwindow.h \
    maingraphicswidget.h \
    tidegraphwidget.h \
    mainview.h \
    tidedatacache.h \
    mainscene.h

RESOURCES += \
    tideGraph.qrc
