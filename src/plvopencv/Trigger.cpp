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
#include "Trigger.h"

#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

/**
 * Constructor.
 */
Trigger::Trigger() :
        m_activate(false),
        m_continuous(false),
        m_count(1)
{
    m_inputPin   = createDynamicInputPin( "input", this );
    m_inputCount = createInputPin<int>("count", this, IInputPin::CONNECTION_OPTIONAL, IInputPin::CONNECTION_ASYNCHRONOUS );
    m_outputPin  = createOutputPin<bool>( "trigger", this );
}

/**
 * Destructor.
 */
Trigger::~Trigger()
{
}

bool Trigger::process()
{
    assert(m_inputPin != 0);

    // do this to decrease queue at input pin
    QVariant v;
    m_inputPin->getVariant(v);

    if( ++m_received >= m_count )
    {
        m_activate = true;
        m_received = 0;
    }

    // check if we received a new count value
    if( m_inputCount->isConnected() && m_inputCount->hasData() )
    {
        int num = m_inputCount->get();
        if( num != m_count)
        {
            m_count = num;
            emit countChanged(num);
        }
    }

    //send a trigger to the output with the value of activate
    if( m_activate )
    {
        m_outputPin->put(true);

        //check if it is continuous or not
        if(!m_continuous) updateActivate(false);
    }
    else
    {
        m_outputPin->put(false);
    }
    return true;
}

bool Trigger::getActivate() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_activate;
}

bool Trigger::getContinuous() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_continuous;
}

int Trigger::getCount() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_count;
}

void Trigger::setActivate(bool b)
{
    QMutexLocker lock(m_propertyMutex);
    m_activate = b;
}

void Trigger::setContinuous(bool b)
{
    QMutexLocker lock(m_propertyMutex);
    m_continuous = b;
}

void Trigger::setCount(int i)
{
    QMutexLocker lock(m_propertyMutex);
    if( i > 0 )
        m_count = i;
    emit countChanged(m_count);
}

