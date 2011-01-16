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
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Sub::Sub()
{
    m_inputPin1 = createCvMatDataInputPin( "input A", this );
    m_inputPin2 = createCvMatDataInputPin( "input B", this );
    m_inputPin3 = createCvMatDataInputPin( "mask" , this, IInputPin::CONNECTION_OPTIONAL );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputPin1->addAllChannels();
    m_inputPin1->addAllDepths();

    m_inputPin2->addAllChannels();
    m_inputPin2->addAllDepths();

    m_inputPin3->addSupportedChannels(1);
    m_inputPin3->addSupportedDepth(CV_8U);
    m_inputPin3->addSupportedDepth(CV_8S);

    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();
}

Sub::~Sub()
{
}

void Sub::init()
{
}

void Sub::deinit() throw ()
{
}

void Sub::start()
{
}

void Sub::stop()
{
}

void Sub::process()
{
    CvMatData in1 = m_inputPin1->get();
    CvMatData in2 = m_inputPin2->get();
    CvMatData maskData;

    // check if there is mask data
    if( m_inputPin3->isConnected() )
    {
        maskData = m_inputPin3->get();
    }

    //check format of images
    if( in1.channels() != in2.channels() )
    {
        QString msg = tr("Images do not have same number of channels.");
        error( PLE_FATAL, msg );
        return;
    }

    if( in1.depth() != in2.depth() )
    {
        QString msg = tr("Input images are not of the same depth. "
                      "Input 1 has depth %1 and input 2 has depth %2. " )
                .arg(CvMatData::depthToString(in1.depth()))
                .arg(CvMatData::depthToString(in2.depth()));
        error( PLE_FATAL, msg );
        return;
    }

    CvMatData out = CvMatData::create(in1.properties());

    const cv::Mat& src1 = in1;
    const cv::Mat& src2 = in2;
    const cv::Mat& mask = maskData;
    cv::Mat& dst = out;

    cv::subtract( src1, src2, dst, mask );

    // publish the new image
    m_outputPin->put( out );

}
