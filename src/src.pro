#TODO conditional compile directives in this file

TEMPLATE = subdirs

SRC_SUBDIRS += plvcore
SRC_SUBDIRS += plvgui
SRC_SUBDIRS += plvopencv
SRC_SUBDIRS += plvpluginexample
SRC_SUBDIRS += plvgraphical
SRC_SUBDIRS += plvconsole

SUBDIRS += $$SRC_SUBDIRS

