QT += core gui widgets

TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR = ../build/kviewer
DESTDIR += $$OBJECTS_DIR/lib
CONFIG += 14

HEADERS = \
    include/c_socket.h \
    include/receiver.h \
    include/basedecoder.h \
    include/jpegdecoder.h \
    include/baseviewer.h \
    include/qviewer.h \
    ../common/utils.h


SOURCES = \
    client/c_socket.cpp \
    client/receiver.cpp \
    decoder/basedecoder.cpp \
    decoder/jpegdecoder.cpp \
    viewer/baseviewer.cpp \
    viewer/qviewer.cpp \
    ../common/utils.cpp


QMAKE_EXTRA_TARGETS += all buildpackage
buildpackage.commands = @mkdir -p $$OBJECTS_DIR/libkviewer-dev/DEBIAN; \
                         mkdir -p $$OBJECTS_DIR/libkviewer-dev/usr/local/lib; \
                         mkdir -p $$OBJECTS_DIR/libkviewer-dev/usr/local/include/kviewer; \
                         cp $$OBJECTS_DIR/lib/*.a $$OBJECTS_DIR/libkviewer-dev/usr/local/lib; \
                         cp -r $$PWD/include/*.h $$OBJECTS_DIR/libkviewer-dev/usr/local/include/kviewer; \
                         cp -r $$PWD/../common/utils.h $$OBJECTS_DIR/libkviewer-dev/usr/local/include/kviewer; \
                         echo "Package: libkviewer-dev" >> $$OBJECTS_DIR/libkviewer-dev/DEBIAN/control; \
                         echo "Version: 0.1" >> $$OBJECTS_DIR/libkviewer-dev/DEBIAN/control; \
                         echo "Maintainer: mkh" >> $$OBJECTS_DIR/libkviewer-dev/DEBIAN/control; \
                         echo "Architecture: all" >> $$OBJECTS_DIR/libkviewer-dev/DEBIAN/control; \
                         echo "Description: статическая библиотека функционала просмотра видеопотока" >> $$OBJECTS_DIR/libkviewer-dev/DEBIAN/control;\
                         dpkg-deb --build $$OBJECTS_DIR/libkviewer-dev; \
                         rm -Rf $$OBJECTS_DIR/libkviewer-dev;
all.depends = buildpackage

INCLUDEPATH += $$PWD/include $$PWD/../common
LIBS = -ljpeg
