#include "PinConnection.h"
#include "Types.h"
#include "Pin.h"

using namespace plv;

PinConnection::PinConnection( IOutputPin* producer, IInputPin* consumer ) :
        m_producer( producer ),
        m_consumer( consumer )
{
    assert(m_consumer.isNotNull());
    assert(m_producer.isNotNull());

    m_producer->addConnection(this);

    assert(!m_consumer->isConnected());
    m_consumer->setConnection(this);
    assert( m_consumer->isConnected() );

    // TODO should also be done in release mode at runtime.
    assert( producer->getTypeInfo() == consumer->getTypeInfo() );
}

PinConnection::~PinConnection()
{
    m_producer->removeConnection(this);
    m_consumer->removeConnection(this);

    assert( !m_consumer->isConnected() );
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
