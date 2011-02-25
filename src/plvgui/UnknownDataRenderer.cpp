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

#include "UnknownDataRenderer.h"

#include <QDebug>
#include <plvgui/ImageWidget.h>

#include <QVBoxLayout>
#include <QPainter>

using namespace plvgui;
using namespace plv;

UnknownDataRenderer::UnknownDataRenderer(QWidget *parent)
    : DataRenderer(parent)
{
    m_layout      = new QVBoxLayout(this);
    m_imageWidget = new ImageWidget;

    m_layout->addWidget( m_imageWidget );

    QString msg = QString("Unknown datatype. No renderer found for this datatype");

    QSize size( 640,480 );
    QImage qimage( size, QImage::Format_ARGB32_Premultiplied );
    qimage.fill( 0 );
    QPainter painter(&qimage);
    painter.fillRect( qimage.rect(), QColor( Qt::white ));
    painter.setPen( Qt::black );
    QFont font = painter.font();
    font.setPointSize( 16 );
    painter.setFont( font );
    painter.drawText( qimage.rect(), Qt::AlignCenter | Qt::TextWordWrap, msg);
    painter.end();

    // TODO make minimum configurable somewhere
    m_imageWidget->setMinimumSize( 160, 120 );
    m_imageWidget->setImage( qimage );

    this->setLayout( m_layout );
}

UnknownDataRenderer::~UnknownDataRenderer()
{
}

void UnknownDataRenderer::newData( unsigned int serial, QVariant data )
{
    Q_UNUSED( data );
    Q_UNUSED( serial );
}
