TARGET = kvideocapture

TEMPLATE = app
OBJECTS_DIR = ../build/$$TARGET
DESTDIR += $$OBJECTS_DIR/app
CONFIG += 14

SOURCES += \
    src/service.cpp \
    src/main.cpp

HEADERS += \
    src/service.h

QMAKE_EXTRA_TARGETS += all buildpackage
buildpackage.commands = @mkdir -p $$OBJECTS_DIR/$$TARGET/DEBIAN; \
                         mkdir -p $$OBJECTS_DIR/$$TARGET/usr/sbin; \
                         mkdir -p $$OBJECTS_DIR/$$TARGET/etc/init.d; \
                         cp $$OBJECTS_DIR/app/$$TARGET $$OBJECTS_DIR/$$TARGET/usr/sbin/kvideocaptured; \
                         cp contrib/kvideocapture $$OBJECTS_DIR/$$TARGET/etc/init.d; \
                         chmod +x $$OBJECTS_DIR/$$TARGET/etc/init.d/kvideocapture; \
                         echo "Package: kvideocapture" >> $$OBJECTS_DIR/$$TARGET/DEBIAN/control; \
                         echo "Version: 0.1" >> $$OBJECTS_DIR/$$TARGET/DEBIAN/control; \
                         echo "Maintainer: mkh" >> $$OBJECTS_DIR/$$TARGET/DEBIAN/control; \
                         echo "Architecture: all" >> $$OBJECTS_DIR/$$TARGET/DEBIAN/control; \
                         echo "Description:  серверное приложение выдачи видеокадров с видеустройства в сеть" >> $$OBJECTS_DIR/$$TARGET/DEBIAN/control;\
                         echo "chmod +x /etc/init.d/kvideocapture" >> $$OBJECTS_DIR/$$TARGET/DEBIAN/postinst; \
                         echo "update-rc.d kvideocapture defaults" >> $$OBJECTS_DIR/$$TARGET/DEBIAN/postinst; \
                         echo "update-rc.d kvideocapture enable" >> $$OBJECTS_DIR/$$TARGET/DEBIAN/postinst; \
                         echo "update-rc.d kvideocapture remove" >> $$OBJECTS_DIR/$$TARGET/DEBIAN/postrm; \
                         chmod +x $$OBJECTS_DIR/$$TARGET/DEBIAN/postinst; \
                         chmod +x $$OBJECTS_DIR/$$TARGET/DEBIAN/postrm; \
                         dpkg-deb --build $$OBJECTS_DIR/$$TARGET; \
                         rm -Rf $$OBJECTS_DIR/$$TARGET;
all.depends = buildpackage

INCLUDEPATH += ../kformat/include ../common
LIBS += ../build/kformat/lib/libkformat.a -ljpeg
