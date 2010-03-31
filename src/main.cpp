#include "QOpenCVWidget.h"
#include "CameraWindow.h"
#include "OpenCVCamera.h"

#include <QApplication>

int main(int argc, char **argv)
{
    OpenCVCamera* camera = new OpenCVCamera( 0 );
    assert(camera);
    if( camera->init() != 0 )
        return -1;

    QApplication app(argc, argv);
    CameraWindow* mainWin = new CameraWindow(camera);

    mainWin->setWindowTitle("OpenCV --> QtImage");
    mainWin->show();

    camera->start();

    int retval = app.exec();

    // wait for camera to finish thread execution
    camera->exit();
    camera->wait();
    return retval;
}

