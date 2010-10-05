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
#include "OpenCVImage.h"

#include <plvcore/Pin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

//FIXME:
//why do we need to do all this complicated conversion up and down in process()? Better to just do a simple Canny,
//but refuse to process (i.e., throw away all frames) when input is not proper grayscale.
//maybe also send message or something...
EdgeDetectorCanny::EdgeDetectorCanny() :
        m_apertureSize(3),
        m_thresholdLow(0.1),
        m_thresholdHigh(1.0)
{
    m_inputPin = createInputPin<OpenCVImage>( "input", this );
    m_outputPin = createOutputPin<OpenCVImage>( "output", this );
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
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> img = m_inputPin->get();
    if(img->getDepth() != IPL_DEPTH_8U)
    {
        throw std::runtime_error("format not yet supported");
    }

    // temporary image with extra room (depth), see e.g. http://www.emgu.com/wiki/files/1.5.0.0/Help/html/8b5dffff-5fa5-f3f1-acb4-9adbc60dd7fd.htm
    RefPtr<OpenCVImage> tmp = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_16U , 1 );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // open image for writing
    IplImage* iplImg2 = img2->getImageForWriting();
    IplImage* iplTmp = tmp->getImageForWriting();

    // INPUT REQUIRED TO BE GRAYSCALED! take the first channel as grayscale image
    cvSplit(iplImg1,iplTmp,NULL,NULL,NULL);

    // do a canny edge detection operator of the image
    cvCanny( iplTmp, iplTmp, m_thresholdLow, m_thresholdHigh, m_apertureSize);

    // convert the image back to 8bit depth
//    cvConvertScale(iplTmp, iplImg2, 1, 0);
    cvMerge( iplTmp, iplTmp, iplTmp, NULL, iplImg2 );

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}

/*
void alternativeProcess()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> img = m_inputPin->get();
    if(img->getDepth() != IPL_DEPTH_8U)
    {
        throw std::runtime_error("format not yet supported");
    }

    // temporary image with extra room (depth), see e.g. http://www.emgu.com/wiki/files/1.5.0.0/Help/html/8b5dffff-5fa5-f3f1-acb4-9adbc60dd7fd.htm

    RefPtr<OpenCVImage> tmp = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_16S , img->getNumChannels() );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // perform canny filter
    IplImage* tmpImg = tmp->getImageForWriting();

    cvCanny(iplImg1, tmpImg, m_thresholdLow, m_thresholdHigh, m_apertureSize);

    // scale back to output format
    IplImage* iplImg2 = img2->getImageForWriting();
    cvConvertScale( tmpImg, iplImg2, 1, 0 );

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}
*/
void EdgeDetectorCanny::setApertureSize(int i)
{
    //aperture size must be odd and positive, min 3, max 7 (but that is already way too much for sensible results)
    if (i < 3) i = 3;
    if (i > 7) i = 7;
    if (i%2 == 0)
    {   //even: determine appropriate new odd value
        if (i > m_apertureSize) i++; //we were increasing -- increase to next odd value
        else i--;                    //we were decreasing -- decrease to next odd value
    }
    m_apertureSize = i;
    emit(apertureSizeChanged(m_apertureSize));
}
