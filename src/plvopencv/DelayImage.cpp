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

#include "DelayImage.h"
#include <plvcore/OpenCVImage.h>

#include <plvcore/OpenCVImagePin.h>

using namespace plv;
using namespace plvopencv;

DelayImage::DelayImage():
        m_steps(5)
{
    m_inputPin = createOpenCVImageInputPin( "input", this, IInputPin::INPUT_REQUIRED );
    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_outputPin = createOpenCVImageOutputPin( "output", this );
    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();

    m_delayedOutputPin = createOpenCVImageOutputPin( "delayed", this );
    m_delayedOutputPin->addAllChannels();
    m_delayedOutputPin->addAllDepths();
}

DelayImage::~DelayImage()
{
}

void DelayImage::init()
{
}

void DelayImage::deinit() throw ()
{
}

void DelayImage::start()
{
    assert( m_images.isEmpty() );
}

void DelayImage::stop()
{
    m_images.clear();
}

void DelayImage::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    // get the input image and append it to the list
    // of buffered images
    RefPtr<OpenCVImage> imgIn = m_inputPin->get();
    m_images.append( imgIn );

    // propagate image if we have an history of m_steps images
    while( m_images.size() >= m_steps )
    {
        m_delayedOutputPin->put( m_images.first().getPtr() );
        m_images.removeFirst();

        // and propagate non delayed output
        m_outputPin->put( imgIn.getPtr() );
    }
    // not enough in buffer, do not propagate anything
}

void DelayImage::setSteps(int i)
{
    if( i < 0)
    {
        m_steps = 0;
    }
    else if( i > MAX_STEPS )
    {
        m_steps = MAX_STEPS;
    }
    else
    {
        m_steps = i;
    }
    emit( stepsChanged(m_steps) );
}
