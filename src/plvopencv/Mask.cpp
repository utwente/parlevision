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
#include "Mask.h"

#include <QDebug>
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Mask::Mask()
{
    m_inputPin1 = createCvMatDataInputPin( "input", this );
    m_inputPin2 = createCvMatDataInputPin( "mask", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputPin1->addSupportedChannels(1);
    m_inputPin1->addSupportedChannels(3);
    m_inputPin1->addSupportedDepth(CV_8U);
    m_inputPin1->addSupportedDepth(CV_32F);

    m_inputPin2->addSupportedChannels(1);
    m_inputPin2->addSupportedDepth(CV_8U);
    m_inputPin2->addSupportedDepth(CV_32F);

    m_outputPin->addSupportedChannels(1);
    m_outputPin->addSupportedChannels(3);
    m_outputPin->addSupportedDepth(CV_8U);
}

Mask::~Mask()
{
}

bool Mask::process()
{
    // get the inputs
    CvMatData in1 = m_inputPin1->get();
    CvMatData in2 = m_inputPin2->get();
    CvMatData out = CvMatData::create(in1.properties());

    const cv::Mat& src  = in1;
    const cv::Mat& maskIn = in2;
    cv::Mat& dst = out;

    cv::Mat mask;
    if( maskIn.type() == CV_32F )
    {
        maskIn.convertTo(mask, CV_8U);
    }
    else
    {
        mask = maskIn;
    }

    dst = cvScalar(0);
    if(m_negative)
    {
        cv::Mat maskn = in2;
        maskn = (mask == 0);
    }
    src.copyTo(dst, mask);

    // publish the new image
    m_outputPin->put( out );
    return true;
}

void Mask::setNegative(bool n)
{
    QMutexLocker lock(m_propertyMutex);
    m_negative = n;
    emit negativeChanged(n);
}

bool Mask::getNegative() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_negative;
}
