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

#ifndef PINCONNECTION_H
#define PINCONNECTION_H

#include <queue>
#include <QMutexLocker>
#include <QVariant>

#include "RefPtr.h"
#include "RefCounted.h"
#include "PlvExceptions.h"

namespace plv
{
    class Pin;
    class IOutputPin;
    class IInputPin;

    /** Container class for user data */
    class Data
    {
    private:
        /** serial number, used for synchronisation */
        unsigned int m_serial;

        /** the actual data being sent wrapped in a QVariant union */
        QVariant m_payload;

        /** true when this is a NULL packet which means that
            m_payload is empty */
        bool m_null;

    public:
        inline Data(unsigned int serial=0, bool isNull = true) : m_serial(serial), m_null(isNull) {}
        inline Data(unsigned int serial, const QVariant& payload ) : m_serial(serial), m_payload(payload), m_null(false) {}
        inline Data(const Data& other) : m_serial(other.m_serial), m_payload(other.m_payload), m_null(other.m_null) {}
        inline ~Data() {}

        inline unsigned int getSerial() const { return m_serial; }
        inline void setSerial( unsigned int serial ) { m_serial = serial; }

        inline QVariant getPayload() const { return m_payload; }
        inline void setPayload( const QVariant& payload ) { m_payload = payload; }

        /** used to signal a NULL entry. Null entries are ignored
          * by viewers but used to synchronize the system. This is done
          * automatically. Producers should generally never produce a Data item
          * which are NULL. Processors automaticall produce them when they generate
          * no output for a process call */
        inline bool isNull() const { return m_null; }
    };

    class PLVCORE_EXPORT PinConnection : public RefCounted
    {
      public:
        class PLVCORE_EXPORT IllegalConnectionException : public plv::Exception
        {
        public:
            IllegalConnectionException(const QString& str) : plv::Exception(str) {}
            virtual ~IllegalConnectionException() throw() {}
        };

        class PLVCORE_EXPORT IncompatibleTypeException : public plv::Exception
        {
        public:
            IncompatibleTypeException(const QString& str) : plv::Exception(str) {}
            virtual ~IncompatibleTypeException() throw() {}
        };

        class PLVCORE_EXPORT DuplicateConnectionException : public plv::Exception
        {
        public:
            DuplicateConnectionException(const QString& str) : plv::Exception(str) {}
            virtual ~DuplicateConnectionException() throw() {}
        };

        enum ConnectionType {
            LOSSLESS,
            LOSSY_FIFO,
            LOSSY_LIFO
        };
        friend class Pipeline;

        PinConnection( IOutputPin* producer, IInputPin* consumer )
                throw ( IllegalConnectionException,
                        IncompatibleTypeException,
                        DuplicateConnectionException );
        virtual ~PinConnection();

        bool hasData() const;
        int size() const;
        inline ConnectionType getType() const;

        Data get();
        Data peek() const;
        void peek( unsigned int& serial, bool& isNull ) const;
        void put( const Data& data );

        bool fastforward( unsigned int target );

        /** Throw away all data in this connection. */
        void flush();

        /** @return the output pin which provides the input to this connection */
        const IOutputPin* fromPin() const;

        /** @return the input pin which provides output from connection */
        const IInputPin*  toPin() const;

    protected:
        static bool canConnectPins( IOutputPin* out, IInputPin* in, QString& errStr );

        void connect() throw ( IllegalConnectionException,
                               IncompatibleTypeException,
                               DuplicateConnectionException );

        /** Disconnect this connection from its pins
          * this makes it a "floating" connection that can safely be removed.
          * this does not notify the pipeline or remove itself from it.
          * you probably won't call this directly,
          * use Pipeline::disconnect(connection) instead
          */
        void disconnect();

    private:
        IOutputPin* m_producer;
        IInputPin*  m_consumer;
        std::queue< Data > m_queue;
        ConnectionType m_type;
        mutable QMutex m_connectionMutex;
    };
}

namespace plv
{
    PinConnection::ConnectionType PinConnection::getType() const { return m_type; }
}

#endif // PINCONNECTION_H
