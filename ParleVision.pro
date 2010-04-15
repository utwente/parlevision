DEPENDPATH += .

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        BUILD_DIR = build/debug
    }
   CONFIG(release, debug|release) {
        BUILD_DIR = build/release
    }
}

MOC_DIR     = $$BUILD_DIR
OBJECTS_DIR = $$BUILD_DIR
RCC_DIR     = $$BUILD_DIR
UI_DIR      = $$BUILD_DIR
DESTDIR     = $$BUILD_DIR

win32 { 
    LIBS += -lcv200 \
        -lcxcore200 \
        -lcvaux200 \
        -lhighgui200 \
        -lcxts200 \
        -lml200
    LIBPATH += c:\opencv2.0\lib
    INCLUDEPATH += c:\OpenCV2.0\include
}
macx:LIBS += -framework OpenCV
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
    src/Pipeline.cpp
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
    include/Pipeline.h
FORMS += resources/mainwindow.ui
RESOURCES += resources/icons.qrc
