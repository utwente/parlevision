#ifndef PINCONNECTION_H
#define PINCONNECTION_H

#include <queue>
#include <QMutexLocker>

#include "RefPtr.h"
#include "RefCounted.h"

namespace plv
{
    class IInputPin;
    class IOutputPin;
    class Data;

    class IPinConnection : public RefCounted
    {
    public:
        virtual int  size();
        virtual bool hasData();
    };

    template< typename T >
    class PinConnection : public IPinConnection
    {
    public:
        PinConnection( IOutputPin* producer, IInputPin* consumer );
        ~PinConnection();

        virtual bool hasData()
        {
            QMutexLocker lock( &m_mutex );
            return !m_queue.empty();
        }

        virtual int size()
        {
            QMutexLocker lock( &m_mutex );
            return static_cast<int>( m_queue.size() );
        }

        T* get()
        {
            QMutexLocker lock( &m_mutex );
            if( !m_queue.empty() )
            {
                RefPtr<T> data = m_queue.front();
                T* d = data.getPtr();
                d->dec();
                m_queue.pop();
                return d;
            }
            return 0;
        }

        void put( T* data )
        {
            QMutexLocker lock( &m_mutex );
            data->inc();
            m_queue.push( data );
        }

    protected:
        RefPtr<IOutputPin> m_producer;
        RefPtr<IInputPin>  m_consumer;

        std::queue<T*> m_queue;

        QMutex m_mutex;
    };
}

#endif // PINCONNECTION_H
