TARGET = plvcore
TEMPLATE = lib
CONFIG += plugin

DEPENDPATH +=   .\
                ..

include(../ParleVision.local)
macx { 
    LIBS += -framework OpenCV
    
    # Make sure there is no mess in ./
    # but put all output files in build/(debug|release)
#    !debug_and_release|build_pass {
#        CONFIG(debug, debug|release):BUILDDIR = test/build/debug
#        CONFIG(release, debug|release):BUILDDIR = build/release
#    }
#    MOC_DIR = $$BUILDDIR
#    OBJECTS_DIR = $$BUILDDIR
#    RCC_DIR = $$BUILDDIR
#    UI_DIR = $$BUILDDIR
#    DESTDIR = $$BUILDDIR
}

DESTDIR = ../libs

CONFIG(debug, debug|release):DEFINES += DEBUG

QT += core
QT -= gui
QT += xml

#QMAKE_CXXFLAGS_DEBUG += -pedantic \
# -Wunused-parameter \
# -Wunused-variable

INCLUDEPATH += ../include/plvcore

DEFINES += PLVCORE_LIBRARY

SOURCES += \
    src/Application.cpp \
    src/CameraProducer.cpp \
    src/Pin.cpp \
    src/PinConnection.cpp \
    src/PipelineElement.cpp \
    src/PipelineProducer.cpp \
    src/OpenCVCamera.cpp \
    src/OpenCVImage.cpp \
    src/RefCounted.cpp \
    src/DummyProcessor.cpp \
    src/PipelineProcessor.cpp \
    src/Pipeline.cpp \
    src/PipelineLoader.cpp \
    src/Scheduler.cpp \
    src/EdgeDetector.cpp

HEADERS += ../include/plvcore/plvcore_global.h \
    ../include/plvcore/Application.h \
    ../include/plvcore/Plugin.h \
    ../include/plvcore/CameraProducer.h \
    ../include/plvcore/Pin.h \
    ../include/plvcore/PinConnection.h \
    ../include/plvcore/PipelineElement.h \
    ../include/plvcore/PipelineProducer.h \
    ../include/plvcore/OpenCVCamera.h \
    ../include/plvcore/RefPtr.h \
    ../include/plvcore/RefCounted.h \
    ../include/plvcore/Types.h \
    ../include/plvcore/OpenCVImage.h \
    ../include/plvcore/DummyProcessor.h \
    ../include/plvcore/PipelineProcessor.h \
    ../include/plvcore/Pipeline.h \
    ../include/plvcore/PlvExceptions.h \
    ../include/plvcore/PipelineLoader.h \
    ../include/plvcore/Scheduler.h \
    ../include/plvcore/EdgeDetector.h
