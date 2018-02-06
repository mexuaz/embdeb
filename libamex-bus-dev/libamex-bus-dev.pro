#-------------------------------------------------
#
# Project created by QtCreator 2016-11-14T12:55:42
#
#-------------------------------------------------

QT       -= core gui

TARGET = amex-bus-dev
TEMPLATE = lib
CONFIG += staticlib c++11

SOURCES += \
    bus.cpp \
    i2cbus.cpp \
    spibus.cpp

HEADERS += \
    bus.h \
    i2cbus.h \
    spibus.h
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
