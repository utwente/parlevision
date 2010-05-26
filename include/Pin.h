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




    template< class N >
    class OutputPin : public IOutputPin
    {
    public:
        OutputPin( const QString& name, PipelineElement* owner ) :
                IOutputPin( name, owner ) {}

        inline void put( N* data )
        {
            RefPtr<N> theData = data;

            emit( newData( RefPtr<Data>(theData) ) );

            for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
                    itr != m_connections.end(); ++itr)
            {
                RefPtr<PinConnection> connection = *itr;

                assert(connection.isNotNull());
                connection->put( theData );
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

        virtual bool isConnected() const
        {
            return !m_connections.empty();
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( N );
        }

        ~OutputPin() {}

    protected:
        // empty list by default
        std::list< RefPtr<PinConnection > > m_connections;

    };












    template< class J >
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

        J* get()
        {
            if( this->m_connection.isNotNull() &&
                this->m_connection->hasData() )
            {
                Data* data = this->m_connection->get();
                push( data );

                // We can safely do this because this Pin is
                // guaranteed to be connected to a Pin of the
                // same type T.
                J* typedData = reinterpret_cast<J*>( data );

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

        virtual bool isConnected() const
        {
            return this->m_connection.isNotNull();
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( J );
        }

        ~InputPin() {}

    protected:

        /** null if there is no connection */
        RefPtr<PinConnection>   m_connection;
    };

//    template< class T >
//    class TypedInputPin : public InputPin
//    {
//    public:
//        TypedInputPin( const QString& name, PipelineElement* owner ) :
//                InputPin( name, owner ) {}
//
//        virtual const std::type_info& getTypeInfo() const
//        {
//            return typeid( T );
//        }
//
//        virtual T* get()
//        {
//            return dynamic_cast<T*>( InputPin::get() );
//        }
//
//        virtual Data* get()
//        {
//            if( this->m_connection.isNotNull() &&
//                this->m_connection->hasData() )
//            {
//                RefPtr<Data> obj = this->m_connection->get();
//                if( obj.isNotNull() )
//                {
//                    Data* d = obj.getPtr();
//                    push( d );
//                    return d;
//                }
//            }
//            return 0;
//        }
//    };
//
//    template< class T >
//    class TypedOutputPin : public OutputPin
//    {
//    public:
//        TypedOutputPin( const QString& name, PipelineElement* owner ) :
//                OutputPin( name, owner ) {}
//
//        virtual const std::type_info& getTypeInfo() const
//        {
//            return typeid( T );
//        }
//    };
}

#endif // PIN_H
