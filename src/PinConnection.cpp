#include "PinConnection.h"

#include "Types.h"
#include "Pin.h"

using namespace plv;

PinConnection::PinConnection( IOutputPin* producer, IInputPin* consumer ) :
        m_producer( producer ),
        m_consumer( consumer )
{
    m_producer->addConnection(this);

    assert(m_consumer.isNotNull());
    assert(m_producer.isNotNull());

    assert(!m_consumer->isConnected());
    m_consumer->setConnection(this);
}

PinConnection::~PinConnection()
{
    //TODO remove connections?
}
