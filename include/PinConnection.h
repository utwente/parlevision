#ifndef PINCONNECTION_H
#define PINCONNECTION_H

#include <queue>

#include <QMutex>
#include <QMutexLocker>

#include "RefPtr.h"
#include "RefCounted.h"

namespace plv
{
    class Pin;
    class Data;

    class PinConnection : public RefCounted
    {
    public:
        PinConnection( Pin* in, Pin* out );

        bool isConnected();
        bool arePinDataTypesEqual();

        virtual bool hasData();

        Data* get();
        void put( Data* );

    protected:
        virtual ~PinConnection();

        RefPtr<Pin> m_in;
        RefPtr<Pin> m_out;

        std::queue< RefPtr<Data> > m_queue;

        QMutex m_mutex;
    };
}

#endif // PINCONNECTION_H
