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
    throw ( IllegalConnectionException, IncompatibleTypeException, DuplicateConnectionException ) :
        m_producer( producer ),
        m_consumer( consumer ),
        m_type( LOSSLESS )
{
    assert(m_consumer.isNotNull());
    assert(m_producer.isNotNull());

    connect();
}

PinConnection::~PinConnection()
{
    // on deletion there should not be a connection left
    // disconnection should be called before deletion
    assert( m_consumer.isNull() );
    assert( m_producer.isNull() );
}

void PinConnection::connect()
        throw (IllegalConnectionException, IncompatibleTypeException, DuplicateConnectionException)
{
    QMutexLocker lock( &m_mutex );

    assert(m_consumer.isNotNull());
    assert(m_producer.isNotNull());

    if( m_consumer->getOwner() == m_producer->getOwner() )
        throw IllegalConnectionException( "It is not allowed to connect " +
                                          m_consumer->getOwner()->getName() +
                                          " to itself" );

    if(m_consumer->isConnected())
        throw DuplicateConnectionException( m_consumer->getName() + " is already connected" );

    const std::type_info& producerTypeInfo = m_producer->getTypeInfo();
    const std::type_info& consumerTypeInfo = m_consumer->getTypeInfo();

    if( producerTypeInfo != consumerTypeInfo )
    {
        QString producerName = m_producer->getName();
        QString consumerName = m_consumer->getName();

        QString producerTypeName = producerTypeInfo.name();
        QString consumerTypeName = consumerTypeInfo.name();

        QString errStr = "Cannot connect pins of incompatible type: producer "
                         % producerName % " and consumer " % consumerName %
                         " with types " % producerTypeName %
                         " and " % consumerTypeName;

        throw IncompatibleTypeException( errStr );
    }

    m_producer->addConnection(this);

    m_consumer->setConnection(this);
    assert( m_consumer->isConnected() );
}

void PinConnection::flush()
{
    QMutexLocker lock(&m_mutex);
    // clear queue
    while (!m_queue.empty())
    {
        m_queue.pop();
    }
}

void PinConnection::disconnect()
{
    QMutexLocker lock( &m_mutex );

    // remove connections
    m_producer->removeConnection( this );
    m_consumer->removeConnection();

    // clear and free producer and consumer
    m_producer.set( 0 );
    m_consumer.set( 0 );

    // clear queue
    while (!m_queue.empty())
    {
        m_queue.pop();
    }
}

bool PinConnection::hasData()
{
    QMutexLocker lock( &m_mutex );
    return !m_queue.empty();
}

int PinConnection::size()
{
    QMutexLocker lock( &m_mutex );
    return static_cast<int>( m_queue.size() );
}

RefPtr<Data> PinConnection::get() throw ( PipelineException )
{
    QMutexLocker lock( &m_mutex );
    if( m_queue.empty() )
    {
        throw PipelineException( "Illegal: method get() called on PinConnection which has no data available" );
    }

    RefPtr<Data> data = m_queue.front();
    m_queue.pop();
    return data;
}

void PinConnection::put( RefPtr<Data> data )
{
    QMutexLocker lock( &m_mutex );
    m_queue.push( data );
}

RefPtr<const IOutputPin> PinConnection::fromPin() const
{
    return RefPtr<const IOutputPin>(m_producer);
}

RefPtr<const IInputPin>  PinConnection::toPin() const
{
    return RefPtr<const IInputPin>(m_consumer);
}
