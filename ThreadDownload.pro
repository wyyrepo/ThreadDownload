#-------------------------------------------------
#
# Project created by QtCreator 2014-06-24T11:21:03
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThreadDownload
TEMPLATE = app


SOURCES += main.cpp\
    downloadthread.cpp \
    downloadcontrol.cpp \
    widget.cpp \
    downloadwidgets.cpp

HEADERS  += \
    downloadthread.h \
    downloadcontrol.h \
    config.h \
    widget.h \
    downloadwidgets.h

RC_ICONS += download.ico
