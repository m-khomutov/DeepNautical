TEMPLATE = subdirs

SUBDIRS = kformat kviewer simulator/kgraphics simulator viewer videodevice

kformat.files = $$PWD/kformat/kformat.pro
kviewer.files = $$PWD/kviewer/kviewer.pro
kgraphics.files = $$PWD/simulator/kgraphics/kgraphics.pro
simulator.files = $$PWD/simulator/simulator.pro
viewer.files = $$PWD/viewer/viewer.pro
videodevice.files = $$PWD/videodevice/videodevice.pro
