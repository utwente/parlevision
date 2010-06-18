#ifndef PIN_H
#define PIN_H

#include <assert.h>
#include <typeinfo>
#include <stack>

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

        virtual void removeConnection( PinConnection* connection ) = 0;
    };

    class IInputPin : public Pin
    {
    public:
        IInputPin( const QString& name, PipelineElement* owner ) :
                Pin( name, owner )
        {
        }

        virtual void scope() = 0;
        virtual void unscope() = 0;
        virtual bool isConnected() const = 0;

        /** @returns the std::type_info struct belonging to the type
          * this pin is initialized with. Is implemented by
          * the TypedPin sub class.
          */
        virtual const std::type_info& getTypeInfo() const = 0;

        virtual void setConnection(PinConnection* connection) = 0;

    protected:
        std::stack<Data*> m_scope;
    };

    class IOutputPin : public Pin
    {
        Q_OBJECT

    public:
        IOutputPin( const QString& name, PipelineElement* owner )
            : Pin( name, owner )
        {
        }

        virtual bool isConnected() const = 0;

        /** @returns the std::type_info struct belonging to the type
          * this pin is initialized with. Is implemented by
          * the TypedPin sub class.
          */
        virtual const std::type_info& getTypeInfo() const = 0;

        /** Adds a connection to the set of connections this pin outputs to
          * @ensure this->isConnected();
          */
        virtual void addConnection(PinConnection* connection) = 0;

    signals:
        void newData( RefPtr<Data> data );
    };

    template< class T >
    class OutputPin : public IOutputPin
    {
    public:
        OutputPin( const QString& name, PipelineElement* owner ) :
                IOutputPin( name, owner ) {}

        inline void put( RefPtr<T> data )
        {
            RefPtr<Data> untypedData = data.getPtr();

            emit( newData( untypedData ) );

            for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
                    itr != m_connections.end(); ++itr)
            {
                RefPtr<PinConnection> connection = *itr;
                assert(connection.isNotNull());
                connection->put( untypedData );
            }
        }

        /** Adds a connection to the set of connections this pin outputs to
          * @ensure this->isConnected();
          */
        virtual void addConnection( PinConnection* connection )
        {
            assert(connection != 0);

            this->m_connections.push_back(connection);
        }

        virtual void removeConnection( PinConnection* connection )
        {
            assert( connection != 0 );

            for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
                    itr != m_connections.end(); ++itr)
            {
                RefPtr<PinConnection> current = *itr;
                if( current.getPtr() == connection )
                {
                    m_connections.erase( itr );
                    return;
                }
            }
            // we should never be here
            assert( false );
        }

        virtual bool isConnected() const
        {
            return !m_connections.empty();
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }

        ~OutputPin() {}

    protected:
        // empty list by default
        std::list< RefPtr<PinConnection > > m_connections;

    };

    template< class T >
    class InputPin : public IInputPin
    {
    public:
        InputPin( const QString& name, PipelineElement* owner ) :
                IInputPin( name, owner ) {}

        virtual void scope()
        {
            assert( m_scope.empty() );
            // prepares stack to receive objects
            // for current scope
        }

        virtual void unscope()
        {
            // release all objects
            while( !m_scope.empty() )
            {
                Data* d = m_scope.top();
                m_scope.pop();
                d->dec();
            }
        }

        inline void push( Data* d )
        {
            assert( d != 0 );

            // increase ref count and store on stack
            d->inc();
            m_scope.push( d );
        }

        T* get()
        {
            if( this->m_connection.isNotNull() &&
                this->m_connection->hasData() )
            {
                Data* data = this->m_connection->get();
                push( data );
#ifdef DEBUG
                // we use a dynamic cast in debug mode
                // so we can check for correct casting
                // at the cost of speed.
                T* typedData = dynamic_cast<T*>( data );
                assert( typedData != 0 );
#else
                // We can safely do this because this Pin is
                // guaranteed to be connected to a Pin of the
                // same type T.
                T* typedData = static_cast<T*>( data );
#endif
                return typedData;
            }
            return 0;
        }

        /** Connects this pin through the given connection.
          * @require !this->isConnected()
          * @ensure this->isConnected();
          */
        virtual void setConnection(PinConnection* connection)
        {
            assert(!this->isConnected());
            assert(connection != 0);

            this->m_connection = connection;
        }

        virtual void removeConnection( PinConnection* connection )
        {
            assert( connection != 0 );
            assert( connection == m_connection );

            // clear connection
            m_connection.set( 0 );
        }

        virtual bool isConnected() const
        {
            return this->m_connection.isNotNull();
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }

        ~InputPin() {}

    protected:

        /** isNull() if there is no connection */
        RefPtr<PinConnection> m_connection;
    };
}

#endif // PIN_H
