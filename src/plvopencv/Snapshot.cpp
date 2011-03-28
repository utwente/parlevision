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

#include "Snapshot.h"
#include <plvcore/CvMatDataPin.h>

using namespace plv;
using namespace plvopencv;

Snapshot::Snapshot() :
    m_makeSnapshot(true),
    m_keepSending(false)
{
    m_inputPin = createCvMatDataInputPin( "input", this, IInputPin::CONNECTION_REQUIRED );
    m_inputTrigger = createInputPin<bool>("trigger", this, IInputPin::CONNECTION_OPTIONAL, IInputPin::CONNECTION_ASYNCHRONOUS );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();
}

Snapshot::~Snapshot()
{
}

bool Snapshot::process()
{
    CvMatData in = m_inputPin->get();

    if( m_inputTrigger->isConnected() && m_inputTrigger->hasData() )
    {
        bool trigger = m_inputTrigger->get();
        m_makeSnapshot = trigger || m_makeSnapshot;
    }

    // make snapshot if requested
    // since we init m_makeSnapshot to true, we do this on the first frame
    if( m_makeSnapshot )
    {
        m_snapshot = in;
        m_makeSnapshot = false;
        emit makeSnapshotChanged(false);
        m_outputPin->put( m_snapshot );
    }
    else if( !m_snapshot.isEmpty() && m_keepSending)
    {
        m_outputPin->put(m_snapshot);
    }
    return true;
}

/** propery methods */
bool Snapshot::getMakeSnapshot() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_makeSnapshot;
}

void Snapshot::setMakeSnapshot(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_makeSnapshot = b;
    emit makeSnapshotChanged(b);
}

bool Snapshot::getKeepSending() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_keepSending;
}

void Snapshot::setKeepSending(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_keepSending = b;
    emit keepSendingChanged(b);
}

