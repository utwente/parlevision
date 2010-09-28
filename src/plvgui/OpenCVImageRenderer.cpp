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

#include "OpenCVImageRenderer.h"

#include <QDebug>
#include <QtGui>

#include <plvcore/Types.h>
#include <plvopencv/OpenCVImage.h>
#include <plvcore/RefPtr.h>

#include <plvgui/ImageConverter.h>
#include <plvgui/ImageWidget.h>

using namespace plv;
using namespace plvgui;
using namespace plvopencv;

OpenCVImageRenderer::OpenCVImageRenderer(QWidget* parent) :
    DataRenderer(parent),
    m_busy(false)

{
    m_layout      = new QVBoxLayout(this);
    m_imageWidget = new ImageWidget;

    m_layout->addWidget( m_imageWidget );
//    m_layout->setSizeConstraint(QLayout::SetDefaultConstraint);
//    m_layout->addStretch(0);

    QImage image = QImage(320,240,QImage::Format_RGB32);
    image.fill( qRgb(0,0,0) );
    m_imageWidget->setMinimumSize( 320, 240 );
    m_imageWidget->setImage( image );

    this->setLayout( m_layout );

//    QSizePolicy labelPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    labelPolicy.setHeightForWidth(true);
//    m_imagelabel->setSizePolicy(labelPolicy);

//    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    sizePolicy.setHeightForWidth(true);
//    this->setSizePolicy(sizePolicy);

    m_converter = new ImageConverter();
}

void OpenCVImageRenderer::showEvent(QShowEvent* event)
{
    qDebug() << "connecting renderer";

    connect(m_converter.getPtr(), SIGNAL( converted( QImage ) ),
             this,                 SLOT( updateImage( QImage ) ),
             Qt::UniqueConnection);

    QWidget::showEvent(event);
}

void OpenCVImageRenderer::hideEvent(QHideEvent* event)
{
    qDebug() << "disconnecting renderer";
    this->disconnect();
    QWidget::hideEvent(event);
}


void OpenCVImageRenderer::newData( RefPtr<Data> data )
{
    QMutexLocker lock( &m_busy_mutex );

    if(m_busy)
    {
        return;
    }
    else
    {
        RefPtr<OpenCVImage> img = ref_ptr_static_cast<OpenCVImage>(data);
        assert(img.isNotNull());
        if(img.isNotNull())
        {
            m_busy = true;
            // dispatch an asynchronous call
            m_converter->convert_OpenCVImage(img);
        }
    }
}

void OpenCVImageRenderer::updateImage( QImage image )
{
    //TODO better make this a separate mutex, but this will do for now
    QMutexLocker lock( &m_busy_mutex );
    m_imageWidget->setImage( image );
    m_busy = false;
}
