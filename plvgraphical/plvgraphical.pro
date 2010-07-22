TARGET = ParleVision
TEMPLATE = app

DESTDIR= ../libs/

DEPENDPATH += . \
              ..
include (../common.pri)
include(../ParleVision.local)
macx { 
    LIBS += -framework OpenCV
    
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

#   QMAKE_POST_LINK=cp -f ../libs/*.dylib ../libs/parlevision.app/Contents/MacOS/
}

LIBS += -L../libs -lplvcore -lplvgui

CONFIG(debug, debug|release):DEFINES += DEBUG
QT      += xml

#QMAKE_CXXFLAGS_DEBUG += -pedantic \
# -Wunused-parameter \
# -Wunused-variable
INCLUDEPATH +=  ../include/plvcore \
                ../include/plvgui
                
SOURCES += src/main.cpp
