#include "PinConnection.h"
#include "Types.h"
#include "Pin.h"

using namespace plv;

bool PinConnection::arePinDataTypesEqual()
{
    return( m_in->getType() == m_out->getType() );
}

bool PinConnection::isConnected()
{
    //TODO what is this function supposed to do?
    QMutexLocker lock( &m_mutex );
    return true;
}

/****************************************************************************
 * PinConnectionValueData
 ****************************************************************************/

PinConnectionValueData::PinConnectionValueData( Pin* in, Pin* out ) :
       PinConnection( in, out )
{
}

PinConnectionValueData::~PinConnectionValueData()
{
}

bool PinConnectionValueData::hasData()
{
    QMutexLocker lock( &m_mutex );
    return !m_queue.empty();
}

ValueData PinConnectionValueData::get()
{
    QMutexLocker lock( &m_mutex );
    ValueData data = m_queue.front();
    return data;
}

void PinConnectionValueData::put( ValueData data )
{
    QMutexLocker lock( &m_mutex );
    m_queue.push( data );
}

/****************************************************************************
 * PinConnectionReferencedData
 ****************************************************************************/

PinConnectionReferencedData::PinConnectionReferencedData( Pin* in, Pin* out ) :
       PinConnection( in, out )
{
}

PinConnectionReferencedData::~PinConnectionReferencedData()
{
}


bool PinConnectionReferencedData::hasData()
{
    QMutexLocker lock( &m_mutex );
    return !m_queue.empty();
}

// TODO add exception
ReferencedData* PinConnectionReferencedData::get()
{
    QMutexLocker lock( &m_mutex );
    if( !m_queue.empty() )
    {
        RefPtr<ReferencedData> data = m_queue.front();
        return data.getPtr();
    }
    return 0;
}

void PinConnectionReferencedData::put( ReferencedData* data )
{
    QMutexLocker lock( &m_mutex );
    m_queue.push( RefPtr<ReferencedData>(data) );
}

