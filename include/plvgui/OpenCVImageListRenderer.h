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

#ifndef OPENCVIMAGELISTRENDERER_H
#define OPENCVIMAGELISTRENDERER_H

#include <QMutex>
#include "ImageConverter.h"
#include "DataRenderer.h"

QT_BEGIN_NAMESPACE
class QPixmap;
class QHBoxLayout;
class QImage;
QT_END_NAMESPACE

namespace plv
{
    class Data;
}

namespace plvgui
{
    class ImageWidget;

    class PLVGUI_EXPORT OpenCVImageListRenderer : public DataRenderer
    {
        Q_OBJECT

    public:
        OpenCVImageListRenderer(QWidget* parent);
        virtual ~OpenCVImageListRenderer();

    protected:
        void showEvent(QShowEvent* event);
        void hideEvent(QHideEvent* event);

    private:
        QHBoxLayout*          m_layout;
        QVector<ImageWidget*> m_imageWidgets;
        bool                  m_busy;
        QMutex                m_busy_mutex;
        ImageConverter*       m_converter;

    public slots:
        virtual void newData( unsigned int serial, QVariant v );
        void updateImages( QList<QImage> images, int id );
    };
}
#endif //OPENCVIMAGELISTRENDERER_H
