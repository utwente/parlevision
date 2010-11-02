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
    if( dataAvailableOnInputPins() )
    {
        return isReadyForProcessing();
    }
    return false;
}

void PipelineProcessor::__process()
{
    assert( requiredPinsConnected() );
    assert( dataAvailableOnInputPins() );

    QMutexLocker lock( &m_pleMutex );

    std::vector<unsigned int> serials;
    serials.reserve( m_inputPins.size() );
    bool nullDetected = false;
    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end() && !nullDetected; ++itr )
    {
        IInputPin* in = itr->second.getPtr();
        in->pre();
        if( in->isConnected() &&
            in->isSynchronous() )
        {
            unsigned int serial = in->getNextSerial();
            serials.push_back( serial );
            if( serial == 0 )
            {
                nullDetected = true;
            }

        }
    }

    // if one data item is a null
    // we throw away all data from all synchronous pins
    if( nullDetected )
    {
        for( InputPinMap::iterator itr = m_inputPins.begin();
             itr != m_inputPins.end(); ++itr )
        {
            IInputPin* in = itr->second.getPtr();

            // just remove first data in queue
            RefPtr<Data> d;
            in->getUntyped(d);
        }

        // call post on input pins
        for( InputPinMap::iterator itr = m_inputPins.begin();
             itr != m_inputPins.end(); ++itr )
        {
            IInputPin* in = itr->second.getPtr();
            in->post();
        }
    }
    else
    {
        // sort on serial number
        std::sort( serials.begin(), serials.end(), std::greater<unsigned int>() );

        // check for equality
        // we are a processor, so there is guaranteed to be at least one input pin
        unsigned int max = serials[0];
        bool fastforward = false;
        for( std::vector<unsigned int>::iterator itr = serials.begin();
             itr != serials.end(); ++itr )
        {
            if( (*itr) < max)
            {
                fastforward = true;
                assert( false );
            }

        }

//        bool fastforwardSucceeded = true;
//        if(fastforward)
//        {
//            qDebug() << "Fastforward needed on pin inputs of processor " << getName();

//            for( InputPinMap::iterator itr = m_inputPins.begin();
//                 itr != m_inputPins.end(); ++itr )
//            {
//                IInputPin* in = itr->second.getPtr();
//                if( in->isConnected() && in->isSynchronous() )
//                {
//                    fastforwardSucceeded = in->fastforward( max ) && fastforwardSucceeded;
//                }
//            }
//        }

        // call pre on output pins
        for( OutputPinMap::iterator itr = m_outputPins.begin();
             itr != m_outputPins.end(); ++itr )
        {
            IOutputPin* out = itr->second.getPtr();
            out->pre();
        }

        // call process function which does the actual work
        // if fastforward did not succeed we just exit
        try
        {
            // set the serial number for this processing run
            this->setProcessingSerial( max );

            // do the actual processing
            this->process();
        }
        catch( ... )
        {
            // call post before we re-throw
            for( InputPinMap::iterator itr = m_inputPins.begin();
                 itr != m_inputPins.end(); ++itr )
            {
                IInputPin* in = itr->second.getPtr();
                in->post();
            }
            throw;
        }

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
}


