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

#include "PipelineProducer.h"
#include "Pin.h"

using namespace plv;

PipelineProducer::PipelineProducer() : m_serial( 0 )
{
}

PipelineProducer::~PipelineProducer()
{
}

void PipelineProducer::__init()
{
    QMutexLocker lock( &m_pleMutex );

    if( m_inputPins.size() != 0 )
    {
        throw PlvRuntimeException( "Producer " + getName() + " has input pins defined",
                                   __FILE__, __LINE__);
    }

    if( m_outputPins.size() == 0 )
    {
        throw PlvRuntimeException( "Producer " + getName() + " has no output pins defined",
                                   __FILE__, __LINE__);
    }

    this->init();
}

bool PipelineProducer::__isReadyForProcessing() const
{
    return isReadyForProcessing();
}

void PipelineProducer::__process()
{
    QMutexLocker lock( &m_pleMutex );

    // set the serial number for this processing run
    this->setProcessingSerial( getNextSerial() );

    for( OutputPinMap::iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        IOutputPin* out = itr->second.getPtr();
        out->pre();
    }

    // do the actual processing
    this->process();

    for( OutputPinMap::iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        IOutputPin* out = itr->second.getPtr();
        out->post();
    }
}
