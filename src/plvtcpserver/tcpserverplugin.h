#ifndef TCPSERVERPLUGIN_H
#define TCPSERVERPLUGIN_H

#include "tcpserver_global.h"
#include <QObject>
#include <plvcore/Plugin.h>

class TCP_SERVER_EXPORT TCPServerPlugin : public QObject, public plv::Plugin
{
    Q_OBJECT
    Q_INTERFACES(plv::Plugin)

public:
    TCPServerPlugin();
    virtual ~TCPServerPlugin();
    void onLoad();
};

#endif // TCPSERVERPLUGIN_H
