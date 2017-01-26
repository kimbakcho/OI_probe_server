#-------------------------------------------------
#
# Project created by QtCreator 2017-01-16T13:35:16
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OI_probe_server
TEMPLATE = app

TRANSLATIONS += lang_oi_ko.ts

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
