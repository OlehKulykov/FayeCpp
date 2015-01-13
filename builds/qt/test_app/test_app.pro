#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T16:45:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_app
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

DEFINES += FAYECPP_DEBUG_MESSAGES=1

include(../../../fayecpp.pri)

