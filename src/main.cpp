#include <QApplication>
#include <iostream>
#include <QDebug>
#include <QtConcurrentRun>

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
#include "XmlMapper.h"
#include "PipelineLoader.h"

using namespace plv;
using namespace plvgui;

void initAndStartPipeline(Pipeline* pipeline)
{
    bool state = pipeline->init();
    assert(state);
}

/* Use Introspection */
static Pipeline* loadTestPipeline(void)
{
    Pipeline* pl = 0;

    try
    {
        pl = PipelineLoader::parsePipeline( "/Users/dawuss/Projects/parlevision/test/test_pipeline.plv" );
    }
    catch( std::runtime_error& e )
    {
        qDebug() << "Pipeline loading failed with: " << e.what();
    }
    catch( ... )
    {
        qDebug() << "Caught unknown exception.";
    }
    return pl;
}

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(icons);
    QApplication app(argc, argv);
    app.setApplicationName("ParleVision");
    app.setOrganizationName("University of Twente");

    // register classes with Qt so they can be used in signals and slots
    qRegisterMetaType< RefPtr<Data> >("RefPtr<Data>");
    qRegisterMetaType< RefPtr<QtImage> >("RefPtr<QtImage>");
    plvRegisterPipelineElement<plv::CameraProducer>("plv::CameraProducer", "Camera");
    plvRegisterPipelineElement<plv::DummyProcessor>("plv::DummyProcessor", "Dummy");
	
    MainWindow* mainWin = new MainWindow();

    // Make a pipeline
    {
        RefPtr<Pipeline> pipeline = loadTestPipeline();
        initAndStartPipeline( pipeline.getPtr() );
        mainWin->setPipeline( pipeline.getPtr() );
        mainWin->show();
    }

    int retval = app.exec();

    return retval;
}

