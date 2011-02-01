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

#ifndef TYPES_H
#define TYPES_H

#include <QMap>
#include <QStringList>
#include <QStringBuilder>
#include <QMetaType>
#include <QVariant>
#include <QRect>
#include <QSharedData>

#include <opencv/cv.h>
#include "plvglobal.h"
#include "RefPtr.h"
#include "assert.h"

namespace plv 
{
    class RectangleDataPrivate : public QSharedData
    {
    public:
        int m_width;
        int m_height;
        QList<QRect> m_rects;

        RectangleDataPrivate(int w, int h) : m_width(w), m_height(h) {}
        RectangleDataPrivate( const RectangleDataPrivate& other ) :
                QSharedData(),
                m_width(other.m_width), m_height(other.m_height),
                m_rects(other.m_rects) {}
    };

    /** DataContainer around a QList with QRect rectangle list */
    class PLVCORE_EXPORT RectangleData
    {
    private:
        QSharedDataPointer<RectangleDataPrivate> d;

    public:
        /** Constructor. Takes width and height of the image from which the
            rectangles are taken if relevant. This is necessary for correct
            rendering of the rectangles.  */
        RectangleData( int width=0, int height=0 );
        RectangleData( const RectangleData& other );
        ~RectangleData();

        int width() const { return d->m_width; }
        int height() const { return d->m_height; }

        /** adds a rectangle to internal rectangle list */
        void add( const QRect& rect ) { d->m_rects.append( rect ); }

        /** QList uses implicit sharing so we return by value */
        QList<QRect> getRects() const { return d->m_rects; }
    };
}

PLVCORE_EXPORT QDataStream &operator<<(QDataStream &out, const cv::Scalar &s);
PLVCORE_EXPORT QDataStream &operator>>(QDataStream &in, cv::Scalar &s);

/** Declare as Qt Metatype so we can pass data type along with
    PinConnections and Qt's signals and slots */
Q_DECLARE_METATYPE( plv::RectangleData )
Q_DECLARE_METATYPE( cv::Scalar );

#endif // TYPES_H
