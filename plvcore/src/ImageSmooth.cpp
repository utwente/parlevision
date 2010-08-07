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
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"


//FIXME: [DR] properties must be validated on change, see e.g. laplace edge detector for example

#define PLV_ENUM_ADD( plvEnum, type ) plvEnum.add( #type, type )

ImageSmooth::ImageSmooth() :
        m_one(3),
        m_two(3),
        m_three(0.0),
        m_four(0.0)
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );

    PLV_ENUM_ADD( m_method, CV_BLUR_NO_SCALE );
    PLV_ENUM_ADD( m_method, CV_BLUR );
    PLV_ENUM_ADD( m_method, CV_GAUSSIAN );
    PLV_ENUM_ADD( m_method, CV_MEDIAN );
    PLV_ENUM_ADD( m_method, CV_BILATERAL );

}

ImageSmooth::~ImageSmooth()
{
}

void ImageSmooth::init() throw (PipelineException)
{
}

bool ImageSmooth::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void ImageSmooth::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> img = m_inputPin->get();
    if(img->getDepth() != IPL_DEPTH_8U)
    {
        throw std::runtime_error("format not yet supported");
    }

    // temporary image with extra room (depth)
    RefPtr<OpenCVImage> tmp = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_16S , img->getNumChannels() );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImgIn = img->getImage();

    // open image for writing
    IplImage* iplImgOut = img2->getImageForWriting();

    // perform smooth operation on the image
    cvSmooth( iplImgIn, iplImgOut, m_method.getSelectedValue(), m_one, m_two, m_three, m_four);

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}

void ImageSmooth::setOne(int i)
{
    //aperture size must be odd and positive
    if (i < 1) i = 1;
    if (i%2 == 0)
    {   //even: determine appropriate new odd value
        if (i > m_one) i++; //we were increasing -- increase to next odd value
        else i--;                    //we were decreasing -- decrease to next odd value
    }
    m_one = i;
    emit(oneChanged(m_one));
}

void ImageSmooth::setTwo(int i)
{
    //aperture size must be odd and positive
    if (i < 1) i = 1;
    if (i%2 == 0)
    {   //even: determine appropriate new odd value
        if (i > m_two) i++; //we were increasing -- increase to next odd value
        else i--;                    //we were decreasing -- decrease to next odd value
    }
    m_two = i;
    emit(twoChanged(m_two));
}
