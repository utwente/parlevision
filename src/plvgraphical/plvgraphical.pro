TARGET = ParleVision
TEMPLATE = app

DESTDIR= ../../libs/

DEPENDPATH += . \
              ..
include (../../common.pri)
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

PRE_TARGETDEPS += ../../libs/libplvcore.dylib ../../libs/libplvgui.dylib

MACX_APP_BUNDLE_ROOT = ../../libs/ParleVision.app
QMAKE_POST_LINK  = mkdir -p $$MACX_APP_BUNDLE_ROOT/Contents/Frameworks
QMAKE_POST_LINK += `date` >> $$MACX_APP_BUNDLE_ROOT/timestamp
QMAKE_POST_LINK += && cp -f ../../libs/*.dylib $$MACX_APP_BUNDLE_ROOT/Contents/Frameworks
QMAKE_POST_LINK += && install_name_tool -id @executable_path/../Frameworks/libplvcore.dylib $$MACX_APP_BUNDLE_ROOT/Contents/Frameworks/libplvcore.dylib
QMAKE_POST_LINK += && install_name_tool -id @executable_path/../Frameworks/libplvcore.1.dylib $$MACX_APP_BUNDLE_ROOT/Contents/Frameworks/libplvcore.1.dylib
QMAKE_POST_LINK += && install_name_tool -id @executable_path/../Frameworks/libplvgui.dylib $$MACX_APP_BUNDLE_ROOT/Contents/Frameworks/libplvgui.dylib
QMAKE_POST_LINK += && install_name_tool -id @executable_path/../Frameworks/libplvgui.1.dylib $$MACX_APP_BUNDLE_ROOT/Contents/Frameworks/libplvgui.1.dylib
QMAKE_POST_LINK += && install_name_tool -change libplvcore.1.dylib @executable_path/../Frameworks/libplvcore.1.dylib $$MACX_APP_BUNDLE_ROOT/Contents/MacOS/ParleVision
QMAKE_POST_LINK += && install_name_tool -change libplvcore.1.dylib @executable_path/../Frameworks/libplvcore.1.dylib $$MACX_APP_BUNDLE_ROOT/Contents/Frameworks/libplvgui.1.dylib
QMAKE_POST_LINK += && install_name_tool -change libplvgui.1.dylib @executable_path/../Frameworks/libplvgui.1.dylib $$MACX_APP_BUNDLE_ROOT/Contents/MacOS/ParleVision
}

LIBS += -L../../libs -L../../libs/plugins -lplvcore -lplvgui -lplvcore

CONFIG(debug, debug|release):DEFINES += DEBUG
QT      += xml

QMAKE_INFO_PLIST = ../resources/Info.plist

#QMAKE_CXXFLAGS_DEBUG += -pedantic \
# -Wunused-parameter \
# -Wunused-variable
INCLUDEPATH +=  ../../include \
                ../../include/plvcore \
                ../../include/plvgui
                
SOURCES += main.cpp \
    openeventhandler.cpp

HEADERS += \
    openeventhandler.h
