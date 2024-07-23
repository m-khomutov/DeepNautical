TEMPLATE = subdirs

SUBDIRS = lib visual
#kviewer viewer videodevice

lib.files = $$PWD/lib/lib.pro
visual.files = $$PWD/visual/visual.pro
kviewer.files = $$PWD/kviewer/kviewer.pro
viewer.files = $$PWD/viewer/viewer.pro
videodevice.files = $$PWD/videodevice/videodevice.pro
