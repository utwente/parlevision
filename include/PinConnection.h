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
    class ValueData;
    class ReferencedData;

    class PinConnection : public RefCounted
    {
    public:
        PinConnection( Pin* in, Pin* out ) :
                m_in( in ),
                m_out( out )
        {
        }

        bool isConnected();
        bool arePinDataTypesEqual();

        virtual bool hasData() = 0;

    protected:
        virtual ~PinConnection() {}

        RefPtr<Pin> m_in;
        RefPtr<Pin> m_out;

        QMutex m_mutex;
    };

    class PinConnectionValueData : public PinConnection
    {
    public:
        PinConnectionValueData( Pin* in, Pin* out );

        virtual bool hasData();

        ValueData get();
        void put( ValueData data );

    protected:
        virtual ~PinConnectionValueData();

        std::queue< ValueData > m_queue;
    };

    class PinConnectionReferencedData : public PinConnection
    {
    public:
        PinConnectionReferencedData( Pin* in, Pin* out );

        virtual bool hasData();

        ReferencedData* get();
        void put( ReferencedData* );

    protected:
        virtual ~PinConnectionReferencedData();

        std::queue< RefPtr<ReferencedData> > m_queue;
    };
}

#endif // PINCONNECTION_H
