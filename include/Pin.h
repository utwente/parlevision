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
            assert( m_owner.isNotNull() );
        }

        const QString& getName() { return m_name; }

    protected:
         /** the name of this Pin e.g. "BlackAndWhite" */
        QString m_name;

        /** data received from or delivered to this pipeline element */
        RefPtr<PipelineElement> m_owner;

        /** RefCounted objects have protected destructor */
        ~Pin() {}

    public:
        inline PipelineElement* getOwner() const { return m_owner.getPtr(); }

        virtual bool isConnected() const = 0;

        /** @returns the std::type_info struct belonging to the type
          * this pin is initialized with. Is implemented by
          * the TypedPin sub class.
          */
        virtual const std::type_info& getTypeInfo() const = 0;

    signals:
        void newData( RefPtr<Data> data );

    };

    class OutputPin : public Pin
    {
    public:
        OutputPin( const QString& name, PipelineElement* owner ) :
                Pin( name, owner ) {}

        inline void put( RefPtr<Data> data )
        {
            emit( newData( data ) );

            for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
                    itr != m_connections.end(); ++itr)
            {
                RefPtr<PinConnection> connection = *itr;

                assert(connection.isNotNull());
                connection->put( data );
            }
        }

        virtual bool isConnected() const;

        /** Adds a connection to the set of connections this pin outputs to
          * @ensure this->isConnected();
          */
        void addConnection(PinConnection* connection);

    protected:
        ~OutputPin() {}
        // empty list by default
        std::list< RefPtr<PinConnection> > m_connections;
    };

    class InputPin : public Pin
    {
    public:
        InputPin( const QString& name, PipelineElement* owner ) :
                Pin( name, owner ) {}

        inline RefPtr<Data> get()
        {
            if( this->m_connection.isNotNull() &&
                this->m_connection->hasData() )
            {
                RefPtr<Data> obj = this->m_connection->get();
                return obj;
            }
            return 0;
        }

        virtual bool isConnected() const;

        /** Connects this pin through the given connection.
          * @require !this->isConnected()
          * @ensure this->isConnected();
          */
        void setConnection(PinConnection* connection);

    protected:
        ~InputPin() {}

        /** null if there is no connection */
        RefPtr<PinConnection>   m_connection;
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
