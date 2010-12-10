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

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <plvcore/Util.h>

using namespace plv;
using namespace plvopencv;

ImageSmooth::ImageSmooth() :
        m_kernelSizeWidth(1),
        m_kernelSizeHeight(1),
        m_sigmaOne(0),
        m_sigmaTwo(0)
{
    m_inputPin  = createCvMatDataInputPin( "input", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    addDefaultBorderInterpolationTypes( m_borderType );
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
    CvMatData srcPtr = m_inputPin->get();
    CvMatData dstPtr = CvMatData::create( srcPtr.getProperties() );

    // open for reading
    const cv::Mat& src = srcPtr;

    // open image for writing
    cv::Mat& dst = dstPtr;

    // perform smooth operation on the image
    // Smoothes image using a Gaussian filter
    // Parameters:
    // * src – The source image
    // * dst – The destination image; will have the same size and the same type as src
    // * ksize – The Gaussian kernel size; ksize.width and ksize.height can differ, but they both must be positive and odd. Or, they can be zero’s, then they are computed from sigma*
    // * sigmaX, sigmaY – The Gaussian kernel standard deviations in X and Y direction. If sigmaY is zero, it is set to be equal to sigmaX . If they are both zeros, they are computed from ksize.width and ksize.height , respectively, see getGaussianKernel() . To fully control the result regardless of possible future modification of all this semantics, it is recommended to specify all of ksize , sigmaX and sigmaY
    // * borderType – The pixel extrapolation method; see borderInterpolate()
    cv::GaussianBlur( src, dst, cv::Size(m_kernelSizeWidth,m_kernelSizeHeight),
                      m_sigmaOne, m_sigmaTwo,
                      m_borderType.getSelectedValue() );

    // publish the new image
    m_outputPin->put( dstPtr );
}

void ImageSmooth::setKernelSizeWidth(int i)
{
    QMutexLocker lock( m_propertyMutex );

    //aperture size must be odd and positive
    if (i < 1)
        i = 1;
    else if( isEven(i) )
    {   //even: determine appropriate new odd value
        //we were increasing -- increase to next odd value
        if (i > m_kernelSizeWidth)
            i++;
        //we were decreasing -- decrease to next odd value
        else
            i--;
    }
    m_kernelSizeWidth = i;
    emit(kernelSizeWidthChanged((m_kernelSizeWidth)));
}

void ImageSmooth::setKernelSizeHeight(int i)
{
    QMutexLocker lock( m_propertyMutex );

    //aperture size must be odd and positive
    if (i < 1)
        i = 1;
    else if( isEven(i) )
    {   //even: determine appropriate new odd value
        //we were increasing -- increase to next odd value
        if (i > m_kernelSizeHeight)
            i++;
        //we were decreasing -- decrease to next odd value
        else
            i--;
    }
    m_kernelSizeHeight = i;
    emit(kernelSizeHeightChanged((m_kernelSizeHeight)));
}

void ImageSmooth::setBorderType(plv::Enum bt)
{
    QMutexLocker lock(m_propertyMutex);
    m_borderType = bt;
}

void ImageSmooth::setSigmaOne(double s1)
{
    QMutexLocker lock(m_propertyMutex);
    m_sigmaOne = s1 > 0 ? s1 : 0;
    emit(sigmaOneChanged(m_sigmaOne));
}

void ImageSmooth::setSigmaTwo(double s2)
{
    QMutexLocker lock(m_propertyMutex);
    m_sigmaTwo = s2 > 0 ? s2 : 0;
    emit(sigmaTwoChanged(m_sigmaTwo));
}

int ImageSmooth::getKernelSizeWidth()
{
    QMutexLocker lock(m_propertyMutex);
    return m_kernelSizeWidth;
}

int ImageSmooth::getKernelSizeHeight()
{
    QMutexLocker lock(m_propertyMutex);
    return m_kernelSizeHeight;
}

double ImageSmooth::getSigmaOne()
{
    QMutexLocker lock(m_propertyMutex);
    return m_sigmaOne;
}

double ImageSmooth::getSigmaTwo()
{
    QMutexLocker lock(m_propertyMutex);
    return m_sigmaTwo;
}

plv::Enum ImageSmooth::getBorderType()
{
    QMutexLocker lock(m_propertyMutex);
    return m_borderType;
}
