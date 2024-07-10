QT += core gui widgets opengl

TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR = ../../build/kgraphics
DESTDIR += $$OBJECTS_DIR/lib
CONFIG += 14

SOURCES += \
    src/figures/air.cpp \
    src/figures/skybox.cpp \
    src/figures/waves.cpp \
    src/figures/surge.cpp \
    src/figures/vessel.cpp \
    src/figures/figure.cpp \
    src/specification.cpp \
    src/visitor.cpp \
    src/figureset.cpp \
    src/camera.cpp \
    src/scene.cpp \
    src/blender.cpp \
    src/qscreen.cpp \
    src/qservice.cpp

HEADERS += \
    src/include/figures/air.h \
    src/include/figures/skybox.h \
    src/include/figures/waves.h \
    src/include/figures/surge.h \
    src/include/figures/vessel.h \
    src/include/figures/figure.h \
    src/include/specification.h \
    src/include/visitor.h \
    src/include/figureset.h \
    src/include/camera.h \
    src/include/scene.h \
    src/include/blender.h \
    src/include/screencommand.h \
    src/include/qscreen.h \
    src/include/qservice.h

QMAKE_EXTRA_TARGETS += all buildpackage
buildpackage.commands = @mkdir -p $$OBJECTS_DIR/libkgraphics-dev/DEBIAN; \
                         mkdir -p $$OBJECTS_DIR/libkgraphics-dev/usr/local/lib; \
                         mkdir -p $$OBJECTS_DIR/libkgraphics-dev/usr/local/include/kgraphics/figures; \
                         cp $$OBJECTS_DIR/lib/*.a $$OBJECTS_DIR/libkgraphics-dev/usr/local/lib; \
                         cp -r $$PWD/src/include/*.h $$OBJECTS_DIR/libkgraphics-dev/usr/local/include/kgraphics; \
                         cp -r $$PWD/src/include/figures/*.h $$OBJECTS_DIR/libkgraphics-dev/usr/local/include/kgraphics/figures; \
                         echo "Package: libkgraphics-dev" >> $$OBJECTS_DIR/libkgraphics-dev/DEBIAN/control; \
                         echo "Version: 0.1" >> $$OBJECTS_DIR/libkgraphics-dev/DEBIAN/control; \
                         echo "Maintainer: mkh" >> $$OBJECTS_DIR/libkgraphics-dev/DEBIAN/control; \
                         echo "Architecture: all" >> $$OBJECTS_DIR/libkgraphics-dev/DEBIAN/control; \
                         echo "Description: статическая библиотека функционала графического эмулятора" >> $$OBJECTS_DIR/libkgraphics-dev/DEBIAN/control;\
                         dpkg-deb --build $$OBJECTS_DIR/libkgraphics-dev; \
                         rm -Rf $$OBJECTS_DIR/libkgraphics-dev;
all.depends = buildpackage

INCLUDEPATH += src/include ../../common ../../kformat/include
LIBS += ../build/kformat/lib/libkformat.a -ljpeg
