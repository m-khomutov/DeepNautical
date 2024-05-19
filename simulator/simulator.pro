#-------------------------------------------------
#
# Project created by QtCreator 2023-02-01T13:16:31
#
#-------------------------------------------------

QT       += core gui widgets opengl

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
    src/graphics/figures/sky.cpp \
    src/graphics/figures/waves.cpp \
    src/graphics/figures/surge.cpp \
    src/graphics/figures/vessel.cpp \
    src/graphics/figures/figure.cpp \
    src/graphics/specification.cpp \
    src/graphics/visitor.cpp \
    src/graphics/figureset.cpp \
    src/graphics/scene.cpp \
    src/graphics/blender.cpp \
    src/graphics/qscreen.cpp \
    src/service/qservice.cpp \
    src/main.cpp

HEADERS += \
    src/graphics/include/figures/sky.h \
    src/graphics/include/figures/waves.h \
    src/graphics/include/figures/surge.h \
    src/graphics/include/figures/vessel.h \
    src/graphics/include/figures/figure.h \
    src/graphics/include/specification.h \
    src/graphics/include/visitor.h \
    src/graphics/include/figureset.h \
    src/graphics/include/scene.h \
    src/graphics/include/blender.h \
    src/graphics/include/qscreen.h \
    src/service/qservice.h

INCLUDEPATH += ../simulator/src/graphics/include ../common ../kformat/include
LIBS += ../build/kformat/lib/libkformat.a -ljpeg
