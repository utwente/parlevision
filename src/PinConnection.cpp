#include "PinConnection.h"

#include "Types.h"
#include "Pin.h"

using namespace plv;

PinConnection::PinConnection( Pin* in, Pin* out ) :
        m_in( in ),
        m_out( out )
{
}

PinConnection::~PinConnection()
{
}

bool PinConnection::arePinDataTypesEqual()
{
    return( m_in->getTypeInfo() == m_out->getTypeInfo() );
}

bool PinConnection::isConnected()
{
    //TODO what is this function supposed to do?
    QMutexLocker lock( &m_mutex );
    return true;
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
        return m_queue.front();
    }
    return RefPtr<Data>( 0 );
}

void PinConnection::put( RefPtr<Data> data )
{
    QMutexLocker lock( &m_mutex );
    m_queue.push( data );
}

