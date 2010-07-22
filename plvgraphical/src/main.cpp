#include <QApplication>
#include <QtPlugin>
#include <iostream>
#include <QDebug>
#include <QtConcurrentRun>

#include "PipelineElement.h"

#include "MainWindow.h"
#include "ElementConfigFormBuilder.h"
#include "CameraConfigFormBuilder.h"
#include "OpenCVCamera.h"
#include "DummyProcessor.h"
#include "RefPtr.h"
#include "Pipeline.h"
#include "CameraProducer.h"
#include "RendererFactory.h"
#include "Pin.h"
#include "DataRenderer.h"
#include "QtImage.h"
#include "PipelineLoader.h"

using namespace plv;
using namespace plvgui;

#include <QDir>
#include <QPluginLoader>
#include "Plugin.h"
void loadPlugins(QApplication* app)
 {
     QDir pluginsDir(app->applicationDirPath());
 #if defined(Q_OS_WIN)
     if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
         pluginsDir.cdUp();
 #elif defined(Q_OS_MAC)
     if (pluginsDir.dirName() == "MacOS")
     {
         pluginsDir.cdUp();
         pluginsDir.cdUp();
         pluginsDir.cdUp();
         qDebug() << "base dir: " << pluginsDir;
     }
 #endif
     bool inPluginsDir = pluginsDir.cd("plugins");
     qDebug() << "looking in " << pluginsDir;
     if(!inPluginsDir)
     {
         qWarning() << "Plugins directory not found. Skipping loading of plugins";
         return;
     }

     foreach (QString fileName, pluginsDir.entryList(QDir::Files))
     {
         qDebug() << "Trying " << fileName;
         QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
         QObject *plugin = pluginLoader.instance();
         if (plugin)
         {
             Plugin* thePlugin = qobject_cast<Plugin*>(plugin);
             if (thePlugin)
             {
                 thePlugin->onLoad();
             }
         }
     }
 }


int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(icons);
    QApplication app(argc, argv);
    loadPlugins(&app);
    app.setApplicationName("ParleVision");
    app.setOrganizationName("University of Twente");

    // register classes with Qt so they can be used in signals and slots
    qRegisterMetaType< RefPtr<Data> >("RefPtr<Data>");
    qRegisterMetaType< RefPtr<QtImage> >("RefPtr<QtImage>");
    plvRegisterPipelineElement<plv::CameraProducer>("plv::CameraProducer", "Camera");
    plvRegisterPipelineElement<plv::DummyProcessor>("plv::DummyProcessor", "Dummy");
    plvRegisterConfigFormBuilder<plvgui::CameraConfigFormBuilder>("plv::CameraProducer","plvgui::CameraConfigFormBuilder");
	
    MainWindow* mainWin = new MainWindow();

    mainWin->show();

    int retval = app.exec();

    return retval;
}
