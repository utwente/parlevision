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

#ifndef PLVBLOBTRACKERPLUGIN_H
#define PLVBLOBTRACKERPLUGIN_H

#include "plvblobtracker_global.h"
#include <QObject>
#include <plvcore/Plugin.h>

class PLV_BLOBTRACKER_EXPORT PlvBlobTrackPlugin : public QObject, public plv::Plugin
{
    Q_OBJECT
    Q_INTERFACES(plv::Plugin)

public:
    PlvBlobTrackPlugin();
    virtual ~PlvBlobTrackPlugin();
    void onLoad();
};

#endif // PLVBLOBTRACKERPLUGIN_H
