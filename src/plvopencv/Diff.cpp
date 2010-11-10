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

#include "Diff.h"
#include <plvcore/OpenCVImage.h>

#include <plvcore/OpenCVImagePin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Diff::Diff()
{
    m_inputPin1 = createOpenCVImageInputPin( "input 1", this );
    m_inputPin2 = createOpenCVImageInputPin( "input 2", this );
    m_outputPin = createOpenCVImageOutputPin( "output", this );

    m_inputPin1->addAllChannels();
    m_inputPin1->addAllDepths();

    m_inputPin2->addAllChannels();
    m_inputPin2->addAllDepths();

    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();
}

Diff::~Diff()
{
}

void Diff::init()
{
}

void Diff::deinit() throw ()
{
}

void Diff::start()
{
}

void Diff::stop()
{
}

void Diff::process()
{
    RefPtr<OpenCVImage> img1 = m_inputPin1->get();
    RefPtr<OpenCVImage> img2 = m_inputPin2->get();

    //check format of images?
    if(!img1->isCompatible(img2))
    {
        //TODO: we could use some modifications when the images do not match
        // -- e.g., copy one of the mismatching images into a duplicate that
        // DOES match (stretch? shrink? Diff depth?)
        throw plv::PlvRuntimeException("The two images need to be the same "
                                       "in depth, size and nr of channels",
                                       __FILE__, __LINE__ );
    }

    // open input images for reading
    const IplImage* iplImgIn1 = img1->getImage();
    const IplImage* iplImgIn2 = img2->getImage();

    //get a new output image of same depth and size as input image
    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::get( img1->getProperties() );

    // open output image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();

    // do the diff function
    cvAbsDiff(iplImgIn1,iplImgIn2,iplImgOut);

    // publish the new image
    m_outputPin->put( imgOut );
}

