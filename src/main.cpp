#include <QApplication>
#include <iostream>
#include <QDebug>
#include <QtConcurrentRun>

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
#include "QtImage.h"

using namespace plv;
using namespace plvgui;

void initAndStartPipeline(Pipeline* pipeline)
{
    PlvPipelineElementState state = pipeline->init();
    assert(state == PLV_PLE_STATE_READY);

    pipeline->start();
}

int main(int argc, char **argv)
{
    //plv::OpenCVCamera* camera = new plv::OpenCVCamera( 0 );
    //assert(camera);
    //if( camera->init() != 0 )
     //   return -1;

    //camera->setDimensions( 640, 480 );

    QApplication app(argc, argv);

    plvRegisterPipelineElement<plv::CameraProducer>("plv::CameraProducer");
    plvRegisterPipelineElement<plv::DummyProcessor>("plv::DummyProcessor");
	
	// register classes with Qt so they can be used in signals and slots
    int id = qRegisterMetaType< RefPtr<Data> >("RefPtr<Data>");
    //RefPtr<Data> test = static_cast< RefPtr<Data> >(QMetaType::construct( id ));
    qRegisterMetaType< RefPtr<QtImage> >("RefPtr<QtImage>");

//    for( int i=0; i<500000; ++i )
//    {
//        QString* myClassPtr = static_cast<QString*>( QMetaType::construct(QMetaType::type("QString")) );
//        myClassPtr->append( "DIT IS EEN TEST" );
//        //qDebug() << myClassPtr;
//        delete myClassPtr;
//    }

//    InspectorFactory::create(typeid(OpenCVImage).name());
//    plvgui::CameraWindow* mainWin = new plvgui::CameraWindow(camera);
    MainWindow* mainWin = new MainWindow();
//    FrameWidget* cvWidget = new FrameWidget( mainWin );

    // Make a pipeline
    RefPtr<Pipeline> pipeline = new Pipeline();
    // Make a CameraProducer
    RefPtr<CameraProducer> cp = static_cast<CameraProducer*>( QMetaType::construct(QMetaType::type("plv::CameraProducer")) );
    // Add it to the pipeline
    pipeline->add(cp);

    RefPtr<DummyProcessor> dp = static_cast<DummyProcessor*>( QMetaType::construct(QMetaType::type("plv::DummyProcessor")) );
    pipeline->add(dp);

    pipeline->connectPins(cp->getOutputPin("output"), dp->getInputPin("input image"));



    //cvWidget->setSource(camera);

    //mainWin->addCamera(camera);
    //mainWin->addWidget( cvWidget );

    // pipeline initialisation is blocking, but also depends on the Qt run loop
    // being initialised. This is a cyclic dependency that we solve by
    // initialising the pipeline in a separate thread.
    //QtConcurrent::run(initAndStartPipeline, pipeline);
    initAndStartPipeline( pipeline );

    mainWin->setPipeline(pipeline);

    //this is temporary
    qDebug() << "output pin type" << cp->getOutputPin("output")->getTypeInfo().name();

    Inspector* inspector = InspectorFactory::create(
                                cp->getOutputPin("output")->getTypeInfo().name(), mainWin);

    inspector->setPin(cp->getOutputPin("output"));

//    delete inspector;
    mainWin->addWidget(inspector);

    Inspector* dummy_inspector = InspectorFactory::create(
                                dp->getOutputPin("output image")->getTypeInfo().name(), mainWin);

    dummy_inspector->setPin(dp->getOutputPin("output image"));

    mainWin->addWidget(dummy_inspector);

    mainWin->show();

    //camera->start();

    int retval = app.exec();

    // Terminate camera thread
    //camera->release();
    //camera->exit();

    return retval;
}

