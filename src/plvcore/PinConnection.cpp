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

#include "PinConnection.h"
#include "Types.h"
#include "Pin.h"

#include <QStringBuilder>
#include <QString>

using namespace plv;

PinConnection::PinConnection( IOutputPin* producer, IInputPin* consumer )
    throw ( IllegalConnectionException,
            IncompatibleTypeException,
            DuplicateConnectionException ) :
        m_producer( producer ),
        m_consumer( consumer ),
        m_type( LOSSLESS )
{
    assert(m_consumer != 0);
    assert(m_producer != 0);

    connect();
}

PinConnection::~PinConnection()
{
    // on deletion there should not be a connection left
    // disconnection should be called before deletion
    assert(m_consumer == 0);
    assert(m_producer == 0);
}

bool PinConnection::canConnectPins( IOutputPin* out, IInputPin* in,
                                    QString& errStr )
{
    if( out->getOwner() == in->getOwner() )
    {
        errStr = "Cannot connect " % out->getOwner()->getName() +
                 " to itself" ;
        return false;
    }

    if( in->isConnected() )
    {
        errStr = "Input pin " % in->getName() % " is already connected";
        return false;
    }

    int producerTypeId = out->getTypeId();
    int consumerTypeId = in->getTypeId();

    if( producerTypeId != consumerTypeId )
    {
        QString producerName = out->getName();
        QString consumerName = in->getName();
        QString producerTypeName = out->getTypeName();
        QString consumerTypeName = in->getTypeName();

        errStr = "Cannot connect pins of incompatible type: producer "
                 % producerName % " and consumer " % consumerName %
                 " with types " % producerTypeName %
                 " and " % consumerTypeName;
        return false;
    }

    // ask pins for pin type specific objections to connection
    if( !out->acceptsConnectionWith( in, errStr ) )
    {
        return false;
    }

    // ask pins for pin type specific objections to connection
    if( !in->acceptsConnectionWith( out, errStr ) )
    {
        return false;
    }

    return true;
}

void PinConnection::connect()
        throw (IllegalConnectionException,
               IncompatibleTypeException,
               DuplicateConnectionException)
{
    QMutexLocker lock( &m_connectionMutex );

    if( m_consumer->getOwner() == m_producer->getOwner() )
        throw IllegalConnectionException( "It is not allowed to connect " +
                                          m_consumer->getOwner()->getName() +
                                          " to itself" );

    if(m_consumer->isConnected())
    {
        throw DuplicateConnectionException(m_consumer->getName() + " is already connected");
    }

    int producerTypeId = m_producer->getTypeId();
    int consumerTypeId = m_consumer->getTypeId();

    if( producerTypeId != consumerTypeId )
    {
        QString producerName = m_producer->getName();
        QString consumerName = m_consumer->getName();
        QString producerTypeName = m_producer->getTypeName();
        QString consumerTypeName = m_consumer->getTypeName();

        QString errStr = "Cannot connect pins of incompatible type: producer "
                 % producerName % " and consumer " % consumerName %
                 " with types " % producerTypeName %
                 " and " % consumerTypeName;
        throw IncompatibleTypeException(errStr);
    }

    QString errStr;
    // ask pins for pin type specific objections to connection
    if( !m_producer->acceptsConnectionWith( m_consumer, errStr ) )
    {
        throw IncompatibleTypeException(errStr);
    }

    if(! m_consumer->acceptsConnectionWith( m_producer, errStr ) )
    {
        throw IncompatibleTypeException(errStr);
    }

    // finally, make the connection
    m_producer->addConnection(this);
    m_consumer->setConnection(this);

    assert( m_consumer->isConnected() );
}

bool PinConnection::fastforward( unsigned int target )
{
    QMutexLocker lock(&m_connectionMutex);

    bool success = false;
    while( !success && !m_queue.empty())
    {
        Data data = m_queue.front();
        if( data.getSerial() == target )
        {
            success = true;
        }
        else
        {
            m_queue.pop();
        }
    }
    return success;
}

void PinConnection::flush()
{
    QMutexLocker lock(&m_connectionMutex);
    // clear queue
    while(!m_queue.empty())
    {
        m_queue.pop();
    }
}

void PinConnection::disconnect()
{
    QMutexLocker lock( &m_connectionMutex );

    // remove connections
    m_producer->removeConnection(this);
    m_consumer->removeConnection();

    // clear producer and consumer
    m_producer = 0;
    m_consumer = 0;

    // clear queue
    while (!m_queue.empty())
    {
        m_queue.pop();
    }
}

bool PinConnection::hasData()
{
    QMutexLocker lock( &m_connectionMutex );
    return !m_queue.empty();
}

int PinConnection::size()
{
    QMutexLocker lock( &m_connectionMutex );
    return static_cast<int>( m_queue.size() );
}

Data PinConnection::get()
{
    QMutexLocker lock( &m_connectionMutex );
    if( m_queue.empty() )
    {
        QString producerName = m_producer->getOwner()->getName();
        QString consumerName = m_consumer->getOwner()->getName();

        QString msg = "Illegal: method get() called on PinConnection"
                      "which has no data available"
                      " with producer owner " % producerName %
                      " and consumer owner " % consumerName;

        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }
    Data d = m_queue.front();
    m_queue.pop();
    return d;
}

Data PinConnection::peek() const
{
    QMutexLocker lock( &m_connectionMutex );
    if( m_queue.empty() )
    {
        QString producerName = m_producer->getOwner()->getName();
        QString consumerName = m_consumer->getOwner()->getName();

        QString msg = "Illegal: method peek() called on PinConnection"
                      "which has no data available"
                      " with producer owner " % producerName %
                      " and consumer owner " % consumerName;

        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }
    return m_queue.front();
}

void PinConnection::put( const Data& data )
{
    QMutexLocker lock( &m_connectionMutex );
    m_queue.push( data );
}

const IOutputPin* PinConnection::fromPin() const
{
    return m_producer;
}

const IInputPin*  PinConnection::toPin() const
{
    return m_consumer;
}
