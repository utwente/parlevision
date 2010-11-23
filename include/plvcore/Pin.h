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

#include <QSet>
#include <QStringBuilder>

#include "RefPtr.h"
#include "RefCounted.h"
#include "PinConnection.h"
#include "PipelineElement.h"
#include "Types.h"
#include "PlvExceptions.h"
#include "OpenCVImage.h"

namespace plv
{
    /** functor base class which can be implemented for compatibility callbacks */
//    class PLVCORE_EXPORT DataCompatibilityCallback
//    {
//    public:
//        virtual void operator()( const RefPtr<Data>& d ) throw (PlvRuntimeException) = 0;
//    };

    class PLVCORE_EXPORT Pin : public QObject, public RefCounted
    {
        Q_OBJECT

    public:

        Pin( const QString& name, PipelineElement* owner ) :
            m_name( name ),
            m_owner( owner )//,
            //m_callback( 0 )
        {
            assert( !m_name.isEmpty() );
            assert( m_owner != 0 );
        }

        inline const QString& getName() const { return m_name; }
        inline PipelineElement* getOwner() const { return m_owner; }

        /** @returns the std::type_info struct belonging to the type
          * this pin is initialized with. Is implemented by
          * the TypedPin sub class. */
        virtual const std::type_info& getTypeInfo() const = 0;

        /** returns true if it accepts a connection with the other pin */
        //virtual bool acceptsConnectionWith( const Pin* pin, QString& errStr ) const = 0;

        /** Checks if data is according to contract if the compatibility
          * callback is set. Does not check if no compatibility callback is
          * set. Throws a PlvRuntimeException if data is not compatible
          */
//        inline void checkData( const RefPtr<Data>& data ) const
//        {
//            if( m_callback != 0 )
//                (*m_callback)( data );
//        }

    protected:
         /** the name of this Pin e.g. "BlackAndWhite" */
        QString m_name;

        /** data received from or delivered to this pipeline element */
        PipelineElement* m_owner;

        /** functor callback to check for data compatibility. Defaults to 0 */
        //DataCompatibilityCallback* m_callback;
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

        inline InputPinType getType() const { return m_type; }
        inline bool isRequired() const { return m_type == INPUT_REQUIRED; }
        inline bool isOptional() const { return m_type == INPUT_OPTIONAL; }

        // TODO
        inline bool isSynchronous() const { return true; }
        inline bool isAsynchronous() const { return false; }

        void setConnection(PinConnection* connection);
        void removeConnection();
        PinConnection* getConnection() const;
        bool hasData() const;
        unsigned int getNextSerial() const;
        void flushConnection();
        bool fastforward( unsigned int target );
        void getUntyped(RefPtr<Data>& dataPtr) throw ( PlvRuntimeException );

        /** @returns true when pin is connected */
        bool isConnected() const;

        /** this method is called before each call to process() in the owner */
        void pre();

        /** this method is called after each call to process() in the owner */
        void post();

        /** set called to value of val */
        inline void setCalled( bool val ) { m_called = val; }

        /** returns true when get() has been called */
        inline bool isCalled() const { return m_called; }

        /** methods not implemented from abstract Pin class */
        virtual const std::type_info& getTypeInfo() const = 0;

        /** checks if this pin is compatible with output pin */
        virtual bool acceptsConnectionWith( const IOutputPin* pin, QString& errStr ) const = 0;

        /** checks if this pin is compatible with other pin */
        //virtual bool acceptsConnectionWith( const Pin* pin, QString& errStr ) const;

    protected:
        InputPinType m_type;

        /** isNull() if there is no connection */
        RefPtr<PinConnection> m_connection;

        /** temporarily holds data items in current scope to protect against
          * faulty processors
          */
        std::stack< RefPtr<Data> > m_scope;

        /** true when get() has been called */
        bool m_called;
    };

    class PLVCORE_EXPORT IOutputPin : public Pin
    {
        Q_OBJECT

    public:
        IOutputPin( const QString& name, PipelineElement* owner )
            : Pin( name, owner )//,
            //m_callback(0)
        {}

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

        std::list< RefPtr<PinConnection > > getConnections();
        int connectionCount() const;
        void putUntyped( const RefPtr<Data>& data );

        /** @returns true when pin is connected */
        inline bool isConnected() const { return !m_connections.empty(); }

        /** this method is called before each call to process() in the owner */
        void pre();

        /** this method is called after each call to process() in the owner */
        void post();

        /** returns wheter put() has been called since last pre() */
        inline bool isCalled() const { return m_called; }

        /** methods not implemented from abstract Pin class */
        virtual const std::type_info& getTypeInfo() const = 0;

        /** checks if this pin is compatible with input pin */
        virtual bool acceptsConnectionWith( const IInputPin* pin, QString& errStr ) const = 0;

        /** checks if this pin is compatible with other pin */
        //virtual bool acceptsConnectionWith( const Pin* pin, QString& errStr ) const;

    signals:
        void newData( plv::RefPtr<plv::Data> data );

    protected:
        std::list< RefPtr<PinConnection > > m_connections;

        /** true when put() has been called */
        bool m_called;

        /** functor callback to check for data compatibility. Defaults to 0 */
        //DataCompatibilityCallback* m_callback;
    };

    template< class T >
    class PLVCORE_EXPORT OutputPin : public IOutputPin
    {
    public:
        OutputPin( const QString& name, PipelineElement* owner ) :
                IOutputPin( name, owner ) {}

        /** Puts data in connection. Drops data if no connection present.
          * Throws an exception if the data violates format contract,
          * which is checked in the callback functor of IOutputPin
          */
        inline void put( const RefPtr<T>& typed )
        {
            RefPtr<Data> data = ref_ptr_static_cast<Data>(typed);
            putUntyped( data );
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }

        virtual bool acceptsConnectionWith( const IInputPin* pin,
                                            QString& errStr ) const
        {
            return true;
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

        // We can safely do this because this Pin is
        // guaranteed to be connected to a Pin of the
        // same type T.
        inline RefPtr<T> get() throw ( PlvRuntimeException )
        {
            // We can safely do this because this Pin is
            // guaranteed to be connected to a Pin of the
            // same type T.
            RefPtr<Data> data;
            this->getUntyped( data );
#ifdef QDEBUG
            RefPtr<T> typed = ref_ptr_dynamic_cast<T>(data);
            assert(typed.isNotNull());
#else
            RefPtr<T> typed = ref_ptr_static_cast<T>(data);
#endif
            return typed;
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }

        virtual bool acceptsConnectionWith( const IOutputPin* pin,
                                            QString& errStr ) const
        {
            return true;
        }
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
