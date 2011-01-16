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

#include "ImageCornerHarris.h"

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <plvcore/Util.h>

using namespace plv;
using namespace plvopencv;

ImageCornerHarris::ImageCornerHarris() :
        m_kernelSize(3),
        m_blockSize(3),
        m_k(0.04)
{
    m_inputPin = createCvMatDataInputPin( "input", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputPin->addSupportedDepth( CV_8U );
    m_inputPin->addSupportedDepth( CV_8S );
    m_inputPin->addSupportedChannels( 1 );

    m_outputPin->addSupportedDepth( CV_32F );
    m_outputPin->addSupportedChannels( 1 );

    Util::addDefaultBorderInterpolationTypes( m_borderType );
}

ImageCornerHarris::~ImageCornerHarris()
{
}

void ImageCornerHarris::init()
{
}

void ImageCornerHarris::deinit() throw ()
{
}

void ImageCornerHarris::start()
{
}

void ImageCornerHarris::stop()
{
}

void ImageCornerHarris::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    // get the src image
    CvMatData in = m_inputPin->get();

    // make a target image
    // cv::CornerHarris expects a 32F dst image
    CvMatDataProperties props( in.width(), in.height(), CV_32FC1 );
    CvMatData out = CvMatData::create( props );

    const cv::Mat& src = in;
    cv::Mat& dst = out;

    // do a canny edge detection operator of the image
    cv::cornerHarris( src, dst, m_blockSize, m_kernelSize,
                      m_k, m_borderType.getSelectedValue() );

    // publish the new image
    m_outputPin->put( out );
}

int ImageCornerHarris::getKernelSize() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_kernelSize;
}

int ImageCornerHarris::getBlockSize() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_blockSize;
}

double ImageCornerHarris::getK() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_k;
}

plv::Enum ImageCornerHarris::getBorderType() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_borderType;
}

void ImageCornerHarris::setBlockSize(int i)
{
    QMutexLocker lock( m_propertyMutex );
    if( i < 1 )
        m_blockSize = 1;
    else
        m_blockSize = i;
    emit(blockSizeChanged(i));
}

void ImageCornerHarris::setK( double k )
{
    QMutexLocker lock( m_propertyMutex );
    if( k < 0.0 )
        m_k = 0.0;
    else
        m_k = k;
    emit(kChanged(k));
}

void ImageCornerHarris::setBorderType(plv::Enum bt)
{
    QMutexLocker lock(m_propertyMutex);
    m_borderType = bt;
}

void ImageCornerHarris::setKernelSize(int i)
{
    QMutexLocker lock( m_propertyMutex );

    //aperture size must be odd and positive,
    //min 1 and max 7
    if (i < 1)
        i = 1;
    else if (i > 7)
        i = 7;
    else if( Util::isEven(i) )
    {   //even: determine appropriate new odd value
        //we were increasing -- increase to next odd value
        if( i > m_kernelSize )
            i++;
        //we were decreasing -- decrease to next odd value
        else
            i--;
    }
    m_kernelSize = i;
    emit(kernelSizeChanged(i));
}
