#include "FrameWidget.h"
#include "MainWindow.h"
#include "OpenCVCamera.h"

#include <QApplication>

int main(int argc, char **argv)
{
    plv::OpenCVCamera* camera = new plv::OpenCVCamera( 0 );
    assert(camera);
    if( camera->init() != 0 )
        return -1;

    QApplication app(argc, argv);

//    plvgui::CameraWindow* mainWin = new plvgui::CameraWindow(camera);
    plvgui::MainWindow* mainWin = new plvgui::MainWindow();

    plvgui::FrameWidget* cvWidget = new plvgui::FrameWidget(mainWin);
    cvWidget->setSource(camera);

    mainWin->addWidget( cvWidget );

    mainWin->show();

    camera->start();

    int retval = app.exec();

    // Terminate camera thread
    camera->stop();
    camera->exit();

    delete camera;

    return retval;
}

