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
INCLUDEPATH += ../../include/plvopencv
LIBS += -L../../libs -lplvcore

macx {
    QMAKE_POST_LINK = install_name_tool -change libplvcore.1.dylib @executable_path/../Frameworks/libplvcore.1.dylib ../../libs/plugins/libplvopencv.dylib
}

include (../../common.pri)

#macx {
#    LITERAL_DOT=.
#    LITERAL_LIB=lib
#    LIBRARYFILE = $$DISTDIR$$LITERAL_LIB$$TARGET$$LITERAL_DOT$$QMAKE_EXTENSION_SHLIB
#    LIBS+= -L../../libs/ParleVision.app/Contents/Frameworks
#    LIBS+= -framework OpenCV
#    QMAKE_POST_LINK  = install_name_tool -change libplvcore.dylib @loader_path/../Frameworks/libplvcore.dylib $$LIBRARYFILE
#    #QMAKE_POST_LINK += && install_name_tool -change libplvgui.dylib loader_path/../Frameworks/libplvgui.dylib $$LIBRARYFILE
#}

DEFINES += PLV_OPENCV_PLUGIN_LIBRARY

SOURCES += plvopencvplugin.cpp \
    CameraProducer.cpp \
    OpenCVCamera.cpp \
    ExampleProcessor.cpp \
    EdgeDetectorLaplace.cpp \
    GaussianSmooth.cpp \
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
    ViolaJonesFaceDetector.cpp \
    SaveImageToFile.cpp \
    ImageThreshold.cpp \
    Trigger.cpp \
    PixelSum.cpp \
    CropSubImage.cpp \
    Xor.cpp \
    Split.cpp \
    Multiply.cpp \
    ImageDirectoryProducer.cpp \
    ImageProducer.cpp \
    BackgroundSubtractor.cpp \
    VideoProducer.cpp \
    DilateErode.cpp \
    Average.cpp \
    RunningAverage.cpp \
    ForegroundDetector.cpp \
    Mask.cpp \
    Stitch.cpp \
    Crop.cpp
	
HEADERS +=  plvopencvplugin.h \
            plvopencv_global.h \
            CameraProducer.h \
            OpenCVCamera.h \
            ExampleProcessor.h \
            EdgeDetectorLaplace.h \
            GaussianSmooth.h \
            EdgeDetectorSobel.h \
            ImageFlip.h \
            ImageCornerHarris.h \
            ImageColorConvert.h \
            EdgeDetectorCanny.h \
            Add.h \
            Sub.h \
            Snapshot.h \
            Diff.h \
            DelayImage.h \
            ViolaJonesFaceDetector.h  \
            SaveImageToFile.h  \
            ImageThreshold.h \
            Trigger.h \
            PixelSum.h \
            CropSubImage.h \
            Xor.h \
            Split.h \
            Multiply.h \
            ImageDirectoryProducer.h \
            ImageProducer.h \
            BackgroundSubtractor.h \
            VideoProducer.h \
            DilateErode.h \
            Average.h \
            RunningAverage.h \
            ForegroundDetector.h \
            Mask.h \
            Stitch.h \
            Crop.h
    
