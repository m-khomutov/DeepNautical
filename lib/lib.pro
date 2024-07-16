TARGET = nautical
TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR = ../build/$$TARGET-dev
DESTDIR += $$OBJECTS_DIR/lib
CONFIG += 14

HEADERS = \
    ../utils/utils.h \
    include/screen.h \
    include/tcpsock.h \
    include/vdev.h \
    include/loop.h \
    include/connection.h \
    include/service.h \
    include/frame.h \
    include/jpeg.h \
    include/proto.h \
    include/flv.h \
    include/mjpeg.h \
    include/api.h


SOURCES = \
        ../utils/utils.cpp \
        base/screen.cpp \
        base/service.cpp \
        base/frame.cpp \
        base/protocol.cpp \
        service/tcpsock.cpp \
        service/vdev.cpp \
        service/connection.cpp \
        service/loop.cpp \
        frame/jpeg.cpp \
        protocol/flv.cpp \
        protocol/mjpeg.cpp \
        protocol/api.cpp

INCLUDEPATH += $$PWD/include $$PWD/../utils
LIBS = -ljpeg
