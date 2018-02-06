#-------------------------------------------------
#
# Project created by QtCreator 2016-11-23T22:37:22
#
#-------------------------------------------------

QT       -= gui

TARGET = amex-sucmder-dev
TEMPLATE = lib
CONFIG += staticlib c++11

INCLUDEPATH += "../libSimpleCrypt"

SOURCES += sudocommander.cpp

HEADERS += sudocommander.h

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
