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
#include <QVBoxLayout>
#include <QScopedPointer>

#include <plvcore/Types.h>
#include <plvcore/CvMatData.h>
#include <plvcore/RefPtr.h>

#include <plvgui/ImageConverter.h>
#include <plvgui/ImageWidget.h>

using namespace plvgui;

OpenCVImageRenderer::OpenCVImageRenderer(QWidget* parent) :
    DataRenderer(parent),
    m_busy(false)

{
    m_layout      = new QVBoxLayout(this);
    m_imageWidget = new ImageWidget;

    m_layout->addWidget( m_imageWidget );

    QImage image = QImage(320,240,QImage::Format_RGB32);
    image.fill( qRgb(0,0,0) );

    // TODO make minimum configurable somewhere
    m_imageWidget->setMinimumSize( 160, 120 );
    m_imageWidget->setImage( image );

    this->setLayout( m_layout );

    m_converter = new ImageConverter(this);

    connect( m_converter, SIGNAL( converted( QImage, int ) ),
             this,        SLOT( updateImage( QImage, int ) ),
             Qt::UniqueConnection);
}

OpenCVImageRenderer::~OpenCVImageRenderer()
{
}

void OpenCVImageRenderer::showEvent(QShowEvent* event)
{
    qDebug() << "Showing renderer";
    QWidget::showEvent(event);
}

void OpenCVImageRenderer::hideEvent(QHideEvent* event)
{
    qDebug() << "Hiding renderer";
    QWidget::hideEvent(event);
}

void OpenCVImageRenderer::newData( QVariant v )
{
    QMutexLocker lock( &m_busy_mutex );

    if(m_busy || !this->isVisible() )
        return;

    m_busy = true;
    lock.unlock();

    // dispatch an asynchronous call
    if( v.canConvert<plv::CvMatData>() )
    {
        plv::CvMatData data = v.value<plv::CvMatData>();
        m_converter->convertCvMatData(data);
    }
}

void OpenCVImageRenderer::updateImage( QImage image, int id )
{
    Q_UNUSED(id)

    m_imageWidget->setImage(image);
    m_layout->update();

    QMutexLocker lock2(&m_busy_mutex);
    m_busy = false;
}
