#ifndef PINCONNECTION_H
#define PINCONNECTION_H

#include <queue>
#include <QMutexLocker>

#include "RefPtr.h"
#include "RefCounted.h"
#include "PlvExceptions.h"

namespace plv
{
    class Data;
    class IOutputPin;
    class IInputPin;

    class PinConnection : public RefCounted
    {
        enum ConnectionType {
            LOSSLESS,
            LOSSY_FIFO,
            LOSSY_LIFO
        };


        friend class Pipeline;
    public:
        PinConnection( IOutputPin* producer, IInputPin* consumer )
                throw ( IncompatibleTypeException, DuplicateConnectionException );
        ~PinConnection();

        bool hasData();
        int size();
        inline ConnectionType getType();
        RefPtr<Data> get() throw ( PipelineException );
        void put( RefPtr<Data> data );

        /** Throw away any data in this connection.
          */
        void flush();
        RefPtr<const IOutputPin> fromPin() const;
        RefPtr<const IInputPin>  toPin() const;

    protected:
        void connect() throw ( IncompatibleTypeException, DuplicateConnectionException );

        /** Disconnect this connection from its pins
          * this makes it a "floating" connection that can safely be removed.
          * this does not notify the pipeline or remove itself from it.
          * you probably won't call this directly,
          * use Pipeline::disconnect(connection) instead
          */
        void disconnect();

        RefPtr<IOutputPin> m_producer;
        RefPtr<IInputPin>  m_consumer;
        std::queue< RefPtr<Data> > m_queue;
        ConnectionType m_type;
        QMutex m_mutex;
    };
}

namespace plv
{
    PinConnection::ConnectionType PinConnection::getType() { return m_type; }
}

#endif // PINCONNECTION_H
