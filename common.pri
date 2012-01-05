# using parlevision as a shared library
DEFINES += PLV_SHARED_LIBRARY

OPENCV_PATH = c:/develop/OpenCV-2.3.1

#Include path relative to where source is located
INCLUDEPATH += ../../include
INCLUDEPATH += $${OPENCV_PATH}/build/include

#Library path relative to where source is located
#Do NOT use trailing slashes in the libdir, this will make the linker choke
QMAKE_LIBDIR += ../../libs

#Windows specific libraries, library paths and include paths
win32-g++ {
    ## Windows common build for mingw here

    CONFIG(debug, debug|release) {
        LIBS += -L$${OPENCV_PATH}/build/x86/mingw/debug/lib
        LIBS += -L$${OPENCV_PATH}/build/x86/mingw/debug/bin

        # prefix and postfix are applied to all library files
        # e.g.  LIBPREFIX = libopencv_
        #       LIBPOSTFIX = d
        # library calib3d231 becomes libopencv_calib3d231d
        # OpenCV-2.3.1 is a bit inconsistent in this regard
        # libpostfix should be empty when using the prebuild debug libraries
        # but should contain 'd' when you build OpenCV libs yourself.
        LIBPREFIX = libopencv_
        LIBPOSTFIX =
    }

    CONFIG(release, debug|release) {
        LIBS += -L$${OPENCV_PATH}/build/x86/mingw/release/lib
        LIBS += -L$${OPENCV_PATH}/build/x86/mingw/release/bin

        LIBPREFIX = libopencv_
        LIBPOSTFIX =
    }
}

win32-msvc2010 {
    CONFIG(debug, debug|release) {
        LIBPREFIX = opencv_
        LIBPOSTFIX = d
    }

    CONFIG(release, debug|release) {
        LIBPREFIX = opencv_
        LIBPOSTFIX =
    }

    LIBS += -L$${OPENCV_PATH}/build/x86/vc10/lib
    LIBS += -L$${OPENCV_PATH}/build/x86/vc10/bin
}

win32 {
    LIBS += -l$${LIBPREFIX}calib3d231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}contrib231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}core231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}features2d231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}flann231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}gpu231$${LIBPOSTFIX} \
#            -l$${LIBPREFIX}haartraining_engine$${LIBPOSTFIX} \
            -l$${LIBPREFIX}highgui231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}imgproc231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}legacy231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}ml231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}objdetect231$${LIBPOSTFIX} \
#            -l$${LIBPREFIX}ts231$${LIBPOSTFIX} \
            -l$${LIBPREFIX}video231$${LIBPOSTFIX}


    !contains(QMAKE_HOST.arch, x86_64) {
        message("x86 build")
        ## Windows x86 (32bit) specific build here
    } else {
        message("x86_64 build")
        ## Windows x64 (64bit) specific build here
    }
}

#Unix specific libraries
unix {
    #LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect  \
    #        -lopencv_ml -lopencv_features2d -lopencv_objdetect -lopencv_calib3d -lopencv_video
    #LIBS += -lcv \
    #            -lcxcore \
    #            -lcvaux \
    #            -lhighgui \
    #            #-lcxts \
    #            -lml
}

macx {
    #QMAKE_LIBDIR += /opt/local/lib
    #QMAKE_LIBDIR += /usr/local/Cellar/opencv/2.2/lib/
    #INCLUDEPATH += /opt/local/include/opencv
    #INCLUDEPATH += /usr/local/Cellar/opencv/2.2/include/opencv
}
