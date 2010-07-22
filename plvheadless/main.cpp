#include <iostream>
#include <QtCore/QCoreApplication>
#include <plvcore/Types.h>
#include <plvcore/DummyProcessor.h>
#include <plvcore/CameraProducer.h>
#include <plvcore/PipelineLoader.h>
#include <plvcore/Pipeline.h>
#include <plvcore/RefPtr.h>

using namespace plv;
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // register classes with Qt so they can be used in signals and slots
    qRegisterMetaType< RefPtr<Data> >("RefPtr<Data>");
    plvRegisterPipelineElement<plv::CameraProducer>("plv::CameraProducer", "Camera");
    plvRegisterPipelineElement<plv::DummyProcessor>("plv::DummyProcessor", "Dummy");

    if(argc < 1)
    {
        cerr << "Usage: " << argv[0] << " path/to/pipeline.plv";
    }

    cout << "Loading " << argv[1];
    RefPtr<Pipeline> pipeline = PipelineLoader::deserialize(argv[1]);
    pipeline->init();
    pipeline->start();

    return a.exec();
}
