#-------------------------------------------------
#
# Project created by QtCreator 2016-08-07T21:15:55
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StarWay-SatUpdate
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp \
    datareadtodb.cpp \
    downloadfromspacetrack.cpp

HEADERS  += \
    mainwindow.h \
    datareadtodb.h \
    downloadfromspacetrack.h

FORMS    += \
    mainwindow.ui \
    datareadtodb.ui

RESOURCES += \
    datareadtodb.qrc
