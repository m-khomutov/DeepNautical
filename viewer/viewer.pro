#-------------------------------------------------
#
# Project created by QtCreator 2023-02-01T11:41:07
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = viewer
TEMPLATE = app
OBJECTS_DIR = ../build/viewer
DESTDIR += $$OBJECTS_DIR/app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    src/decoder/basedecoder.cpp \
    src/decoder/jpegdecoder.cpp \
    src/receiver/c_socket.cpp \
    src/receiver/receiver.cpp \
    src/main.cpp \
    src/viewer/baseviewer.cpp \
    src/viewer/qviewer.cpp

HEADERS += \
    src/decoder/basedecoder.h \
    src/decoder/jpegdecoder.h \
    src/receiver/c_socket.h \
    src/receiver/receiver.h \
    src/viewer/baseviewer.h \
    src/viewer/qviewer.h

INCLUDEPATH += ../kformat
LIBS += ../build/kformat/lib/libkformat.a -ljpeg
