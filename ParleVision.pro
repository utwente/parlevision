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
macx {
    LIBS += -framework OpenCV

    # Make sure there is no mess in ./
    # but put all output files in build/(debug|release)
    !debug_and_release|build_pass {
        CONFIG(debug, debug|release) {
            BUILDDIR = build/debug
        }
       CONFIG(release, debug|release) {
            BUILDDIR = build/release
        }
    }

    MOC_DIR     = $$BUILDDIR
    OBJECTS_DIR = $$BUILDDIR
    RCC_DIR     = $$BUILDDIR
    UI_DIR      = $$BUILDDIR
    DESTDIR     = $$BUILDDIR

}
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
    src/plvgui/MainWindow.cpp
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
    include/plvgui/MainWindow.h
FORMS += resources/mainwindow.ui
RESOURCES += resources/icons.qrc
