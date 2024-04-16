TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR = ../build/kformat
DESTDIR += $$OBJECTS_DIR/lib
CONFIG += 14

HEADERS = kformat.h \
    ../common/utils.h \
    include/basescreen.h \
    include/s_socket.h \
    include/videodevice.h \
    include/s_poll.h \
    include/connection.h \
    include/baseservice.h \
    include/baseframe.h \
    include/jpegframe.h \
    include/baseprotocol.h \
    include/flvprotocol.h \
    include/httpapi.h


SOURCES = \
        ../common/utils.cpp \
        screen/basescreen.cpp \
        service/s_socket.cpp \
        service/videodevice.cpp \
        service/connection.cpp \
        service/s_poll.cpp \
        service/baseservice.cpp \
        encoding/baseframe.cpp \
        encoding/jpegframe.cpp \
        protocol/baseprotocol.cpp \
        protocol/flvprotocol.cpp \
        protocol/httpapi.cpp

QMAKE_EXTRA_TARGETS += all buildpackage
buildpackage.commands = @mkdir -p $$OBJECTS_DIR/libkformat-dev/DEBIAN; \
                         mkdir -p $$OBJECTS_DIR/libkformat-dev/usr/local/lib; \
                         mkdir -p $$OBJECTS_DIR/libkformat-dev/usr/local/include/kformat; \
                         cp $$OBJECTS_DIR/lib/*.a $$OBJECTS_DIR/libkformat-dev/usr/local/lib; \
                         cp -r $$PWD/include/*.h $$OBJECTS_DIR/libkformat-dev/usr/local/include/kformat; \
                         cp -r $$PWD/../common/utils.h $$OBJECTS_DIR/libkformat-dev/usr/local/include/kformat; \
                         echo "Package: libkformat-dev" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control; \
                         echo "Version: 0.1" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control; \
                         echo "Maintainer: mkh" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control; \
                         echo "Architecture: all" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control; \
                         echo "Description: статическая библиотека функционала формирования и выдачи видеопотока" >> $$OBJECTS_DIR/libkformat-dev/DEBIAN/control;\
                         dpkg-deb --build $$OBJECTS_DIR/libkformat-dev; \
                         rm -Rf $$OBJECTS_DIR/libkformat-dev;
all.depends = buildpackage

INCLUDEPATH += $$PWD/include $$PWD/../common
LIBS = -ljpeg
