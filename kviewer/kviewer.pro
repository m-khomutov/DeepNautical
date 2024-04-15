TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR = ../build/kviewer
DESTDIR += $$OBJECTS_DIR/lib
CONFIG += 14

HEADERS = \
    client/c_socket.h \
    client/receiver.h \
    decoder/basedecoder.h \
    decoder/jpegdecoder.cpp \
    viewer/baseviewer.h


SOURCES = \
    client/c_socket.cpp \
    client/receiver.cpp \
    decoder/basedecoder.cpp \
    decoder/jpegdecoder.cpp \
    viewer/baseviewer.cpp


INCLUDEPATH += $$PWD ../kformat
LIBS = -ljpeg
