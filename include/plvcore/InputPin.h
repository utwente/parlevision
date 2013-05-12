#ifndef INPUTPIN_H
#define INPUTPIN_H

#include "IInputPin.h"

namespace plv
{
    template< class T >
    class InputPin : public IInputPin
    {
    public:
        InputPin( const QString& name, DataConsumer* consumer, Required required, Synchronized sync ) :
                  IInputPin( name, consumer, required, sync )
        {
        }

        /** Gets a value from the connection and tries to convert it to the
            type id of this pin. Returns an empty newly constructed T if
            conversion fails */
        inline T get()
        {
            QVariant v;
            getVariant(v);
            if( !v.canConvert<T>() )
            {
               emit( error( "Type conversion error") );
            }
            return v.value<T>();
        }

        /** @returns the QMetaType typeId of the data type this pin is initialized with */
        virtual int getTypeId() const { return qMetaTypeId<T>(); }

        /** @returns the name of the type this pin is initialized with */
        virtual QString getTypeName() const { return QMetaType::typeName( qMetaTypeId<T>() ); }

        virtual bool acceptsConnectionWith( const IOutputPin* /*pin*/, QString& /*errStr*/ ) const { return true; }

        virtual bool isDynamicallyTyped() const { return false; }
    };

    template<typename T>
    plv::InputPin<T> * createInputPin( const QString& name, plv::DataConsumer* owner,
                                  plv::IInputPin::Required required = plv::IInputPin::CONNECTION_REQUIRED,
                                  plv::IInputPin::Synchronized synchronized = plv::IInputPin::CONNECTION_SYNCHRONOUS )
    throw (plv::IllegalArgumentException)
    {
        // if add fails pin is automatically deleted and exception is thrown
        plv::InputPin<T> * pin = new plv::InputPin<T>( name, owner, required, synchronized );
        owner->addInputPin( pin );
        return pin;
    }
}

#endif
