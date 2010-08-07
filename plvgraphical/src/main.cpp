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

#include <QApplication>
#include <iostream>
#include <QDebug>

#include <Application.h>

#include <MainWindow.h>
#include <ElementConfigFormBuilder.h>
#include <CameraConfigFormBuilder.h>

#include "openeventhandler.h"

using namespace plv;
using namespace plvgui;

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(icons);
    QApplication app(argc, argv);

    plvRegisterConfigFormBuilder<plvgui::CameraConfigFormBuilder>
                    ("plv::CameraProducer", "plvgui::CameraConfigFormBuilder");

    Application parlevision(&app);
    parlevision.init();
    app.installEventFilter(new OpenEventHandler());

    MainWindow* mainWin = new MainWindow();

    mainWin->show();
    mainWin->showWelcomeScreen();

    int retval = app.exec();

    return retval;
}
