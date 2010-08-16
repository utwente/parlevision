/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

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
#include "PlvExceptions.h"

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

        const QString& getName() const { return m_name; }

    protected:
         /** the name of this Pin e.g. "BlackAndWhite" */
        QString m_name;

        /** data received from or delivered to this pipeline element */
        RefPtr<PipelineElement> m_owner;

        /** RefCounted objects have protected destructor */
        ~Pin() {qDebug() << "Destroying Pin " << m_name;}

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
        typedef enum InputPinType {
            INPUT_OPTIONAL,
            INPUT_REQUIRED
        } InputPinType;

        IInputPin( const QString& name, PipelineElement* owner, InputPinType type = INPUT_REQUIRED ) :
                Pin( name, owner ), m_type( type )
        {
        }

        virtual void scope() = 0;
        virtual void unscope() = 0;
        virtual bool isConnected() const = 0;
        virtual bool hasData() const = 0;

        /** @returns the std::type_info struct belonging to the type
          * this pin is initialized with. Is implemented by
          * the TypedPin sub class.
          */
        virtual const std::type_info& getTypeInfo() const = 0;

        virtual void setConnection(PinConnection* connection) = 0;
        virtual void removeConnection() = 0;
        virtual PinConnection* getConnection() const = 0;

        InputPinType getType() const { return m_type; }

    protected:
        InputPinType m_type;
        std::stack< RefPtr<Data> > m_scope;
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
        virtual int connectionCount() const = 0;

        /** @returns the std::type_info struct belonging to the type
          * this pin is initialized with. Is implemented by
          * the TypedPin sub class.
          */
        virtual const std::type_info& getTypeInfo() const = 0;

        /** Adds a connection to the set of connections this pin outputs to
          * @ensure this->isConnected();
          */
        virtual void addConnection(PinConnection* connection) = 0;

        /** Removes the connection from this pin.
          * Assumes it has been disconnect()'ed
          */
        virtual void removeConnection( PinConnection* connection ) = 0;

        // TODO ugly hack to make things work ...
        // TODO should all PinConnection stuff be moved to IOutputPin
        // and should IOutputPin be called an interface then?
        // returns a copy of the connections
        virtual std::list< RefPtr<PinConnection> > getConnections() = 0;

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

            // this might be published to multiple processors which might be run in
            // multiple threads. Make immutable to prevent writes corrupting reads
            // in other threads.
            untypedData->makeImmutable();

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

        virtual void removeConnections()
        {
            for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
                    itr != m_connections.end(); ++itr)
            {
                RefPtr<PinConnection> current = *itr;
//                current->disconnect();
                m_connections.erase( itr );
            }
        }

        virtual bool isConnected() const
        {
            return !m_connections.empty();
        }

        virtual std::list< RefPtr<PinConnection > > getConnections()
        {
            // makes a copy
            return m_connections;
        }

        virtual int connectionCount() const
        {
            return m_connections.size();
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
        InputPin( const QString& name, PipelineElement* owner, InputPinType type = INPUT_REQUIRED ) :
                IInputPin( name, owner, type ) {}

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
                m_scope.pop();
            }
        }

        inline void push( RefPtr<Data> d )
        {
            assert( d.isNotNull() );

            // increase ref count and store on stack
            //d->inc();
            m_scope.push( d );
        }

        /** @returns true if there is a connection and that connection has data
          * available
          */
        virtual bool hasData() const
        {
            if( m_connection.isNotNull() )
            {
                return this->m_connection->hasData();
            }
            return false;
        }

        RefPtr<T> get() throw ( PipelineException )
        {
            if( !(this->m_connection.isNotNull() && this->m_connection->hasData() ))
            {
                throw PipelineException( "Illegal: method get() called on pin which has no data available" );
            }

            RefPtr<Data> data = this->m_connection->get();
            push( data );
#ifdef DEBUG
            // we use a dynamic cast in debug mode
            // so we can check for correct casting
            // at the cost of speed.
            RefPtr<T> typedData = ref_ptr_dynamic_cast<T>( data );
            if( typedData.isNull() )
            {
               throw PipelineException( "Data delivered to pin of incomaptible type " );
            }
#else
            // We can safely do this because this Pin is
            // guaranteed to be connected to a Pin of the
            // same type T.
            RefPtr<T> typedData = ref_ptr_static_cast<T>( data );
#endif
            return typedData;
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

        virtual void removeConnection()
        {
            assert( m_connection.isNotNull() );
            m_connection.set( 0 );
        }

        virtual PinConnection* getConnection() const
        {
            assert( m_connection.isNotNull() );
            return m_connection.getPtr();
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

    template <class T>
    OutputPin<T> * createOutputPin( const QString& name, PipelineElement* owner )
    throw (IllegalArgumentException)
    {
        // if add fails pin is automatically deleted and exception is thrown
        OutputPin<T> * pin = new OutputPin<T>( name, owner );
        owner->addOutputPin( pin );
        return pin;
    }

    template<typename T>
    InputPin<T> * createInputPin( const QString& name, PipelineElement* owner, IInputPin::InputPinType type = IInputPin::INPUT_REQUIRED )
    throw (IllegalArgumentException)
    {
        // if add fails pin is automatically deleted and exception is thrown
        InputPin<T> * pin = new InputPin<T>( name, owner, type );
        owner->addInputPin( pin );
        return pin;
    }
}

#endif // PIN_H
