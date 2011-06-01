#Include path relative to where source is located
INCLUDEPATH += ../../include

#Library path relative to where source is located
#Do NOT use trailing slashes in the libdir, this will make the linker choke
QMAKE_LIBDIR += ../../libs

#Windows specific libraries, library paths and include paths
win32 {
    ## Windows common build here
    INCLUDEPATH += c:/OpenCV-2.1.0/include

    LIBS += -Lc:/OpenCV-2.1.0/lib
    LIBS += -Lc:/Qxt/lib

    LIBS += -Lc:/OpenCV-2.1.0/bin
    LIBS += -Lc:/Qxt/bin

    CONFIG(debug, debug|release) {
        LIBS += -lcv210d \
                -lcxcore210d \
                -lcvaux210d \
                -lhighgui210d \
                -lcxts210d \
                -lml210d
    }
    CONFIG(release, debug|release) {

        LIBS += -lcv210 \
                -lcxcore210 \
                -lcvaux210 \
                -lhighgui210 \
                -lcxts210 \
                -lml210
    }

    !contains(QMAKE_HOST.arch, x86_64) {
        message("x86 build")

        ## Windows x86 (32bit) specific build here

    } else {
        message("x86_64 build")

        ## Windows x64 (64bit) specific build here

    }
}

#win32 {
#    ## Windows common build here
#    CONFIG(debug, debug|release) {
#        LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_ml
#        QMAKE_LIBDIR += c:/develop/OpenCV-2.2.0/lib
#        QMAKE_LIBDIR += c:/develop/OpenCV-2.2.0/bin
#        LIBS += -Lc:/develop/OpenCV-2.2.0/bin
#    }
#    CONFIG(release, debug|release) {
#        LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_ml
#        QMAKE_LIBDIR += c:/develop/OpenCV-2.2.0/lib
#        QMAKE_LIBDIR += c:/develop/OpenCV-2.2.0/bin
#        LIBS += -Lc:/develop/OpenCV-2.2.0/bin
#    }
#    INCLUDEPATH += c:/develop/OpenCV-2.2.0/include
#}

#Unix specific libraries
unix {
    LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_ml -lopencv_features2d -lopencv_objdetect -lopencv_calib3d -lopencv_video
    #LIBS += -lcv \
    #            -lcxcore \
    #            -lcvaux \
    #            -lhighgui \
    #            #-lcxts \
    #            -lml
}

macx {
    #QMAKE_LIBDIR += /opt/local/lib
    QMAKE_LIBDIR += /usr/local/Cellar/opencv/2.2/lib/
    #INCLUDEPATH += /opt/local/include/opencv
    INCLUDEPATH += /usr/local/Cellar/opencv/2.2/include/opencv
}
