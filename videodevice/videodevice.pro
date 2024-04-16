TARGET = videodevice

TEMPLATE = app
OBJECTS_DIR = ../build/videodevice
DESTDIR += $$OBJECTS_DIR/app
CONFIG += 14

SOURCES += \
    src/service.cpp \
    src/main.cpp

HEADERS += \
    src/service.h

INCLUDEPATH += ../kformat/include ../common
LIBS += ../build/kformat/lib/libkformat.a -ljpeg
