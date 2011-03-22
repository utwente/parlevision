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
#include <plvcore/CvMatData.h>

#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Diff::Diff()
{
    m_inputPin1 = createCvMatDataInputPin( "input 1", this );
    m_inputPin2 = createCvMatDataInputPin( "input 2", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

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

bool Diff::process()
{
    CvMatData img1 = m_inputPin1->get();
    CvMatData img2 = m_inputPin2->get();

    //TODO check format of images

    // open input images for reading
    const cv::Mat& in1 = img1;
    const cv::Mat& in2 = img2;

    //get a new output image of same depth and size as input image
    CvMatData imgOut = CvMatData::create( img1.properties() );

    // open output image for writing
    cv::Mat& out = imgOut;

    // do the diff function
    cv::absdiff( in1, in2, out );

    // publish the new image
    m_outputPin->put( imgOut );

    return true;
}

