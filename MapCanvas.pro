TEMPLATE = app

QT += qml quick widgets

#CONFIG += c++11

SOURCES += main.cpp \
    MapManager.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

unix:LIBS += $$PWD/mitab/libmitab.so
win32:LIBS += $$PWD/mitab/cpl.lib
win32:LIBS += $$PWD/mitab/mitab_i.lib
win32:LIBS += $$PWD/mitab/ogr.lib
win32:LIBS += $$PWD/mitab/mitab.lib

INCLUDEPATH += $$PWD/mitab
DEPENDPATH += $$PWD/mitab

HEADERS += \
    MapManager.h
