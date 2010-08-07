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

#include "ImageCornerHarris.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"


//FIXME: [DR] all properties must be validated
ImageCornerHarris::ImageCornerHarris() :
        m_apertureSize(3),
        m_blockSize(3),
        m_k(0.04)
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

ImageCornerHarris::~ImageCornerHarris()
{
}

void ImageCornerHarris::init() throw (PipelineException)
{
}

bool ImageCornerHarris::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void ImageCornerHarris::process()
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
            img->getWidth(), img->getHeight(), IPL_DEPTH_8U , 1 );
    RefPtr<OpenCVImage> tmp2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_32F , 1 );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_32F, img->getNumChannels() );
    RefPtr<OpenCVImage> img3 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // open image for writing, lots of unused images
    IplImage* iplImg2 = img2->getImageForWriting();
    IplImage* iplImg3 = img3->getImageForWriting();
    IplImage* iplTmp = tmp->getImageForWriting();
    IplImage* iplTmp2 = tmp2->getImageForWriting();

    // INPUT REQUIRED TO BE GRAYSCALED! take the first channel as grayscale image
    cvSplit(iplImg1,iplTmp,NULL,NULL,NULL);

    // do a canny edge detection operator of the image
    cvCornerHarris( iplTmp, iplTmp2, m_blockSize, m_apertureSize, m_k);

    // convert the image back to 8bit depth
    cvMerge( iplTmp2, iplTmp2, iplTmp2, NULL, iplImg2 );
    cvConvertScale(iplImg2, iplImg3, 1, 0);

    // publish the new image
    m_outputPin->put( img3.getPtr() );
}
