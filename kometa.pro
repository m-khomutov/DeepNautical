TEMPLATE = subdirs

SUBDIRS = kformat kviewer simulator/kgraphics simulator viewer videodevice

kformat.files = $$PWD/kformat/kformat.pro
kviewer.files = $$PWD/kviewer/kviewer.pro
kgraphics.files = $$PWD/simulator/kgraphics/kgraphics.pro
simulator.files = $$PWD/simulator/simulator.pro
viewer.files = $$PWD/viewer/viewer.pro
videodevice.files = $$PWD/videodevice/videodevice.pro

OBJECTS_DIR = ../build/doc

QMAKE_EXTRA_TARGETS += all builddocpackage
builddocpackage.commands = @doxygen; \
                            mkdir -p doc/libksimulator-doc/DEBIAN; \
                            mkdir -p doc/libksimulator-doc/usr/local/share/ksimulator/doc; \
                            cp -r doc/html doc/libksimulator-doc/usr/local/share/ksimulator/doc; \
                            echo "Package: libksimulator-doc" >> doc/libksimulator-doc/DEBIAN/control; \
                            echo "Version: 0.1" >> doc/libksimulator-doc/DEBIAN/control; \
                            echo "Maintainer: mkh" >> doc/libksimulator-doc/DEBIAN/control; \
                            echo "Architecture: all" >> doc/libksimulator-doc/DEBIAN/control; \
                            echo "Description: документация графического эмулятора, сгенерированная doxygen" >> doc/libksimulator-doc/DEBIAN/control;\
                            dpkg-deb --build doc/libksimulator-doc; \
                            rm -Rf doc/libksimulator-doc; \
                            rm -Rf deb; mkdir deb; \
                            mv build/kformat/libkformat-dev.deb deb; \
                            mv build/kgraphics/libkgraphics-dev.deb deb; \
                            mv build/kvideocapture/kvideocapture.deb deb; \
                            mv build/kviewer/libkviewer-dev.deb deb; \
                            mv build/simulator/ksimulator.deb deb; \
                            mv doc/libksimulator-doc.deb deb;
all.depends = builddocpackage
