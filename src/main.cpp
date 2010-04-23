#include <QApplication>
#include <iostream>
#include <QDebug>

#include "FrameWidget.h"
#include "MainWindow.h"
#include "OpenCVCamera.h"
#include "DummyProcessor.h"
#include "RefPtr.h"
#include "Pipeline.h"
#include "CameraProducer.h"
#include "InspectorFactory.h"
#include "Pin.h"
#include "Inspector.h"

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

//    InspectorFactory::create(typeid(OpenCVImage).name());
//    plvgui::CameraWindow* mainWin = new plvgui::CameraWindow(camera);
    MainWindow* mainWin = new MainWindow();
    FrameWidget* cvWidget = new FrameWidget( mainWin );

    // Make a pipeline
    RefPtr<Pipeline> pipeline = new Pipeline();
    // Make a CameraProducer
    RefPtr<CameraProducer> cp = new CameraProducer();
    // Add it to the pipeline
    pipeline->add(cp);

    cp->init();

    //cvWidget->setSource(camera);

    //mainWin->addCamera(camera);
    //mainWin->addWidget( cvWidget );

    pipeline->init();
    pipeline->start();

    mainWin->setPipeline(pipeline);

    //this is temporary
    qDebug() << "output pin type" << cp->getOutputPin("output")->getTypeInfo().name();

    Inspector* inspector = InspectorFactory::create(
                                cp->getOutputPin("output")->getTypeInfo().name(), mainWin);

    inspector->setPin(cp->getOutputPin("output"));

//    delete inspector;
    mainWin->addWidget(inspector);

    mainWin->show();

    //camera->start();

    int retval = app.exec();

    // Terminate camera thread
    //camera->release();
    //camera->exit();

    return retval;
}

