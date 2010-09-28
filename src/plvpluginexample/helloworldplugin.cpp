#include "helloworldplugin.h"
#include <QtPlugin>
#include <QtDebug>

#include "helloworldprocessor.h"

using namespace plv;

HelloWorldPlugin::HelloWorldPlugin()
{
    qDebug() << "HelloWorldPlugin constructor";
}

HelloWorldPlugin::~HelloWorldPlugin()
{
    qDebug() << "HelloWorldPlugin destructor";
}

void HelloWorldPlugin::onLoad()
{
    qDebug() << "HelloWorldPlugin onLoad";
    plvRegisterPipelineElement<HelloWorldProcessor>("HelloWorldProcessor", "Hello World");
}

Q_EXPORT_PLUGIN2(hello_world_plugin, HelloWorldPlugin)
