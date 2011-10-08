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
#include <limits>

Stitch::Stitch() :
    destinationWidth(STITCH_DESTINATION_WIDTH_DEFAULT),
    destinationHeight(STITCH_DESTINATION_HEIGHT_DEFAULT),
    in0x(0), in0y(0),
    in1x(0), in1y(0),
    in2x(0), in2y(0),
    in3x(0), in3y(0),
    m_blend(false)
{
    m_inputPin0 = createCvMatDataInputPin( "in1", this, IInputPin::CONNECTION_REQUIRED );
    m_inputPin1 = createCvMatDataInputPin( "in2", this, IInputPin::CONNECTION_REQUIRED );
    m_inputPin2 = createCvMatDataInputPin( "in3", this, IInputPin::CONNECTION_OPTIONAL );
    m_inputPin3 = createCvMatDataInputPin( "in4", this, IInputPin::CONNECTION_OPTIONAL );

    m_outputPin = createCvMatDataOutputPin( "out", this );

    m_inputPin0->addAllChannels();
    m_inputPin0->addSupportedDepth(CV_16U);

    m_inputPin1->addAllChannels();
    m_inputPin1->addSupportedDepth(CV_16U);

    m_inputPin2->addAllChannels();
    m_inputPin2->addSupportedDepth(CV_16U);

    m_inputPin3->addAllChannels();
    m_inputPin3->addSupportedDepth(CV_16U);

    m_outputPin->addSupportedDepth(CV_16U);
    m_outputPin->addSupportedChannels(1);
}

Stitch::~Stitch()
{
}

bool Stitch::process()
{
    plv::CvMatData in0Img = m_inputPin0->get();
    plv::CvMatData in1Img = m_inputPin1->get();

    plv::CvMatData in2Img;
    plv::CvMatData in3Img;

    if( m_inputPin2->isConnected() )
    {
        in2Img = m_inputPin2->get();
    }
    else
    {
        in2Img = CvMatData::create(in1Img.properties());
    }

    if( m_inputPin3->isConnected() )
    {
        in3Img = m_inputPin3->get();
    }
    else
    {
        in3Img = CvMatData::create(in1Img.properties());
    }

    if( in0Img.depth() != in1Img.depth() ||
        in0Img.depth() != in2Img.depth() ||
        in0Img.depth() != in3Img.depth() )
    {
        setError( PlvPipelineRuntimeError, "Not all images have equal depth" );
        return false;
    }

    if( in0Img.channels() != in1Img.channels() ||
        in0Img.channels() != in2Img.channels() ||
        in0Img.channels() != in3Img.channels() )
    {
        setError( PlvPipelineRuntimeError, "Not all images have an equal number of channels" );
        return false;
    }

    // open input images for reading
    const cv::Mat& in0 = in0Img;
    const cv::Mat& in1 = in1Img;
    const cv::Mat& in2 = in2Img;
    const cv::Mat& in3 = in3Img;

    // create output image, output image size may vary depending on parameters
//    int outWidthRow1 = -(in0x) + in0Img.width() + in1Img.width() + in1x;
//    int outWidthRow2 = -(in2x) + in2Img.width() + in3Img.width() + in3x;
//    int outWidth = outWidthRow1 > outWidthRow2 ? outWidthRow1 : outWidthRow2;

//    int outHeightCol1 = in0y + in0Img.height() + in2Img.height() - in2y;
//    int outHeightCol2 = in1y + in1Img.height() + in3Img.height() - in3y;
//    int outHeight = outHeightCol1 > outHeightCol2 ? outHeightCol1 : outHeightCol2;

    int outWidth  = destinationWidth;
    int outHeight = destinationHeight;

//    qDebug() << QString("Creating output image with size (%1, %2, %3, %4)")
//                .arg(outWidth)
//                .arg(outHeight)
//                .arg(in0Img.depth())
//                .arg(in0Img.channels());

    plv::CvMatData outImg = CvMatData::create( outWidth, outHeight, in0Img.depth(), in0Img.channels() );
    cv::Mat& out = outImg;
    
    // set output image to black
    //out = 0;

    copyImgInto( in0, out, in0x, in0y );
    copyImgInto( in1, out, in1x + in0.cols, in1y );
    copyImgInto( in2, out, in2x, in2y + in0.rows );
    copyImgInto( in3, out, in3x + in0.cols, in3y + in0.rows );

    m_outputPin->put(outImg);
    return true;
}

void Stitch::copyImgInto( const cv::Mat& in, cv::Mat& out , int posX, int posY )
{
    int displacementX = posX; // > 0 ? (posX < out.height() ? posX : out.height()) : 0;
    int displacementY = posY; // > 0 ? (posY < out.width()  ? posY : out.width() ) : 0;

    int xStart = displacementX < 0 ? -displacementX : 0;
    int yStart = displacementY < 0 ? -displacementY : 0;
    
    int xStop = in.cols;
    int yStop = in.rows; 
    
    if( (xStop + displacementX) > out.cols ) xStop -= (xStop + displacementX) - out.cols;
    if( (yStop + displacementY) > out.rows ) yStop -= (yStop + displacementY) - out.rows;

    int x,y;

    try
    {
        if( m_blend )
        {
            for( y = yStart; y < yStop; ++y )
            {
                for( x = xStart; x < xStop; ++x )
                {
                    float outval = (float) out.at<unsigned short>(y+displacementY,x+displacementX);
                    float inval =  (float) in.at<unsigned short>(y,x);
                    out.at<unsigned short>(y+displacementY,x+displacementX) = outval > 0 ? (unsigned short) (inval + outval) * 0.5f : inval;
                }
            }
        }
        else
        {
            for( y = yStart; y < yStop; ++y )
            {
                for( x = xStart; x < xStop; ++x )
                {
                   out.at<unsigned short>(y+displacementY,x+displacementX) = in.at<unsigned short>(y,x);
                }
            }
        }
    }
    catch( cv::Exception& e )
    {
        qDebug() << "Exception!";
    }
}

int Stitch::getDestinationHeight()
{
    return destinationHeight;
}

int Stitch::getDestinationWidth()
{
    return destinationWidth;
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

void Stitch::setDestinationHeight( int height )
{
    QMutexLocker lock( m_propertyMutex );
    if( height < 0 ) height = 0;
    if( height > STITCH_DESTINATION_HEIGHT_MAX ) height = STITCH_DESTINATION_HEIGHT_MAX;
    destinationHeight = height;
    emit destinationHeightChanged(height);

}

void Stitch::setDestinationWidth( int width )
{
    QMutexLocker lock( m_propertyMutex );

    if( width < 0 ) width = 0;
    if( width > STITCH_DESTINATION_WIDTH_MAX ) width = STITCH_DESTINATION_WIDTH_MAX;
    destinationWidth = width;
    emit destinationWidthChanged(width);
}

void Stitch::setBlend( bool blend )
{
    QMutexLocker lock( m_propertyMutex );
    if( blend != m_blend ) emit blendChanged(blend);
    m_blend = blend;
}

bool Stitch::getBlend()
{
    QMutexLocker lock( m_propertyMutex );
    return m_blend;
}
