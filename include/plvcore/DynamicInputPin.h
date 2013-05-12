#ifndef DYNAMICINPUTPIN_H
#define DYNAMICINPUTPIN_H

#include "IInputPin.h"

namespace plv
{
    class DynamicInputPin : public IInputPin
    {
    private:
        /** The dynamically set type id */
        int m_typeId;
    public:
        DynamicInputPin( const QString& name, DataConsumer* consumer, Required required, Synchronized sync, int typeId ) :
                  IInputPin( name, consumer, required, sync ),
                  m_typeId( typeId )
        {
        }

        /** Gets a value from the connection and tries to convert it to the
            type id of this pin. Returns an empty newly constructed T if
            conversion fails */
        template <typename T>
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
        virtual int getTypeId() const { return m_typeId; }

        template <typename T> bool setTypeId() { return setTyeId( qMetaTypeId<T>() ); }

        /** sets the type id of this pin. This type id should be a
            valid id with Qt's metatype system, e.g. the value returned
            by qMetaTypeId<T>. Also see setTypeId() template method. */
        bool setTypeId( int type )
        {
            if( isConnected() || !QMetaType::isRegistered(type) ) return false;
            m_typeId = type;
            return true;
        }

        /** @returns the name of the type this pin is initialized with */
        virtual QString getTypeName() const { return QMetaType::typeName( m_typeId ); }

        virtual bool acceptsConnectionWith( const IOutputPin*, QString&) const
        {
            return true;
        }

        virtual bool isDynamicallyTyped() const { return true; }
    };

    PLVCORE_EXPORT plv::DynamicInputPin* createDynamicInputPin( const QString& name, plv::DataConsumer* owner,
                                  plv::IInputPin::Required required = plv::IInputPin::CONNECTION_REQUIRED,
                                  plv::IInputPin::Synchronized synchronized = plv::IInputPin::CONNECTION_SYNCHRONOUS,
                                  int typeId = 0 )
    throw (plv::IllegalArgumentException);
}

#endif
