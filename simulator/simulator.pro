QT += core gui widgets opengl

TARGET = simulator
TEMPLATE = app
OBJECTS_DIR = ../build/simulator
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


SOURCES += main.cpp

INCLUDEPATH += ../common ../kformat/include kgraphics/src/include
LIBS += ../build/kformat/lib/libkformat.a ../build/kgraphics/lib/libkgraphics.a -ljpeg

QMAKE_EXTRA_TARGETS += all buildpackage
buildpackage.commands = @mkdir -p $$OBJECTS_DIR/ksimulator/DEBIAN; \
                         mkdir -p $$OBJECTS_DIR/ksimulator/usr/local/bin; \
                         cp $$OBJECTS_DIR/app/simulator $$OBJECTS_DIR/ksimulator/usr/local/bin/ksimulator; \
                         echo "Package: ksimulator" >> $$OBJECTS_DIR/ksimulator/DEBIAN/control; \
                         echo "Version: 0.1" >> $$OBJECTS_DIR/ksimulator/DEBIAN/control; \
                         echo "Maintainer: mkh" >> $$OBJECTS_DIR/ksimulator/DEBIAN/control; \
                         echo "Architecture: all" >> $$OBJECTS_DIR/ksimulator/DEBIAN/control; \
                         echo "Description: графический эмулятор" >> $$OBJECTS_DIR/ksimulator/DEBIAN/control;\
                         dpkg-deb --build $$OBJECTS_DIR/ksimulator; \
                         rm -Rf $$OBJECTS_DIR/ksimulator;
all.depends = buildpackage
