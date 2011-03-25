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
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>

using namespace plv;
using namespace plvopencv;

DelayImage::DelayImage():
        m_steps(5)
{
    m_inputPin = createCvMatDataInputPin( "input", this, IInputPin::CONNECTION_REQUIRED,
                                          IInputPin::CONNECTION_SYNCHRONOUS );
    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_delayedOutputPin = createCvMatDataOutputPin( "delayed", this );
    m_delayedOutputPin->addAllChannels();
    m_delayedOutputPin->addAllDepths();
}

DelayImage::~DelayImage()
{
}

bool DelayImage::start()
{
    assert( m_images.isEmpty() );
    return true;
}

bool DelayImage::stop()
{
    m_images.clear();
    return true;
}

bool DelayImage::process()
{
    // get the input image and append it to the list
    // of buffered images
    CvMatData imgIn = m_inputPin->get();
    m_images.append( imgIn );

    // propagate image if we have an history of m_steps images
    if( m_images.size() >= m_steps )
    {
        m_delayedOutputPin->put( m_images.first() );
        m_images.removeFirst();
    }
    // not enough in buffer, do not propagate anything

    return true;
}

int DelayImage::getSteps() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_steps;
}

void DelayImage::setSteps(int i)
{
    QMutexLocker lock( m_propertyMutex );
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
