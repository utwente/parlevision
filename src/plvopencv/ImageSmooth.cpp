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

#include "ImageSmooth.h"

#include <plvcore/OpenCVImage.h>
#include <plvcore/OpenCVImagePin.h>
#include <plvcore/Util.h>

using namespace plv;
using namespace plvopencv;

ImageSmooth::ImageSmooth() :
        m_one(3),
        m_two(3),
        m_three(0.0),
        m_four(0.0)
{
    m_inputPin = createOpenCVImageInputPin( "input", this );
    m_outputPin = createOpenCVImageOutputPin( "output", this );

    PLV_ENUM_ADD( m_method, CV_BLUR_NO_SCALE );
    PLV_ENUM_ADD( m_method, CV_BLUR );
    PLV_ENUM_ADD( m_method, CV_GAUSSIAN );
    PLV_ENUM_ADD( m_method, CV_MEDIAN );
    PLV_ENUM_ADD( m_method, CV_BILATERAL );
}

ImageSmooth::~ImageSmooth()
{
}

void ImageSmooth::init()
{
}

void ImageSmooth::deinit() throw ()
{
}

void ImageSmooth::start()
{
}

void ImageSmooth::stop()
{
}

void ImageSmooth::process()
{
    RefPtr<OpenCVImage> srcPtr = m_inputPin->get();
    RefPtr<OpenCVImage> dstPtr = OpenCVImageFactory::get( srcPtr->getProperties() );

    // open for reading
    const IplImage* src = srcPtr->getImage();

    // open image for writing
    IplImage* dst = dstPtr->getImageForWriting();

    // perform smooth operation on the image
    cvSmooth( src, dst, m_method.getSelectedValue(), m_one, m_two, m_three, m_four);

    // publish the new image
    m_outputPin->put( dstPtr );
}

void ImageSmooth::setOne(int i)
{
    QMutexLocker lock( m_propertyMutex );

    //aperture size must be odd and positive
    if (i < 1)
        i = 1;
    else if( isEven(i) )
    {   //even: determine appropriate new odd value
        //we were increasing -- increase to next odd value
        if (i > m_one)
            i++;
        //we were decreasing -- decrease to next odd value
        else
            i--;
    }
    m_one = i;
    emit(oneChanged(m_one));
}

void ImageSmooth::setTwo(int i)
{
    //aperture size must be odd and positive
    if (i < 1)
        i = 1;
    else if( isEven(i) )
    {   //even: determine appropriate new odd value
        if (i > m_two) i++; //we were increasing -- increase to next odd value
        else i--;           //we were decreasing -- decrease to next odd value
    }
    m_two = i;
    emit(twoChanged(m_two));
}
