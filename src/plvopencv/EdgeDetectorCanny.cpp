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

#include "EdgeDetectorCanny.h"

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <plvcore/Util.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

EdgeDetectorCanny::EdgeDetectorCanny() :
        m_apertureSize(3),
        m_thresholdLow(0.1),
        m_thresholdHigh(1.0)
{
    m_inputPin  = createCvMatDataInputPin( "input", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputPin->addSupportedDepth( CV_8S );
    m_inputPin->addSupportedDepth( CV_8U );
    m_inputPin->addSupportedChannels( 1 );

    m_outputPin->addSupportedDepth( CV_8S );
    m_outputPin->addSupportedDepth( CV_8U );
    m_outputPin->addSupportedChannels( 1 );
}

EdgeDetectorCanny::~EdgeDetectorCanny()
{
}

bool EdgeDetectorCanny::process()
{
    // get the source
    CvMatData in = m_inputPin->get();

    // get a new target image with the same properties as the src
    CvMatData out = CvMatData::create( in.properties() );

    // open for reading
    const cv::Mat& src = in;

    // open image for writing
    cv::Mat& edges = out;

    // do a canny edge detection operator of the image
    // the input should be grayscaled
    cv::Canny( src, edges, m_thresholdLow, m_thresholdHigh, m_apertureSize, m_l2Gradient );

    // publish the new image
    m_outputPin->put( out );

    return true;
}

int EdgeDetectorCanny::getApertureSize() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_apertureSize;
}

double EdgeDetectorCanny::getThresholdLow() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_thresholdLow;
}

double EdgeDetectorCanny::getThresholdHigh() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_thresholdHigh;
}

bool EdgeDetectorCanny::getL2Gradient() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_l2Gradient;
}

void EdgeDetectorCanny::setApertureSize(int i)
{
    QMutexLocker lock( m_propertyMutex );
    //aperture size must be odd and positive, min 3, max 7 (but that is already way too much for sensible results)
    if (i < 3)
        i = 3;
    else if (i > 7)
        i = 7;
    else if( Util::isEven(i) )
    {   //even: determine appropriate new odd value
        //we were increasing -- increase to next odd value
        if( i > m_apertureSize )
            i++;
        //we were decreasing -- decrease to next odd value
        else
            i--;
    }
    m_apertureSize = i;
    emit(apertureSizeChanged(m_apertureSize));
}

void EdgeDetectorCanny::setThresholdLow (double newValue)
{
    QMutexLocker lock( m_propertyMutex );
    m_thresholdLow  = newValue;
    emit(thresholdLowChanged(newValue));
}

void EdgeDetectorCanny::setThresholdHigh(double newValue)
{
    QMutexLocker lock( m_propertyMutex );
    m_thresholdHigh = newValue;
    emit(thresholdHighChanged(newValue));
}

void EdgeDetectorCanny::setL2Gradient(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_l2Gradient = b;
    emit( l2GradientChanged(b) );
}
