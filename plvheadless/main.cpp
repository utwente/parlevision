#include <iostream>
#include <QtCore/QCoreApplication>
#include <plvcore/PipelineLoader.h>
#include <plvcore/Pipeline.h>
#include <plvcore/RefPtr.h>
#include <plvcore/Application.h>

using namespace plv;
using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " path/to/pipeline.plv";
    }

    QCoreApplication app(argc, argv);
    Application parlevision(&app);
    parlevision.init();

    cout << "Loading " << argv[1];
    RefPtr<Pipeline> pipeline = PipelineLoader::deserialize(argv[1]);
//    pipeline->init();
    pipeline->start();

    return app.exec();
}
