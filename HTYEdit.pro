QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HTYEdit
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    mdichild.cpp \
    dialogfind.cpp \
    highlighter.cpp


HEADERS  += mainwindow.h \
    mdichild.h \
    dialogfind.h \
    highlighter.h

FORMS    += mainwindow.ui \
    dialogfind.ui

RESOURCES += \
    res.qrc
