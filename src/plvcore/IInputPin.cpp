#include "IInputPin.h"
#include "DataConsumer.h"

using namespace plv;

IInputPin::IInputPin( const QString& name, DataConsumer* consumer,
                      Required required, Synchronized sync ) :
        Pin( name, consumer ),
        m_consumer( consumer ),
        m_required( required ),
        m_synchronous( sync )
{
    assert( m_consumer != 0 );
}

IInputPin::~IInputPin()
{
    m_consumer = 0;
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
    m_consumer->onInputConnectionSet(this,connection);
}

void IInputPin::removeConnection()
{
    assert( m_connection.isNotNull() );
    PinConnection* con = m_connection;
    m_connection.set( 0 );
    m_consumer->onInputConnectionRemoved(this, con);
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

void IInputPin::acceptData(const Data& data)
{
    m_consumer->newData(this, data.getSerial());
}

void IInputPin::getVariant(QVariant& v)
{
    // check if get is not called twice during one process call
    if( m_called )
    {
        QString msg = tr("Illegal: method get() called twice during process() "
                         "on InputPin %1 of processor %2.")
                      .arg(m_name)
                      .arg(m_consumer->getName());
        throw RuntimeError(msg,__FILE__, __LINE__ );
    }
    m_called = true;
    if( !(this->m_connection.isNotNull() &&
          this->m_connection->hasData() ))
    {
        QString msg = tr("Illegal: method get() called on InputPin which "
                         "has no data available. Pin name is %1 of processor %2")
                        .arg(m_name)
                        .arg(m_consumer->getName());
        throw RuntimeError(msg, __FILE__, __LINE__);
    }
    Data d = m_connection->get();
    v.setValue(d.getPayload());
}
