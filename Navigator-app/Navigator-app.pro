QT += core serialport network
QT -= gui

CONFIG += c++11

TARGET = Navigator
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += "../libqmodules"
INCLUDEPATH += "../libNavout"

#linux-g++-64 {
#    LIBS += -L"../build/release"
#    LIBS += -L"../build/debug"
#}

LIBS += -L"../build/release"
LIBS += -L"../build/debug"

LIBS += -lNavout # shared object
LIBS += -lqmodules
LIBS += -lamex-license-dev
LIBS += -lSimpleCrypt
LIBS += -lamex-mmcid-dev
LIBS += -lamex-entry-dev
LIBS += -lamex-sucmder-dev
LIBS += -lamex-module-neom8ngps-dev
LIBS += -lamex-module-adis16405-dev
LIBS += -lamex-module-bmp280-dev
LIBS += -lamex-module-serialout-dev
LIBS += -lamex-bus-dev
LIBS += -lamex-gpio-dev

TEMPLATE = app

SOURCES += Navigator.cpp \
    qnavapplication.cpp

HEADERS += \
    qnavapplication.h




release: DESTDIR = ../build/release
debug:   DESTDIR = ../build/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
