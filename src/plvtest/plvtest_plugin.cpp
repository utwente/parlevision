#include "plvtest_plugin.h"

#include <QtPlugin>
#include <QtDebug>

#include "TestProducer.h"

using namespace plv;

PlvTestPlugin::PlvTestPlugin()
{
    qDebug() << "PlvTestPlugin constructor";
}

PlvTestPlugin::~PlvTestPlugin()
{
    qDebug() << "PlvTestPlugin destructor";
}

void PlvTestPlugin::onLoad()
{
    qDebug() << "PlvTestPlugin onLoad";
    plvRegisterPipelineElement<TestProducer>();
}

Q_EXPORT_PLUGIN2(plv_test_plugin, PlvTestPlugin)
