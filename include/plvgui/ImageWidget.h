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

#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include "plvgui_global.h"

class QSize;
class QImage;

namespace plvgui
{
    class PLVGUI_EXPORT ImageWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit ImageWidget( QWidget *parent = 0 );
        void setImage( const QImage &image );
        QImage getImage();
        float getZoomFactor();
        void setZoomFactor( float zoomFactor );
        void setZoomToFit( bool zoomToFit );

    protected:
        QImage m_image;
        float  m_zoomFactor;
        float  m_aspectRatio;
        bool   m_zoomToFit;

        void resizeEvent(QResizeEvent * event);

        /** Custom paint method inherited from QWidget */
        void paintEvent( QPaintEvent * );

        /** Iherited from QWidget. Returns our preferred size */
        QSize sizeHint() const;

        void computeZoomFactorToFitImage();
    };
}

#endif // IMAGEWIDGET_H
