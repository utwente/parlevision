#ifndef MSKINECTPLUGIN_H
#define MSKINECTPLUGIN_H

#include "mskinectplugin_global.h"
#include <QObject>
#include <plvcore/Plugin.h>
#include <plvgui/RendererFactory.h>

//class Test : public plvgui::DataRendererConstructor
//{
//public:
//    virtual const char* getDataTypeName() const;
//    virtual plvgui::DataRenderer* create(QWidget* parent) const;
//};

class MSKINECT_PLUGIN_EXPORT MSKinectPlugin : public QObject, public plv::Plugin
{
    Q_OBJECT
    Q_INTERFACES(plv::Plugin)

public:
    MSKinectPlugin();
    virtual ~MSKinectPlugin();
    void onLoad();
};

#endif // MSKINECTPLUGIN_H
