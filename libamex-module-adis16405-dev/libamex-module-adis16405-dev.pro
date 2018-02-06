#-------------------------------------------------
#
# Project created by QtCreator 2016-11-14T15:29:41
#
#-------------------------------------------------

QT       -= core gui

TARGET = amex-module-adis16405-dev
TEMPLATE = lib
CONFIG += staticlib c++11
INCLUDEPATH += "../libamex-bus-dev"

SOURCES += \
    adis16405.cpp

HEADERS += \
    adis16405.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}





release: DESTDIR = ../build/release
debug:   DESTDIR = ../build/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

