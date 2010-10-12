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

void PipelineProcessor::__init()
{
    QMutexLocker lock( &m_pleMutex );

    if( m_inputPins.size() == 0 )
    {
        throw PlvRuntimeException( "Processor " + getName() + " has no input pins defined",
                                   __FILE__, __LINE__);
    }

    if( m_outputPins.size() == 0 )
    {
        throw PlvRuntimeException( "Processor " + getName() + " has no output pins defined",
                                   __FILE__, __LINE__);
    }

    this->init();
}

bool PipelineProcessor::__isReadyForProcessing() const
{
    assert( requiredPinsConnected() );

    // see if data is available and the processor is ready for processing
    if( dataAvailableOnRequiredPins() )
    {
        return isReadyForProcessing();
    }
    return false;
}

void PipelineProcessor::__process()
{
    assert( requiredPinsConnected() );
    assert( dataAvailableOnRequiredPins() );

    QMutexLocker lock( &m_pleMutex );

    std::vector<unsigned int> serials;
    serials.reserve( m_inputPins.size() );
    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        IInputPin* in = itr->second.getPtr();
        in->pre();
        if( in->isConnected() && in->isSynchronous() )
        {
            serials.push_back( in->getNextSerial() );
        }
    }

    // sort on serial number
    std::sort( serials.begin(), serials.end(), std::greater<unsigned int>() );

    // check for equality
    // we are a processor, so there is guaranteed to be at least one input pin
    unsigned int max = serials[0];
    bool fastforward = false;
    for( std::vector<unsigned int>::iterator itr = serials.begin();
         itr != serials.end(); ++itr )
    {
        if(*itr < max)
        {
            fastforward = true;
        }
    }

    bool fastforwardSucceeded = true;
    if(fastforward)
    {
        qDebug() << "Fastforward needed on pin inputs of processor " << getName();

        for( InputPinMap::iterator itr = m_inputPins.begin();
             itr != m_inputPins.end(); ++itr )
        {
            IInputPin* in = itr->second.getPtr();
            if( in->isConnected() && in->isSynchronous() )
            {
                fastforwardSucceeded = in->fastforward( max ) && fastforwardSucceeded;
            }
        }
    }

    // call process function which does the actual work
    // if fastforward did not succeed we just exit
    if( fastforwardSucceeded )
    {
        try
        {
            // set the serial number for this processing run
            this->setProcessingSerial( max );

            // do the actual processing
            this->process();
        }
        catch( ... )
        {
            // decrease refcount before we re-throw
            for( InputPinMap::iterator itr = m_inputPins.begin();
                 itr != m_inputPins.end(); ++itr )
            {
                IInputPin* in = itr->second.getPtr();
                in->post();
            }
            throw;
        }
    }

    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        IInputPin* in = itr->second.getPtr();
        in->post();

        // check if get() method has been called and
        // data has been removed from this pin
        if( fastforwardSucceeded && in->isConnected() &&
            in->isSynchronous() && !in->called() )
        {
            QString processorName = in->getOwner()->getName();
            QString msg = "Processor " % processorName %
                          " did not call madatory get() "
                          " on input Pin " %
                          in->getName();
            throw PlvRuntimeException(msg, __FILE__, __LINE__);
        }
    }
}


