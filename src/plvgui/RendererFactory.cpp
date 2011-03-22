/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
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

#include "RendererFactory.h"

#include <QDebug>
#include <typeinfo>

#include "OpenCVImageRenderer.h"
#include "DataRenderer.h"
#include "UnknownDataRenderer.h"

#include <plvcore/CvMatData.h>

using namespace plvgui;

RendererFactory* RendererFactory::m_instance = 0;

void RendererFactory::add(DataRendererConstructor* constructor )
{
    const char* dataTypeName = constructor->getDataTypeName();
    if( m_rendererConstructors.contains(dataTypeName))
    {
        qWarning() << "Renderer for data type " << dataTypeName << " already exists, "
                      "ignoring.";
        return;
    }
    m_rendererConstructors.insert( dataTypeName, constructor );
}

DataRenderer* RendererFactory::create(QString dataType, QWidget *parent)
{
    if( m_rendererConstructors.contains(dataType) )
    {
        DataRendererConstructor* c = m_rendererConstructors[dataType].getPtr();
        return c->create(parent);
    }
    else
    {
        qWarning() << "No renderer exists for data type " << dataType;
        return new UnknownDataRenderer(parent);
    }
}
