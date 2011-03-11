/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvopencv module of ParleVision.
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

#include "ImageThreshold.h"
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

ImageThreshold::ImageThreshold() :
        m_threshold( 0.0 ),
        m_maxValue( 255.0 )
{
    m_inputPin = createCvMatDataInputPin( "input", this,  IInputPin::CONNECTION_REQUIRED );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    PLV_ENUM_ADD( m_method, CV_THRESH_BINARY );
    PLV_ENUM_ADD( m_method, CV_THRESH_BINARY_INV );
    PLV_ENUM_ADD( m_method, CV_THRESH_TRUNC );
    PLV_ENUM_ADD( m_method, CV_THRESH_TOZERO );
    PLV_ENUM_ADD( m_method, CV_THRESH_TOZERO_INV );

    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();
}

ImageThreshold::~ImageThreshold(){}

bool ImageThreshold::process()
{
    CvMatData in = m_inputPin->get();
    CvMatData out = CvMatData::create( in.width(), in.height(), in.type() );

    const cv::Mat& src = in;
    cv::Mat& dst = out;

    // perform threshold operation on the image
    cv::threshold( src, dst, m_threshold, m_maxValue, m_method.getSelectedValue() );

    // publish the new image
    m_outputPin->put( out );

    return true;
}

/** propery methods */
plv::Enum ImageThreshold::getMethod() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_method;
}

double ImageThreshold::getThreshold() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_threshold;
}

double ImageThreshold::getMaxValue() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_maxValue;
}

void ImageThreshold::setMethod( plv::Enum m )
{
    QMutexLocker lock( m_propertyMutex );
    m_method = m;
    emit(methodChanged(m_method));
}

void ImageThreshold::setThreshold(double d)
{
    QMutexLocker lock( m_propertyMutex );
    m_threshold = d;
    emit(thresholdChanged(m_threshold));
}

void ImageThreshold::setMaxValue(double d)
{
    QMutexLocker lock( m_propertyMutex );
    m_maxValue = d;
    emit(maxValueChanged(m_maxValue));
}
