#ifndef PIN_H
#define PIN_H

#include <assert.h>
#include <typeinfo>
#include <QImage>

#include "RefPtr.h"
#include "RefCounted.h"
#include "PinConnection.h"
#include "PipelineElement.h"

namespace plv {
    class Pin : public RefCounted
    {
    public:
        typedef enum _PinType {
            CONSUMER,
            PRODUCER
        } PinType;

        Pin( const QString& name, PipelineElement* owner ) :
            m_name( name ),
            m_owner( owner )
        {
            assert( m_owner.isValid() );
        }

        const QString& getName() { return m_name; }

    protected:
         /** the name of this Pin e.g. "BlackAndWhite" */
        QString m_name;

        /** data received from or delivered to this pipeline element */
        RefPtr<PipelineElement> m_owner;

        /** null if there is no connection */
        RefPtr<PinConnection>   m_connection;

        /** RefCounted objects have protected destructor */
        ~Pin() {};

    public:
        inline PipelineElement* getOwner() const { return m_owner.getPtr(); }
        inline PinConnection* getPinConnection() const { return m_connection.getPtr(); }

        /** @returns the std::type_info struct belonging to the type
          * this pin is initialized with. Is implemented by
          * the TypedPin sub class.
          */
        virtual const std::type_info& getType() const = 0;
    };

    template< class T >
    class TypedPin : public Pin
    {
    public:
        TypedPin( const QString& name, PipelineElement* owner ) :
                Pin( name, owner ) {}

        virtual const std::type_info& getType() const
        {
            return typeid( T );
        }
    };

    template< class T >
    class TypedConsumerPin : public TypedPin<T>
    {
    public:
        inline T* get()
        {
            if( this->m_connection.isValid() )
            {
                if( this->m_connection->isConnected() )
                {
                    if( this->m_connection->hasData() )
                    {
                        return (T*) this->m_connection->get();
                    }
                }
            }
        }
    };

    template< class T >
    class TypedProducerPin : public TypedPin<T>
    {
    public:
        void put( T* obj )
        {
            if( this->m_connection.isValid() )
            {
                if( this->m_connection->isConnected() )
                {
                    this->m_connection->put( obj );
                }
            }
        }
    };

}

#endif // PIN_H
