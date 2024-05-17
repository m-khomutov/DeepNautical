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
    src/graphics/screens/glscreen.cpp \
    src/graphics/screens/qscreen.cpp \
    src/service/qservice.cpp \
    src/main.cpp

HEADERS += \
    src/graphics/figures/sky.h \
    src/graphics/figures/waves.h \
    src/graphics/figures/surge.h \
    src/graphics/figures/vessel.h \
    src/graphics/figures/figure.h \
    src/graphics/specification.h \
    src/graphics/visitor.h \
    src/graphics/figureset.h \
    src/graphics/scene.h \
    src/graphics/blender.h \
    src/graphics/screens/glscreen.h \
    src/graphics/screens/qscreen.h \
    src/service/qservice.h

INCLUDEPATH += ../simulator/src ../common ../kformat/include
LIBS += ../build/kformat/lib/libkformat.a -ljpeg
