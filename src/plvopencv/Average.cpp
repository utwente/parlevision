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

#include "Average.h"
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Average::Average() :
    m_numFrames(10),
    m_total(0)
{
    m_inputPin = createCvMatDataInputPin( "input", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputFrames = createInputPin<int>( "num frames", this, IInputPin::CONNECTION_OPTIONAL, IInputPin::CONNECTION_ASYNCHRONOUS );

    m_inputPin->addAllChannels();
    m_inputPin->addSupportedDepth(CV_8U);
    m_inputPin->addSupportedDepth(CV_32F);

    m_outputPin->addAllChannels();
    m_outputPin->addSupportedDepth(CV_8U);
    m_outputPin->addSupportedDepth(CV_32F);
}

Average::~Average()
{
}

bool Average::start()
{
    m_total = 0;
    return true;
}

bool Average::process()
{
    if( m_inputFrames->isConnected() && m_inputFrames->hasData() )
    {
        int frames = m_inputFrames->get();
        if( m_numFrames != frames )
        {
            setNumFrames(frames);
        }
    }

    CvMatData in = m_inputPin->get();
    const cv::Mat& src = in;

    if( m_avg.width() != in.width() || m_avg.height() != in.height() || m_avg.channels() != in.channels() )
    {
        m_avg = CvMatData::create( in.width(), in.height(), CV_32F, in.channels() );
        m_out = CvMatData::create( in.width(), in.height(), CV_8U, in.channels() );
        src.convertTo(m_avg, m_avg.type());
    }
    cv::Mat& avg = m_avg;

    cv::accumulate(src, avg);

    if( m_total >= m_numFrames )
    {
        avg.convertTo(m_out, m_out.type(), 1.0 / m_total );
        m_outputPin->put(m_out);

        src.convertTo(m_avg, m_avg.type());
        m_total = 0;
    }
    ++m_total;
    return true;
}

int Average::getNumFrames() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_numFrames;
}

void Average::setNumFrames(int f)
{
    QMutexLocker lock( m_propertyMutex );
    if( f > 0 )
    {
        m_numFrames = f;
    }
    emit numFramesChanged(m_numFrames);
}
