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

#include "ImageColorConvert.h"
#include "OpenCVImage.h"

#include <plvcore/Pin.h>

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

using namespace plv;
using namespace plvopencv;

/* Constants for color conversion
CV_BGR2BGRA
CV_RGB2RGBA
CV_BGRA2BGR
CV_RGBA2RGB

CV_BGR2RGBA
CV_RGB2BGRA

CV_RGBA2BGR
CV_BGRA2RGB

CV_BGR2RGB
CV_RGB2BGR

CV_BGRA2RGBA
CV_RGBA2BGRA

CV_BGR2GRAY
CV_RGB2GRAY
CV_GRAY2BGR
CV_GRAY2RGB
CV_GRAY2BGRA
CV_GRAY2RGBA
CV_BGRA2GRAY
CV_RGBA2GRAY

CV_BGR2BGR565
CV_RGB2BGR565
CV_BGR5652BGR
CV_BGR5652RGB
CV_BGRA2BGR565
CV_RGBA2BGR565
CV_BGR5652BGRA
CV_BGR5652RGBA

CV_GRAY2BGR565
CV_BGR5652GRAY

CV_BGR2BGR555
CV_RGB2BGR555
CV_BGR5552BGR
CV_BGR5552RGB
CV_BGRA2BGR555
CV_RGBA2BGR555
CV_BGR5552BGRA
CV_BGR5552RGBA

CV_GRAY2BGR555
CV_BGR5552GRAY

CV_BGR2XYZ
CV_RGB2XYZ
CV_XYZ2BGR
CV_XYZ2RGB

CV_BGR2YCrCb
CV_RGB2YCrCb
CV_YCrCb2BGR
CV_YCrCb2RGB

CV_BGR2HSV
CV_RGB2HSV

CV_BGR2Lab
CV_RGB2Lab

CV_BayerBG2BGR
CV_BayerGB2BGR
CV_BayerRG2BGR
CV_BayerGR2BGR

CV_BayerBG2RGB
CV_BayerGB2RGB
CV_BayerRG2RGB
CV_BayerGR2RGB

CV_BGR2Luv
CV_RGB2Luv
CV_BGR2HLS
CV_RGB2HLS

CV_HSV2BGR
CV_HSV2RGB

CV_Lab2BGR
CV_Lab2RGB
CV_Luv2BGR
CV_Luv2RGB
CV_HLS2BGR
CV_HLS2RGB
*/

#define PLV_ENUM_ADD( plvEnum, type ) plvEnum.add( #type, type )

ImageColorConvert::ImageColorConvert()
{
    m_inputPin = createInputPin<OpenCVImage>( "input", this );
    m_outputPin = createOutputPin<OpenCVImage>( "output", this );

    // first one added is default
    PLV_ENUM_ADD( m_conversionType, CV_RGB2GRAY );
    PLV_ENUM_ADD( m_conversionType, CV_RGB2RGBA );
}

ImageColorConvert::~ImageColorConvert()
{
}

void ImageColorConvert::init()
{
}

void ImageColorConvert::deinit() throw ()
{
}

void ImageColorConvert::start()
{
}

void ImageColorConvert::stop()
{
}

void ImageColorConvert::process()
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
            img->getWidth(), img->getHeight(), img->getDepth(), 1 );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // open image for writing
    IplImage* iplImg2 = img2->getImageForWriting();
    IplImage* iplTmp = tmp->getImageForWriting();

    cvCvtColor(iplImg1, iplTmp, m_conversionType.getSelectedValue());

    // merge the temporary image in the three channels of the output image
    cvMerge(iplTmp, iplTmp, iplTmp, NULL, iplImg2);

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}
