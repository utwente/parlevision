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

#include "PipelineProcessor.h"

#include "Pin.h"

#include <vector>
#include <algorithm>
#include <functional>

using namespace plv;

PipelineProcessor::PipelineProcessor()
{
}

PipelineProcessor::~PipelineProcessor()
{
}

bool PipelineProcessor::__ready( unsigned int& nextSerial )
{
    assert( getState() != READY ); // shouldn't be called twice
    assert( requiredPinsConnected() );

    // see if data is available and the processor is ready for processing
    PipelineElement::State state = getState();
    if( state == DONE || state == UNDEFINED )
    {
        if( dataAvailableOnInputPins( nextSerial ) )
        {
            setState( READY );
            return true;
        }
    }
    return false;
}

void PipelineProcessor::__process( unsigned int serial )
{
    assert( requiredPinsConnected() );
    assert( getState() == RUNNING );

    QMutexLocker lock( &m_pleMutex );

    // we do not want properties to change in the middle of an operation
    QMutexLocker lock2( m_propertyMutex );

    // set the serial number
    m_serial = serial;

    // call pre on input pins
    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        IInputPin* in = itr->second.getPtr();
        in->pre();
    }

    // call pre on output pins
    for( OutputPinMap::iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        IOutputPin* out = itr->second.getPtr();
        out->pre();
    }

    // do the actual processing
    this->process();

    // call post on input pins
    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        IInputPin* in = itr->second.getPtr();
        in->post();
    }

    // call post on output pins
    for( OutputPinMap::iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        IOutputPin* out = itr->second.getPtr();
        out->post();
    }
}


