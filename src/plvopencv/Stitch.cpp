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

#include "Stitch.h"

#include <plvcore/CvMatDataPin.h>
#include <plvcore/CvMatData.h>

using namespace plv;
using namespace plvopencv;

#include <vector>

Stitch::Stitch() :
    in0x(0), in0y(0),
    in1x(0), in1y(0),
    in2x(0), in2y(0),
    in3x(0), in3y(0)
{
    m_inputPin0 = createCvMatDataInputPin( "in1", this, IInputPin::CONNECTION_REQUIRED );
    m_inputPin1 = createCvMatDataInputPin( "in2", this, IInputPin::CONNECTION_REQUIRED );
    m_inputPin2 = createCvMatDataInputPin( "in3", this, IInputPin::CONNECTION_REQUIRED );
    m_inputPin3 = createCvMatDataInputPin( "in4", this, IInputPin::CONNECTION_REQUIRED );

    m_outputPin = createCvMatDataOutputPin( "out", this );

    m_inputPin0->addAllChannels();
    m_inputPin0->addAllDepths();

    m_inputPin1->addAllChannels();
    m_inputPin1->addAllDepths();

    m_inputPin2->addAllChannels();
    m_inputPin2->addAllDepths();

    m_inputPin3->addAllChannels();
    m_inputPin3->addAllDepths();

    m_outputPin->addAllDepths();
    m_outputPin->addSupportedChannels(1);

}

Stitch::~Stitch()
{
}

bool Stitch::process()
{
    plv::CvMatData in0Img = m_inputPin0->get();
    plv::CvMatData in1Img = m_inputPin1->get();
    plv::CvMatData in2Img = m_inputPin2->get();
    plv::CvMatData in3Img = m_inputPin3->get();

    if( in0Img.properties() != in1Img.properties() ||
        in0Img.properties() != in2Img.properties() ||
        in0Img.properties() != in3Img.properties() )
    {
        setError( PlvPipelineRuntimeError, "Not all images have equal properties" );
        return false;
    }

    // open input images for reading
    const cv::Mat& in0 = in0Img;
    const cv::Mat& in1 = in1Img;
    const cv::Mat& in2 = in2Img;
    const cv::Mat& in3 = in3Img;

    // create output image
    plv::CvMatData outImg = CvMatData::create( in0Img.width() * 2, in0Img.height() * 2, in0Img.depth(), in0Img.channels() );

    cv::Mat& out = outImg;

    int width  = in0Img.width();
    int height = in0Img.height();

    int yFrom = in0y;
    int yTo   = height + in0y;
    int xFrom = in0x;
    int xTo   = width + in0x;

    if( yFrom < 0 ) yFrom = 0;
    if( xFrom < 0 ) xFrom = 0;

    if( yTo > in0Img.height() ) yTo = in0Img.height();
    if( xTo > in0Img.width() )  xTo = in0Img.width();

    for( int y = yFrom; y < yTo ; y++ )
    {
        for( int x = xFrom ; x < xTo ; x++ )
        {
            out.at<unsigned short>(y,x) = in0.at<unsigned short>(y,x);
        }
    }

    for( int y = 0 ; y < height ; y++ )
    {
        for( int x = 0 ; x < width ; x++ )
        {
            out.at<unsigned short>(y+height,x) = in1.at<unsigned short>(y,x);
        }
    }

    for( int y = 0 ; y < height ; y++ )
    {
        for( int x = 0 ; x < width ; x++ )
        {
            out.at<unsigned short>(y,x+width) = in2.at<unsigned short>(y,x);
        }
    }

    for( int y = 0 ; y < height ; y++ )
    {
        for( int x = 0 ; x < width ; x++ )
        {
            out.at<unsigned short>(y+height,x+width) = in3.at<unsigned short>(y,x);
        }
    }

    m_outputPin->put(outImg);

    return true;
}

int Stitch::getIn0X()
{
    QMutexLocker lock( m_propertyMutex );
    return in0x;
}

int Stitch::getIn0Y()
{
    QMutexLocker lock( m_propertyMutex );
    return in0y;
}

void Stitch::setIn0X( int value )
{
    QMutexLocker lock( m_propertyMutex );
    in0x = value;
}

void Stitch::setIn0Y( int value )
{
    QMutexLocker lock( m_propertyMutex );
    in0y = value;
}

int Stitch::getIn1X()
{
    QMutexLocker lock( m_propertyMutex );
    return in1x;
}

int Stitch::getIn1Y()
{
    QMutexLocker lock( m_propertyMutex );
    return in1y;
}

void Stitch::setIn1X( int value )
{
    QMutexLocker lock( m_propertyMutex );
    in1x = value;
}

void Stitch::setIn1Y( int value )
{
    QMutexLocker lock( m_propertyMutex );
    in1y = value;
}

int Stitch::getIn2X()
{
    QMutexLocker lock( m_propertyMutex );
    return in2x;
}

int Stitch::getIn2Y()
{
    QMutexLocker lock( m_propertyMutex );
    return in2y;
}

void Stitch::setIn2X( int value )
{
    QMutexLocker lock( m_propertyMutex );
    in2x = value;
}

void Stitch::setIn2Y( int value )
{
    QMutexLocker lock( m_propertyMutex );
    in2y = value;
}

int Stitch::getIn3X()
{
    QMutexLocker lock( m_propertyMutex );
    return in3x;
}

int Stitch::getIn3Y()
{
    QMutexLocker lock( m_propertyMutex );
    return in3y;
}

void Stitch::setIn3X( int value )
{
    QMutexLocker lock( m_propertyMutex );
    in3x = value;
}

void Stitch::setIn3Y( int value )
{
    QMutexLocker lock( m_propertyMutex );
    in3y = value;
}
