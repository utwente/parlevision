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
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

EdgeDetectorSobel::EdgeDetectorSobel():
        m_apertureSize(3)
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

EdgeDetectorSobel::~EdgeDetectorSobel()
{
}

void EdgeDetectorSobel::init() throw (PipelineException)
{
}

void EdgeDetectorSobel::start() throw (PipelineException)
{
}

void EdgeDetectorSobel::stop() throw (PipelineException)
{
}

bool EdgeDetectorSobel::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void EdgeDetectorSobel::process()
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

    // open image for writing
    IplImage* iplImg2 = img2->getImageForWriting();
    IplImage* iplTmp = tmp->getImageForWriting();

    // do a sobel operator of the image
    //FIXME: missing parameters for x_order and y_order
    cvSobel( iplImg1, iplTmp, 1,0,m_apertureSize);
    // convert the image back to 8bit depth
    cvConvertScale(iplTmp, iplImg2, 1, 0);

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}


void EdgeDetectorSobel::setApertureSize(int i)
{
    //aperture size must be odd and positive, max 31 (?)
    if (i < 1) i = 1;
    if (i > 31) i = 31;
    if (i%2 == 0)
    {   //even: determine appropriate new odd value
        if (i > m_apertureSize) i++; //we were increasing -- increase to next odd value
        else i--;                    //we were decreasing -- decrease to next odd value
    }
    m_apertureSize = i;
    emit(apertureSizeChanged(m_apertureSize));
}

