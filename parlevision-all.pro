# Main build file.
# Builds all libraries and applications.
TARGET = parlevision-all
TEMPLATE = subdirs
SUBDIRS =   src/plvcore \
            src/plvopencv \
            src/plvconsole \
            src/plvgui \
            src/plvgraphical


CONFIG += ordered
DESTDIR = dist

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
}
