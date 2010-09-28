#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T14:39:10
#
#-------------------------------------------------

TEMPLATE = lib
TARGET = plvopencv
CONFIG += plugin
QT += core
QT -= gui
DESTDIR = ../../libs/plugins

LIBS += -lplvcore
#macx {
#    LITERAL_DOT=.
#    LITERAL_LIB=lib
#    LIBRARYFILE = $$DISTDIR$$LITERAL_LIB$$TARGET$$LITERAL_DOT$$QMAKE_EXTENSION_SHLIB
#    LIBS+= -L../../libs/ParleVision.app/Contents/Frameworks
#    LIBS+= -framework OpenCV
#    QMAKE_POST_LINK  = install_name_tool -change libplvcore.dylib @loader_path/../Frameworks/libplvcore.dylib $$LIBRARYFILE
#    #QMAKE_POST_LINK += && install_name_tool -change libplvgui.dylib loader_path/../Frameworks/libplvgui.dylib $$LIBRARYFILE
#}
win32 {
    LIBS += -lcv200d \
        -lcxcore200d \
        -lcvaux200d \
        -lhighgui200d \
        -lcxts200d \
        -lml200d
    #do NOT use trailing slashes in the libdir, this will make the linker choke
    QMAKE_LIBDIR += c:/OpenCV2.0/lib/Debug ../../libs
    INCLUDEPATH += c:/OpenCV2.0/include
}

INCLUDEPATH += ../../include ../../include/plvopencv

DEFINES += PLV_OPENCV_PLUGIN_LIBRARY

SOURCES += plvopencvplugin.cpp \
    CameraProducer.cpp \
    OpenCVCamera.cpp \
    OpenCVImage.cpp \
    DummyProcessor.cpp \
    EdgeDetectorLaplace.cpp \
    ImageSmooth.cpp \
    EdgeDetectorSobel.cpp \
    ImageFlip.cpp \
    ImageCornerHarris.cpp \
    ImageColorConvert.cpp \
    EdgeDetectorCanny.cpp \
    Add.cpp \
    Sub.cpp \
    Snapshot.cpp \
    Diff.cpp \
    DelayImage.cpp \
    ViolaJonesFaceDetector.cpp
	
HEADERS += ../../include/plvopencv/plvopencvplugin.h \
	../../include/plvopencv/plvopencv_global.h \
    ../../include/plvopencv/CameraProducer.h \
    ../../include/plvopencv/OpenCVCamera.h \
    ../../include/plvopencv/OpenCVImage.h \
    ../../include/plvopencv/DummyProcessor.h \
    ../../include/plvopencv/EdgeDetectorLaplace.h \
    ../../include/plvopencv/ImageSmooth.h \
    ../../include/plvopencv/EdgeDetectorSobel.h \
    ../../include/plvopencv/ImageFlip.h \
    ../../include/plvopencv/ImageCornerHarris.h \
    ../../include/plvopencv/ImageColorConvert.h \
    ../../include/plvopencv/EdgeDetectorCanny.h \
    ../../include/plvopencv/Add.h \
    ../../include/plvopencv/Sub.h \
    ../../include/plvopencv/Snapshot.h \
    ../../include/plvopencv/Diff.h \
    ../../include/plvopencv/DelayImage.h \
    ../../include/plvopencv/ViolaJonesFaceDetector.h        
    
