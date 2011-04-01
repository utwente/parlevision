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

#include "Pin.h"
using namespace plv;

void Pin::setName(const QString& name)
{
    m_name = name;
    emit nameChanged(name);
}

IInputPin::~IInputPin()
{
}

/** Connects this pin through the given connection.
  * @require !this->isConnected()
  * @ensure this->isConnected();
  */
void IInputPin::setConnection(PinConnection* connection)
{
    assert(!this->isConnected());
    assert(connection != 0);

    m_connection = connection;
    m_owner->inputConnectionSet(this,connection);
}

void IInputPin::removeConnection()
{
    assert( m_connection.isNotNull() );
    PinConnection* con = m_connection;
    m_connection.set( 0 );
    m_owner->inputConnectionRemoved(this, con);
}

PinConnection* IInputPin::getConnection() const
{
    assert( m_connection.isNotNull() );
    return m_connection.getPtr();
}

bool IInputPin::isConnected() const
{
    return this->m_connection.isNotNull();
}

bool IInputPin::hasData() const
{
    if( m_connection.isNotNull() )
    {
        return this->m_connection->hasData();
    }
    return false;
}

bool IInputPin::fastforward( unsigned int target )
{
    return m_connection->fastforward( target );
}

void IInputPin::flushConnection()
{
    m_connection->flush();
}

void IInputPin::peekNext( unsigned int& serial, bool& isNull ) const
{
    assert( m_connection != 0 );
    m_connection->peek(serial, isNull);
}

void IInputPin::pre()
{
    m_called = false;
}

void IInputPin::post()
{
    // check if mandatory get() method has been called and
    // data has been removed from this pin
    if( this->isConnected() && this->isSynchronous() )
    {
        if( !m_called )
        {
            QString msg = tr("Processor %1 did not call mandatory get() on input Pin %2.")
                          .arg(getOwner()->getName())
                          .arg(getName());
            throw RuntimeError(msg, __FILE__, __LINE__);
        }
    }
}

void IInputPin::removeFirst()
{
    assert(m_connection.isNotNull());
    m_connection->get();
}

void IInputPin::getVariant( QVariant& v )
{
    // check if get is not called twice during one process call
    if( m_called )
    {
        QString msg = tr("Illegal: method get() called twice during process() on InputPin %1 of processor %2.")
                      .arg(m_name)
                      .arg(m_owner->getName());
        throw RuntimeError(msg,__FILE__, __LINE__ );
    }
    m_called = true;
    if( !(this->m_connection.isNotNull() &&
          this->m_connection->hasData() ))
    {

        QString processorName = this->m_owner->getName();
        QString msg = "Illegal: method get() called on InputPin "
                      "which has no data available. Pin name is " %
                      this->m_name %
                      " of processor " % processorName;
        throw RuntimeError(msg, __FILE__, __LINE__);
    }
    Data d = m_connection->get();
    v.setValue(d.getPayload());
}

IOutputPin::~IOutputPin()
{
    removeConnections();
}

/** Adds a connection to the set of connections this pin outputs to
  * @ensure this->isConnected();
  */
void IOutputPin::addConnection( PinConnection* connection )
{
    assert(connection != 0);

    m_connections.push_back(connection);
    m_owner->outputConnectionAdded(this,connection);
}

/** Removes the connection from this pin.
  * Assumes it has been disconnect()'ed
  */
void IOutputPin::removeConnection( PinConnection* connection )
{
    assert( connection != 0 );

    for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr)
    {
        RefPtr<PinConnection> current = *itr;
        if( current.getPtr() == connection )
        {
            m_owner->outputConnectionRemoved(this, (*itr).getPtr());
            m_connections.erase( itr );
            return;
        }
    }
    // we should never be here
    assert( false );
}

void IOutputPin::removeConnections()
{
    for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr)
    {
        m_owner->outputConnectionRemoved( this, (*itr).getPtr() );
        m_connections.erase( itr );
    }
}

std::list< RefPtr<PinConnection > > IOutputPin::getConnections()
{
    // makes a copy
    return m_connections;
}

int IOutputPin::connectionCount() const
{
    return m_connections.size();
}

int IOutputPin::maxDataOnConnection() const
{
    int max = 0;

    for(std::list< RefPtr<PinConnection> >::const_iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr)
    {
        int size = (*itr)->size();
        if( size > max ) max = size;
    }

    return max;
}

void IOutputPin::pre()
{
    m_called = false;
}

void IOutputPin::post()
{
    // publish NULL data to all listeners
    // this is to keep everything synchronized
    if( this->isConnected() && !m_called )
    {
        unsigned int serial = m_owner->getProcessingSerial();
        Data nullData( serial );

        // publish to all pin connections
        for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
                itr != m_connections.end(); ++itr)
        {
            PinConnection* connection = (*itr).getPtr();
            if( connection->isSynchronous() )
                connection->put( nullData );
        }
    }
}

void IOutputPin::putVariant( unsigned int serial, const QVariant& v )
{
    // check if get is not called twice during one process call
    if( m_called )
    {
        QString processorName = this->m_owner->getName();
        QString msg = "Illegal: method put() called twice "
                      "during process() on OutputPin. Pin name is \"" %
                      this->m_name %
                      "\" of processor \"" % processorName % "\"";
        throw RuntimeError( msg,__FILE__, __LINE__ );
    }
    m_called = true;

    // propagate the serial number
    Data data( serial, v );

    // publish data to viewers
    emit newData( serial, v );

    // publish to all pin connections
    for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr)
    {
        PinConnection* connection = (*itr).getPtr();
        connection->put( data );
    }
}

plv::DynamicInputPin* plv::createDynamicInputPin( const QString& name, plv::PipelineElement* owner,
                              plv::IInputPin::Required required,
                              plv::IInputPin::Synchronized synchronized, int typeId)
throw (plv::IllegalArgumentException)
{
    // if add fails pin is automatically deleted and exception is thrown
    plv::DynamicInputPin* pin = new plv::DynamicInputPin( name, owner, required, synchronized, typeId );
    owner->addInputPin( pin );
    return pin;
}
