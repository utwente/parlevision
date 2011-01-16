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

#ifndef OpenCVImageRenderer_H
#define OpenCVImageRenderer_H

#include <QMutex>
#include <plvcore/RefPtr.h>

#include "ImageConverter.h"
#include "DataRenderer.h"

class QPixmap;
class QVBoxLayout;
class QImage;

namespace plv
{
    class Data;
}

namespace plvgui
{
    class ImageWidget;

    class PLVGUI_EXPORT OpenCVImageRenderer : public DataRenderer
    {
        Q_OBJECT

    public:
        OpenCVImageRenderer(QWidget* parent);
        virtual ~OpenCVImageRenderer();

    protected:
        void showEvent(QShowEvent* event);
        void hideEvent(QHideEvent* event);

    private:
        QVBoxLayout*    m_layout;
        ImageWidget*    m_imageWidget;
        bool            m_busy;
        QMutex          m_busy_mutex;
        plv::RefPtr<ImageConverter> m_converter;

    public slots:
        virtual void newData( QVariant v );
        void updateImage( QImage img );
    };
}
#endif // OpenCVImageRenderer_H
