#include "tcpserverplugin.h"
#include <QtPlugin>
#include <QtDebug>

#include "TCPServerProcessor.h"

using namespace plv;

TCPServerPlugin::TCPServerPlugin()
{
    qDebug() << "TCPServerPlugin constructor";
}

TCPServerPlugin::~TCPServerPlugin()
{
    qDebug() << "TCPServerPlugin destructor";
}

void TCPServerPlugin::onLoad()
{
    qDebug() << "TCPServerPlugin onLoad";
    plvRegisterPipelineElement<TCPServerProcessor>();
}

Q_EXPORT_PLUGIN2(tcp_server_plugin, TCPServerPlugin)
