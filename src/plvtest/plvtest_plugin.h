#ifndef PLVTESTPLUGIN_H
#define PLVTESTPLUGIN_H

#include "plvtest_global.h"
#include <QObject>
#include <plvcore/Plugin.h>

class PLV_TEST_EXPORT PlvTestPlugin : public QObject, public plv::Plugin
{
    Q_OBJECT
    Q_INTERFACES(plv::Plugin)

public:
    PlvTestPlugin();
    virtual ~PlvTestPlugin();
    void onLoad();
};

#endif // PLVTESTPLUGIN_H
