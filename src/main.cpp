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

    mainWin->show();

    int retval = app.exec();

    return retval;
}

