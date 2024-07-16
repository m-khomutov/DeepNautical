TEMPLATE = subdirs

SUBDIRS = lib
#kviewer simulator viewer videodevice

lib.files = $$PWD/lib/lib.pro
kviewer.files = $$PWD/kviewer/kviewer.pro
simulator.files = $$PWD/simulator/simulator.pro
viewer.files = $$PWD/viewer/viewer.pro
videodevice.files = $$PWD/videodevice/videodevice.pro
