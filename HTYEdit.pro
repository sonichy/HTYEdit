#-------------------------------------------------
#
# Project created by QtCreator 2017-01-10T08:44:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HTYEdit
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mdichild.cpp \
    dialogfind.cpp

HEADERS  += mainwindow.h \
    mdichild.h \
    dialogfind.h

FORMS    += mainwindow.ui \
    dialogfind.ui

RESOURCES += \
    qtrf.qrc
