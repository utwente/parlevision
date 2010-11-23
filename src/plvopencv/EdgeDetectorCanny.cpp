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

#include <plvcore/OpenCVImage.h>
#include <plvcore/OpenCVImagePin.h>
#include <plvcore/Util.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

EdgeDetectorCanny::EdgeDetectorCanny() :
        m_apertureSize(3),
        m_thresholdLow(0.1),
        m_thresholdHigh(1.0)
{
    m_inputPin  = new OpenCVImageInputPin( "input", this );
    m_outputPin = new OpenCVImageOutputPin( "output", this );

    this->addInputPin( m_inputPin );
    this->addOutputPin( m_outputPin );

    m_inputPin->addSupportedDepth( IPL_DEPTH_8S );
    m_inputPin->addSupportedDepth( IPL_DEPTH_8U );
    m_inputPin->addSupportedChannels( 1 );

    m_outputPin->addSupportedDepth( IPL_DEPTH_8S );
    m_outputPin->addSupportedDepth( IPL_DEPTH_8U );
    m_outputPin->addSupportedChannels( 1 );
}

EdgeDetectorCanny::~EdgeDetectorCanny()
{
}

void EdgeDetectorCanny::init()
{
}

void EdgeDetectorCanny::deinit() throw ()
{
}

void EdgeDetectorCanny::start()
{
}

void EdgeDetectorCanny::stop()
{
}

void EdgeDetectorCanny::process()
{
    // get the source
    RefPtr<OpenCVImage> srcPtr = m_inputPin->get();

    // get a new target image with the same properties as the src
    RefPtr<OpenCVImage> targetPtr = OpenCVImageFactory::get( srcPtr->getProperties() );

    // open for reading
    const IplImage* src = srcPtr->getImage();

    // open image for writing
    IplImage* target = targetPtr->getImageForWriting();

    // do a canny edge detection operator of the image
    // the input should be grayscaled
    cvCanny( src, target, m_thresholdLow, m_thresholdHigh, m_apertureSize);

    // publish the new image
    m_outputPin->put( targetPtr );
}

void EdgeDetectorCanny::setApertureSize(int i)
{
    //aperture size must be odd and positive, min 3, max 7 (but that is already way too much for sensible results)
    if (i < 3)
        i = 3;
    else if (i > 7)
        i = 7;
    else if( isEven(i) )
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
