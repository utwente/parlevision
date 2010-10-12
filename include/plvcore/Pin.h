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

#include <QStringBuilder>

#include "RefPtr.h"
#include "RefCounted.h"
#include "PinConnection.h"
#include "PipelineElement.h"
#include "Types.h"
#include "PlvExceptions.h"

namespace plv
{
    class PLVCORE_EXPORT Pin : public QObject, public RefCounted
    {
        Q_OBJECT

    public:

        Pin( const QString& name, PipelineElement* owner ) :
            m_name( name ),
            m_owner( owner )
        {
            assert( m_owner != 0 );
        }

        const QString& getName() const { return m_name; }
        inline PipelineElement* getOwner() const { return m_owner; }

        virtual bool isConnected() const = 0;

        /** @returns the std::type_info struct belonging to the type
          * this pin is initialized with. Is implemented by
          * the TypedPin sub class.
          */
        virtual const std::type_info& getTypeInfo() const = 0;

    protected:
         /** the name of this Pin e.g. "BlackAndWhite" */
        QString m_name;

        /** data received from or delivered to this pipeline element */
        PipelineElement* m_owner;
    };

    class PLVCORE_EXPORT IInputPin : public Pin
    {
    public:
        typedef enum InputPinType {
            INPUT_OPTIONAL,
            INPUT_REQUIRED
        } InputPinType;

        IInputPin( const QString& name, PipelineElement* owner,
                   InputPinType type = INPUT_REQUIRED ) :
                Pin( name, owner ),
                m_type( type )
        {
        }

        virtual ~IInputPin();

        InputPinType getType() const { return m_type; }
        bool isRequired() const { return m_type == INPUT_REQUIRED; }
        bool isOptional() const { return m_type == INPUT_OPTIONAL; }

        // TODO
        bool isSynchronous() const { return true; }
        bool isAsynchronous() const { return false; }

        virtual void setConnection(PinConnection* connection);
        virtual void removeConnection();
        virtual PinConnection* getConnection() const;
        virtual bool isConnected() const;
        virtual bool hasData() const;
        virtual unsigned int getNextSerial() const;
        virtual void flushConnection();
        virtual bool fastforward( unsigned int target );

        /** this method is called before each call to process() in the owner */
        virtual void pre() = 0;

        /** this method is called after each call to process() in the owner */
        virtual void post() = 0;

        /** returns whether this pin has been used after the pre call */
        virtual bool called() const = 0;

    protected:
        InputPinType m_type;

        /** isNull() if there is no connection */
        RefPtr<PinConnection> m_connection;
    };

    class PLVCORE_EXPORT IOutputPin : public Pin
    {
        Q_OBJECT

    public:
        IOutputPin( const QString& name, PipelineElement* owner )
            : Pin( name, owner )
        {
        }

        virtual ~IOutputPin();

        /** Adds a connection to the set of connections this pin outputs to
          * @ensure this->isConnected();
          */
        void addConnection( PinConnection* connection );

        /** Removes the connection from this pin.
          * Assumes it has been disconnect()'ed
          */
        void removeConnection( PinConnection* connection );

        void removeConnections();

        virtual bool isConnected() const
        {
            return !m_connections.empty();
        }

        inline std::list< RefPtr<PinConnection > > getConnections()
        {
            // makes a copy
            return m_connections;
        }

        inline int connectionCount() const
        {
            return m_connections.size();
        }

    signals:
        void newData( RefPtr<Data> data );

    protected:
        std::list< RefPtr<PinConnection > > m_connections;
    };

    template< class T >
    class PLVCORE_EXPORT OutputPin : public IOutputPin
    {
    public:
        OutputPin( const QString& name, PipelineElement* owner ) :
                IOutputPin( name, owner ) {}

        inline void put( RefPtr<T> data )
        {
            RefPtr<Data> untypedData = data.getPtr();

            // this might be published to multiple processors which might run in
            // multiple threads. Make immutable to prevent writes corrupting reads
            // in other threads.
            untypedData->makeImmutable();

            // propagate the serial number
            untypedData->setSerial( m_owner->getProcessingSerial() );

            // publish to viewers
            emit( newData( untypedData ) );

            // publish to all pin connections
            for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
                    itr != m_connections.end(); ++itr)
            {
                PinConnection* connection = (*itr).getPtr();
                connection->put( untypedData );
            }
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }
    };

    template< class T >
    class PLVCORE_EXPORT InputPin : public IInputPin
    {
    public:
        InputPin( const QString& name, PipelineElement* owner,
                  InputPinType type = INPUT_REQUIRED ) :
                  IInputPin( name, owner, type )
        {
        }

        virtual ~InputPin() {}

        /**
         * prepares stack to receive objects
         * for current scope
         * extra saveguard against faulty processors which
         * do not use wrappers for proper reference counting
         */
        virtual void pre()
        {
            assert( m_scope.empty() );
            m_called = false;
        }

        virtual void post()
        {
            // release all objects
            while( !m_scope.empty() )
            {
                m_scope.pop();
            }
        }

        /** returns wheter get() has been called since last pre() */
        virtual bool called() const
        {
            return m_called;
        }

        RefPtr<T> get() throw ( PlvRuntimeException )
        {
            // check if get is not called twice during one process call
            if( m_called )
            {
                QString processorName = this->m_owner->getName();
                QString msg = "Illegal: method get() called twice "
                              "during process() on pin which has "
                              "no data available. Pin name is " %
                              this->m_name %
                              " of processor " % processorName;
                throw PlvRuntimeException(msg,__FILE__, __LINE__ );
            }
            m_called = true;
            if( !(this->m_connection.isNotNull() &&
                  this->m_connection->hasData() ))
            {

                QString processorName = this->m_owner->getName();
                QString msg = "Illegal: method get() called on pin "
                              "which has no data available. Pin name is " %
                              this->m_name %
                              " of processor " % processorName;
                throw PlvRuntimeException(msg, __FILE__, __LINE__);
            }

            // We can safely do this because this Pin is
            // guaranteed to be connected to a Pin of the
            // same type T.
            RefPtr<T> data = ref_ptr_static_cast<T>( this->m_connection->get() );
            m_scope.push( data );

            return data;
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }

    protected:
        /** temporarily holds data items in current scope to protect against
          * faulty processors
          */
        std::stack< RefPtr<T> > m_scope;

        /** true when get() has been called */
        bool m_called;
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
    InputPin<T> * createInputPin( const QString& name, PipelineElement* owner,
                                  IInputPin::InputPinType type = IInputPin::INPUT_REQUIRED )
    throw (IllegalArgumentException)
    {
        // if add fails pin is automatically deleted and exception is thrown
        InputPin<T> * pin = new InputPin<T>( name, owner, type );
        owner->addInputPin( pin );
        return pin;
    }
}

#endif // PIN_H
