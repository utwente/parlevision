#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T16:55:23
#
#-------------------------------------------------

QT       += core
QT       += xml
QT       -= gui

DEPENDPATH +=   . \
                ..

INCLUDEPATH += ../include
LIBS += -L../libs/ -lplvcore

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


TARGET = plvheadless
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
