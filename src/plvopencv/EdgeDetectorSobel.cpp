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
        m_apertureSize(3),
        m_Scharr( false )
{
    m_inputPin  = createCvMatDataInputPin( "input", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputPin->addSupportedDepth( IPL_DEPTH_8S );
    m_inputPin->addSupportedDepth( IPL_DEPTH_8U );
    m_inputPin->addSupportedDepth( IPL_DEPTH_32F );
    m_inputPin->addSupportedChannels( 1 );

    m_outputPin->addSupportedDepth( IPL_DEPTH_16S );
    m_outputPin->addSupportedDepth( IPL_DEPTH_32F );
    m_outputPin->addSupportedChannels( 1 );
}

EdgeDetectorSobel::~EdgeDetectorSobel()
{
}

void EdgeDetectorSobel::init()
{
}

void EdgeDetectorSobel::deinit() throw ()
{
}

void EdgeDetectorSobel::start()
{
}

void EdgeDetectorSobel::stop()
{
}

void EdgeDetectorSobel::process()
{
    CvMatData srcPtr = m_inputPin->get();

    // destination image should be at least 16 bits avoid overflow of Sobel operation
    // see e.g. http://opencv.willowgarage.com/documentation/image_filtering.html?highlight=cvsobel#cvSobel
    OpenCVImageProperties props = srcPtr->getProperties();
    if( props.getDepth() != IPL_DEPTH_32F )
    {
        props.setDepth( IPL_DEPTH_16S );
    }
    CvMatData dstPtr = CvMatData::create( props );

    // open for reading
    const IplImage* src = srcPtr->getImage();

    // open image for writing
    IplImage* dst = dstPtr->getImageForWriting();

    // do a sobel operator of the image
    // Parameters:
    //  * src – Source image of type CvArr*
    //  * dst – Destination image
    //  * xorder – Order of the derivative x
    //  * yorder – Order of the derivative y
    //  * apertureSize – Size of the extended Sobel kernel, must be 1, 3, 5 or 7
    assert( m_xOrder == 1 || m_xOrder == 0 );
    assert( m_yOrder == 1 || m_yOrder == 0 );
    assert( m_apertureSize == 1 || m_apertureSize == 3 || m_apertureSize == 5 || m_apertureSize == 7 );

    // check for Scharr and use appropriate aperture size
    int apertureSize = m_Scharr ? CV_SCHARR : m_apertureSize;

    // do sobel operation
    cvSobel( src, dst, (int)m_xOrder, (int)m_yOrder, apertureSize );

    // publish output
    m_outputPin->put( dstPtr );
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

bool EdgeDetectorSobel::getScharr() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_Scharr;
}

int EdgeDetectorSobel::getApertureSize() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_apertureSize;
}


/**** setters ****/

void EdgeDetectorSobel::setXOrder(bool x)
{
    QMutexLocker lock( m_propertyMutex );

    if( m_Scharr )
    {
        // x and y are not allowed to both be 1
        if( x )
        {
            m_xOrder = true;
            m_yOrder = false;
        }
        else
        {
            m_xOrder = false;
            m_yOrder = true;
        }
        emit(xOrderChanged(m_xOrder));
        emit(yOrderChanged(m_yOrder));
    }
    else
    {
        // x is only allowed to be 0 when yOrder is 1
        if( !(!x && !m_yOrder) )
        {
            m_xOrder = x;
        }
        emit(xOrderChanged(m_xOrder));
    }
}

void EdgeDetectorSobel::setYOrder(bool y)
{
    QMutexLocker lock( m_propertyMutex );

    if( m_Scharr )
    {
        // x and y are not allowed to both be 1
        if( y )
        {
            m_xOrder = false;
            m_yOrder = true;
        }
        else
        {
            m_xOrder = true;
            m_yOrder = false;
        }
        emit(xOrderChanged(m_xOrder));
        emit(yOrderChanged(m_yOrder));
    }
    else
    {
        // y is only allowed to be 0 when xOrder is 1
        if( !(!y && !m_xOrder) )
        {
            m_yOrder = y;
        }
        emit(yOrderChanged(m_yOrder));
    }
}

void EdgeDetectorSobel::setApertureSize(int i)
{
    QMutexLocker lock( m_propertyMutex );

    //aperture size must be odd and positive,
    //min 1 and max 7
    if (i < 1)
        i = 1;
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
    emit(apertureSizeChanged(i));
}

void EdgeDetectorSobel::setScharr(bool useScharr)
{
    QMutexLocker lock( m_propertyMutex );

    if( useScharr )
    {
        // in Scharr mode x order and y order are mutually exclusive
        if( m_xOrder && m_yOrder )
        {
            m_yOrder = false;
        }
        emit( yOrderChanged(m_yOrder) );
    }

    m_Scharr = useScharr;
    emit( ScharrChanged(useScharr) );
}


