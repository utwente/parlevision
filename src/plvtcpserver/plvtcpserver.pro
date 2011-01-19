TEMPLATE = lib
TARGET = tcp_server_plugin
CONFIG += plugin
QT += core network
QT -= gui
DESTDIR = ../../libs/plugins
INCLUDEPATH += ../../include/plvopencv
LIBS += -lplvcore
QMAKE_LIBDIR += ../../libs/plugins

include(../../ParleVision.local)

DEFINES += TCP_SERVER_PLUGIN_LIBRARY

SOURCES += tcpserverplugin.cpp \
            TCPServerProcessor.cpp \
            Server.cpp \
            ServerConnection.cpp \
            TCPClientProducer.cpp

HEADERS +=  tcpserverplugin.h \
            tcpserver_global.h \
            TCPServerProcessor.h \
            Server.h \
            ServerConnection.h \
            TCPClientProducer.h \
    Proto.h
