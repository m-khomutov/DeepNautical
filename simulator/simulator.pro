#-------------------------------------------------
#
# Project created by QtCreator 2023-02-01T13:16:31
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = simulator
TEMPLATE = app
DESTDIR += ../../app
CONFIG += 14

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
    src/encoding/baseframe.cpp \
    src/encoding/jpegframe.cpp \
    src/graphics/figures/antisubmarinefrigate.cpp \
    src/graphics/figures/figure.cpp \
    src/graphics/figureset.cpp \
    src/graphics/program.cpp \
    src/graphics/scene.cpp \
    src/graphics/screen.cpp \
    src/graphics/shader.cpp \
    src/graphics/texture.cpp \
    src/graphics/uniformblock.cpp \
    src/server/baseprotocol.cpp \
    src/server/flvprotocol.cpp \
    src/server/s_poll.cpp \
    src/server/s_socket.cpp \
    src/main.cpp \
    src/service.cpp \
    ../share/utils.cpp

HEADERS += \
    src/encoding/baseframe.h \
    src/encoding/jpegframe.h \
    src/graphics/figures/antisubmarinefrigate.h \
    src/graphics/figures/figure.h \
    src/graphics/figureset.h \
    src/graphics/program.h \
    src/graphics/scene.h \
    src/graphics/screen.h \
    src/graphics/shader.h \
    src/graphics/texture.h \
    src/graphics/uniformblock.h \
    src/server/baseprotocol.h \
    src/server/flvprotocol.h \
    src/server/s_poll.h \
    src/server/s_socket.h \
    src/service.h \
    ../share/utils.h

LIBS += -ljpeg -lGLEW -lSOIL
