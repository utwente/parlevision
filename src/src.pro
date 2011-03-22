#TODO conditional compile directives in this file

TEMPLATE = subdirs

SRC_SUBDIRS += plvcore
SRC_SUBDIRS += plvgui
SRC_SUBDIRS += plvopencv
SRC_SUBDIRS += plvpluginexample
SRC_SUBDIRS += plvgraphical
SRC_SUBDIRS += plvconsole
SRC_SUBDIRS += plvtcpserver
SRC_SUBDIRS += plvtest

SUBDIRS += $$SRC_SUBDIRS

