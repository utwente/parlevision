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

#include "EdgeDetectorSobel.h"
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <plvcore/Util.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

EdgeDetectorSobel::EdgeDetectorSobel():
        m_xOrder( 1 ),
        m_yOrder( 0 ),
        m_kernelSize(3),
        m_scale( 1 ),
        m_delta( 0 )
{
    m_inputPin  = createCvMatDataInputPin( "input", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    Util::addDefaultBorderInterpolationTypes( m_borderType );

    m_inputPin->addSupportedDepth( CV_8S );
    m_inputPin->addSupportedDepth( CV_8U );
    m_inputPin->addSupportedDepth( CV_32F );
    m_inputPin->addSupportedChannels( 1 );

    m_outputPin->addSupportedDepth( CV_16S );
    m_outputPin->addSupportedDepth( CV_32F );
    m_outputPin->addSupportedChannels( 1 );
}

EdgeDetectorSobel::~EdgeDetectorSobel()
{
}

/**
Calculates the first, second, third or mixed image derivatives using an extended Sobel operator
Parameters:

    * src – The source image
    * dst – The destination image; will have the same size and the same number of channels as src
    * ddepth – The destination image depth
    * xorder – Order of the derivative x
    * yorder – Order of the derivative y
    * ksize – Size of the extended Sobel kernel, must be 1, 3, 5 or 7
    * scale – The optional scale factor for the computed derivative values (by default, no scaling is applied, see getDerivKernels() )
    * delta – The optional delta value, added to the results prior to storing them in dst
    * borderType – The pixel extrapolation method, see borderInterpolate()
*/
bool EdgeDetectorSobel::process()
{
    CvMatData srcPtr = m_inputPin->get();
    CvMatData dstPtr = CvMatData::create( srcPtr.properties() );

    // open for reading
    const cv::Mat& src = srcPtr;

    // open image for writing
    cv::Mat& dst = dstPtr;

    assert( m_xOrder == 1 || m_xOrder == 0 );
    assert( m_yOrder == 1 || m_yOrder == 0 );
    assert( m_kernelSize == 1 || m_kernelSize == 3 || m_kernelSize == 5 || m_kernelSize == 7 );

    // do sobel operation
    cv::Sobel( src, dst, dst.depth() , m_xOrder, m_yOrder,
               m_kernelSize, m_scale, m_delta,
               m_borderType.getSelectedValue() );

    // publish output
    m_outputPin->put( dstPtr );

    return true;
}

/**** getters ******/

bool EdgeDetectorSobel::getXOrder() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_xOrder;
}

bool EdgeDetectorSobel::getYOrder() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_yOrder;
}

int EdgeDetectorSobel::getKernelSize() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_kernelSize;
}

double EdgeDetectorSobel::getScale() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_scale;
}

double EdgeDetectorSobel::getDelta() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_delta;
}

plv::Enum EdgeDetectorSobel::getBorderType() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_borderType;
}

/**** setters ****/

void EdgeDetectorSobel::setXOrder(bool x)
{
    QMutexLocker lock( m_propertyMutex );

    // x is only allowed to be 0 when yOrder is 1
    if( !(!x && !m_yOrder) )
    {
        m_xOrder = x;
    }
    emit(xOrderChanged(m_xOrder));
}

void EdgeDetectorSobel::setYOrder(bool y)
{
    QMutexLocker lock( m_propertyMutex );

    // y is only allowed to be 0 when xOrder is 1
    if( !(!y && !m_xOrder) )
    {
        m_yOrder = y;
    }
    emit(yOrderChanged(m_yOrder));
}

void EdgeDetectorSobel::setKernelSize(int i)
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

void EdgeDetectorSobel::setScale(double scale)
{
    QMutexLocker lock( m_propertyMutex );
    m_scale = scale > 0 ? scale : 0;
    emit( scaleChanged(scale) );
}

void EdgeDetectorSobel::setDelta(double delta)
{
    QMutexLocker lock( m_propertyMutex );
    m_delta = delta;
    emit( deltaChanged(delta) );
}

void EdgeDetectorSobel::setBorderType(plv::Enum bt)
{
    QMutexLocker lock(m_propertyMutex);
    m_borderType = bt;
}


