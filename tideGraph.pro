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
    mainscene.cpp \
    ButtonWidget.cpp \
    LabelWidget.cpp

HEADERS  += mainwindow.h \
    maingraphicswidget.h \
    tidegraphwidget.h \
    mainview.h \
    tidedatacache.h \
    mainscene.h \
    ButtonWidget.h \
    LabelWidget.h

RESOURCES += \
    tideGraph.qrc
