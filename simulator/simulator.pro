#-------------------------------------------------
#
# Project created by QtCreator 2023-02-01T13:16:31
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = simulator
TEMPLATE = app
OBJECTS_DIR = ../build/simulator
DESTDIR += $$OBJECTS_DIR/app
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
    src/graphics/figures/sol.cpp \
    src/graphics/figures/water.cpp \
    src/graphics/figures/horizon.cpp \
    src/graphics/figures/antisubmarinefrigate.cpp \
    src/graphics/figures/vessel.cpp \
    src/graphics/figures/sparklets.cpp \
    src/graphics/figures/figure.cpp \
    src/graphics/figures/specification.cpp \
    src/graphics/visitor.cpp \
    src/graphics/figureset.cpp \
    src/graphics/program.cpp \
    src/graphics/scene.cpp \
    src/graphics/blender.cpp \
    src/graphics/screens/openglscreen.cpp \
    src/graphics/screens/qscreen.cpp \
    src/graphics/shader.cpp \
    src/graphics/texture/texture.cpp \
    src/graphics/texture/avi.cpp \
    src/graphics/uniformblock.cpp \
    src/server/httpapi.cpp \
    src/main.cpp \
    src/service/qservice.cpp

HEADERS += \
    src/graphics/figures/sol.h \
    src/graphics/figures/water.h \
    src/graphics/figures/horizon.h \
    src/graphics/figures/antisubmarinefrigate.h \
    src/graphics/figures/vessel.h \
    src/graphics/figures/sparklets.h \
    src/graphics/figures/figure.h \
    src/graphics/figures/specification.h \
    src/graphics/visitor.h \
    src/graphics/figureset.h \
    src/graphics/program.h \
    src/graphics/scene.h \
    src/graphics/blender.h \
    src/graphics/screens/openglscreen.h \
    src/graphics/screens/qscreen.h \
    src/graphics/shader.h \
    src/graphics/texture/texture.h \
    src/graphics/texture/avi.h \
    src/graphics/uniformblock.h \
    src/server/httpapi.h \
    src/service/qservice.h

INCLUDEPATH += ../simolator/src ../kformat
LIBS += ../build/kformat/lib/libkformat.a -lGLEW -lGLU -ljpeg
