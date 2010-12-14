/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "Application.h"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>

#include "Types.h"
#include "Enum.h"
#include "CvMatData.h"
#include "Plugin.h"
#include "PipelineElement.h"
#include "RefPtr.h"
#include "Pipeline.h"
#include "PipelineLoader.h"
#include "PipelineElementFactory.h"

using namespace plv;

Application::Application(QCoreApplication* app)
{
    this->app = app;
    app->setApplicationName("ParleVision");
    app->setOrganizationName("University of Twente");
}

Application::~Application()
{
    deinit();
}

void Application::init()
{
    loadBuiltins();
    loadPlugins();
}

void Application::deinit()
{
}

void Application::loadBuiltins()
{
    // register classes with Qt so they can be used in signals and slots
    qRegisterMetaType< plv::Data >("plv::Data");
    qRegisterMetaType< plv::Enum >( "plv::Enum" );
    qRegisterMetaType< plv::CvMatData >( "plv::CvMatData" );
    qRegisterMetaType< plv::RectangleData >( "plv::RectangleData" );
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

    QStringList fileList = pluginsDir.entryList(QDir::Files);

    foreach( const QString& fileName, fileList )
    {
        qDebug() << "Trying " << fileName;
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        pluginLoader.load();
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
            Plugin* thePlugin = qobject_cast<plv::Plugin*>(plugin);
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
