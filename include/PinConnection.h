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
    public:
        PinConnection( IOutputPin* producer, IInputPin* consumer ) throw ( IncompatibleTypeException );
        ~PinConnection();

        void connect() throw ( IncompatibleTypeException );
        void disconnect();

        bool hasData();
        int size();
        RefPtr<Data> get() throw ( PipelineException );
        void put( RefPtr<Data> data );

        RefPtr<const IOutputPin> fromPin() const;
        RefPtr<const IInputPin>  toPin() const;

    protected:
        RefPtr<IOutputPin> m_producer;
        RefPtr<IInputPin>  m_consumer;

        std::queue< RefPtr<Data> > m_queue;

        QMutex m_mutex;
    };
}

#endif // PINCONNECTION_H
