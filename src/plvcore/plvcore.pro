TARGET = plvcore
TEMPLATE = lib
CONFIG += dll

DEPENDPATH +=   .\
                ..

include(../../ParleVision.local)
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

DESTDIR = ../../libs

CONFIG(debug, debug|release):DEFINES += DEBUG

QT += core
QT -= gui
QT += xml

#QMAKE_CXXFLAGS_DEBUG += -pedantic \
# -Wunused-parameter \
# -Wunused-variable

INCLUDEPATH += ../../include/plvcore

DEFINES += PLV_DLL PLV_DLL_EXPORTS

SOURCES += \
    Types.cpp \
    Application.cpp \
    Pin.cpp \
    PinConnection.cpp \
    PipelineElement.cpp \
    PipelineProducer.cpp \
    RefCounted.cpp \
    PipelineProcessor.cpp \
    Pipeline.cpp \
    PipelineLoader.cpp \
    Scheduler.cpp \
    Plugin.cpp \
    OpenCVImage.cpp \
    CvMatData.cpp \
    OpenCVImagePin.cpp

HEADERS += ../../include/plvcore/plvglobal.h \
    ../../include/plvcore/Application.h \
    ../../include/plvcore/Plugin.h \
    ../../include/plvcore/Pin.h \
    ../../include/plvcore/PinConnection.h \
    ../../include/plvcore/PipelineElement.h \
    ../../include/plvcore/PipelineProducer.h \
    ../../include/plvcore/RefPtr.h \
    ../../include/plvcore/RefCounted.h \
    ../../include/plvcore/Types.h \
    ../../include/plvcore/PipelineProcessor.h \
    ../../include/plvcore/Pipeline.h \
    ../../include/plvcore/PlvExceptions.h \
    ../../include/plvcore/PipelineLoader.h \
    ../../include/plvcore/Scheduler.h \
    ../../include/plvcore/OpenCVImage.h \
    ../../include/plvcore/CvMatData.h \
    ../../include/plvcore/OpenCVImagePin.h
