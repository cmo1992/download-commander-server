#-------------------------------------------------
#
# Project created by QtCreator 2015-01-02T21:09:43
#
#-------------------------------------------------

QT       += core
QT       += network
QT       += xml

QT       -= gui

TARGET = download-commander-server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    myserver.cpp \
    myserverthread.cpp \
    item.cpp \
    itemlist.cpp \
    jservhandler.cpp \
    config.cpp

HEADERS += \
    myserver.h \
    myserverthread.h \
    item.h \
    itemlist.h \
    jservhandler.h \
    config.h
