TEMPLATE = subdirs

SUBDIRS = kformat kviewer visual/kgraphics visual viewer videodevice

kformat.files = $$PWD/kformat/kformat.pro
kviewer.files = $$PWD/kviewer/kviewer.pro
kgraphics.files = $$PWD/visual/kgraphics/kgraphics.pro
visual.files = $$PWD/visual/visual.pro
viewer.files = $$PWD/viewer/viewer.pro
videodevice.files = $$PWD/videodevice/videodevice.pro

OBJECTS_DIR = ../build/doc

QMAKE_EXTRA_TARGETS += all builddocpackage
builddocpackage.commands = @doxygen; \
                            mkdir -p doc/visual-doc/DEBIAN; \
                            mkdir -p doc/visual-doc/usr/local/share/visual/doc; \
                            cp -r doc/html doc/visual-doc/usr/local/share/visual/doc; \
                            echo "Package: visual-doc" >> doc/visual-doc/DEBIAN/control; \
                            echo "Version: 0.1" >> doc/visual-doc/DEBIAN/control; \
                            echo "Maintainer: mkh" >> doc/visual-doc/DEBIAN/control; \
                            echo "Architecture: all" >> doc/visual-doc/DEBIAN/control; \
                            echo "Description: документация visual, сгенерированная doxygen" >> doc/visual-doc/DEBIAN/control;\
                            dpkg-deb --build doc/visual-doc; \
                            rm -Rf doc/visual-doc; \
                            rm -Rf deb; mkdir deb; \
                            mv build/kformat/libkformat-dev.deb deb; \
                            mv build/kgraphics/libkgraphics-dev.deb deb; \
                            mv build/kvideocapture/kvideocapture.deb deb; \
                            mv build/kviewer/libkviewer-dev.deb deb; \
                            mv build/visual/visual.deb deb; \
                            mv doc/visual-doc.deb deb;
all.depends = builddocpackage
