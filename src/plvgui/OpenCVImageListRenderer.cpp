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

#include "OpenCVImageListRenderer.h"

#include <QDebug>
#include <QHBoxLayout>

#include <plvcore/Types.h>
#include <plvcore/CvMatData.h>
#include <plvcore/RefPtr.h>

#include <plvgui/ImageConverter.h>
#include <plvgui/ImageWidget.h>

using namespace plvgui;

OpenCVImageListRenderer::OpenCVImageListRenderer(QWidget* parent) :
    DataRenderer(parent),
    m_busy(false)

{
    m_layout      = new QHBoxLayout(this);
    ImageWidget* imageWidget = new ImageWidget;

    m_imageWidgets.append(imageWidget);
    m_layout->addWidget( imageWidget );

    QImage image = QImage(320,240,QImage::Format_RGB32);
    image.fill( qRgb(0,0,0) );

    // TODO make minimum configurable somewhere
    imageWidget->setMinimumSize( 160, 120 );
    imageWidget->setImage( image );

    this->setLayout( m_layout );

    m_converter = new ImageConverter(this);

    qRegisterMetaType< QList<QImage> >("QList<QImage>");

    connect( m_converter, SIGNAL( convertedList( QList<QImage>, int ) ),
             this,        SLOT( updateImages( QList<QImage>, int ) ),
             Qt::UniqueConnection);
}

OpenCVImageListRenderer::~OpenCVImageListRenderer()
{
}

void OpenCVImageListRenderer::showEvent(QShowEvent* event)
{
    qDebug() << "Showing renderer";
    QWidget::showEvent(event);
}

void OpenCVImageListRenderer::hideEvent(QHideEvent* event)
{
    qDebug() << "Hiding renderer";
    QWidget::hideEvent(event);
}

void OpenCVImageListRenderer::newData( QVariant v )
{
    QMutexLocker lock( &m_busy_mutex );

    if(m_busy || !this->isVisible() )
        return;

    m_busy = true;
    lock.unlock();

    // dispatch an asynchronous call
    if( v.canConvert< QList<plv::CvMatData> >() )
    {
        QList<plv::CvMatData> dataList = v.value< QList<plv::CvMatData> >();
        m_converter->convertCvMatDataList(dataList);
    }
}

void OpenCVImageListRenderer::updateImages( QList<QImage> images, int id )
{
    Q_UNUSED(id)

    if( m_imageWidgets.size() < images.size() )
    {
        for(int i=m_imageWidgets.size(); i<images.size(); ++i)
        {
            ImageWidget* imageWidget = new ImageWidget();

            m_imageWidgets.append(imageWidget);
            m_layout->addWidget(imageWidget);

            imageWidget->setMinimumSize( 160, 120 );
        }
    }
    else if( m_imageWidgets.size() > images.size() )
    {
        int from = images.size();
        int to = m_imageWidgets.size();
        for( int i=from; i < to; ++i)
        {
            ImageWidget* w = m_imageWidgets.at(i);
            m_layout->removeWidget(w);
            delete w;
        }
        m_imageWidgets.remove(from, to-from);
    }

    int i;
    for( i=0; i<images.size(); ++i)
    {
        ImageWidget* iw = m_imageWidgets.at(i);
        iw->setImage( images.at(i) );
    }
    m_layout->update();

    QMutexLocker lock( &m_busy_mutex );
    m_busy = false;
}
