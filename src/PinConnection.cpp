#include "PinConnection.h"
#include "Types.h"
#include "Pin.h"

#include <QStringBuilder>

using namespace plv;

PinConnection::PinConnection( IOutputPin* producer, IInputPin* consumer )
    throw ( IncompatibleTypeException ) :
        m_producer( producer ),
        m_consumer( consumer )
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

void PinConnection::connect() throw (IncompatibleTypeException)
{
    QMutexLocker lock( &m_mutex );

    assert(m_consumer.isNotNull());
    assert(m_producer.isNotNull());

    m_producer->addConnection(this);

    assert(!m_consumer->isConnected());
    m_consumer->setConnection(this);
    assert( m_consumer->isConnected() );

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

        throw IncompatibleTypeException( errStr.toStdString() );
    }
}

void PinConnection::disconnect()
{
    QMutexLocker lock( &m_mutex );
    m_producer->removeConnection( this );
    //m_consumer->removeConnection( this );
    m_consumer->removeConnection();

    // clear and free producer and consumer
    m_producer.set( 0 );
    m_consumer.set( 0 );
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
