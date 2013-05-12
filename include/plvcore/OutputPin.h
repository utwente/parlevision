#ifndef OUTPUTPIN_H
#define OUTPUTPIN_H

#include "IOutputPin.h"

namespace plv
{
    template< class T >
    class OutputPin : public IOutputPin
    {
    public:
        OutputPin( const QString& name, DataProducer* producer )
            : IOutputPin( name, producer ) {}
        virtual ~OutputPin() {}

        /** Puts data in connection. Drops data if no connection present. */
        inline void put( const T& data )
        {
            QVariant v;
            v.setValue<T>(data);
            unsigned int serial = m_producer->getProcessingSerial();
            putVariant(serial, v);
        }

        /** @returns the QMetaType typeId of the data type this pin is initialized with */
        virtual int getTypeId() const { return qMetaTypeId<T>(); }

        /** @returns the name of the type this pin is initialized with */
        virtual QString getTypeName() const { return QMetaType::typeName( qMetaTypeId<T>() ); }

        virtual bool acceptsConnectionWith( const IInputPin*, QString&) const { return true; }
    };

    template <class T>
    plv::OutputPin<T> * createOutputPin( const QString& name, plv::DataProducer* owner )
    throw (plv::IllegalArgumentException)
    {
        // if add fails pin is automatically deleted and exception is thrown
        plv::OutputPin<T> * pin = new plv::OutputPin<T>( name, owner );
        owner->addOutputPin( pin );
        return pin;
    }
}

#endif
