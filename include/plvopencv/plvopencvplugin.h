#ifndef PLVOPENCVPLUGIN_H
#define PLVOPENCVPLUGIN_H

#include "plvopencv_global.h"
#include <QObject>
#include <plvcore/Plugin.h>

class PLVOPENCV_EXPORT PlvOpenCVPlugin : public QObject, public plv::Plugin
{
    Q_OBJECT
    Q_INTERFACES(plv::Plugin)

public:
    PlvOpenCVPlugin();
    virtual ~PlvOpenCVPlugin();
    void onLoad();
};

#endif // PLVOPENCVPLUGIN_H
