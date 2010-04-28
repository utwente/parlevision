#include "PinConnection.h"

#include "Types.h"
#include "Pin.h"

using namespace plv;

PinConnection::PinConnection( OutputPin* producer, InputPin* consumer ) :
        m_producer( producer ),
        m_consumer( consumer )
{
    m_producer->addConnection(this);

    assert(m_consumer.isValid());
    assert(m_producer.isValid());

    assert(!m_consumer->isConnected());
    m_consumer->setConnection(this);
}

PinConnection::~PinConnection()
{
    //TODO remove connections?
}

bool PinConnection::arePinDataTypesEqual()
{
    return( m_producer->getTypeInfo() == m_consumer->getTypeInfo() );
}

bool PinConnection::hasData()
{
    QMutexLocker lock( &m_mutex );
    return !m_queue.empty();
}

RefPtr<Data> PinConnection::get()
{
    QMutexLocker lock( &m_mutex );
    if( !m_queue.empty() )
    {
        RefPtr<Data> data = m_queue.front();
        m_queue.pop();
        return data;
    }
    return RefPtr<Data>( 0 );
}

void PinConnection::put( RefPtr<Data> data )
{
    QMutexLocker lock( &m_mutex );
    qDebug() << "PinConnection queue length: " << m_queue.size();
    m_queue.push( data );
}

