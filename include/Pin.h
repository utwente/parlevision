#ifndef PIN_H
#define PIN_H

#include <assert.h>
#include <typeinfo>
#include <QImage>
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
        virtual void scope() = 0;
        virtual void unscope() = 0;
        virtual bool isConnected() const = 0;

    protected:
        std::stack<Data*> m_scope;
    };

    class IOutputPin : public Pin
    {

    };













    template< typename T >
    class OutputPin : public IOutputPin
    {
        Q_OBJECT

    public:
        OutputPin( const QString& name, PipelineElement* owner ) :
                Pin( name, owner ) {}

        inline void put( T* data )
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

        void addConnection( PinConnection<T>* connection )
        {
            assert(connection != 0);

            this->m_connections.push_back(connection);
        }

        virtual bool isConnected() const
        {
            return !m_connections.empty();
        }

        /** Adds a connection to the set of connections this pin outputs to
          * @ensure this->isConnected();
          */
        void addConnection(PinConnection<T>* connection);

    protected:
        ~OutputPin() {}
        // empty list by default
        std::list< RefPtr<PinConnection<T> > > m_connections;

    signals:
        void newData( RefPtr<T> data );

    };












    template< typename T >
    class InputPin : public IInputPin
    {
        Q_OBJECT

    public:
        InputPin( const QString& name, PipelineElement* owner ) :
                Pin( name, owner ) {}

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
                T* d = m_scope.top();
                m_scope.pop();
                d->dec();
            }
        }

        void push( T* d )
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
                RefPtr<T> obj = this->m_connection->get();
                if( obj.isNotNull() )
                {
                    T* d = obj.getPtr();
                    push( d );
                    return d;
                }
            }
            return 0;
        }


        /** Connects this pin through the given connection.
          * @require !this->isConnected()
          * @ensure this->isConnected();
          */
        void setConnection(PinConnection<T>* connection)
        {
            assert(!this->isConnected());
            assert(connection != 0);

            this->m_connection = connection;
        }

        virtual bool isConnected() const
        {
            return this->m_connection.isNotNull();
        }

    protected:
        ~InputPin() {}

        /** null if there is no connection */
        RefPtr<PinConnection<T>>   m_connection;


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
