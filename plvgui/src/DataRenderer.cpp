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

#include "DataRenderer.h"

#include <QDebug>

#include <plvcore/Pin.h>

using namespace plvgui;
using namespace plv;

DataRenderer::DataRenderer(QWidget *parent)
    : QWidget(parent)
{
}

DataRenderer::~DataRenderer()
{
    disconnect();
}

void DataRenderer::setPin( const plv::Pin* p )
{
    qDebug() << "attaching inspector to pin";
    disconnect();
    connect( p, SIGNAL( newData( RefPtr<Data> )),
             this, SLOT( newData( RefPtr<Data> )) );
}

void DataRenderer::newData( RefPtr<Data> )
{
    qDebug() << "WARNING: Abstract DataRenderer received newData signal.\n"
                << "Did you forget to implement newData(Data* data) in a subclass?";
}
