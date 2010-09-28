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

#include "Sub.h"
#include "OpenCVImage.h"

#include <plvcore/Pin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Sub::Sub()
{
    m_inputPin1 = createInputPin<OpenCVImage>( "input 1", this );
    m_inputPin2 = createInputPin<OpenCVImage>( "input 2", this );
    m_outputPin = createOutputPin<OpenCVImage>( "output", this );
}

Sub::~Sub()
{
}

void Sub::init() throw (PipelineException)
{
}

void Sub::deinit() throw ()
{
}

void Sub::start() throw (PipelineException)
{
}

void Sub::stop() throw (PipelineException)
{
}

void Sub::process()
{
    assert(m_inputPin1 != 0);
    assert(m_inputPin2 != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> img1 = m_inputPin1->get();
    RefPtr<OpenCVImage> img2 = m_inputPin2->get();

    // open input images for reading
    const IplImage* iplImgIn1 = img1->getImage();
    const IplImage* iplImgIn2 = img2->getImage();

    //check format of images?
    if( !img1->isCompatible( img2.getPtr() ))
    {
        // TODO: we could use some modifications when the images do not match --
        // e.g., copy one of the mismatching images into a duplicate that DOES match (stretch? shrink? Sub depth?)
        throw std::runtime_error("The two images need to be the same in depth, size and nr of channels");
    }

    //get a new output image of same depth and size as input image
    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::instance()->get(
            img1->getWidth(), img1->getHeight(), img1->getDepth(), img1->getNumChannels() );

    // open output image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();

    //subtract 2nd source image from 1st. Store in iplImgOut.
    cvSub(iplImgIn1,iplImgIn2,iplImgOut);

    // publish the new image
    m_outputPin->put( imgOut.getPtr() );
}
