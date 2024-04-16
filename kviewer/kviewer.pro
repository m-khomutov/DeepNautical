TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR = ../build/kviewer
DESTDIR += $$OBJECTS_DIR/lib
CONFIG += 14

HEADERS = \
    include/c_socket.h \
    include/receiver.h \
    include/basedecoder.h \
    include/jpegdecoder.cpp \
    include/baseviewer.h


SOURCES = \
    client/c_socket.cpp \
    client/receiver.cpp \
    decoder/basedecoder.cpp \
    decoder/jpegdecoder.cpp \
    viewer/baseviewer.cpp


INCLUDEPATH += $$PWD/include ../kformat
LIBS = -ljpeg
