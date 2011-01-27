TARGET = plvgui
TEMPLATE = lib
CONFIG += dll

DESTDIR= ../../libs/

DEPENDPATH += . \
              ..
              
include(../../ParleVision.local)
macx { 

    
    # Make sure there is no mess in ./
    # but put all output files in build/(debug|release)
#    !debug_and_release|build_pass { 
#        CONFIG(debug, debug|release):BUILDDIR = build/debug
#        CONFIG(release, debug|release):BUILDDIR = build/release
#    }
#    MOC_DIR = $$BUILDDIR
#    OBJECTS_DIR = $$BUILDDIR
#    RCC_DIR = $$BUILDDIR
#    UI_DIR = $$BUILDDIR
#    DESTDIR = $$BUILDDIR
#    QMAKE_POST_LINK=cp -f ../libs/*.dylib ../libs/parlevision.app/Contents/MacOS/
}

LIBS += -L../../libs -lplvcore

CONFIG(debug, debug|release):DEFINES += DEBUG
QT      += xml
DEFINES += PLVGUI_LIBRARY

#QMAKE_CXXFLAGS_DEBUG += -pedantic \
# -Wunused-parameter \
# -Wunused-variable
INCLUDEPATH +=  ../../include/ ../../include/plvgui
                
SOURCES += \
    MainWindow.cpp \
    OpenCVImageRenderer.cpp \
    RendererFactory.cpp \
    DataRenderer.cpp \
    UnknownDataRenderer.cpp \
    RectangleDataRenderer.cpp \
    ImageConverter.cpp \
    LibraryWidget.cpp \
    InspectorWidget.cpp \
    PipelineScene.cpp \
    PipelineElementWidget.cpp \
    LibraryElement.cpp \
    ConnectionLine.cpp \
    PinWidget.cpp \
    PinClickedEvent.cpp \
    InteractiveLine.cpp \
    ViewerWidget.cpp \
    ElementConfigFormBuilder.cpp \
    ImageWidget.cpp \
    utils.cpp

HEADERS += \
    ../../include/plvgui/utils.h \
    ../../include/plvgui/MainWindow.h \
    ../../include/plvgui/OpenCVImageRenderer.h \
    ../../include/plvgui/RendererFactory.h \
    ../../include/plvgui/DataRenderer.h \
    ../../include/plvgui/UnknownDataRenderer.h \
    ../../include/plvgui/RectangleDataRenderer.h \
    ../../include/plvgui/ImageConverter.h \
    ../../include/plvgui/PipelineScene.h \
    ../../include/plvgui/LibraryWidget.h \
    ../../include/plvgui/InspectorWidget.h \
    ../../include/plvgui/PipelineElementWidget.h \
    ../../include/plvgui/LibraryElement.h \
    ../../include/plvgui/ConnectionLine.h \
    ../../include/plvgui/PinWidget.h \
    ../../include/plvgui/PinClickedEvent.h \
    ../../include/plvgui/InteractiveLine.h \
    ../../include/plvgui/ViewerWidget.h \
    ../../include/plvgui/ElementConfigFormBuilder.h \
    ../../include/plvgui/ImageWidget.h \
    ../../include/plvgui/Exceptions.h \
    ../../include/plvgui/plvgui_global.h

FORMS += ../../resources/mainwindow.ui \
    ../../resources/librarywidget.ui \
    ../../resources/inspectorwidget.ui \
    ../../resources/viewerwidget.ui \
    ../../resources/cameraconfigform.ui \
    ../../resources/welcome.ui

RESOURCES += ../../resources/icons.qrc \
            ../../resources/images.qrc
