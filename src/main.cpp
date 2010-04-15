#include <QApplication>
#include <iostream>

#include "FrameWidget.h"
#include "MainWindow.h"
#include "OpenCVCamera.h"
#include "DummyProcessor.h"
#include "RefPtr.h"
#include "Pipeline.h"
#include "CameraProducer.h"

int main(int argc, char **argv)
{
    //plv::OpenCVCamera* camera = new plv::OpenCVCamera( 0 );
    //assert(camera);
    //if( camera->init() != 0 )
     //   return -1;

    //camera->setDimensions( 640, 480 );

    QApplication app(argc, argv);

//    plvgui::CameraWindow* mainWin = new plvgui::CameraWindow(camera);
    plvgui::MainWindow* mainWin = new plvgui::MainWindow();
    plvgui::FrameWidget* cvWidget = new plvgui::FrameWidget(mainWin);

    plv::RefPtr<plv::Pipeline> pipeline = new plv::Pipeline();
    plv::RefPtr<plv::DummyProcessor> dp = new plv::DummyProcessor( pipeline.getPtr() );

    plv::RefPtr<plv::CameraProducer> cp = new plv::CameraProducer( pipeline );

    cp->produce();

    //cvWidget->setSource(camera);

    //mainWin->addCamera(camera);
    //mainWin->addWidget( cvWidget );

    mainWin->show();

    //camera->start();

    int retval = app.exec();

    // Terminate camera thread
    //camera->release();
    //camera->exit();

    return retval;
}

