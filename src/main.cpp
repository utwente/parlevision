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
    bool state = pipeline->init();
    assert(state);
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
    RefPtr<Pipeline> pipeline = new Pipeline();
    // Make a CameraProducer
    RefPtr<CameraProducer> cp = static_cast<CameraProducer*>( QMetaType::construct(QMetaType::type("plv::CameraProducer")) );
    // Add it to the pipeline
    pipeline->add(cp);

    RefPtr<DummyProcessor> dp = static_cast<DummyProcessor*>( QMetaType::construct(QMetaType::type("plv::DummyProcessor")) );
    pipeline->add(dp);

    pipeline->connectPins(cp->getOutputPin("output"), dp->getInputPin("input image"));

    initAndStartPipeline( pipeline );

    mainWin->setPipeline(pipeline);
    mainWin->show();

    int retval = app.exec();

    return retval;
}

