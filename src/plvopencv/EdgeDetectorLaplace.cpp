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

#include "EdgeDetectorLaplace.h"
#include "OpenCVImage.h"

#include <plvcore/Pin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

EdgeDetectorLaplace::EdgeDetectorLaplace() :
        m_apertureSize(3)
{
    m_inputPin = createInputPin<OpenCVImage>( "input", this );
    m_outputPin = createOutputPin<OpenCVImage>( "output", this );
}

EdgeDetectorLaplace::~EdgeDetectorLaplace()
{
}

void EdgeDetectorLaplace::init() throw (PipelineException)
{
}

void EdgeDetectorLaplace::deinit() throw ()
{
}

void EdgeDetectorLaplace::start() throw (PipelineException)
{
}

void EdgeDetectorLaplace::stop() throw (PipelineException)
{
}

void EdgeDetectorLaplace::process()
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

    // perform laplace filter
    IplImage* tmpImg = tmp->getImageForWriting();

    //FIXME: it seems that this is a openCV2.0 function; the 2.1 function looks different
    //see http://opencv.willowgarage.com/documentation/cpp/image_filtering.html
    cvLaplace( iplImg1, tmpImg, this->m_apertureSize );

    // scale back to output format
    IplImage* iplImg2 = img2->getImageForWriting();
    cvConvertScale( tmpImg, iplImg2, 1, 0 );

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}

void EdgeDetectorLaplace::setApertureSize(int i)
{
    //aperture size must be odd and positive, max 31 (but that is already way too much for sensible results)
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
