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

#include "Split.h"

#include <plvcore/CvMatDataPin.h>
#include <plvcore/CvMatData.h>
using namespace plv;
using namespace plvopencv;

#include <vector>

Split::Split()
{
    m_inputPin = createCvMatDataInputPin( "multi_ch_image", this, IInputPin::CONNECTION_REQUIRED );
    m_outputPin0 = createCvMatDataOutputPin( "single_ch_image_0", this );
    m_outputPin1 = createCvMatDataOutputPin( "single_ch_image_1", this );
    m_outputPin2 = createCvMatDataOutputPin( "single_ch_image_2", this );
    m_outputPin3 = createCvMatDataOutputPin( "single_ch_image_3", this );

    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_outputPin0->addAllDepths();
    m_outputPin1->addAllDepths();
    m_outputPin2->addAllDepths();
    m_outputPin3->addAllDepths();

    m_outputPin0->addSupportedChannels(1);
    m_outputPin1->addSupportedChannels(1);
    m_outputPin2->addSupportedChannels(1);
    m_outputPin3->addSupportedChannels(1);
}

Split::~Split()
{
}

bool Split::process()
{
    plv::CvMatData inImg = m_inputPin->get();
    int numChannels = inImg.channels();

    if(numChannels < 2)
    {
        setError(PlvPipelineRuntimeError, "Input image is single channel, expected a multi channel image.");
        return false;
    }

    // open input images for reading
    const cv::Mat& in = inImg;

    //Create the vector with output data
    std::vector<CvMatData> outImgs;
    outImgs.reserve(numChannels);
    CvMatDataProperties props = inImg.properties();
    props.setNumChannels(1);
    for(int i = 0; i < numChannels; ++i )
    {
        outImgs.push_back(CvMatData::create(props));
    }

    //open output images for writing
    std::vector<cv::Mat> out;
    for(int i=0; i < numChannels; ++i )
        out.push_back( outImgs.at(i) );

    //Split the image in multiple images
    cv::split(in, out);

    // publish the new image based on the number of channels in the input image
    switch(numChannels)
    {
    case 4:
        m_outputPin3->put( outImgs.at(3) );
    case 3:
        m_outputPin2->put( outImgs.at(2) );
    case 2:
        m_outputPin1->put( outImgs.at(1) );
    default:
        m_outputPin0->put( outImgs.at(0) );
    }
    return true;
}
