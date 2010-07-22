TARGET = plvcore
TEMPLATE = lib
CONFIG += plugin

DEPENDPATH += .
include(ParleVision.local)
macx { 
    LIBS += -framework OpenCV
    
    # Make sure there is no mess in ./
    # but put all output files in build/(debug|release)
    !debug_and_release|build_pass { 
        CONFIG(debug, debug|release):BUILDDIR = test/build/debug
        CONFIG(release, debug|release):BUILDDIR = build/release
    }
    MOC_DIR = $$BUILDDIR
    OBJECTS_DIR = $$BUILDDIR
    RCC_DIR = $$BUILDDIR
    UI_DIR = $$BUILDDIR
    DESTDIR = $$BUILDDIR
}
CONFIG(debug, debug|release):DEFINES += DEBUG
QT      += xml

#QMAKE_CXXFLAGS_DEBUG += -pedantic \
# -Wunused-parameter \
# -Wunused-variable

INCLUDEPATH += include/plvcore

SOURCES += \
    src/plvcore/CameraProducer.cpp \
    src/plvcore/Pin.cpp \
    src/plvcore/PinConnection.cpp \
    src/plvcore/PipelineElement.cpp \
    src/plvcore/PipelineProducer.cpp \
    src/plvcore/OpenCVCamera.cpp \
    src/plvcore/QtImage.cpp \
    src/plvcore/OpenCVImage.cpp \
    src/plvcore/RefCounted.cpp \
    src/plvcore/DummyProcessor.cpp \
    src/plvcore/PipelineProcessor.cpp \
    src/plvcore/Pipeline.cpp \
    src/plvcore/PipelineLoader.cpp \

HEADERS += include/plvcore/Plugin.h \
    include/plvcore/CameraProducer.h \
    include/plvcore/Pin.h \
    include/plvcore/PinConnection.h \
    include/plvcore/PipelineElement.h \
    include/plvcore/PipelineProducer.h \
    include/plvcore/OpenCVCamera.h \
    include/plvcore/RefPtr.h \
    include/plvcore/RefCounted.h \
    include/plvcore/Types.h \
    include/plvcore/QtImage.h \
    include/plvcore/OpenCVImage.h \
    include/plvcore/DummyProcessor.h \
    include/plvcore/PipelineProcessor.h \
    include/plvcore/Pipeline.h \
    include/plvcore/PlvExceptions.h \
    include/plvcore/PipelineLoader.h \
