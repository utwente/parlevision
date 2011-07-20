/**
  * Copyright (C)2011 by Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvmskinect module of ParleVision.
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

#ifndef SKELETONDATARENDERER_H
#define SKELETONDATARENDERER_H

#include <plvgui/DataRenderer.h>
#include <qglobal.h>
QT_FORWARD_DECLARE_CLASS( QVBoxLayout )

#include <QPen>
#include <QImage>
#include <QPainter>
#include <QMutex>
#include "mskinectdatatypes.h"

namespace plv
{
    class Data;
}

namespace plvgui
{
    class ImageWidget;
}

namespace plvmskinect
{
    class SkeletonDataViewer : public plvgui::DataRenderer
    {
        Q_OBJECT

    public:
        SkeletonDataViewer( QWidget* parent );
        virtual ~SkeletonDataViewer();

    private:
        QVBoxLayout* m_layout;
        plvgui::ImageWidget* m_imageWidget;

        QVector<QPen> m_pens;
        QPoint m_points[NUI_SKELETON_POSITION_COUNT];
        QImage m_image;
        int m_width;
        int m_height;
        //mutable QMutex m_busy_mutex;
        //bool m_busy;

        void drawSkeletons( plvmskinect::SkeletonFrame frame );
        void drawSkeletonSegment( QPainter& painter, int numJoints, ... );
        void drawSkeleton( QPainter& painter, const NUI_SKELETON_DATA* pSkel, int whichSkeletonColor );
        //void done( unsigned int serial );

    public slots:
        virtual void newData( unsigned int serial, QVariant data );
    };
}

#endif // SKELETONDATARENDERER_H
