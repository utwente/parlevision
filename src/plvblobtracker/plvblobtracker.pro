TEMPLATE = lib
TARGET   = plv_blobtracker_plugin
CONFIG  += plugin
DESTDIR  = ../../libs/plugins
QT += core
QT -= gui
LIBS += -lplvcore

macx {
    QMAKE_POST_LINK = install_name_tool -change libplvcore.1.dylib @executable_path/../Frameworks/libplvcore.1.dylib $${DESTDIR}/$${TARGET}.dylib
}

include (../../common.pri)

DEFINES += PLV_BLOBTRACKER_PLUGIN_LIBRARY

SOURCES += plvblobtracker_plugin.cpp \
           BlobDetector.cpp \
           Blob.cpp \
           BlobTrack.cpp \
           BlobTracker.cpp \
           munkres.cpp \
           matrix.cpp \
    VPBlobToStringConverter.cpp

HEADERS +=  plvblobtracker_plugin.h \
            plvblobtracker_global.h \
            BlobDetector.h \
            Blob.h \
            BlobTrack.h \
            BlobTracker.h \
            munkres.h \
            matrix.h \
    VPBlobToStringConverter.h
			
