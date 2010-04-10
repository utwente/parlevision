#include "FrameWidget.h"
#include "MainWindow.h"
#include "OpenCVCamera.h"

#include <QApplication>
#include <iostream>

int main(int argc, char **argv)
{
    plv::OpenCVCamera* camera = new plv::OpenCVCamera( 0 );
    assert(camera);
    if( camera->init() != 0 )
        return -1;

    camera->setDimensions( 640, 480 );

    QApplication app(argc, argv);

//    plvgui::CameraWindow* mainWin = new plvgui::CameraWindow(camera);
    plvgui::MainWindow* mainWin = new plvgui::MainWindow();

    plvgui::FrameWidget* cvWidget = new plvgui::FrameWidget(mainWin);
    cvWidget->setSource(camera);

    std::cout << "test" << std::endl;
    qDebug("test");

    mainWin->addCamera(camera);
    mainWin->addWidget( cvWidget );

    mainWin->show();

    camera->start();

    int retval = app.exec();

    // Terminate camera thread
    camera->release();
    camera->exit();

    delete camera;

    return retval;
}

