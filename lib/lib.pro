TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR = ../build/lib
DESTDIR += $$OBJECTS_DIR/lib
CONFIG += 14

HEADERS = \
    ../utils/utils.h \
    include/screen.h \
    include/tcpsock.h \
    include/vdev.h \
    include/loop.h \
    include/connection.h \
    include/service.h \
    include/frame.h \
    include/jpeg.h \
    include/proto.h \
    include/flv.h \
    include/mjpeg.h \
    include/api.h


SOURCES = \
        ../utils/utils.cpp \
        base/screen.cpp \
        service/tcpsock.cpp \
        service/vdev.cpp \
        service/connection.cpp \
        service/loop.cpp \
        service/service.cpp \
#        encoding/baseframe.cpp \
#        encoding/jpegframe.cpp \
#        protocol/baseprotocol.cpp \
#        protocol/flvprotocol.cpp \
#        protocol/mjpegprotocol.cpp \
#        protocol/httpapi.cpp

QMAKE_EXTRA_TARGETS += all buildpackage
buildpackage.commands = @mkdir -p $$OBJECTS_DIR/libkformat-dev/DEBIAN; \
                         mkdir -p $$OBJECTS_DIR/libkformat-dev/usr/local/lib; \
                         mkdir -p $$OBJECTS_DIR/libkformat-dev/usr/local/include/kformat; \
                         cp $$OBJECTS_DIR/lib/*.a $$OBJECTS_DIR/libkformat-dev/usr/local/lib; \
                         cp -r $$PWD/include/*.h $$OBJECTS_DIR/libkformat-dev/usr/local/include/kformat; \
                         cp -r $$PWD/../utils/utils.h $$OBJECTS_DIR/libkformat-dev/usr/local/include/kformat; \
                         echo "Package: libkformat-dev" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control; \
                         echo "Version: 0.1" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control; \
                         echo "Maintainer: mkh" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control; \
                         echo "Architecture: all" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control; \
                         echo "Description: статическая библиотека функционала формирования и выдачи видеопотока" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control;\
                         dpkg-deb --build $$OBJECTS_DIR/libkformat-dev; \
                         rm -Rf $$OBJECTS_DIR/libkformat-dev;
all.depends = buildpackage

INCLUDEPATH += $$PWD/include $$PWD/../utils
LIBS = -ljpeg
