#ifndef HELLOWORLDPLUGIN_H
#define HELLOWORLDPLUGIN_H

#include "hello_world_plugin_global.h"
#include <QObject>
#include <plvcore/Plugin.h>

class HELLO_WORLD_PLUGINSHARED_EXPORT HelloWorldPlugin : public QObject, public plv::Plugin
{
    Q_OBJECT
    Q_INTERFACES(plv::Plugin)

public:
    HelloWorldPlugin();
    virtual ~HelloWorldPlugin();
    void onLoad();
};

#endif // HELLOWORLDPLUGIN_H
