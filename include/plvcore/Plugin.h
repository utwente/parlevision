#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>
#include <QtPlugin>

namespace plv
{
    class Plugin
    {
    public:
        virtual void onLoad()=0;
    };
}

Q_DECLARE_INTERFACE(plv::Plugin, "plv.Plugin/1.0")

#endif // PLUGIN_H
