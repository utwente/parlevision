# Main build file.
# Builds all libraries and applications.
TARGET = parlevision-all
TEMPLATE = subdirs
SUBDIRS =   plvcore plvgui \            # libraries
            plvheadless plvgraphical    # applications
