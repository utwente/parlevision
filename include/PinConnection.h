#ifndef PINCONNECTION_H
#define PINCONNECTION_H

#include <queue>
#include <QMutexLocker>

#include "RefPtr.h"
#include "RefCounted.h"

namespace plv
{
    class InputPin;
    class OutputPin;
    class Data;

    class PinConnection : public RefCounted
    {
    public:
        PinConnection(OutputPin* producer, InputPin* consumer );

        bool arePinDataTypesEqual();

        virtual bool hasData();

        RefPtr<Data> get();
        void put( RefPtr<Data> );

    protected:
        virtual ~PinConnection();

        RefPtr<OutputPin> m_producer;
        RefPtr<InputPin> m_consumer;

        std::queue< RefPtr<Data> > m_queue;

        QMutex m_mutex;
    };
}

#endif // PINCONNECTION_H
