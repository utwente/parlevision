#ifndef PIN_H
#define PIN_H

#include <assert.h>
#include <typeinfo>
#include <QImage>

#include "RefPtr.h"
#include "RefCounted.h"
#include "PinConnection.h"
#include "PipelineElement.h"
#include "Types.h"

namespace plv
{
    class Pin : public QObject, public RefCounted
    {
        Q_OBJECT

    public:

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

        inline bool isConnected() const
        {
            return m_connection.isValid() && m_connection->isConnected();
        }

        /** @returns the std::type_info struct belonging to the type
          * this pin is initialized with. Is implemented by
          * the TypedPin sub class.
          */
        virtual const std::type_info& getTypeInfo() const = 0;

    signals:
        void newData(Data* data);

    };

    class OutputPin : public Pin
    {
    public:
        OutputPin( const QString& name, PipelineElement* owner ) :
                Pin( name, owner ) {}

        void put( Data* obj )
        {
            if( this->m_connection.isValid() )
            {
                emit( newData( obj ) );
                this->m_connection->put( obj );
            }
        }

    protected:
        ~OutputPin() {}
    };

    class InputPin : public Pin
    {
    public:
        InputPin( const QString& name, PipelineElement* owner ) :
                Pin( name, owner ) {}

        inline Data* get()
        {
            if( this->m_connection.isValid() &&
                this->m_connection->hasData() )
            {
                Data* obj = this->m_connection->get();
                emit( newData ( obj ) );
                return obj;
            }
            return 0;
        }

    protected:
        ~InputPin() {}
    };

    template< class T >
    class TypedInputPin : public InputPin
    {
    public:
        TypedInputPin( const QString& name, PipelineElement* owner ) :
                InputPin( name, owner ) {}

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }
    };

    template< class T >
    class TypedOutputPin : public OutputPin
    {
    public:
        TypedOutputPin( const QString& name, PipelineElement* owner ) :
                OutputPin( name, owner ) {}

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }
    };





}

#endif // PIN_H
