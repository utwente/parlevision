/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgraphical module of ParleVision.
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
#include "openeventhandler.h"
#include <QFileOpenEvent>

#include <MainWindow.h>
#include <plvcore/Application.h>

using namespace plv;
using namespace plvgui;

OpenEventHandler::OpenEventHandler(plv::Application* app) :
    QObject(app), m_app(app)
{
    assert(m_app!=0);
}

bool OpenEventHandler::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FileOpen)
    {
        MainWindow* mainWin = new MainWindow(m_app);
        mainWin->show();
        mainWin->loadFile(static_cast<QFileOpenEvent *>(event)->file());
        return true;
    }
    return QObject::eventFilter(obj, event);
}
