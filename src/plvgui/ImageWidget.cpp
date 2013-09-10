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

#include "ImageWidget.h"

#include <QImage>
#include <QPainter>
#include <QSize>
#include <QDebug>

using namespace plvgui;

ImageWidget::ImageWidget(QWidget *parent) :
        QWidget(parent),
        m_zoomFactor( 1.0 ),
        m_aspectRatio( 1.0 ),
        m_zoomToFit( true )
{
}

void ImageWidget::setImage(const QImage &img)
{
    m_image = img;
    m_aspectRatio = img.width() / (float)img.height();

    if( m_zoomToFit )
    {
        computeZoomFactorToFitImage();
    }
    else
    {
        m_zoomFactor = 1.0;
    }

    // resize and update widget
    resize( sizeHint() );
    update();
}

void ImageWidget::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    
    if( m_zoomToFit )
        computeZoomFactorToFitImage();
    else
        m_zoomFactor = 1.0;
}

void ImageWidget::paintEvent(QPaintEvent *)
{
    //QRect rect = this->rect();
    int x,y,w,h;

    w = m_image.width() * m_zoomFactor;
    h = m_image.height()* m_zoomFactor;
    x = 0;
    y = 0;

    QRectF target(x,y,w,h);
    QPainter p(this);
    p.drawImage(target, m_image);
}

QImage ImageWidget::getImage()
{
    return m_image;
}

void ImageWidget::setZoomFactor( float zf )
{
    m_zoomFactor = zf;
    //updateGeometry();
    update();
    adjustSize();
}

QSize ImageWidget::sizeHint() const
{
    if( !m_image.isNull() )
    {
        QSize hint = m_image.size() * m_zoomFactor;
        return hint;
    }
    return minimumSize();
}

void ImageWidget::setZoomToFit( bool zoomToFit )
{
    m_zoomToFit = zoomToFit;
    if( zoomToFit)
    {
        computeZoomFactorToFitImage();
    }
    else
    {
        m_zoomFactor = 1.0;
    }

    // resize and update widget
    resize( sizeHint() );
    update();
}

void ImageWidget::computeZoomFactorToFitImage()
{
    QRect rect = this->rect();
    float ratio = rect.width() / (float)rect.height();

    if( ratio > m_aspectRatio )
    {
        m_zoomFactor= rect.height() / (float)m_image.height();
    }
    else
    {
        m_zoomFactor = rect.width() / (float)m_image.width();
    }
}
