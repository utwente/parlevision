#include <QApplication>
#include <iostream>

#include "FrameWidget.h"
#include "MainWindow.h"
#include "OpenCVCamera.h"
#include "DummyProcessor.h"
#include "RefPtr.h"
#include "Pipeline.h"
#include "CameraProducer.h"

using namespace plv;
using namespace plvgui;

int main(int argc, char **argv)
{
    //plv::OpenCVCamera* camera = new plv::OpenCVCamera( 0 );
    //assert(camera);
    //if( camera->init() != 0 )
     //   return -1;

    //camera->setDimensions( 640, 480 );

    QApplication app(argc, argv);

//    plvgui::CameraWindow* mainWin = new plvgui::CameraWindow(camera);
    MainWindow* mainWin = new MainWindow();
    FrameWidget* cvWidget = new FrameWidget( mainWin );

    RefPtr<Pipeline> pipeline = new Pipeline();
    RefPtr<DummyProcessor> dp = new DummyProcessor();

    pipeline->add(dp);

    RefPtr<CameraProducer> cp = new CameraProducer();
    pipeline->add(cp);

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

