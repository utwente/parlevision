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

RectangleData::RectangleData( int width, int height, QList<QRect> rects ) :
    d( new RectangleDataPrivate( width, height, rects ) )
{
}

RectangleData::RectangleData( const RectangleData& other ) :
        d(other.d) {}

RectangleData::~RectangleData() {}

QDataStream &operator<<(QDataStream &out, const RectangleData& d)
{
    out << d.width();
    out << d.height();
    out << d.getRects();
    return out;
}

QDataStream &operator>>(QDataStream &in, RectangleData& d)
{
    int width;
    int height;
    QList<QRect> rects;

    in >> width;
    in >> height;
    in >> rects;

    d.setWidth(width);
    d.setHeight(height);
    d.setRects(rects);

    return in;
}

QDataStream &operator<<(QDataStream &out, const cv::Scalar &s)
{
    // store 4 doubles in the stream
    for( int i=0; i < 4; ++i )
        out << s[i];
    return out;
}

QDataStream &operator>>(QDataStream &in, cv::Scalar &s)
{
    // load 4 doubles from the stream
    for( int i=0; i < 4; ++i )
    {
         double d;
         in >> d;
         s[i] = d;
    }
    return in;
}
