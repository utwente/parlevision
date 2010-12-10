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
    /** abstract base class of input and output pins */
    class PLVCORE_EXPORT Pin : public QObject, public RefCounted
    {
        Q_OBJECT

    public:

        Pin( const QString& name, PipelineElement* owner ) :
            m_name( name ),
            m_owner( owner )
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

        /** @returns the QMetaType typeId of the data type this pin is initialized with */
        virtual int getTypeId() const = 0;

        /** @returns the name of the type this pin is initialized with */
        virtual QString getTypeName() const = 0;

    signals:
        void newData( QVariant v );
        void error( QString msg );

    protected:
         /** the name of this Pin e.g. "BlackAndWhite" */
        QString m_name;

        /** data received from or delivered to this pipeline element */
        PipelineElement* m_owner;
    };

    class PLVCORE_EXPORT IInputPin : public Pin
    {
        Q_OBJECT

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

        /** @returns true when pin is connected */
        bool isConnected() const;

        /** this method is called before each call to process() in the owner */
        void pre();

        /** this method is called after each call to process() in the owner */
        void post();

        void getVariant( QVariant& data );

        /** set called to value of val */
        inline void setCalled( bool val ) { m_called = val; }

        /** returns true when get() has been called */
        inline bool isCalled() const { return m_called; }

        /** checks if this pin is compatible with output pin */
        virtual bool acceptsConnectionWith( const IOutputPin* pin, QString& errStr ) const = 0;

        /** methods not implemented from abstract Pin class */
        virtual const std::type_info& getTypeInfo() const = 0;
        virtual int getTypeId() const = 0;
        virtual QString getTypeName() const = 0;

    protected:
        /** The input pin type, either INPUT_OPTIONAL or INPUT_REQUIRED */
        InputPinType m_type;

        /** isNull() if there is no connection */
        RefPtr<PinConnection> m_connection;

        /** true when get() has been called */
        bool m_called;
    };

    class PLVCORE_EXPORT IOutputPin : public Pin
    {
        Q_OBJECT

    public:
        IOutputPin( const QString& name, PipelineElement* owner ) : Pin( name, owner ) {}
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

        /** @returns true when pin is connected */
        inline bool isConnected() const { return !m_connections.empty(); }

        /** this method is called before each call to process() in the owner */
        void pre();

        /** this method is called after each call to process() in the owner */
        void post();

        void putVariant( const QVariant& data );

        /** returns wheter put() has been called since last pre() */
        inline bool isCalled() const { return m_called; }

        /** checks if this pin is compatible with input pin */
        virtual bool acceptsConnectionWith( const IInputPin* pin, QString& errStr ) const = 0;

        /** methods not implemented from abstract Pin class */
        virtual const std::type_info& getTypeInfo() const = 0;
        virtual int getTypeId() const = 0;
        virtual QString getTypeName() const = 0;

    protected:
        std::list< RefPtr<PinConnection > > m_connections;

        /** true when put() has been called */
        bool m_called;
    };

    template< class T >
    class OutputPin : public IOutputPin
    {
    public:
        OutputPin( const QString& name, PipelineElement* owner ) :
                IOutputPin( name, owner ) {}
        virtual ~OutputPin() {}

        /** Puts data in connection. Drops data if no connection present.
          * Throws an exception if the data violates format contract,
          * which is checked in the callback functor of IOutputPin
          */
        inline void put( const T& data )
        {
            QVariant v( data );
            putVariant( v );
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }

        /** @returns the QMetaType typeId of the data type this pin is initialized with */
        virtual int getTypeId() const
        {
            return qMetaTypeId<T>();
        }

        /** @returns the name of the type this pin is initialized with */
        virtual QString getTypeName() const
        {
            return QMetaType::typeName( qMetaTypeId<T>() );
        }

        virtual bool acceptsConnectionWith( const IInputPin* pin,
                                            QString& errStr ) const
        {
            return true;
        }
    };

    template< class T >
    class InputPin : public IInputPin
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
        inline T get()
        {
            QVariant v = this->get();
            if( !v.canConvert<T>() )
            {
               emit( error( "Type conversion error") );
            }
            // if canConvert returned false, this will
            // return an empty newly constructed T
            return v.value<T>();
        }

        virtual const std::type_info& getTypeInfo() const
        {
            return typeid( T );
        }

        /** @returns the QMetaType typeId of the data type this pin is initialized with */
        virtual int getTypeId() const
        {
            return qMetaTypeId<T>();
        }

        /** @returns the name of the type this pin is initialized with */
        virtual QString getTypeName() const
        {
            return QMetaType::typeName( qMetaTypeId<T>() );
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
