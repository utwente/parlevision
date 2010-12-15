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

#include "PixelSum.h"
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>

using namespace plv;
using namespace plvopencv;

PixelSum::PixelSum()
{
    m_inputPin = createCvMatDataInputPin( "input", this );
    m_outputPin = createOutputPin<cv::Scalar>( "output", this );

    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();
}

PixelSum::~PixelSum()
{
}

void PixelSum::init()
{
}

void PixelSum::deinit() throw ()
{
}

void PixelSum::start()
{
}

void PixelSum::stop()
{
}

void PixelSum::process()
{
    CvMatData in = m_inputPin->get();

    const cv::Mat& src = in;

    // tuple of 1,2,3 or 4 values depending on the number of channels
    cv::Scalar scalar = cv::sum( src );

    for(int i=0;i<scalar.channels;++i)
    {
        qDebug() << scalar[i];
    }

    m_outputPin->put( scalar );
}
