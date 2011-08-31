/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
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

#include <QDebug>

#include "Crop.h"
#include <plvcore/CvMatData.h>
#include <plvcore/Types.h>

#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

CropImage::CropImage() : m_x1(0), m_x2(0), m_y1(0), m_y2(0)
{
    m_in = createCvMatDataInputPin( "image", this );
    m_out = createCvMatDataOutputPin( "out", this );

    m_in->addAllChannels();
    m_in->addAllDepths();

    m_out->addAllDepths();
    m_out->addAllChannels();
}

CropImage::~CropImage()
{
}

bool CropImage::process()
{
    CvMatData img = m_in->get();

    // open input image for reading
    const cv::Mat& mat = img;

    if( m_x1 + m_x2 >= img.width() || m_y1 + m_y2 >= img.height() )
    {
        // cannot execute crop because we would be left with an
        // empty image
        qWarning() << tr("Cannot execute crop because margins combined are larger than the image. ");
    }
    else
    {
        cv::Rect roi( m_x1, m_y1, img.width() - (m_x1 + m_x2), img.height() - (m_y1+m_y2) );
        cv::Mat subregion = mat(roi);
        m_out->put( CvMatData(subregion) );
    }

    return true;
}

int CropImage::getX1()
{
    QMutexLocker lock( m_propertyMutex );
    return m_x1;
}

int CropImage::getX2()
{
    QMutexLocker lock( m_propertyMutex );
    return m_x2;
}

int CropImage::getY1()
{
    QMutexLocker lock( m_propertyMutex );
    return m_y1;
}

int CropImage::getY2()
{
    QMutexLocker lock( m_propertyMutex );
    return m_y2;
}

void CropImage::setX1(int x1)
{
    QMutexLocker lock( m_propertyMutex );

    if( x1 < 0 ) x1 = 0;
    m_x1 = x1;
    emit x1Changed(x1);
}

void CropImage::setX2(int x2)
{
    QMutexLocker lock( m_propertyMutex );

    if( x2 < 0 ) x2 = 0;
    m_x2 = x2;
    emit x2Changed(x2);
}

void CropImage::setY1(int y1)
{
    QMutexLocker lock( m_propertyMutex );

    if( y1 < 0 ) y1 = 0;
    m_y1 = y1;
    emit y1Changed(y1);
}

void CropImage::setY2(int y2)
{
    QMutexLocker lock( m_propertyMutex );

    if( y2 < 0 ) y2 = 0;
    m_y2 = y2;
    emit y2Changed(y2);
}


