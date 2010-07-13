DEPENDPATH += .
include(ParleVision.local)
macx { 
    LIBS += -framework \
        OpenCV
    
    # Make sure there is no mess in ./
    # but put all output files in build/(debug|release)
    !debug_and_release|build_pass { 
        CONFIG(debug, debug|release):BUILDDIR = build/debug
        CONFIG(release, debug|release):BUILDDIR = build/release
    }
    MOC_DIR = $$BUILDDIR
    OBJECTS_DIR = $$BUILDDIR
    RCC_DIR = $$BUILDDIR
    UI_DIR = $$BUILDDIR
    DESTDIR = $$BUILDDIR
}
CONFIG(debug, debug|release):DEFINES += DEBUG

# QMAKE_CXXFLAGS_DEBUG += -pedantic
# -Wunused-parameter \
# -Wunused-variable
INCLUDEPATH += include \
    include/plvgui
SOURCES += src/main.cpp \
    src/CameraProducer.cpp \
    src/Pin.cpp \
    src/PinConnection.cpp \
    src/PipelineElement.cpp \
    src/PipelineProducer.cpp \
    src/OpenCVCamera.cpp \
    src/QtImage.cpp \
    src/OpenCVImage.cpp \
    src/RefCounted.cpp \
    src/plvgui/FrameWidget.cpp \
    src/plvgui/CameraWindow.cpp \
    src/plvgui/MainWindow.cpp \
    src/DummyProcessor.cpp \
    src/PipelineProcessor.cpp \
    src/Pipeline.cpp \
    src/plvgui/OpenCVImageInspector.cpp \
    src/plvgui/InspectorFactory.cpp \
    src/plvgui/Inspector.cpp \
    src/plvgui/ImageConverter.cpp \
    src/plvgui/LibraryWidget.cpp \
    src/plvgui/PipelineScene.cpp \
    src/plvgui/PipelineElementWidget.cpp \
    src/plvgui/LibraryElement.cpp \
    src/plvgui/PipelineView.cpp \
    src/plvgui/ConnectionLine.cpp \
    src/plvgui/PinWidget.cpp
HEADERS += include/CameraProducer.h \
    include/Pin.h \
    include/PinConnection.h \
    include/PipelineElement.h \
    include/PipelineProducer.h \
    include/OpenCVCamera.h \
    include/RefPtr.h \
    include/RefCounted.h \
    include/Types.h \
    include/QtImage.h \
    include/OpenCVImage.h \
    include/plvgui/FrameWidget.h \
    include/plvgui/CameraWindow.h \
    include/plvgui/MainWindow.h \
    include/DummyProcessor.h \
    include/PipelineProcessor.h \
    include/Pipeline.h \
    include/plvgui/OpenCVImageInspector.h \
    include/plvgui/InspectorFactory.h \
    include/plvgui/Inspector.h \
    include/plvgui/ImageConverter.h \
    include/plvgui/PipelineScene.h \
    include/plvgui/LibraryWidget.h \
    include/plvgui/PipelineElementWidget.h \
    include/PlvExceptions.h \
    include/plvgui/LibraryElement.h \
    include/plvgui/PipelineView.h \
    include/plvgui/ConnectionLine.h \
    include/plvgui/PinWidget.h \
    include/plvgui/PinClickedEvent.h
FORMS += resources/mainwindow.ui \
    resources/librarywidget.ui
RESOURCES += resources/icons.qrc
