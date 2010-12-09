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

#include "Types.h"

using namespace plv;

RectangleData::RectangleData( int width, int height ) :
    m_width(width),
    m_height(height)
{
    assert( m_width > 0 );
    assert( m_height > 0 );
}
RectangleData::~RectangleData() {}

int RectangleData::width() const { return m_width; }
int RectangleData::height() const { return m_height; }

/** adds a rectangle to internal rectangle list */
void RectangleData::add( const QRect& rect )
{
    m_rects.append( rect );
}

/** QList uses implicit sharing so we return by value */
QList<QRect> RectangleData::getRects() const
{
    return m_rects;
}


