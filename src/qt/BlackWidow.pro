#-------------------------------------------------
#
# Project created by QtCreator 2019-07-08T20:02:07
#
#-------------------------------------------------

QT       += core gui widgets network sql

TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11
DESTDIR = bin
OBJECTS_DIR = tmp/obj
MOC_DIR = tmp/moc
RCC_DIR = tmp/rcc
UI_DIR = tmp/ui

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        base.hpp \
        blackwidow.hpp \
        database.hpp \
        highlighter.hpp \
        json.hpp \
        mainwindow.h \
        spider.hpp

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources.qrc
