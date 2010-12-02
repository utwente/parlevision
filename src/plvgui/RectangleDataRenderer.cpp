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

#include "RectangleDataRenderer.h"

#include <QDebug>
#include <plvgui/ImageWidget.h>

#include <QVBoxLayout>
#include <QPainter>

using namespace plvgui;
using namespace plv;

RectangleDataRenderer::RectangleDataRenderer(QWidget *parent)
    : DataRenderer(parent)
{
    m_layout      = new QVBoxLayout(this);
    m_imageWidget = new ImageWidget;

    m_layout->addWidget( m_imageWidget );
    QImage image( 320, 240, QImage::Format_ARGB32_Premultiplied );
    image.fill( 0 );

    // TODO make minimum configurable somewhere
    m_imageWidget->setImage( image );
    m_imageWidget->setMinimumSize( 160, 120 );

    this->setLayout( m_layout );
}

RectangleDataRenderer::~RectangleDataRenderer()
{
}

void RectangleDataRenderer::newData( plv::RefPtr<plv::Data> data )
{
    plv::RefPtr<plv::RectangleData> rectangles = ref_ptr_static_cast<plv::RectangleData>(data);

    QImage qimage = m_imageWidget->getImage();
    if( qimage.isNull() ||
        qimage.width() != rectangles->width() ||
        qimage.height() != rectangles->height() ||
        qimage.depth() != QImage::Format_ARGB32_Premultiplied )
    {
        qimage = QImage( rectangles->width(), rectangles->height(), QImage::Format_ARGB32_Premultiplied );
    }
    qimage.fill( QColor( Qt::black ).rgb() );
    QPainter painter( &qimage );
    painter.setPen( Qt::green );
    foreach( const QRect& rect, rectangles->getRects() )
    {
        painter.drawRect( rect );
    }
    painter.end();
    m_imageWidget->setImage( qimage );
}
