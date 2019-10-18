TEMPLATE = lib
TARGET  = cancontrollerplugin
QT += qml quick serialbus
CONFIG += qt plugin c++11

uri = com.yazaki.CanController
load(qmlplugin)

SOURCES += \
    plugin.cpp \
    cancontroller.cpp \

HEADERS += \
    cancontroller.h \

DISTFILES += \
    amp_can_message_example.json \
    ccan_database_example.json
