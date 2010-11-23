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
#include <plvcore/OpenCVImage.h>
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

    m_converter = new ImageConverter();

    connect(m_converter.getPtr(), SIGNAL( converted( QImage ) ),
             this,                 SLOT( updateImage( QImage ) ),
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

void OpenCVImageRenderer::newData( plv::RefPtr<plv::Data> data )
{
    m_busy_mutex.lock();

    if(m_busy || !this->isVisible() )
    {
        m_busy_mutex.unlock();
        return;
    }
    else
    {
        m_busy = true;
        m_busy_mutex.unlock();
    }

    // dispatch an asynchronous call
    plv::OpenCVImage* img = static_cast<plv::OpenCVImage*>(data.getPtr());
    m_converter->convert_OpenCVImage(img);
}

void OpenCVImageRenderer::updateImage( QImage image )
{
    //TODO better make this a separate mutex, but this will do for now
    QMutexLocker lock( &m_busy_mutex );
    m_imageWidget->setImage( image );
    m_busy = false;
}
