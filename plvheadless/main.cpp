/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvheadless module of ParleVision.
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

#include <iostream>
#include <QtCore/QCoreApplication>
#include <plvcore/PipelineLoader.h>
#include <plvcore/Pipeline.h>
#include <plvcore/RefPtr.h>
#include <plvcore/Application.h>

using namespace plv;
using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " path/to/pipeline.plv";
    }

    QCoreApplication app(argc, argv);
    Application parlevision(&app);
    parlevision.init();

    cout << "Loading " << argv[1];
    RefPtr<Pipeline> pipeline = PipelineLoader::deserialize(argv[1]);
//    pipeline->init();
    pipeline->start();

    return app.exec();
}
