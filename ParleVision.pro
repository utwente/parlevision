DEPENDPATH += .
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
macx:LIBS += -framework \
    OpenCV
INCLUDEPATH += include
SOURCES += src/main.cpp \
    src/CameraProducer.cpp \
    src/DummyProcessor.cpp \
    src/Pin.cpp \
    src/PinConnection.cpp \
    src/PipelineElement.cpp \
    src/PipelineProcessor.cpp \
    src/PipelineProducer.cpp \
    src/QOpenCVWidget.cpp \
    src/CameraWindow.cpp \
    src/OpenCVCamera.cpp \
    src/QtImage.cpp \
    src/OpenCVImage.cpp \
    src/RefCounted.cpp
HEADERS += include/CameraProducer.h \
    include/DummyProcessor.h \
    include/Pin.h \
    include/PinConnection.h \
    include/PipelineElement.h \
    include/PipelineProcessor.h \
    include/PipelineProducer.h \
    include/QOpenCVWidget.h \
    include/CameraWindow.h \
    include/OpenCVCamera.h \
    include/RefPtr.h \
    include/RefCounted.h \
    include/Types.h \
    include/QtImage.h \
    include/OpenCVImage.h
