TEMPLATE = lib
TARGET  = cancontrollerplugin
QT += qml quick serialbus
CONFIG += qt plugin c++11

uri = com.yazaki.CanController
load(qmlplugin)

SOURCES += \
    jsonsignal.cpp \
    plugin.cpp \
    cancontroller.cpp \

HEADERS += \
    cancontroller.h \ \
    jsonsignal.h

DISTFILES += \
    ccan_database_example.json
