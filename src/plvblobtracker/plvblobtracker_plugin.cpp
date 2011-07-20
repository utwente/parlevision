/**
  * Copyright (C)2011 by Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvtest plugin of ParleVision.
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

#include "plvblobtracker_plugin.h"

#include <QtPlugin>
#include <QtDebug>

#include "BlobDetector.h"
#include "BlobTracker.h"

using namespace plv;

PlvBlobTrackPlugin::PlvBlobTrackPlugin()
{
}

PlvBlobTrackPlugin::~PlvBlobTrackPlugin()
{
}

void PlvBlobTrackPlugin::onLoad()
{
    // register custom types
    qRegisterMetaType< QList<plvblobtracker::Blob*> >("QList<plvopencv::Blob*>");

    plvRegisterPipelineElement<plvblobtracker::BlobTracker>();
    plvRegisterPipelineElement<plvblobtracker::BlobDetector>();
}

Q_EXPORT_PLUGIN2(plv_blobtracker_plugin, PlvBlobTrackPlugin)
