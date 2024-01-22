TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR = ../build/kformat
DESTDIR += $$OBJECTS_DIR/lib
CONFIG += 14

HEADERS = kformat.h \
    utils.h \
    screen/basescreen.h \
    encoding/baseframe.h \
    encoding/jpegframe.h \
    protocol/baseprotocol.h \
    protocol/flvprotocol.h


SOURCES = \
        utils.cpp \
        screen/basescreen.cpp \
        encoding/baseframe.cpp \
        encoding/jpegframe.cpp \
        protocol/baseprotocol.cpp \
        protocol/flvprotocol.cpp


INCLUDEPATH += $$PWD
LIBS = -ljpeg
