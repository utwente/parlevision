#include "Application.h"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>

#include "Types.h"
#include "Plugin.h"
#include "PipelineElement.h"
#include "RefPtr.h"
#include "Pipeline.h"
#include "PipelineLoader.h"

#include "CameraProducer.h"

#include "Add.h"
#include "DummyProcessor.h"
#include "EdgeDetectorLaplace.h"
#include "EdgeDetectorCanny.h"
#include "ImageColorConvert.h"
#include "ImageCornerHarris.h"
#include "ImageFlip.h"
#include "ImageSmooth.h"
#include "EdgeDetectorSobel.h"

using namespace plv;

Application::Application(QCoreApplication* app)
{
    this->app = app;
    app->setApplicationName("ParleVision");
    app->setOrganizationName("University of Twente");
}

void Application::init()
{
    loadBuiltins();
    loadPlugins();
}

void Application::loadBuiltins()
{
    //FIXME: [DR] can we, instead of using the 2nd name arguments, get this information within plvRegisterPipelineElement from Q_CLASSINFO"name"? And use actual classname if this Q_CLASSINFO was not set?

    // register classes with Qt so they can be used in signals and slots
    qRegisterMetaType< RefPtr<Data> >("RefPtr<Data>");
    qRegisterMetaType< plv::Enum >( "plv::Enum" );

    //producers
    plvRegisterPipelineElement<plv::CameraProducer>("plv::CameraProducer", "Camera");

    //processors
    plvRegisterPipelineElement<plv::Add>("plv::Add", "A add B");
    plvRegisterPipelineElement<plv::DummyProcessor>("plv::DummyProcessor", "Dummy");
    plvRegisterPipelineElement<plv::EdgeDetectorCanny>("plv::EdgeDetectorCanny", "Edge Canny");
    plvRegisterPipelineElement<plv::EdgeDetectorLaplace>("plv::EdgeDetectorLaplace", "Edge Laplace");
    plvRegisterPipelineElement<plv::EdgeDetectorSobel>("plv::EdgeDetectorSobel", "Edge Sobel");
    plvRegisterPipelineElement<plv::ImageColorConvert>("plv::ImageColorConvert", "Color Conversion");
    plvRegisterPipelineElement<plv::ImageCornerHarris>("plv::ImageCornerHarris", "Harris Corner Detection");
    plvRegisterPipelineElement<plv::ImageFlip>("plv::ImageFlip", "Flip Image");
    plvRegisterPipelineElement<plv::ImageSmooth>("plv::ImageSmooth", "Smooth");
    //consumers
}

void Application::loadPlugins()
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
        pluginLoader.load();
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
            Plugin* thePlugin = qobject_cast<Plugin*>(plugin);
            if (thePlugin)
            {
                thePlugin->onLoad();
            }
            else
            {
                qWarning() << "Plugin library " << fileName
                        << " is not a valid plugin: " << pluginLoader.errorString();
            }
        }
        else
        {
            qWarning() << "Failed to load plugin " << fileName;
            qWarning() << "Reason: " << pluginLoader.errorString();
        }
    }
}
