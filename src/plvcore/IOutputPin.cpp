#include "IOutputPin.h"
#include "DataProducer.h"

using namespace plv;

IOutputPin::IOutputPin( const QString& name, DataProducer* producer ) :
    Pin( name, producer ), m_producer(producer)
{
    assert( m_producer != 0 );
}

IOutputPin::~IOutputPin()
{
    removeConnections();
    m_producer = 0;
}

/** Adds a connection to the set of connections this pin outputs to
  * @ensure this->isConnected();
  */
void IOutputPin::addConnection( PinConnection* connection )
{
    assert(connection != 0);

    m_connections.push_back(connection);
    m_producer->onOutputConnectionAdded(this,connection);
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
            m_producer->onOutputConnectionRemoved(this, (*itr).getPtr());
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
        m_producer->onOutputConnectionRemoved( this, (*itr).getPtr() );
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
        unsigned int serial = m_producer->getProcessingSerial();
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
        QString processorName = m_producer->getName();
        QString msg = tr("Illegal: method put() called twice during process() "
                         "on OutputPin. Pin name is \"%1\" of processor \"%2\"")
                         .arg( this->m_name )
                         .arg( processorName );
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
