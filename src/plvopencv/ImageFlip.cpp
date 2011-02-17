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

#include "ImageFlip.h"
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>

using namespace plv;
using namespace plvopencv;

ImageFlip::ImageFlip()
{
    m_inputPin  = createCvMatDataInputPin( "input", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();

    // flipCode – Specifies how to flip the array: 0 means flipping
    // around the x-axis, positive (e.g., 1) means flipping around y-axis,
    // and negative (e.g., -1) means flipping around both axes.
    // See also the discussion below for the formulas.
    m_method.add( "flip around x and y-axis", -1 );
    m_method.add( "flip around x-axis", 0 );
    m_method.add( "flip around y-axis", 1 );
}

ImageFlip::~ImageFlip() {}

bool ImageFlip::process()
{
    CvMatData in = m_inputPin->get();
    CvMatData out = CvMatData::create(in.properties());

    // open for reading
    const cv::Mat& src = in;

    // open image for writing
    cv::Mat& target = out;

    // do a flip of the image
    cv::flip( src, target, m_method.getSelectedValue() );

    // publish the new image
    m_outputPin->put( target );

    return true;
}

void ImageFlip::setMethod( plv::Enum e )
{
    QMutexLocker lock( m_propertyMutex );
    m_method = e;
    emit( methodChanged(e) );
}

plv::Enum ImageFlip::getMethod() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_method;
}
