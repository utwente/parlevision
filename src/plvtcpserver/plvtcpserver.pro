TEMPLATE = lib
TARGET = tcp_server_plugin
CONFIG += plugin
QT += core gui network
DESTDIR = ../../libs/plugins
INCLUDEPATH += ../../include/plvopencv
LIBS += -lplvcore -lplvgui
QMAKE_LIBDIR += ../../libs/plugins

macx {
    QMAKE_POST_LINK = install_name_tool -change libplvcore.1.dylib @executable_path/../Frameworks/libplvcore.1.dylib $${DESTDIR}/$${TARGET}.dylib
}

include(../../common.pri)

DEFINES += TCP_SERVER_PLUGIN_LIBRARY

SOURCES +=  tcpserverplugin.cpp \
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
