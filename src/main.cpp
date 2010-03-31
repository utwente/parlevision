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
    sleep(1000);
    camera->stop();
    sleep(1000);
    camera->start();


    int retval = app.exec();

    // Terminate camera thread
    camera->stop();
    camera->wait(1000);
    camera->exit();
    return retval;
}

